///////////////////////////////////////////////////////////////////////////////
//
// dnf.cpp: Disjunctive normal form for boolean expressions.
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

#include "axlgen.h"

bool    dnfDebug        = false;
#define dnfDEBUG(s)     DEBUG_IF(dnfDebug, s)

struct DNF_AndRep {
        Length          argc;
        DNF_Atom        argv[NARY];
};
using DNF_And = DNF_AndRep *;

struct DNF_Rep {
        int             argc;
        DNF_And         argv[NARY];
};

struct DNF_Access {
        static DNF_Rep *rep(DNF dnf) noexcept { return dnf.rep_; }
        static DNF make(DNF_Rep *rep) noexcept { return DNF(rep); }
};

/*****************************************************************************
 *
 * :: Forward declarations for local functions.
 *
 ****************************************************************************/

local bool      dnfAtomLT               (DNF_Atom, DNF_Atom);

local DNF_And   dnfAndNew               (int argc);
local DNF_And   dnfAndCopy              (DNF_And);
local void      dnfAndFree              (DNF_And);
local bool      dnfAndIsTrue            (DNF_And);
local DNF_And   dnfAndMerge             (DNF_And, DNF_And);
local bool      dnfAndImplies           (DNF_And, DNF_And);
local DNF       dnfAndNot               (DNF_And);

local DNF       dnfOrNew                (int argc);
local DNF       dnfOrCopy               (DNF);
local void      dnfOrFree               (DNF);
local bool      dnfOrIsFalse            (DNF);
local void      dnfOrMerge              (DNF);

/*****************************************************************************
 *
 * :: Atoms
 *
 ****************************************************************************/

local bool
dnfAtomLT(DNF_Atom l1, DNF_Atom l2)
{
        if (l1 < 0) l1 = -l1;
        if (l2 < 0) l2 = -l2;
        return l1 < l2;
}

/*****************************************************************************
 *
 * :: Conjunctions
 *
 ****************************************************************************/

local DNF_And
dnfAndNew(int argc)
{
        int     i;
        DNF_And xx;

        xx = (DNF_And) stoAlloc(OB_DNF, fullsizeof(*xx, argc, DNF_Atom));

        xx->argc = argc;
        for (i = 0; i < argc; i += 1)
                xx->argv[i] = 0;

        dnfDEBUG(fprintf(dbOut, ">dnfAndNew: %p[%d]\n", (void *) xx, argc));

        return xx;
}

local DNF_And
dnfAndCopy(DNF_And xx)
{
        Length  i;
        DNF_And yy;

        dnfDEBUG(fprintf(dbOut, ">dnfAndCopy: %p[%d]\n",
                         (void *) xx, (int) xx->argc));

        yy = dnfAndNew(xx->argc);
        for (i = 0; i < xx->argc; i += 1)
                yy->argv[i] = xx->argv[i];

        return yy;
}

local void
dnfAndFree(DNF_And xx)
{
        dnfDEBUG(fprintf(dbOut, ">dnfAndFree: %p[%d]\n",
                         (void *) xx, (int) xx->argc));
        stoFree((void *) xx);
}

local bool
dnfAndIsTrue(DNF_And xx)
{
        return xx->argc == 0;
}

/*
 * Combine xx and yy into a new conjunction.
 * Remove redundant literals along the way.
 * Return NULL if a literal and its negation are found.
 */
