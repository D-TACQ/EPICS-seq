# -*- coding: utf-8 -*-
#
# seq documentation build configuration file, created by
# sphinx-quickstart on Tue May 11 18:36:35 2010.
#
# This file is execfile()d with the current directory set to its containing dir.
#
# Note that not all possible configuration values are present in this
# autogenerated file.
#
# All configuration values have a default; values that are commented out
# serve to show the default.

import sys, os, re

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#sys.path.append(os.path.abspath('.'))

# -- General configuration -----------------------------------------------------

# If your documentation needs a minimal Sphinx version, state it here.
needs_sphinx = '1.3'

# Add any Sphinx extension module names here, as strings. They can be extensions
# coming with Sphinx (named 'sphinx.ext.*') or your custom ones.
extensions = ['sphinx.ext.todo', 'sphinxcontrib.blockdiag']

blockdiag_antialias = True;
blockdiag_html_image_format = 'SVG'

todo_include_todos = False

# Add any paths that contain templates here, relative to this directory.
templates_path = ['templates']

# The suffix of source filenames.
source_suffix = '.txt'

# The encoding of source files.
source_encoding = 'utf-8'

# The master toctree document.
master_doc = 'index'

# General information about the project.
project = u'seq'
copyright = u"""
2000, William Lupton, W. M. Keck Observatory, Kamuela, HI 96743, USA;
2010-2015, Benjamin Franksen, Helmholtz-Zentrum Berlin für Materialien und Energie GmbH
"""

# The version info for the project you're documenting, acts as replacement for
# |version| and |release|, also used in various other places throughout the
# built documents.

def get_seq_release():
  for line in open('../configure/CONFIG').readlines():
    m = re.match('\s*SEQ_RELEASE\s*=\s*([^\s]+)', line)
  if m is not None:
    return m.group(1)

# The full version, including alpha/beta/rc tags.
release = get_seq_release()

# The short X.Y version.
version = re.match('(^[^.]+\.[^.]+)', release).group(1)

# The language for content autogenerated by Sphinx. Refer to documentation
# for a list of supported languages.
#language = None

# There are two options for replacing |today|: either, you set today to some
# non-false value, then it is used:
#today = ''
# Else, today_fmt is used as the format for a strftime call.
#today_fmt = '%B %d, %Y'

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
exclude_patterns = ['O.Common', 'About.txt']

# The reST default role (used for this markup: `text`) to use for all documents.
default_role = 'any'

# If true, '()' will be appended to :func: etc. cross-reference text.
add_function_parentheses = False

# If true, the current module name will be prepended to all description
# unit titles (such as .. function::).
#add_module_names = True

# If true, sectionauthor and moduleauthor directives will be shown in the
# output. They are ignored by default.
show_authors = True

highlight_language = "none"

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = 'sphinx'

# A list of ignored prefixes for module index sorting.
#modindex_common_prefix = []


# -- Options for HTML output ---------------------------------------------------

# The theme to use for HTML and HTML Help pages.  Major themes that come with
# Sphinx are currently 'default' and 'sphinxdoc'.
html_theme = 'naturefixed'

# Theme options are theme-specific and customize the look and feel of a theme
# further.  For a list of options available for each theme, see the
# documentation.
# html_theme_options = {
#   "stickysidebar": "true",
#   "fontsize": "90%",
# }

# Add any paths that contain custom themes here, relative to this directory.
html_theme_path = ["themes"]

# The name for this set of Sphinx documents.  If None, it defaults to
# "<project> v<release> documentation".
html_title = "EPICS Sequencer Version %s" % version

# A shorter title for the navigation bar.  Default is the same as html_title.
#html_short_title = "Home"

# The name of an image file (relative to this directory) to place at the top
# of the sidebar.
#html_logo = None

# The name of an image file (within the static path) to use as favicon of the
# docs.  This file should be a Windows icon file (.ico) being 16x16 or 32x32
# pixels large.
#html_favicon = None

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['html_static']

# If not '', a 'Last updated on:' timestamp is inserted at every page bottom,
# using the given strftime format.
html_last_updated_fmt = '%b %d, %Y'

# If true, SmartyPants will be used to convert quotes and dashes to
# typographically correct entities.
#html_use_smartypants = True

# Custom sidebar templates, maps document names to template names.
html_sidebars = {
 'index' : ['home.html', 'searchbox.html'],
 'Manual' : ['home.html', 'toc.html', 'relations.html', 'searchbox.html'],
 '*' : ['toc.html', 'relations.html', 'searchbox.html']
}

# Additional templates that should be rendered to pages, maps page names to
# template names.
#html_additional_pages = {}

# If false, no module index is generated.
html_use_modindex = False

# If false, no index is generated.
html_use_index = False

# If true, the index is split into individual pages for each letter.
#html_split_index = False

# If true, links to the reST sources are added to the pages.
html_show_sourcelink = False

# If true, an OpenSearch description file will be output, and all pages will
# contain a <link> tag referring to it.  The value of this option must be the
# base URL from which the finished HTML is served.
#html_use_opensearch = ''

# If nonempty, this is the file name suffix for HTML files (e.g. ".xhtml").
#html_file_suffix = ''

# Output file base name for HTML help builder.
#htmlhelp_basename = 'seqdoc'


# -- Options for LaTeX output --------------------------------------------------

latex_authors = u"""
William Lupton (wlupton$@$keck.hawaii.edu)\\\\*
Benjamin Franksen (benjamin.franksen$@$helmholtz-berlin.de)
"""

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title, author, documentclass [howto/manual]).
latex_documents = [
  ('Manual', 'Manual.tex', 'State Notation Language and Sequencer Users\' Guide',
    latex_authors, 'manual'),
]
