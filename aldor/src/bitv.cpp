///////////////////////////////////////////////////////////////////////////////
//
// bitv.cpp: Bit vector operations.
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

/****************************************************************************
 * This file provides all the basic operations for bit vectors manipulation.
 *
 * For space saving reasons, the size of a bitv is not saved in the bitv
 * representation. This avoid, in example in the dflow analysis algorithms,
 * to allocate many copies of the same length-information, since generally
 * all the bit vectors used have the same size.
 *
 * Therefore creating a bitvClass is needed before working with bitv of size
 * NBITS, and the bitvClass is passed to all the bitv operations.
 *
 * Example:
 *
 *      BitvClass       bitvClass = bitvClassCreate(NBITS);
 *
 *      Bitv            bitv      = bitvNew(bitvClass);
 *
 *      bitvClearAll    (bitvClass, bitv);
 *      bitvSet         (bitvClass, bitv);
 *      bitvPrint       (fout, bitvClass, bitv);
 *
 *      bitvFree        (bitv);         // class not needed
 *      bitvClassDestroy(bitvClass);
 *
 ***************************************************************************/

#include "axlgen.h"

#define         BpW     bitsizeof(BitvWord)

BitvClass
bitvClassCreate(int nbits)
{
        BitvClass       bitvClass;

        bitvClass = (BitvClass) stoAlloc(OB_Other, sizeof(struct _BitvClass));

        bitvClass->nbits  = nbits;
        bitvClass->nwords = QUO_ROUND_UP(nbits, BpW);

        return bitvClass;
}

void
bitvClassDestroy(BitvClass class_)
{
        if (class_) stoFree(class_);
}

Bitv
bitvNew(BitvClass class_)
{
        Bitv bv = (Bitv) stoAlloc(OB_Other, class_->nwords * sizeof(BitvWord));
        return bv;
}

void
bitvFree(Bitv a)
{
        stoFree((void *) a);
}

/* Given a bitv and its class, convert is to a new class, possible w/out
 * reallocate memory.
 * If the lenght is bigger, the old bits are copied, and the new ones are
 * undefined.
 * If the length is less than the original, it's truncated.
 */
Bitv
bitvResize(BitvClass newc, BitvClass oldc, Bitv b)
{
        Bitv    new_;
        int     i;

        if (oldc->nwords >= newc->nwords) return b;

        new_ = bitvNew(newc);
        for (i = 0; i < oldc->nwords; i++)
                new_[i] = *b++;

        bitvFree(b);

        return new_;
}

Bitv *
bitvManyNew(BitvClass class_, Length n)
{
#       define bitvOffset(n,i) ((n)*sizeof(Bitv) + (i)*class_->nwords*sizeof(BitvWord))

        Bitv    *bvv = (Bitv *) stoAlloc(OB_Other, bitvOffset(n,n));
        int     i;

        for (i = 0; i < n; i++)
                bvv[i] = (Bitv)((char *) bvv + bitvOffset(n, i));
        return bvv;
}

void
bitvManyFree(Bitv * bvv)
{
        stoFree((void *) bvv);
}

int
bitvPrint(FILE *fout, BitvClass class_, Bitv a)
{
        int     i, cc = 0;

        if (!class_) {
                cc += fprintf(fout,
                              "[** Unprintable bitv: bitvClass required **]");
                return cc;
        }

        cc += fprintf(fout, "[");
        for (i = 0; i < class_->nbits; i++) {
                cc += fprintf(fout, "%d",
                              bitvTest(class_, a,i));
                if (i % 5 == 4) cc += fprintf(dbOut, " ");
        }
        cc += fprintf(fout, "]");
        return cc;
}

int
bitvPrintDb(BitvClass class_, Bitv a)
{
        int i = bitvPrint(dbOut, class_, a);
        fprintf(dbOut, "\n");
        return i;
}

bool
bitvEqual(BitvClass class_, Bitv a, Bitv b)
{
        int      i;
        BitvWord mask;

        if (class_->nwords == 0) return true;

        for (i = 0; i < class_->nwords-1; i++) if (*a++ != *b++) return false;
        if (class_->nbits % BpW == 0) return *a == *b;

        /*
         * Test last word differently -- mask out extra bits so they
         * do not influence the equality test.
         */
        mask = ~((~0L) << (class_->nbits % BpW));
        return (*a & mask) == (*b & mask);
}

void
bitvSetAll(BitvClass class_, Bitv r)
{
        int     i;

        for (i = 0; i < class_->nwords; i++) *r++ = (BitvWord)(~0L);
}

void
bitvClearAll(BitvClass class_, Bitv r)
{
        int     i;
        for (i = 0; i < class_->nwords; i++) *r++ = 0;
}

int
bitvTest(BitvClass class_, Bitv r, int ix)
{
        assert(ix < class_->nbits);
        return !!(r[ix/BpW] & (1L << (ix % BpW)));
}

void
bitvSet(BitvClass class_, Bitv r, int ix)
{
        assert(ix < class_->nbits);
        r[ix/BpW] |=  (1L << (ix % BpW));
}

void
bitvClear(BitvClass class_, Bitv r, int ix)
{
        assert(ix < class_->nbits);
        r[ix/BpW] &= ~(1L << (ix % BpW));
}

void
bitvCopy(BitvClass class_, Bitv r, Bitv a)
{
        int     i;
        for (i = 0; i < class_->nwords; i++) *r++ = *a++;
}

void
bitvNot(BitvClass class_, Bitv r, Bitv a)
{
        int     i;
        for (i = 0; i < class_->nwords; i++) *r++ = ~*a++;
}

void
bitvAnd(BitvClass class_, Bitv r, Bitv a, Bitv b)
{
        int     i;
        for (i = 0; i < class_->nwords; i++) *r++ = *a++ & *b++;
}

void
bitvOr(BitvClass class_, Bitv r, Bitv a, Bitv b)
{
        int     i;
        for (i = 0; i < class_->nwords; i++) *r++ = *a++ | *b++;
}

void
bitvMinus(BitvClass class_, Bitv r, Bitv a, Bitv b)
{
        int     i;
        for (i = 0; i < class_->nwords; i++) *r++ = *a++ & ~*b++;
}

int
bitvUnique1IndexInRange(BitvClass class_, Bitv bv, int org, int lim)
{
        int     n1s, i, last1=-1;

        n1s = 0;

        for (i = org; i < lim; i++)
                if (bitvTest(class_, bv, i)) {
                        n1s++;
                        last1 = i;
                        if (n1s > 1) return -1;
                }

        if (n1s != 1) return -1;
        return last1;
}