local DNF_And
dnfAndMerge(DNF_And xx, DNF_And yy)
{
        DNF_And rr;
        Length  xxi, yyi, rri;

        if (dnfAndIsTrue(xx))
                return dnfAndCopy(yy);
        if (dnfAndIsTrue(yy))
                return dnfAndCopy(xx);

        rr = dnfAndNew(xx->argc + yy->argc);
        rri = xxi = yyi = 0;

        /* Merge, preserving order, removing redundant literals. */
        while (xxi < xx->argc && yyi < yy->argc) {
                if (dnfAtomLT(xx->argv[xxi], yy->argv[yyi]))
                        rr->argv[rri++] = xx->argv[xxi++];
                else if (dnfAtomLT(yy->argv[yyi], xx->argv[xxi]))
                        rr->argv[rri++] = yy->argv[yyi++];
                else if (xx->argv[xxi] == yy->argv[yyi]) {
                        xxi += 1;
                }
                else {
                        assert(xx->argv[xxi] == - yy->argv[yyi]);
                        dnfAndFree(rr);
                        return NULL;
                }
        }
        while (xxi < xx->argc)
                rr->argv[rri++] = xx->argv[xxi++];
        while (yyi < yy->argc)
                rr->argv[rri++] = yy->argv[yyi++];
        rr->argc = rri;

        return rr;
}

/*
 * This tests to see if xx implies yy.
 * If xx => yy then (xx or yy) == yy.
 */
local bool
dnfAndImplies(DNF_And xx, DNF_And yy)
{
        Length  xxi, yyi;

        /* xx implies yy if each atom in yy can be found in xx. */
        if (xx->argc < yy->argc)
                return false;

        xxi = yyi = 0;
        for (xxi = yyi = 0; xxi < xx->argc && yyi < yy->argc; ) {
                DNF_Atom xxa = xx->argv[xxi];
                DNF_Atom yya = yy->argv[yyi];

                if (dnfAtomLT(xxa, yya))
                        xxi += 1;       /* Keep looking for yyi. */

                else if (xxa == yya) {
                        xxi += 1;       /* Found yyi. */
                        yyi += 1;
                }
                else
                        return false;   /* Failed to find yyi. */
        }

        /* Return true if we found each of the yyi. */
        return yyi == yy->argc;
}

local DNF
dnfAndNot(DNF_And xx)
{
        DNF     rr = dnfOrNew(xx->argc);
        DNF_Rep *rrp = DNF_Access::rep(rr);
        Length  i;

        for (i = 0; i < xx->argc; i += 1) {
                rrp->argv[i] = dnfAndNew(1);
                rrp->argv[i]->argv[0] = - xx->argv[i];
        }

        return rr;
}

/*****************************************************************************
 *
 * :: Disjunctions
 *
 ****************************************************************************/

local DNF
dnfOrNew(int argc)
{
        int      i;
        DNF_Rep *xx;

        xx = (DNF_Rep *) stoAlloc(OB_DNF,
                                  fullsizeof(*xx, argc, DNF_And));

        xx->argc = argc;
        for (i = 0; i < argc; i += 1)
                xx->argv[i] = 0;

        dnfDEBUG(fprintf(dbOut, ">dnfOrNew: %p[%d]\n", (void *) xx, argc));

        return DNF_Access::make(xx);
}

local DNF
dnfOrCopy(DNF xx)
{
        int      i;
        DNF_Rep *xxp = DNF_Access::rep(xx);
        DNF      yy = dnfOrNew(xxp->argc);
        DNF_Rep *yyp = DNF_Access::rep(yy);

        dnfDEBUG(fprintf(dbOut, ">dnfOrCopy: %p[%d]\n",
                         (void *) xxp, xxp->argc));

        for (i = 0; i < xxp->argc; i += 1)
                yyp->argv[i] = dnfAndCopy(xxp->argv[i]);

        return yy;
}

local void
dnfOrFree(DNF xx)
{
        int      i;
        DNF_Rep *xxp = DNF_Access::rep(xx);

        dnfDEBUG(fprintf(dbOut, ">dnfOrFree: %p[%d]\n",
                         (void *) xxp, xxp->argc));

        for (i = 0; i < xxp->argc; i += 1)
                dnfAndFree(xxp->argv[i]);

        stoFree((void *) xxp);
}

local bool
dnfOrIsFalse(DNF xx)
{
        return DNF_Access::rep(xx)->argc == 0;
}

