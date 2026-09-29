///////////////////////////////////////////////////////////////////////////////
//
// priq_t.cpp: Test priority queue data structure.
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

#if !defined(TEST_PRIQ) && !defined(TEST_ALL)

void testPriq(void) { }

#else

#include "axlgen.h"

static PriQ::Key  testData[] = {
        100, 98, 7, 2, 33, 4, 5, 56, 78, 7, 1, 7, 4,
        200, 150, 175, 125, 3, 8, 2, 23, 100, 100,
        -1
};

void
testPriq(void)
{
        PriQ    pq;
        int     i;

        for (i = 1; i < 7; i++) {
                printf("priqNew: ");
                pq = PriQ::create(i);
                pq.print(osStdout);

                printf("priqFree: ");
                pq.free();
                printf("done\n");
        }

        printf("priqInsert:\n");
        pq = PriQ::create(int0);
        for (i = 0; testData[i] >= 0; i++) {
                printf("... inserting %.6f\n", testData[i]);
                pq.insert(testData[i], NULL);
        }
        pq.print(osStdout);

        printf("priqExtractMin:\n");
        while (pq.count() > 0) {
                PriQ::Key mk;
                pq.extractMin(&mk);
                printf("... extracted %.6f\n", mk);
        }
        pq.free();
}

#endif
