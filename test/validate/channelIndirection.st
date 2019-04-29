/*************************************************************************\
Copyright (c) 2010-2015 Helmholtz-Zentrum Berlin f. Materialien
                        und Energie GmbH, Germany (HZB)
This file is distributed subject to a Software License Agreement found
in the file LICENSE that is included with this distribution.
\*************************************************************************/
program channelIndirectionTest

option +s;

%%#include "../testSupport.h"

#define MAX_TEST 4

int pv x = pv ("", monitor);
int pv y = pv ("", monitor);

struct xxx {
    int pv *xs[1];
};

entry {
    seq_test_init(MAX_TEST);
}

ss main {
    int pv *xp;
    int pv *yp;
    struct xxx xxx;
    struct xxx *pxxx;
    int i = 0;

    state init {
        when () {
            xp = &x;
            pxxx = &xxx;
            xxx.xs[0] = &x;
        } state test1
    }

    state test1 {
        when (delay(0.25)) {
            testFail("timeout, x = %d", x);
        } state test2
        when (*xp>=1) {
            testPass("monitor event on *xp");
        } state test2
    }

    state test2 {
        entry {
            xp = &x;
            yp = &y;
        }
        when (delay(0.25)) {
            testFail("timeout, x = %d", x);
        } state test3
        when (xp[0] + yp[i]>=1) {
            testPass("monitor event on xp[0]");
        } state test3
    }

    state test3 {
        when (delay(0.25)) {
            testFail("timeout, x = %d", x);
        } state test4
        when (*pxxx->xs[i]>=2) {
            testPass("monitor event on pxxx->xs[i]");
        } state test4
    }

    state test4 {
        when (delay(0.25)) {
            testFail("timeout, x = %d", x);
        } exit
        when (test()) {
            testPass("event: test()");
        } exit
    }
}

ss countx {
    int counter = 0;
    state count {
        when(delay(0.2)) {
            x = ++counter;
            testDiag("pvPut(x) with x=%d", x);
            pvPut(x);
        } state count
    }
}

exit {
    seq_test_done();
}

int test()
{
    return x>=3;
}
