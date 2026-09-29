///////////////////////////////////////////////////////////////////////////////
//
// ablogic.cpp: Structures for inference about conditional exports.
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

#include "axlobs.h"

bool    ablogDebug              = false;
#define ablogDEBUG(s)           DEBUG_IF(ablogDebug, s)


/*****************************************************************************
 *
 * :: Conversion between internal and external types.
 *
 ****************************************************************************/

/*
 * The representation of this type is opaque --
 * all external access is through the fake type AbLogic
 * and all internal use is through the real type DNF.
 *
 * The ablogIn and ablogOut conversions are by local functions to
 * ensure typechecked, inexpensive coercions.
 *
 * This fire wall ensures that we do not use DNF values as AbLogic in error.
 * This is important, since we only allow aliased dnfs to be used in
 * free operations.
 */

local DNF       ablogIn (AbLogic xx) {
        return DNF::fromPointer(xx).follow();
}

local AbLogic   ablogOut(DNF     xx) {
        return (AbLogic) xx.asPointer();
}

#if 0
local void
ablogAlias(AbLogic old, AbLogic new_)
{
        DNF::fromPointer(old).alias(ablogIn(new_));
}
#endif
/*****************************************************************************
 *
 * :: Correspondence between Sefos and DNF_Atoms.
 *
 ****************************************************************************/

local Table<Sefo, AInt> ablogToTable;
local Table<AInt, Sefo> ablogFrTable;
local DNF_Atom  ablogNextIx;

local void
ablogInitTables(void)
{
        ablogToTable = Table<Sefo, AInt>::create<abHash, sefoEqual>();
        ablogFrTable = Table<AInt, Sefo>::create();
        ablogNextIx  = 1;
}

local void
ablogFiniTables(void)
{
        ablogToTable.freeDeeply<abFree, nullptr>();
        ablogFrTable.free();
}

local DNF_Atom
ablogToAtom(Sefo sefo)
{
        AInt x = ablogToTable.get(sefo, (AInt) 0);
        if (!x) {
                sefo = sefoCopy(sefo);
#if EDIT_1_0_n1_07
                ablogToTable.set(sefo, (AInt) ablogNextIx);
                ablogFrTable.set((AInt) ablogNextIx, sefo);
#else
                ablogToTable.set(sefo, (AInt) ablogNextIx);
                ablogFrTable.set((AInt) ablogNextIx, sefo);
#endif
                x    = ablogNextIx++;
        }
        return x;
}

local Sefo
ablogFrAtom(DNF_Atom lit)
{
        Sefo    sefo;

        if (lit < 0) lit = -lit;

#if EDIT_1_0_n1_07
        sefo = ablogFrTable.get((AInt) lit, nullptr);
#else
        sefo = ablogFrTable.get((AInt) lit, nullptr);
#endif

        return sefo;
}


/*****************************************************************************
 *
 * :: Initialization and finalization.
 *
 ****************************************************************************/

local bool      ablogIsInit = false;

void
ablogInit(void)
{
        if (ablogIsInit) return;

        ablogInitTables();

        ablogIsInit = true;
}

void
ablogFini(void)
{
        if (!ablogIsInit) return;

        ablogFiniTables();

        ablogIsInit = false;
}


/*****************************************************************************
 *
 * :: General operations.
 *
 ****************************************************************************/

AbLogic
ablogCopy(AbLogic xx)
{
        if (!xx || ablogIsTrue(xx) || ablogIsFalse(xx)) return xx;
        return ablogOut(ablogIn(xx).copy());
}

void
ablogFree(AbLogic xx)
{
        if (!xx || ablogIsTrue(xx) || ablogIsFalse(xx)) return;
        ablogIn(xx).free();
}

int
ablogPrintDb(AbLogic xx0)
{
        int cc = ablogPrint(dbOut, xx0);
        fnewline(dbOut);
        return cc+1;
}

