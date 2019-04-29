/*************************************************************************\
Copyright (c) 2010-2015 Helmholtz-Zentrum Berlin f. Materialien
                        und Energie GmbH, Germany (HZB)
This file is distributed subject to a Software License Agreement found
in the file LICENSE that is included with this distribution.
\*************************************************************************/
program scope

int ch, v;
assign ch;

evflag f;

monitor v;              /* error: not a channel */
sync ch to v;           /* error: not an event flag */
syncq ch to v 1;        /* error: not an event flag */
sync v to f;            /* error: not a channel */
syncq v to f 1;         /* error: not a channel */

ss start {
    int lch, lv;
    assign lch;
    monitor lch;

    assign v;           /* error: wrong scope */
    monitor v;
    sync ch to v;       /* error: not an event flag */
    syncq ch to v 1;    /* error: not an event flag */
    sync v to f;        /* error: wrong scope */
    syncq v to f 1;     /* error: wrong scope */

    monitor lv;         /* error: not a channel */
    sync ch to lv;      /* error: not an event flag */
    syncq ch to lv 1;   /* error: not an event flag */
    sync lv to f;       /* error: not a channel */
    syncq lv to f 1;    /* error: not a channel */

    state first {
        when () {
        } exit
    }
}
