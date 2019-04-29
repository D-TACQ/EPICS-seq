/*************************************************************************\
Copyright (c) 2010-2015 Helmholtz-Zentrum Berlin f. Materialien
                        und Energie GmbH, Germany (HZB)
This file is distributed subject to a Software License Agreement found
in the file LICENSE that is included with this distribution.
\*************************************************************************/
program varInitTest

/* good */
int x = 1;
int xx = x;
int *px = &x;

/* bad */
int x1 = --x;
int x2 = ++x;
int x3 = x--;
int x4 = x++;
int x5 = *px;

ss test {

    /* good */
    int y = x;
    int yy = y;
    int *py = &y;

    /* bad */
    int y1 = --y;
    int y2 = ++y;
    int y3 = y--;
    int y4 = y++;
    int y5 = *py;

    state test {

        /* good */
        int z = y;
        int zz = z;
        int *pz = &z;

        /* bad */
        int z1 = --z;
        int z2 = ++z;
        int z3 = z--;
        int z4 = z++;
        int z5 = *pz;

        when () {} exit
    }
}
