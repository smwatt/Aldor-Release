///////////////////////////////////////////////////////////////////////////////
//
// dnf_t.cpp: Test Boolean DNF operations.
//
///////////////////////////////////////////////////////////////////////////////

// This file is part of Aldor.
//
// Aldor is licensed under the Apache License, Version 2.0.
//
//
// See legal/LICENSE in the Aldor distribution for details.
//
// Copyright (C) 1990-2026 Stephen M. Watt.

#if !defined(TEST_DNF) && !defined(TEST_ALL)

void testDnf(void) { }

#else

#include "axlgen.h"

void
testDnf(void)
{
        DNF     tt, ff, x1, x2, nx2, x3, x4, nx4, xx, yy, zz;
        DNF     t0, t1, t2;

        printf("True:   ");
        tt = DNF::trueForm();
        tt.print(osStdout);
        printf("\n");

        printf("False:  ");
        ff = DNF::falseForm();
        ff.print(osStdout);
        printf("\n");


        printf("Atom:   ");
        x1 = DNF::atom(1);
        x1.print(osStdout);
        printf(" ");
        x2 = DNF::atom(2);
        x2.print(osStdout);
        printf(" ");
        nx2 = DNF::notAtom(2);
        nx2.print(osStdout);
        printf(" ");
        x3 = DNF::atom(3);
        x3.print(osStdout);
        printf(" ");
        x4 = DNF::atom(4);
        x4.print(osStdout);
        printf(" ");
        nx4 = DNF::notAtom(4);
        nx4.print(osStdout);
        printf("\n");

        printf("Or:     ");
        t0 = x1.disjoin(nx2);
        t1 = x3.disjoin(x4);
        xx = t0.disjoin(t1);
        xx.print(osStdout);
        t0.free();
        t1.free();
        xx.free();
        printf("\n");

        printf("And:    ");
        t0 = x1.conjoin(nx2);
        t1 = x3.conjoin(x4);
        xx = t0.conjoin(t1);
        xx.print(osStdout);
        t0.free();
        t1.free();
        printf("\n");

        printf("Not:    ");
        yy = xx.negate();
        yy.print(osStdout);
        xx.free();
        yy.free();
        printf("\n");

        printf("And *0: ");
        t0 = x4.conjoin(x3);
        t1 = x2.conjoin(nx4);
        xx = t0.conjoin(t1);
        xx.print(osStdout);
        t0.free();
        t1.free();
        xx.free();
        printf("\n");

        printf("And *0: ");
        t0 = x2.conjoin(nx4);
        xx = ff.conjoin(t0);
        xx.print(osStdout);
        t0.free();
        xx.free();
        printf("\n");

        printf("And *0: ");
        t0 = x2.conjoin(nx4);
        xx = t0.conjoin(ff);
        xx.print(osStdout);
        t0.free();
        xx.free();
        printf("\n");

        printf("And *1: ");
        t0 = x2.conjoin(nx4);
        xx = tt.conjoin(t0);
        xx.print(osStdout);
        t0.free();
        xx.free();
        printf("\n");

        printf("And *1: ");
        t0 = x2.conjoin(nx4);
        xx = t0.conjoin(tt);
        xx.print(osStdout);
        t0.free();
        xx.free();
        printf("\n");

        printf("And *2: ");
        t0 = x4.conjoin(x3);
        t1 = x2.conjoin(x4);
        xx = t0.conjoin(t1);
        xx.print(osStdout);
        t0.free();
        t1.free();
        xx.free();
        printf("\n");

        printf("And 2*3:");
        t0 = x1.conjoin(nx4);
        t1 = x1.conjoin(x2);
        t2 = t1.conjoin(x3);
        xx = t0.disjoin(t2);
        t0.free();
        t1.free();
        t2.free();
        t0 = x2.conjoin(x4);
        t1 = x2.conjoin(x3);
        t2 = t1.conjoin(x4);
        t1.free();
        t1 = t2.disjoin(x1);
        t2.free();
        yy = t0.disjoin(t1);
        t0.free();
        t1.free();
        zz = xx.conjoin(yy);
        zz.print(osStdout);
        xx.free();
        yy.free();
        printf("\n");

        printf("Not 2*3:");
        xx = zz.negate();
        xx.print(osStdout);
        xx.free();
        zz.free();
        printf("\n");

        printf("Free:   ");
        tt.print(osStdout);
        printf(" ");
        ff.print(osStdout);
        printf(" ");
        x1.print(osStdout);
        printf(" ");
        x2.print(osStdout);
        printf(" ");
        nx2.print(osStdout);
        printf(" ");
        x3.print(osStdout);
        printf(" ");
        x4.print(osStdout);
        printf(" ");
        nx4.print(osStdout);

        tt.free();
        ff.free();
        x1.free();
        x2.free();
        nx2.free();
        x3.free();
        x4.free();
        nx4.free();
        printf("\n");
}

#endif