int
ablogPrint(FILE *fout, AbLogic xx0)
{
        DNF      xx = ablogIn(xx0);
        int      i, j, cc = 0;
        Sefo     xxij;
        DNF_Atom atom;

        if (xx.isFalse())
                cc += fprintf(fout, ".FALSE.");

        for (i = 0; i < xx.termCount(); i += 1) {
                if (i > 0) cc += fprintf(fout, " .OR. ");

                if (xx.atomCount(i) == 0)
                        cc += fprintf(fout, ".TRUE.");

                for (j = 0; j < xx.atomCount(i); j += 1) {
                        atom = xx.atomAt(i, j);
                        if (j > 0) cc += fprintf(fout, ".AND.");
                        if (atom < 0)
                                cc += fprintf(fout, ".NOT.");
                        xxij = ablogFrAtom(atom);
                        cc  += sefoPrint(fout, xxij);
                }
        }

        return cc;
}

/******************************************************************************
 *
 * :: Maintaining an And-stack.
 *
 *****************************************************************************/

AbLogic   abCondKnown = NULL;     /* Conditions with known value (tinfer) */
AbLogic   gfCondKnown = NULL;     /* Ditto (genfoam) */

void
ablogAndPush(AbLogic *glo, AbLogic *save, Sefo cond, bool sense)
{
        AbLogic thisCond;

        /* Save the old value of *glo and compute the new one. */
        *save = *glo;

        thisCond = ablogFrSefo(cond);
        if (!sense) {
                AbLogic tt = thisCond;
                thisCond   = ablogNot(tt);
                ablogFree(tt);
        }

        *glo = ablogAnd(*save, thisCond);
        ablogFree(thisCond);

        ablogDEBUG({
                fprintf(dbOut, ">> Changed condition to ");
                ablogPrint(dbOut, *glo);
                fnewline(dbOut);
        });
}

void
ablogAndPop(AbLogic *glo, AbLogic *save)
{
        ablogFree(*glo);
        *glo = *save;

        ablogDEBUG({
                fprintf(dbOut, "<< Changed back\n");
        });
}


/******************************************************************************
 *
 * :: Conversion Sefo -> AbLogic
 *
 *****************************************************************************/

AbLogic
ablogFrSefo(Sefo sefo)
{
        AbLogic  rr, xx, yy;
        Length   i;

        switch (abTag(sefo)) {
        case AB_Not:
                xx = ablogFrSefo(sefo->abNot.expr);
                rr = ablogNot(xx);
                ablogFree(xx);
                break;
        case AB_And:
                if (abArgc(sefo) == 0)
                        rr = ablogTrue();
                else {
                        rr = ablogFrSefo(sefo->abAnd.argv[0]);
                        for (i = 1; i < abArgc(sefo); i++) {
                                xx = rr;
                                yy = ablogFrSefo(sefo->abAnd.argv[i]);
                                rr = ablogAnd(xx, yy);
                                ablogFree(xx);
                                ablogFree(yy);
                        }
                }
                break;
        case AB_Or:
                if (abArgc(sefo) == 0)
                        return ablogFalse();
                else {
                        rr = ablogFrSefo(sefo->abOr.argv[0]);
                        for (i = 1; i < abArgc(sefo); i++) {
                                xx = rr;
                                yy = ablogFrSefo(sefo->abOr.argv[i]);
                                rr = ablogOr(xx, yy);
                                ablogFree(xx);
                                ablogFree(yy);
                        }
                }
                break;
        case AB_Test:
                rr = ablogFrSefo(sefo->abTest.cond);
                break;
        default:
                rr = ablogOut(DNF::atom(ablogToAtom(sefo)));
                break;
        }
        return rr;
}


/*****************************************************************************
 *
 * :: Boolean arithmetic operations.
 *
 ****************************************************************************/

AbLogic
ablogTrue(void)
{
        return ablogOut(DNF::trueForm());
}

AbLogic
ablogFalse(void)
{
        return ablogOut(DNF::falseForm());
}

AbLogic
ablogOr(AbLogic xx, AbLogic yy)
{
        return ablogOut(ablogIn(xx).disjoin(ablogIn(yy)));
}

AbLogic
ablogAnd(AbLogic xx, AbLogic yy)
{
        return ablogOut(ablogIn(xx).conjoin(ablogIn(yy)));
}

