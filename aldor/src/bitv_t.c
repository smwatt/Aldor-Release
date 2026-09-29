/*****************************************************************************
 *
 * bitv_t.c: Test Bitv operations.
 *
 * This file is part of Aldor.
 *
 * Aldor is licensed under the Apache License, Version 2.0.
 *
 *
 * See legal/LICENSE in the Aldor distribution for details.
 *
 * Copyright (C) 1990-2026 Stephen M. Watt.
 */

#if !defined(TEST_BITS) && !defined(TEST_ALL)

void testBitv(void) { }

#else

#include "axlgen.h"

static BitvClass  bitvClass;

void
testBitv(void)
{
        Length  i, j, n, sz;

        Bitv    a, b, c, r;

        printf("Sizes: ");
        for (j = 0, i = 26; i < 66; j++, i++) {
                bitvClass = bitvClassCreate(i);
                a = bitvNew(bitvClass);
                sz = stoSize(a);
                if (sz == 0) {
                        printf("ERROR at %d bits\n", (int) i);
                        exitFailure();
                }
                bitvFree(a);
                bitvClassDestroy(bitvClass);
        }
        printf("OK\n");

        n = 49;
         bitvClass = bitvClassCreate(n);

        a = bitvNew(bitvClass);
        b = bitvNew(bitvClass);
        c = bitvNew(bitvClass);
        r = bitvNew(bitvClass);

        printf("ClearAll: ");
        bitvClearAll(bitvClass, a);
        bitvPrint(osStdout, bitvClass, a);
        printf("\n");

        printf("SetAll:   ");
        bitvSetAll(bitvClass, b);
        bitvPrint(osStdout, bitvClass, b);
        printf("\n");

        bitvSetAll(bitvClass, a);
        printf("\nClear:    ");
        bitvPrint(osStdout, bitvClass, a);
        printf("\n");
        for (i = 0; i < n; i++) {
                bitvClear(bitvClass, a, i);
/*
                if (4*i > n || 4*i < 3*n) continue;
*/
                printf("          ");
                bitvPrint(osStdout, bitvClass, a);
                printf("\n");
        }

        bitvClearAll(bitvClass, a);
        printf("\nSet:      ");
        bitvPrint(osStdout, bitvClass, a);
        printf("\n");
        for (i = 0; i < n; i++) {
                bitvSet(bitvClass, a, i);
/*
                if (4*i > n || 4*i < 3*n) continue;
*/
                printf("          ");
                bitvPrint(osStdout, bitvClass, a);
                printf("\n");
        }
        printf("\n");

        printf("Test:     [");
        for (i = 0; i < n; i++) printf("%c", bitvTest(bitvClass,a,i) ? 'Y' : 'N');
        printf("]\n");
        
        bitvSetAll(bitvClass,a);
        bitvSetAll(bitvClass,b);
        bitvClearAll(bitvClass, c);

        for (i = 0; i < n; i++) bitvSet(bitvClass,c, i);

        bitvClear(bitvClass,a, int0); bitvClear(bitvClass,a, 1);
        bitvSet  (bitvClass,a, 2); bitvSet(bitvClass,a, 3);
        bitvClear(bitvClass,b, int0); bitvSet(bitvClass,b, 1);
        bitvClear(bitvClass,b, 2); bitvSet(bitvClass,b, 3);
        bitvClear(bitvClass,c, int0); bitvSet(bitvClass, c, 1);
        bitvClear(bitvClass,c, 2); bitvSet(bitvClass,c, 3);

        printf("a =       ");
        bitvPrint(osStdout, bitvClass, a);
        printf("\n");

        printf("b =       ");
        bitvPrint(osStdout, bitvClass, b);
        printf(" (extra set)\n");

        printf("c =       ");
        bitvPrint(osStdout, bitvClass, c);
        printf(" (extra clear)\n");

        printf("Test a:   [");
        for (i = 0; i < n; i++) printf("%c", bitvTest(bitvClass,a,i) ? 'Y' : 'N');
        printf("]\n");
        
        printf("Copy a:   ");
        bitvCopy(bitvClass,r, a);
        bitvPrint(osStdout,bitvClass, c);
        printf("\n");

        printf("Not a:    ");
        bitvNot(bitvClass,r, a);
        bitvPrint(osStdout,bitvClass, c);
        printf("\n");

        printf("And a b:  ");
        bitvAnd(bitvClass,r, a, b);
        bitvPrint(osStdout,bitvClass, c);
        printf("\n");

        printf("Or a b:   ");
        bitvOr(bitvClass,r, a, b);
        bitvPrint(osStdout, bitvClass, c);
        printf("\n");

        printf("Minus a b:");
        bitvMinus(bitvClass,r, a, b);
        bitvPrint(osStdout,bitvClass, c);
        printf("\n");

        printf("Equal a a: %s\n", bitvEqual(bitvClass,a,a) ? "true" : "false");
        printf("Equal a b: %s\n", bitvEqual(bitvClass,a,b) ? "true" : "false");
        printf("Equal b c: %s\n", bitvEqual(bitvClass,b,c) ? "true" : "false");

        bitvFree(a);
        bitvFree(b);
        bitvFree(c);
        bitvFree(r);

        bitvClassDestroy(bitvClass);
}

#endif
