///////////////////////////////////////////////////////////////////////////////
//
// absub.cpp: Semantic substitutions.
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

# include "axlobs.h"

bool    absDebug        = false;
#define absDEBUG(s)     DEBUG_IF(absDebug, s)

/******************************************************************************
 *
 * :: Substitution bindings.
 *
 *****************************************************************************/

typedef void    (*AbsFreeBindingFun)    (AbBind);

local AbBind    absNewBinding           (Syme, Sefo);
local void      absFreeBinding          (AbBind);
local void      absFreeBindingDeeply    (AbBind);
local int       absPrintBinding         (FILE *, AbBind);

local AbBind
absNewBinding(Syme key, Sefo val)
{
        AbBind  b;

        b       = (AbBind) stoAlloc((unsigned) OB_AbBind, sizeof(*b));
        b->key  = key;
        b->val  = val;

        return b;
}

local void
absFreeBinding(AbBind b)
{
        stoFree((void *) b);
        return;
}

local void
absFreeBindingDeeply(AbBind b)
{
        abFree(b->val);
        absFreeBinding(b);
}

local int
absPrintBinding(FILE *fout, AbBind b)
{
        int     cc = 0;

        cc += fprintf(fout, "{ ");
        /* Also useful:  cc += symePrint(fout, b->key); */
        cc += fprintf(fout, "%s", symeString(b->key));
        cc += fprintf(fout, " +-> ");
        cc += abPrettyPrint(fout, b->val);
        cc += fprintf(fout, " }\n");

        return cc;
}

/******************************************************************************
 *
 * :: Substitution lists.
 *
 *****************************************************************************/

static int      absSerialNo     = 0;
static Syme     absSymeMark     = (Syme) (0xFFFFFFFF);

AbSub
absNew(Stab stab)
{
        AbSub   sigma;

        sigma   = (AbSub) stoAlloc((unsigned) OB_Other, sizeof(*sigma));

        sigma->stab     = stab;
        sigma->self     = false;
        sigma->lazy     = true;
        sigma->serialNo = absSerialNo++;
        sigma->refc     = 1;
        sigma->l        = listNil<AbBind>;
        sigma->results = Table<void *, void *>::create();
        sigma->fv       = FreeVar{};

        return sigma;
}

AbSub
absFail(void)
{
        return (AbSub) 0;
}

int
absPrint(FILE *fout, AbSub sigma)
{
        if (sigma == absFail())
                return fprintf(fout, "[ FAIL ]");
        else
                return listPrint<AbBind>(fout, sigma->l, absPrintBinding);
}

int
absPrintDb(AbSub sigma)
{
        int     rc = absPrint(dbOut, sigma);
        fnewline(dbOut);
        return rc;
}

AbSub
absRefer(AbSub sigma)
{
        ++sigma->refc;
        return sigma;
}

local void
absFreeDeeply0(AbSub sigma, AbsFreeBindingFun f)
{
        if (sigma == absFail()) return;
        if (--sigma->refc > 0) return;

        listFreeDeeply<AbBind>(sigma->l, f);
        sigma->results.free();
        /*!! Could free sigma->fv. */

        stoFree((void *) sigma);
        return;
}

void
absFree(AbSub sigma)
{
        absFreeDeeply0(sigma, absFreeBinding);
        return;
}

void
absFreeDeeply(AbSub sigma)
{
        absFreeDeeply0(sigma, absFreeBindingDeeply);
        return;
}

bool
absIsEmpty(AbSub sigma)
{
        return sigma->l == listNil<AbBind>;
}

bool
absHasSymes(AbSub sigma, List<Syme> symes)
{
        if (absIsEmpty(sigma))
                return false;

        for (; symes; symes = cdr(symes)) {
                Syme    syme = car(symes), final;
                if (absLookup(syme, NULL, sigma))
                        return true;
                final = absGetSyme(sigma, syme);
                if (final && final != syme)
                        return true;
        }

        return false;
}

AbSub
absExtend(Syme syme, Sefo ab, AbSub sigma)
{
        assert(syme);
        if (sigma == absFail()) return sigma;

        sigma->l = listCons<AbBind>(absNewBinding(syme, ab), sigma->l);
        if (symeIsParam(syme)) {
                List<Syme>        symes;
                for (symes = symeTwins(syme); symes; symes = cdr(symes)) {
                        Syme    twin = car(symes);
                        if (twin != syme) absExtend(twin, sefoCopy(ab), sigma);
                }
        }
        return sigma;
}

Sefo
absLookup(Syme syme, Sefo fail, AbSub sigma)
{
        List<AbBind>      l0;

        for (l0 = sigma->l; l0; l0 = cdr(l0))
                /** if (syme == car(l0)->key) **/ /** commented by C.O. for ALMA usage**/
                    if (symeEqual(syme, car(l0)->key)) /** code introduced by C.O. for ALMA usage**/
                        return car(l0)->val;

        return fail;
}

local void *
absSet(AbSub sigma, void *p, void *q)
{
        return sigma->results.set(p, q);
}

local void *
absGet(AbSub sigma, void *p)
{
        return sigma->results.get(p, nullptr);
}

void
absSetStab(AbSub sigma, Syme syme)
{
        if (symeLib(syme) == NULL && stabLevelIsSubstable(absStab(sigma))) {
                symeSetDefLevel(syme, car(absStab(sigma)));
                symeSetHash(syme, (Hash) 0);
        }
}

Syme
absSetSyme(AbSub sigma, Syme p, Syme q)
{
        return (Syme) absSet(sigma, (void *) p, (void *) q);
}

Syme
absGetSyme(AbSub sigma, Syme p)
{
        return (Syme) absGet(sigma, (void *) p);
}

Syme
absMarkSyme(AbSub sigma, Syme p)
{
        absSetSyme(sigma, p, absSymeMark);
        return p;
}

bool
absSymeIsMarked(AbSub sigma, Syme p)
{
        return absGetSyme(sigma, p) == absSymeMark;
}

TForm
absSetTForm(AbSub sigma, TForm p, TForm q)
{
        return (TForm) absSet(sigma, (void *) p, (void *) q);
}

TForm
absGetTForm(AbSub sigma, TForm p)
{
        return (TForm) absGet(sigma, (void *) p);
}
