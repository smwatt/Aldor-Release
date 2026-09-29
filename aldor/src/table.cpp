///////////////////////////////////////////////////////////////////////////////
//
// table.cpp: Hash table data type.
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

# include "axlgen.h"

# define TBL_InitBuckC   7       /* Largest prime under 2**i, for some i. */
# define TBL_MaxLoad     5

local RawTable     tblNew0         (TblHashFun, TblEqFun, int buckc);
local void      tblEnlarge      (RawTable);

RawTable
tblNew(TblHashFun hash, TblEqFun eq)
{
        return tblNew0(hash, eq, TBL_InitBuckC);
}

local RawTable
tblNew0(TblHashFun hash, TblEqFun eq, int buckc)
{
        Length  i;
        RawTable  t   = (RawTable) stoAlloc((unsigned) OB_Table, sizeof(*t));
        t->hashFun = hash;
        t->eqFun   = eq;
        t->info    = 0;
        t->count   = 0;
        t->buckc   = buckc;
        t->buckv   = (struct TblSlot **)
                 stoAlloc((unsigned) OB_Other, buckc*sizeof(struct TblSlot *));
        for (i = 0; i < buckc; i++) t->buckv[i] = 0;
        return t;
}

void
tblFreeDeeply(RawTable t, TblFreeKeyFun fk, TblFreeEltFun fe)
{

        struct TblSlot  *b, *nb;
        int             i;

        for (i = 0; i < t->buckc; i++) {
                for (b = t->buckv[i]; b; ) {
                        nb = b->next;
                        if (fk) fk(b->key);
                        if (fe) fe(b->elt);
                        stoFree((void *) b);
                        b = nb;
                }
        }
        stoFree((void *) t->buckv);
        stoFree((void *) t);
}

void
tblFree(RawTable t)
{
        struct TblSlot  *b, *nb;
        int             i;

        for (i = 0; i < t->buckc; i++)
                for (b = t->buckv[i]; b; ) {
                        nb = b->next;
                        stoFree((void *) b);
                        b = nb;
                }
        stoFree((void *) t->buckv);
        stoFree((void *) t);
}

RawTable
tblCopy(RawTable ot)
{
        RawTable           nt;
        struct TblSlot  *ob, *nb, **pnb;
        int             i;

        nt = tblNew0(ot->hashFun, ot->eqFun, ot->buckc);
        nt->count = ot->count;
        for (i = 0; i < ot->buckc; i++) {
                pnb = nt->buckv + i;
                for (ob = ot->buckv[i]; ob; ob = ob->next) {
                        nb = (struct TblSlot *)
                                stoAlloc((unsigned) OB_Other, sizeof(*nb));
                        nb->key  = ob->key;
                        nb->elt  = ob->elt;
                        nb->hash = ob->hash;
                        *pnb     = nb;
                        pnb      = &(nb->next);
                }
                *pnb = 0;
        }
        return nt;
}

/* Given table, freeFun, testFun, apply freeFun to all (non-null) elements in
 * table that satisfy testFun
 */
RawTable
tblRemoveIf(RawTable t, TblFreeEltFun fe, TblTestEltFun f)
{
        struct TblSlot  *b;
        int             i;

        for (i = 0; i < t->buckc; i++)
                for (b = t->buckv[i]; b; b = b->next)
                        if (b->elt && f(b->elt)) {
                                fe(b->elt);
                                b->elt = NULL;    /* !! */
                        }
        return t;
}

RawTable
tblNMap(TblMapEltFun f, RawTable t)
{
        struct TblSlot  *b;
        int             i;

        for (i = 0; i < t->buckc; i++)
                for (b = t->buckv[i]; b; b = b->next)
                        b->elt = f(b->elt);
        return t;
}


Length
tblSize(RawTable t)
{
        return t->count;
}

#if 0
#define BUCKET_SEARCH(t,x,b,h,k,efun,ACTION) {                          \
        struct TblSlot *p = 0;                                 \
        if (efun) {                                                     \
                do {                                                    \
                        if (b->hash == h)                               \
                                if (efun(k, b->key)) {                  \
                                        if (p) {                        \
                                                /* Move to front */     \
                                                p->next = b->next;      \
                                                b->next = t->buckv[x];  \
                                                t->buckv[x] = b;        \
                                        }                               \
                                        ACTION;                         \
                                }                                       \
                        p = b;                                          \
                        b = b->next;                                    \
                } while (b);                                            \
        }                                                               \
        else {                                                          \
                do {                                                    \
                        if (b->hash == h) {                             \
                                        if (p) {                        \
                                                /* Move to front */     \
                                                p->next = b->next;      \
                                                b->next = t->buckv[x];  \
                                                t->buckv[x] = b;        \
                                        }                               \
                                        ACTION;                         \
                        }                                               \
                        p = b;                                          \
                        b = b->next;                                    \
                } while (b);                                            \
        }                                                               \
}
#endif

