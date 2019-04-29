/*************************************************************************\
Copyright (c) 2010-2015 Helmholtz-Zentrum Berlin f. Materialien
                        und Energie GmbH, Germany (HZB)
This file is distributed subject to a Software License Agreement found
in the file LICENSE that is included with this distribution.
\*************************************************************************/
program waitTest

option +s;

%%#include "../testSupport.h"
%%#include "epicsTime.h"

#define MAX_TEST 2

void delay_task(double time, int pv y)
{
    wait {
        when (delay(time)) {
            testDiag("delay has expired");
        }
        when (y==2) {
            testDiag("monitor event on pv parameter y");
        }
    }
}

int pv x = pv ("", monitor);

entry {
    seq_test_init(MAX_TEST);
}

ss main {
    double nom_delay = 1.0;
    state zero {
        when () {
            typename epicsTimeStamp before, after;
            double real_delay;

            epicsTimeGetCurrent(&before);
            wait {
                when (delay(nom_delay)) {
                    testDiag("delay has expired, x = %d", x);
                }
                when (x) {
                    testDiag("monitor event on pv x");
                }
            }
            epicsTimeGetCurrent(&after);
            real_delay = epicsTimeDiffInSeconds(&after,&before);
            testOk(real_delay < nom_delay, "real_delay(=%f) < nom_delay(=%f)", real_delay, nom_delay);

            epicsTimeGetCurrent(&before);
            delay_task(nom_delay, x);
            epicsTimeGetCurrent(&after);
            real_delay = epicsTimeDiffInSeconds(&after,&before);
            testOk(real_delay < nom_delay, "real_delay(=%f) < nom_delay(=%f)", real_delay, nom_delay);
        } exit
#if 0
        when (delay(nom_delay)) {
            testDiag("delay has expired, x = %d", x);
        } state zero
        when (x) {
            testDiag("monitor event on pv parameter x");
        } exit
#endif
    }
}

ss help {
    int xx = 0;
    state zero {
        when(delay(0.7)) {
            xx++;
            x = xx;
            pvPut(x);
        } state zero
    }
}

exit {
    seq_test_done();
}
