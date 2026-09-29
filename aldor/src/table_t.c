/*****************************************************************************
 *
 * table_t.c: Test table type.
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

#if !defined(TEST_TABLE) && !defined(TEST_ALL)

void testTable(void) { }

#else

# include "axlgen.h"

local void      tblHistogram    (RawTable);
local TblElt    tblTestMap      (TblElt);
local int       intprint        (FILE *, void *);

local int
intprint(FILE *fout, void * p)
{
        return fprintf(fout, "%ld", (long) p);
}

local TblElt
tblTestMap(TblElt e)
{
        return (TblElt) (10 * (long) e);
}

local void
tblHistogram(RawTable t)
{
        int     count[50];
        int     infty = sizeof(count)/sizeof(int);

        int     n, i, j, max, maxc;
        struct TblSlot *b;

#if EDIT_1_0_n1_07
        printf("Table with %d buckets (%d entries):\n",
                (int) t->buckc, (int) t->count);
#else
        printf("Table with %d buckets (%d entries):\n", t->buckc, t->count);
#endif

        for (i = 0; i < infty; i++) count[i] = 0;
        max = 0;

        for (i = 0; i < t->buckc; i++) {
                for (b = t->buckv[i], n = 0; b; b = b->next, n++)
                        ;
                if (n > infty) n = infty;
                if (n > max)   max = n;
                count[n]++;
        }
        printf("  L:  No. of buckets with L entries (%d = infinity)\n", infty);
        
        for (maxc=0, i=0; i<=max; i++) if (count[i] > maxc) maxc = count[i];
        for (i = 0; i <= max; i++) {
                printf("%3d: %4d ", i, count[i]);
                n = (int) (50 * ((float) count[i])/((float) maxc));
                for (j = 0; j < n; j++) putchar('+');
                printf("\n");
        }
}

void
testTable(void)
{
        RawTable           t, tc;
#ifndef __cplusplus
        RawTableIterator   tit;
#endif
        int             i;

        printf("tblNew: ");
        t = tblNew((TblHashFun) 0, (TblEqFun) 0);
        tblPrint(osStdout,t, (TblPrKeyFun) intprint, (TblPrEltFun) intprint);
        printf("\n");

        printf("tblSetElt i->2*i: ");
#if EDIT_1_0_n1_07
        for (i = 0; i < 37; i++)
                tblSetElt(t, (TblKey) (long) i, (TblElt) (long) (2*i));
#else
        for (i = 0; i < 37; i++) tblSetElt(t, (void *) i, (void *)(2*i));
#endif
        printf("DONE.\n");

        printf("tblElt: ");
        for (i = 0; i < 50; i += 11) {
                printf("%d -> %ld; ",i,
#if EDIT_1_0_n1_07
                        (long)tblElt(t,(TblKey) (long) i,(TblElt) (long) (-1)));
#else
                        (long)tblElt(t,(void *) i,(void *)(-1)));
#endif
        }
        printf("\n for ");
        tblPrint(osStdout,t, (TblPrKeyFun) intprint, (TblPrEltFun) intprint);
        printf("\n");

        printf("tblSetElt i->i+1: ");
#if EDIT_1_0_n1_07
        for (i = 0; i < 37; i++)
                tblSetElt(t, (TblKey) (long) i, (TblElt) (long) (i+1));
#else
        for (i = 0; i < 37; i++) tblSetElt(t, (void *) i, (void *) (i+1));
#endif
        printf("DONE.\n");

        printf("tblElt: ");
        for (i = 0; i < 50; i += 11) {
                printf("%d -> %ld; ",
#if EDIT_1_0_n1_07
                        i,(long)tblElt(t,(TblKey) (long) i,(TblElt)(long)(-1)));
#else
                        i,(long)tblElt(t,(void *) i,(void *)(-1)));
#endif
        }
        printf("\n");

        printf("tblHistogram: ");
        tblHistogram(t);

        printf("tblDrop: ");
#if EDIT_1_0_n1_07
        for (i = 0; i < 37; i += 2) tblDrop(t, (TblKey) (long) i);
#else
        for (i = 0; i < 37; i += 2) tblDrop(t, (void *) i);
#endif
        tblPrint(osStdout,t, (TblPrKeyFun) intprint, (TblPrEltFun) intprint);
        printf("\n");

        printf("tblDrop: ");
#if EDIT_1_0_n1_07
        for (i = 1; i < 37; i += 2) tblDrop(t, (TblKey) (long) i);
#else
        for (i = 1; i < 37; i += 2) tblDrop(t, (void *) i);
#endif
        tblPrint(osStdout,t, (TblPrKeyFun) intprint, (TblPrEltFun) intprint);
        printf("\n");

        printf("tblFree: ");
        tblFree(t);
        printf("DONE.\n");

        printf("tblITER: ");
        t = tblNew((TblHashFun) 0, (TblEqFun) 0);
#if EDIT_1_0_n1_07
        for (i = 1; i < 60; i += 17)
                tblSetElt(t, (TblKey) (long) i, (TblElt) (long)(2*i));
#else
        for (i = 1; i < 60; i += 17) tblSetElt(t, (void *) i, (void *)(2*i));
#endif
        tblSetElt(t, (void *) 0, (void *) 1);
#ifdef __cplusplus
        {
                Table<void *, void *> table =
                        Table<void *, void *>::fromRaw(t);
                for (Table<void *, void *>::Iterator it(table);
                     it.more(); it.step()) {
                        printf("%ld->%ld, ", (long) it.key(),
                               (long) it.value());
                }
        }
#else
        for (tblITER(tit, t); tblMORE(tit); tblSTEP(tit)) {
                printf("%ld->%ld, ", (long)tblKEY(tit), (long)tblELT(tit));
        }
#endif
        printf("\n for ");
        tblPrint(osStdout,t,(TblPrKeyFun) intprint, (TblPrEltFun) intprint);
        printf("\n");

        printf("tblCopy: ");
        tc = tblCopy(t);
        tblPrint(osStdout, tc, (TblPrKeyFun) intprint, (TblPrEltFun) intprint);
        printf("\n");

        printf("tblNMap: ");
        tblNMap(tblTestMap, tc);
        tblPrint(osStdout, tc, (TblPrKeyFun) intprint, (TblPrEltFun) intprint);
        printf("\n");

        tblFree(t);
        tblFree(tc);

}

#endif