#define BUCKET_SEARCH(t,x,b,h,k,efun,ACTION) {                  \
        struct TblSlot *p = 0;                         \
        do {                                                    \
                if (b->hash == h)                               \
                        if (!efun || efun(k, b->key)) {         \
                                if (p) {                        \
                                        /* Move to front */     \
                                        p->next = b->next;      \
                                        b->next = t->buckv[x];  \
                                        t->buckv[x] = b;        \
                                }                               \
                                ACTION;                         \
                        }                                       \
                p = b;                                          \
                b = b->next;                                    \
        } while (b);                                            \
}

TblElt
tblElt(RawTable t, TblKey k, TblElt notFound)
{
        Hash           h;
        int            x;
        struct TblSlot *b;
        TblHashFun     hfun = t->hashFun;
        TblEqFun       efun = t->eqFun;

        h = hfun ? hfun(k) : (Hash) ptrCanon(k);
        x = h % t->buckc;
        b = t->buckv[x];

        if (b) BUCKET_SEARCH(t,x,b,h,k,efun, return b->elt);

        return notFound;
}

TblElt
tblSetElt(RawTable t, TblKey k, TblElt e)
{
        Hash           h;
        int            x;
        struct TblSlot *b;
        TblHashFun     hfun = t->hashFun;
        TblEqFun       efun = t->eqFun;

        h = hfun ? hfun(k) : (Hash) ptrCanon(k);
        x = h % t->buckc;
        b = t->buckv[x];

        if (b) BUCKET_SEARCH(t,x,b,h,k,efun, return b->elt = e);

        b = (struct TblSlot *) stoAlloc((unsigned) OB_Other, sizeof(*b));
        b->key  = k;
        b->elt  = e;
        b->hash = h;
        b->next = t->buckv[x];
        t->buckv[x] = b;
        t->count++;

        if (t->count > TBL_MaxLoad * t->buckc) tblEnlarge(t);

        return e;
}


RawTable
tblDrop(RawTable t, TblKey k)
{
        Hash           h;
        int            x;
        struct TblSlot *b;
        TblHashFun     hfun = t->hashFun;
        TblEqFun       efun = t->eqFun;

        h = hfun ? hfun(k) : (Hash) ptrCanon(k);
        x = h % t->buckc;
        b = t->buckv[x];

        if (b) BUCKET_SEARCH(t,x,b,h,k,efun, {
                t->buckv[x] = b->next;
                t->count--;
                stoFree((void *) b);
                return t;
        });

        return t;
}

local void
tblEnlarge(RawTable t)
{
        struct TblSlot **nbuckv, *b, *hd;
        int            nbuckc, i, x;

        nbuckc = binPrime(cielLg(t->buckc)+1);
        nbuckv = (struct TblSlot **)
                stoAlloc((unsigned) OB_Other, nbuckc*sizeof(struct TblSlot*));
        for (i = 0; i < nbuckc; i++) nbuckv[i] = 0;

        for (i = 0; i < t->buckc; i++) {
                b = t->buckv[i];
                while (b) {
                        hd = b;
                        b  = b->next;

                        x  = hd->hash % nbuckc;
                        hd->next  = nbuckv[x];
                        nbuckv[x] = hd;
                }
        }
        stoFree((void *) t->buckv);
        t->buckc = nbuckc;
        t->buckv = nbuckv;
}

int
tblPrint(FILE *fout, RawTable t, TblPrKeyFun prk, TblPrEltFun pre)
{
        struct TblSlot  *b;
        int             cc, i, j;

        cc = fprintf(fout, "Table(");
        for (i = 0; i < t->buckc; i++) {
                cc += fprintf(fout, "[%d] ", i);
                for (j = 0, b = t->buckv[i]; b; j++, b = b->next) {
                        if (j > 0)
                                cc += fprintf(fout, ", ");
                        if (prk)
                                cc += prk(fout, b->key);
                        if (pre) {
                                cc += fprintf(fout, "=");
                                cc += pre(fout, b->elt);
                        }
                }
                cc += fprintf(fout, ";");
        }
        cc += fprintf(fout, ")");
        return cc;
}

#ifndef FOAM_RTS
int
tblColumnPrint(FILE *fout, RawTable t, TblPrKeyFun prk, TblPrEltFun pre)
{
        struct TblSlot  *b;
        int             cc, i;

        /* print table entries in a single column */

        cc = fnewline(fout);
        for (i = 0; i < t->buckc; i++) {
                for (b = t->buckv[i]; b; b = b->next) {
                        if (prk)
                                cc += prk(fout, b->key);
                        if (pre) {
                                if (prk)
                                    cc += fprintf(fout, "=");
                                cc += pre(fout, b->elt);
                                cc += fnewline(fout);
                        }
                }
        }
        return cc;
}

#endif

int
_tblITER(RawTableIterator *pit, RawTable t)
{
        pit->curr = t->buckv;
        pit->last = t->buckv + t->buckc - 1;
        pit->link = t->buckv[0];

        if (pit->link) return 1;

        /* Skip over initial empty buckets. */
        return _tblSTEP(pit);
}

int
_tblSTEP(RawTableIterator *pit)
{
        /* Skip to next non-empty bucket. */
        do {
                pit->curr++;
                if (pit->curr > pit->last) return 0;
        } while (!pit->curr[0]);

        pit->link = pit->curr[0];
        return 1;
}
