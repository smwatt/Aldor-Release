/*****************************************************************************
 *
 * btree_t.c: Test B-trees.
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

#if !defined(TEST_BTREE) && !defined(TEST_ALL)

void testBtree(void) { }

#else

# include "axlgen.h"

void
testBtree(void)
{
#define BTREE_TEST_T            2
#define BTREE_TEST_ITERS        1000
#define BTREE_TEST_KEY_LIMIT    2000

        int             i, j, rc;
        BTree           b = btreeNew(BTREE_TEST_T);
        Length          maxk = 0;
        BTreeKey        key[BTREE_TEST_ITERS];
        int             searchErrors = 0;

        /*
         * Use a portable deterministic permutation of the even keys 0..1998.
         * This tests insertion shape without depending on libc rand().
         */
        printf("Inserting: ");
        for (i = 0; i < BTREE_TEST_ITERS; i++) {
                key[i] = 2 * ((37 * i + 17) % (BTREE_TEST_KEY_LIMIT/2));
                if (key[i] > maxk) maxk = key[i];
                btreeInsert(&b, key[i], NULL);
                if ((rc = btreeCheck(b)) != 0) {
                        printf("ERROR %d\n", rc);
                        exitFailure();
                }
        }
        printf("OK\n");

        /* Search for the least stored key greater than or equal to i. */
        printf("SearchGE: ");
        for (i = (int) maxk - 20; i < (int) maxk + 5; i++) {
                BTree x;
                int xi;
                long expected = -1;

                for (j = 0; j < BTREE_TEST_ITERS; j++)
                        if (key[j] >= i && (expected < 0 || key[j] < expected))
                                expected = key[j];

                x = btreeSearchGE(b, i, &xi);
                if (expected < 0) {
                        if (x) searchErrors++;
                }
                else if (!x || x->part[xi].key != expected)
                        searchErrors++;
        }
        if (searchErrors) {
                printf("ERROR (%d mismatches)\n", searchErrors);
                exitFailure();
        }
        printf("OK\n");

        printf("Deleting: ");
        for (i = 0; i < BTREE_TEST_ITERS; i++) {
                btreeDelete(&b, key[i], NULL);
                if ((rc = btreeCheck(b)) != 0) {
                        printf("ERROR %d\n", rc);
                        exitFailure();
                }
        }
        printf("OK\n");
        btreeFree(b);
}

#endif