AbLogic
ablogNot(AbLogic xx)
{
        return ablogOut(ablogIn(xx).negate());
}

bool
ablogIsTrue(AbLogic xx)
{
        return ablogIn(xx).isTrue();
}

bool
ablogIsFalse(AbLogic xx)
{
        return ablogIn(xx).isFalse();
}

bool
ablogEqual(AbLogic xx, AbLogic yy)
{
        return ablogIn(xx).equals(ablogIn(yy));
}

/******************************************************************************
 *
 * :: Test whether given forms are implied.
 *
 *****************************************************************************/

/*
 * ToDo: ablogIsListImplied should use ablogImplies.
 * ablogImplies should do some form of result caching.
 */
extern TForm    tiGetTForm              (Stab, AbSyn);

local bool ablogIsListImpliedInner(AbLogic, List<Sefo>, AbLogic *);
local bool ablogExpandKnown       (AbLogic known, Sefo sefo, AbLogic *final);
local bool ablogAtomize           (void *, DNF_Atom);
local bool ablogTestAtoms         (void *, DNF_Atom);
local bool ablogTestProperties    (Sefo test, Sefo know);
local bool ablogIsListImplied0    (AbLogic xx, List<Sefo> sefolist);
local bool ablogTestImplies       (void *clos, DNF_Atom a, DNF_Atom b);

typedef struct ablogExpandClos {
        AbLogic   new_;
        int       atomc;
        int       lim;
        DNF_Atom *atomv;
        DNF_Atom  atom;
} _AbLogExpandClos, *AbLogExpandClos;

typedef struct ablogImpliesClos {
        int cache;      /* Unused.  Need to think about
                         * good caching schemes.
                         */
} _AblogImpliesClos, *AblogImpliesClos;

bool
ablogImplies(AbLogic known, AbLogic query)
{
        _AblogImpliesClos clos;
        bool result;

        if (ablogIn(known).implies(ablogIn(query)))
                return true;

        result = ablogIn(known).expandImplies(ablogTestImplies, &clos,
                                             ablogIn(query));

        return result;
}

/* Test for a => b */
local bool
ablogTestImplies(void *clos, DNF_Atom a, DNF_Atom b)
{
        Sefo know, test;
        bool result = false;

        if (a < 0) {
                int t = a;
                a = -b;
                b = -t;
        }
        /* Can't do negatives yet */
        if (a < 0 || b < 0) return false;

        know = ablogFrAtom(a);
        test = ablogFrAtom(b);

        ablogDEBUG(printf("Implies test\n");
                   abPrintDb(know);
                   abPrintDb(test);
                   );
        if (abTag(test) != AB_Has || abTag(know) != AB_Has)
                return false;

        if (sefoEqual(test->abHas.expr, know->abHas.expr))
                result = ablogTestProperties(test, know);

        /* if result, we should stash the result someplace. */
        return result;
}

bool
ablogIsListKnown(List<Sefo> sefolist)
{
        AbLogic new_  = ablogCopy(abCondKnown);
        bool res;
        res = ablogIsListImpliedInner(abCondKnown, sefolist, &new_);
        return res;
}

bool
ablogIsListImplied(AbLogic xx, List<Sefo> sl)
{
        AbLogic final = ablogCopy(xx);
        bool    result;
        result = ablogIsListImpliedInner(xx, sl, &final);
        return result;
}

local bool
ablogIsListImpliedInner(AbLogic xx, List<Sefo> sefolist, AbLogic *final)
{
        List<Sefo> sl;

        if (ablogIsListImplied0(xx, sefolist))
                return true;

        sl = sefolist;
        while (sl != listNil<Sefo>) {
                if (ablogExpandKnown(xx, car(sl), final)
                    && ablogIsListImplied0(*final, sefolist))
                        return true;
                sl = cdr(sl);
        }
        return false;
}



local bool
ablogIsListImplied0(AbLogic xx, List<Sefo> sefolist)
{
        for ( ; sefolist; sefolist = cdr(sefolist)) {
                Sefo sefo = car(sefolist);
                if (!ablogIsImplied(xx, sefo)) return false;
        }

        return true;
}

