///////////////////////////////////////////////////////////////////////////////
//
// list_t.cpp: Test the list type.
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

#if !defined(TEST_LIST) && !defined(TEST_ALL)

void testList(void) { }

#else

# include "axlgen.h"
# include <stdint.h>

int
testListPrintElt(FILE *fout, void *p)
{
        return fprintf(fout, "%lld", (long long) (intptr_t) p);
}

void
testList(void)
{
        int             i, j;
        void            *p, **v;
        List<void *>    l, l0, l1, l2;

        printf("listCons: \n");
        l = listCons<void *>(nullptr, listNil<void *>);
        for (i = 1; i < 10; i++) {
                l = listCons<void *>(reinterpret_cast<void *>((long) i), l);
                printf("list %d = ", i);
                listPrint<void *>(osStdout, l, testListPrintElt);
                printf("\n");
        }

        printf("listLength = ");
        printf("%d\n", (int) listLength<void *>(l));

        if (listIsLength<void *>(l, i))
                printf("listIsLength is %d: true\n", i);
        else
                printf("listIsLength is %d: false\n", i);

        printf("listReverse: \n");
        l1 = listReverse<void *>(l);
        listPrint<void *>(osStdout, l1, testListPrintElt);
        printf("\n");

        printf("listConcat: \n");
        l2 = listConcat<void *>(l, l1);
        listPrint<void *>(osStdout, l2, testListPrintElt);
        printf("\n");

        printf("listFreeCons: \n");
        l = listFreeCons<void *>(l);
        listPrint<void *>(osStdout, l, testListPrintElt);
        printf("\n");

        l0 = l;
        printf("listDrop: \n");
        l = listDrop<void *>(l, 1);
        listFreeCons<void *>(l0);
        listPrint<void *>(osStdout, l, testListPrintElt);
        printf("\n");

        printf("listElt: \n");
        p = listElt<void *>(l, 1);
        printf("element %lld ", (long long) (intptr_t) p);
        if (listMemq<void *>(l, p)) printf("is a list member.\n");
        else printf("is not a list member.\n");

        i = (int) listLength<void *>(l);
        v = reinterpret_cast<void **>(stoAlloc(OB_Other, i * sizeof(void *)));
        printf("listFillVector: \n");
        listFillVector<void *>(v, l);
        printf("vector = <");
        for (j = 0; j < i; j++)
                printf("%lld ", (long long) (intptr_t) v[j]);
        printf(">\n");
        stoFree(v);
        printf("\n");

        printf("listFree: ");
        listFree<void *>(l);
        listFreeTo<void *>(l2, l1);     /* l1 is contained in l2 */
        listFree<void *>(l1);
}

#endif