/*
 * Modify xx to merge redundant disjuncts.
 */
local void
dnfOrMerge(DNF xx)
{
        int      i, j;
        DNF_Rep *xxp = DNF_Access::rep(xx);

        dnfDEBUG(fprintf(dbOut, ">dnfOrMerge: %p[%d]\n",
                         (void *) xxp, xxp->argc));

        /* As we work, terms are merged by replacing them with NULL. */
        for (i = 0; i < xxp->argc; i += 1) {
                for (j = 0; j < xxp->argc; j += 1) {
                        /* If xxi => xxj then (xxi or xxj) == xxj. */
                        if (i != j && xxp->argv[i] && xxp->argv[j] &&
                            dnfAndImplies(xxp->argv[i], xxp->argv[j])) {
                                dnfAndFree(xxp->argv[i]);
                                xxp->argv[i] = NULL;
                        }
                }
        }

        /* Now squeeze out NULLs. */
        for (i = 0, j = 0; j < xxp->argc; j += 1)
                if (xxp->argv[j])
                        xxp->argv[i++] = xxp->argv[j];
        xxp->argc = i;

        dnfDEBUG(fprintf(dbOut, "<dnfOrMerge: %p[%d]\n",
                         (void *) xxp, xxp->argc));
}

/*****************************************************************************
 *
 * :: True and False.
 *
 ****************************************************************************/

local DNF_AndRep dnfTrueProd = {};
local DNF_Rep dnfTrueStruct = { 1, { &dnfTrueProd }};
local DNF_Rep dnfFalseStruct = {};

DNF
DNF::trueForm() noexcept
{
        return DNF_Access::make(&dnfTrueStruct);
}

DNF
DNF::falseForm() noexcept
{
        return DNF_Access::make(&dnfFalseStruct);
}

bool
DNF::isTrue() const noexcept
{
        return rep_->argc == 1 && dnfAndIsTrue(rep_->argv[0]);
}

bool
DNF::isFalse() const noexcept
{
        return dnfOrIsFalse(*this);
}

/*****************************************************************************
 *
 * :: Atoms
 *
 ****************************************************************************/

DNF
DNF::atom(DNF_Atom atom)
{
        DNF      xx = dnfOrNew(1);
        DNF_Rep *xxp = DNF_Access::rep(xx);

        dnfDEBUG(fprintf(dbOut, ">dnfAtom: %d\n", atom));

        xxp->argv[0] = dnfAndNew(1);
        xxp->argv[0]->argv[0] = atom;

        return xx;
}

/*****************************************************************************
 *
 * :: Or
 *
 ****************************************************************************/

DNF
DNF::disjoin(DNF yy) const
{
        DNF      rr;
        int      i, rri;
        DNF_Rep *xxp = rep_;
        DNF_Rep *yyp = DNF_Access::rep(yy);

        if (isTrue() || yy.isTrue())
                return trueForm();

        if (isFalse())
                return yy.copy();
        if (yy.isFalse())
                return copy();

        dnfDEBUG(fprintf(dbOut, ">dnfOr: %p[%d] %p[%d]\n",
                         (void *) xxp, xxp->argc,
                         (void *) yyp, yyp->argc));

        rr = dnfOrNew(xxp->argc + yyp->argc);
        DNF_Rep *rrp = DNF_Access::rep(rr);
        rri = 0;

        for (i = 0; i < xxp->argc; i += 1)
                rrp->argv[rri++] = dnfAndCopy(xxp->argv[i]);
        for (i = 0; i < yyp->argc; i += 1)
                rrp->argv[rri++] = dnfAndCopy(yyp->argv[i]);

        dnfOrMerge(rr);

        dnfDEBUG(fprintf(dbOut, "<dnfOr: %p[%d]\n",
                         (void *) rrp, rrp->argc));

        return rr;
}