bool
ablogIsImplied(AbLogic xx, Sefo sefo)
{
        AbLogic rhs = ablogFrSefo(sefo);

        if (ablogIn(xx).implies(ablogIn(rhs)))
                return true;

        return false;
}

local bool
ablogExpandKnown(AbLogic known, Sefo sefo, AbLogic *final)
{
        AbLogic rhs;
        _AbLogExpandClos clos;
        bool res;
        int  i;

        /* Find all the atoms in rhs */
        rhs = ablogFrSefo(sefo);

        clos.new_ = ablogTrue();
        clos.atomc = 0;
        clos.atomv = NULL;

        ablogIn(rhs).map(ablogAtomize, (void *) &clos);

        for (i=0; i<clos.atomc; i++) {
                clos.atom = clos.atomv[i];
                ablogIn(known).map(ablogTestAtoms, (void *) &clos);
        }

        *final = ablogAnd(known, clos.new_);
        res = !ablogEqual(clos.new_, ablogTrue());
        ablogFree(clos.new_);
        if (clos.atomv) stoFree(clos.atomv);

        return res;
}

local bool
ablogAtomize(void *ptr, DNF_Atom atom)
{
        AbLogExpandClos clos = (AbLogExpandClos) ptr;

        if (clos->atomv == NULL) {
                clos->atomv = (DNF_Atom *) stoAlloc(OB_Other, 5*sizeof(DNF_Atom));
                clos->lim = 5;
        }

        /* Grow slowly */
        if (clos->atomc == clos->lim) {
                clos->atomv = (DNF_Atom*)
                        stoResize(clos->atomv,
                                  (clos->lim + 5)*sizeof(DNF_Atom));
                clos->lim += 5;
        }
        clos->atomv[clos->atomc++] = atom;
        return false; /* false => dnfMap continues */
}


local bool
ablogTestAtoms(void *ptr, DNF_Atom known)
{
        AbLogExpandClos clos = (AbLogExpandClos) ptr;
        Sefo  test = ablogFrAtom(clos->atom);
        Sefo  know = ablogFrAtom(known);
        bool  result = false;

        if (abTag(test) != AB_Has || abTag(know) != AB_Has)
                return false;

        /*
         * This test is a bit strong,
         * eg. if A then Foo(A)
         */
        if (sefoEqual(test->abHas.expr, know->abHas.expr))
                result = ablogTestProperties(test, know);
        if (result) {
                clos->new_ = ablogAnd(clos->new_, ablogOut(DNF::atom(clos->atom)));
                return true;
        }
        return false;
}

local bool
ablogTestProperties(Sefo test, Sefo know)
{
        TForm tftest, tfknown;
        bool  result;
        test = test->abHas.property;
        know = know->abHas.property;

        tftest  = abTForm(test) ?
                abTForm(test) : tiGetTForm(stabFile(), test);
        tfknown = abTForm(know) ?
                abTForm(know) : tiGetTForm(stabFile(), know);

        ablogDEBUG({
                fprintf(dbOut, "Checking: \n");
                tfPrintDb(tftest);
                tfPrintDb(tfknown);
        });

        result = tfSatBit(tfSatHasMask(), tfknown, tftest);
        return result;
}

int
bputAblog(Buffer buf, AbLogic abl)
{
        /* Hacked from ablogPrint() */
        DNF      xx = ablogIn(abl);
        int      i, j, cc = 0;
        Sefo     xxij;
        DNF_Atom atom;

        if (xx.isFalse())
                cc += buf.printf("false");

        for (i = 0; i < xx.termCount(); i += 1) {
                if (i > 0) cc += buf.printf(" \\/ ");

                if (xx.atomCount(i) == 0)
                        cc += buf.printf("true");

                for (j = 0; j < xx.atomCount(i); j += 1) {
                        atom = xx.atomAt(i, j);
                        if (j > 0) cc += buf.printf(" /\\ ");
                        if (atom < 0)
                                cc += buf.printf("not ");
                        xxij = ablogFrAtom(atom);
                        cc  += buf.printf("%s", abPretty(xxij));
                }
        }

        return cc;
}
