TOP = .
include $(TOP)/configure/CONFIG

DIRS += configure

DIRS += src
src_DEPEND_DIRS  = configure

DIRS += test
test_DEPEND_DIRS = src

DIRS += examples
examples_DEPEND_DIRS = src

ifdef docs
DIRS += documentation
documentation_DEPEND_DIRS = src
endif

BRANCH = 2-1
DEFAULT_REPO = /opt/repositories/controls/darcs/epics/support/seq/branch-$(BRANCH)
GIT_MIRROR = /opt/repositories/controls/git/seq/branch-$(BRANCH)
SEQ_PATH = www/control/SoftDist/sequencer-$(BRANCH)
USER_AT_HOST = wwwcsr@www-csr.bessy.de
DATE = $(shell date -I)
SNAPSHOT = seq-$(BRANCH)-snapshot-$(DATE)
SEQ_TAG = seq-$(subst .,-,$(SEQ_RELEASE))
SEQ_TAG_TIME := $(shell darcs changes --all --xml-output \
	--matches 'exact "TAG $(SEQ_TAG)"' | perl -ne 'print "$$1.$$2" if /date=.(\d{12})(\d{2})/')

include $(TOP)/configure/RULES_TOP

recreate-git-mirror:
	rm -rf $(GIT_MIRROR)
	git init $(GIT_MIRROR)
	touch $(GIT_MIRROR)/git.marks

sync-repos:
	darcs push $(DEFAULT_REPO)
	cd $(DEFAULT_REPO) && darcs push --all $(USER_AT_HOST):$(SEQ_PATH)/repo/branch-$(BRANCH)
	cd $(DEFAULT_REPO) && darcs convert export --read-marks $(GIT_MIRROR)/darcs.marks --write-marks $(GIT_MIRROR)/darcs.marks |\
	  (cd $(GIT_MIRROR) && git fast-import --import-marks=git.marks --export-marks=git.marks)
	cd $(GIT_MIRROR)/.git && git --bare update-server-info
	rsync -r --delete $(GIT_MIRROR)/.git/ $(USER_AT_HOST):$(SEQ_PATH)/repo/branch-$(BRANCH).git/

upload-docs:
	$(MAKE) -sj docs=1 pdf=1
	rsync -r -t $(TOP)/html/ $(USER_AT_HOST):$(SEQ_PATH)/

snapshot: sync-repos
	darcs dist -d $(SNAPSHOT)
	rsync $(SNAPSHOT).tar.gz $(USER_AT_HOST):$(SEQ_PATH)/releases/
	ssh $(USER_AT_HOST) 'cd $(SEQ_PATH)/releases && ln -f -s $(SNAPSHOT).tar.gz seq-$(BRANCH)-snapshot-latest.tar.gz'
	$(RM) $(SNAPSHOT).tar.gz

release:
	darcs show files | xargs touch -t $(SEQ_TAG_TIME)
	darcs dist -d seq-$(SEQ_RELEASE) -t '^$(SEQ_TAG)$$'
	rsync seq-$(SEQ_RELEASE).tar.gz $(USER_AT_HOST):$(SEQ_PATH)/releases/
	$(RM) seq-$(SEQ_RELEASE).tar.gz

changelog: force
	DARCS_ALWAYS_COLOR=0 darcs changes -a --from-tag=. | egrep -v '^(Author|Date|patch)' > changelog

force:

.PHONY: mirror sync-repos upload-docs snapshot release recreate-git-mirror
