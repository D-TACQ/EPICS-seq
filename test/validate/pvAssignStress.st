program pvAssignStressTest

option +s;

%%#include "../testSupport.h"

int nruns = 5;

int nrepeat = 5;

double interfere_delay = 0.1;

#define NCHANS 3

string names[NCHANS] = {
    "pvAssignStress0",
    "pvAssignStress1",
    "pvAssignStress2"
};

int pv x[NCHANS];

entry {
    int const ninterferes = NCHANS * nrepeat * nruns;
    int const npvassigns = NCHANS * 2 * 3 * nruns;
    seq_test_init(3 * ninterferes + npvassigns);
}

ss pvassign {
    int i;
    int shift = 0;
    state disconn {
        option -t;
        entry {
            for (i=0; i<NCHANS; i++) {
                int r = pvAssign(x[i], names[(i+shift)%NCHANS]);
                testOk1(r==pvStatOK);
            }
        }
        when (pvConnected(x[0]) && pvConnected(x[1]) && pvConnected(x[2])) {
            for (i=0; i<NCHANS; i++) {
                pvGet(x[i],SYNC,0.1);
                testOk(x[i] == (i+shift)%NCHANS,"pvassign: %d==%d",x[i],(i+shift)%NCHANS);
            }
            shift++;
        } state conn
        when (delay(10)) {
            /* This long timeout is really necessary, because tests are
            usually not run with realtime priorities; at least on my machine
            I get "Virtual circuit disconnect" every few runs and then it
            takes more than 5 seconds to re-establish the connection. */
            testAbort("pvassign: timeout waiting for connect");
        } exit
    }
    state conn {
        entry {
            for (i=0; i<NCHANS; i++) {
                pvAssign(x[i], "");
            }
        }
        when (shift == 3*nruns) {
        } exit
        when (!pvAssigned(x[0]) && !pvAssigned(x[1]) && !pvAssigned(x[2])) {
        } state disconn
    }
}

ss interfere1 {
    int count = 0;
    state test {
        when (delay(interfere_delay)) {
            if (count++ < nrepeat*nruns)
                get_and_test("interfere1");
        } state test
    }
}

ss interfere2 {
    int count = 0;
    state test {
        when (delay(interfere_delay)) {
            if (count++ < nrepeat*nruns)
                get_and_test("interfere2");
        } state test
    }
}

ss interfere3 {
    int count = 0;
    state test {
        when (delay(interfere_delay)) {
            if (count++ < nrepeat*nruns)
                get_and_test("interfere3");
        } state test
    }
}

exit {
    seq_test_done();
}

void get_and_test(char *stateset)
{
    int i;
    for (i=0; i<NCHANS; i++) {
        enum pvStat r;
        char *name;
        if (!pvAssigned(x[i])) {
            testPass("%s: not assigned", stateset);
            continue;
        }
        if (!pvConnected(x[i])) {
            testPass("%s: not connected", stateset);
            continue;
        }
        name = pvName(x[i]);
        r = pvGet(x[i],SYNC,0.1);
        if (r==pvStatOK) {
            testOk(0==strcmp(names[x[i]],name),"%s: %s==%s",stateset, names[x[i]], name);
        } else {
            testPass("%s: pvGet failed", stateset);
        }
    }
}