/*****************************************************************************
 *
 * :: And
 *
 ****************************************************************************/

DNF
DNF::conjoin(DNF yy) const
{
        DNF      rr;
        int      i, j, rri;
        DNF_Rep *xxp = rep_;
        DNF_Rep *yyp = DNF_Access::rep(yy);

        if (isFalse() || yy.isFalse())
                return falseForm();

        if (isTrue())
                return yy.copy();
        if (yy.isTrue())
                return copy();

        dnfDEBUG(fprintf(dbOut, ">dnfAnd: %p[%d] %p[%d]\n",
                         (void *) xxp, xxp->argc,
                         (void *) yyp, yyp->argc));

        rr = dnfOrNew(xxp->argc * yyp->argc);
        DNF_Rep *rrp = DNF_Access::rep(rr);
        rri = 0;

        for (i = 0; i < xxp->argc; i += 1)
                for (j = 0; j < yyp->argc; j += 1, rri += 1)
                        rrp->argv[rri] =
                                dnfAndMerge(xxp->argv[i], yyp->argv[j]);

        dnfOrMerge(rr);

        dnfDEBUG(fprintf(dbOut, "<dnfAnd: %p[%d]\n",
                         (void *) rrp, rrp->argc));

        return rr;
}

/*****************************************************************************
 *
 * :: Not
 *
 ****************************************************************************/

DNF
DNF::negate() const
{
        DNF     aa, bb, rr;
        int     i;

        if (isFalse())
                return trueForm();
        if (isTrue())
                return falseForm();

        dnfDEBUG(fprintf(dbOut, ">dnfNot: %p[%d]\n",
                         (void *) rep_, rep_->argc));

        rr = trueForm();
        for (i = 0; i < rep_->argc; i += 1) {
                aa = rr;
                bb = dnfAndNot(rep_->argv[i]);

                rr = aa.conjoin(bb);
                aa.free();
                bb.free();
        }

        dnfDEBUG(fprintf(dbOut, "<dnfNot: %p[%d]\n",
                         rr.asPointer(), DNF_Access::rep(rr)->argc));

        return rr;
}

/*****************************************************************************
 *
 * :: General operations.
 *
 ****************************************************************************/

DNF
DNF::copy() const
{
        return dnfOrCopy(*this);
}

void
DNF::free() const
{
        if (rep_->argc == -1)
                stoFree(rep_);

        if (rep_ != &dnfTrueStruct && rep_ != &dnfFalseStruct)
                dnfOrFree(*this);
}

int
DNF::print(FILE *fout) const
{
        DNF_And xxi;
        int     i, cc = 0;
        Length  j;

        cc = fprintf(fout, "DNF{");
        for (i = 0; i < rep_->argc; i += 1) {
                xxi = rep_->argv[i];
                if (i > 0) cc += fprintf(fout, " ");
                cc += fprintf(fout, "[");
                for (j = 0; j < xxi->argc; j += 1) {
                        if (j > 0) cc += fprintf(fout, " ");
                        cc += fprintf(fout, "%d", xxi->argv[j]);
                }
                cc += fprintf(fout, "]");
        }
        cc += fprintf(fout, "}");
        return cc;
}

/*****************************************************************************/
/*
 * :: Equal
 *
 *****************************************************************************/

bool
DNF::equals(DNF yy) const
{
        if (*this == yy) return true;
        return implies(yy) && yy.implies(*this);
}

/*
 * xx => yy if each disjunct in xx implies a disjunct in yy.
 */
bool
DNF::implies(DNF yy) const
{
        bool     result = true;
        int      i, j;
        DNF_Rep *yyp = DNF_Access::rep(yy);

        for (i = 0; result && i < rep_->argc; i += 1) {
                result = false;
                for (j = 0; !result && j < yyp->argc; j += 1)
                        result = dnfAndImplies(rep_->argv[i], yyp->argv[j]);
        }

        return result;
}

/*****************************************************************************/
/*
 * :: Mapping
 *
 *****************************************************************************/
local bool
dnfExpandAndImplies(bool (*testFn)(void *, DNF_Atom, DNF_Atom),
                    void *clos, DNF_And xx, DNF_And yy);

void
DNF::map(bool (*mapFn)(void *, DNF_Atom), void *clos) const
{
        DNF_And xxi;
        int    i;
        Length j;

        for (i = 0; i < rep_->argc; i++) {
                xxi = rep_->argv[i];
                for (j = 0; j < xxi->argc; j++)
                        if (mapFn(clos, xxi->argv[j]))
                                return;
        }
}

bool
DNF::expandImplies(bool (*testFn)(void *, DNF_Atom, DNF_Atom),
                   void *clos, DNF yy) const
{
        bool     result = true;
        int      i, j;
        DNF_Rep *yyp = DNF_Access::rep(yy);

        for (i = 0; result && i < rep_->argc; i += 1) {
                result = false;
                for (j = 0; !result && j < yyp->argc; j += 1)
                        result = dnfExpandAndImplies(testFn, clos,
                                                    rep_->argv[i],
                                                    yyp->argv[j]);
        }

        return result;
}

local bool
dnfExpandAndImplies(bool (*testFn)(void *, DNF_Atom, DNF_Atom),
                    void *clos, DNF_And xx, DNF_And yy)
{
        Length  xxi, yyi;

        /* xx implies yy if each atom in yy can be found in xx. */
        if (xx->argc < yy->argc)
                return false;

        xxi = yyi = 0;
        for (xxi = yyi = 0; xxi < xx->argc && yyi < yy->argc; ) {
                DNF_Atom xxa = xx->argv[xxi];
                DNF_Atom yya = yy->argv[yyi];

                if (dnfAtomLT(xxa, yya))
                        xxi += 1;       /* Keep looking for yyi. */

                else if (xxa == yya) {
                        xxi += 1;       /* Found yyi. */
                        yyi += 1;
                }
                else {
                        /* Be a bit more enthusiastic */
                        bool res = false;
                        Length ti;
                        for (ti = 0; (!res) && ti < xx->argc; ti++) {
                                xxa = xx->argv[ti];
                                res = (*testFn)(clos, xxa, yya);
                        }
                        if (!res)
                                return false;   /* Failed to find yyi. */
                        yyi++;
                }
        }

        /* Return true if we found each of the yyi. */
        return yyi == yy->argc;
}

/*****************************************************************************/
/*
 * :: Aliasing
 * !!! This is currently broken, so don't try to use it.
 *****************************************************************************/

DNF
DNF::follow() const noexcept
{
#if 0
        if (rep_->argc != -1)
                return *this;
        else {
                rep_->argv[0] = DNF_Access::rep(
                        DNF_Access::make((DNF_Rep *) rep_->argv[0]).follow());
                return DNF_Access::make((DNF_Rep *) rep_->argv[0]);
        }
#endif
        return *this;
}

void
DNF::alias(DNF newValue) const
{
        int i;
        /* If we start wanting to alias false (unlikely),
         * then fix min. alloc to be 1 slot.
         */
        if (rep_->argc != -1) {
                assert(rep_->argc != 0);
                for (i = 0; i < rep_->argc; i++)
                        dnfAndFree(rep_->argv[i]);
                rep_->argc = -1;
        }
        rep_->argv[0] = (DNF_And) DNF_Access::rep(newValue);
}

/*****************************************************************************/
/*
 * :: Read-only structural access.
 *****************************************************************************/

int
DNF::termCount() const noexcept
{
        return rep_->argc;
}

int
DNF::atomCount(int term) const noexcept
{
        return rep_->argv[term]->argc;
}

DNF_Atom
DNF::atomAt(int term, int atom) const noexcept
{
        return rep_->argv[term]->argv[atom];
}
