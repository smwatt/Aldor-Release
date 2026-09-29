///////////////////////////////////////////////////////////////////////////////
//
// tposs.cpp: Type possibility sets.
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

/*
 * The bottom-up pass of type inference constructs a set of possible types
 * for each node.  These definitions allow us to change the representation
 * of the type set.
 */

#include "axlobs.h"
#include "terror.h"

/*
 *   Each node is given a set of possible meanings.
 *
 * An idea for fast type inference:
 *
 *   Rather than having a *list* of possible types at each node,
 *   it would be more compact to have a vector of all possible types
 *   in a given scope, and to represent the possible types as a bit vector.
 *
 *   For the Fall 1990 implementation, I will use lists.  But I'll
 *   write the code so that this idea could be used.    -- SMW
 */

#define tpossAlloc()    ((TPoss) stoAlloc(OB_TPoss, sizeof(struct tposs)))

TPoss
tpossEmpty(void)
{
        TPoss tp  = tpossAlloc();
        tp->possl = listNil<TForm>;
        tp->possc = 0;
        tp->refc  = 1;
        return tp;
}

local void
tpossCons(TPoss tp, TForm t)
{
        assert(tp);
        t = tfFollowOnly(t);
        tp->possl = listCons<TForm>(t, tp->possl);
        tp->possc += 1;
}

TPoss
tpossFrTheList(List<TForm> l)
{
        TPoss tp  = tpossAlloc();
        tp->possl = l;
        tp->possc = listLength<TForm>(l);
        tp->refc  = 1;
        return tp;
}

TPoss
tpossSingleton(TForm t)
{
        TPoss tp  = tpossEmpty();
        tpossCons(tp, t);
        return tp;
}

TPoss
tpossFrSymes(List<Syme> symes)
{
        TPoss   tp = tpossEmpty();
        for (; symes; symes = cdr(symes))
        {
/*
                if (car(symes)->type != NULL)
                {
*/
                        tpossAdd1(tp, symeType(car(symes)));
/*
                }
*/
        }
        return tp;
}

TPoss
tpossDeclare(Syme syme, TPoss tp)
{
        TPoss           ntp = tpossEmpty();
        TPossIterator   it;

        for (tpossITER(it, tp); tpossMORE(it); tpossSTEP(it)) {
                TForm   tf = tpossELT(it);
                tf = tfDeclare(abFrSyme(syme), tf);
                tpossAdd1(ntp, tf);
        }

        return ntp;
}

/*
 * tpossMulti(n,v,f) takes the n TPoss sets  f(v,0)...f(v,n-1) and forms
 *   their cross product.
 *!! This will get very expensive if the f(v,i) have many elements so it may
 *!! become necessary to use a symbolic tree form for TPoss sets.
 */

/*
 * tl is one element of the cross product get(v,k)...get(v,n-1)
 */
local void
tposs0Multi(TPoss tp,Length k,List<TForm> tl,Length n,void * v,TPossGetter get)
{
        if (k == 0)
                tpossCons(tp, tfMultiFrList(tl));

        else {
                TPoss           tpk = get(v, k-1);
                TPossIterator   tpi;

                tl = listCons<TForm>(NULL, tl);
                for (tpossITER(tpi,tpk); tpossMORE(tpi); tpossSTEP(tpi)) {
                        TForm t = tpossELT(tpi);
                        t = tfFollowOnly(t);
                        setcar(tl, t);
                        tposs0Multi(tp, k-1, tl, n, v, get);
                }
                listFreeCons<TForm>(tl);
                tpossFree(tpk);
        }
}

TPoss
tpossMulti(Length argc, void * argv, TPossGetter get)
{
        TPoss   tp = tpossEmpty();

        tposs0Multi(tp, argc, listNil<TForm>, argc, argv, get);

        return tp;
}

TPoss
tpossAdd1(TPoss tp, TForm t)
{
        t = tfFollowOnly(t);
        if (! tpossHas(tp, t))
                tpossCons(tp, t);
        return tp;
}

TPoss
tpossRefer(TPoss tp)
{
        if (tp != NULL)
                ++tp->refc;
        return tp;
}

TPoss
tpossCopy(TPoss tp)
{
        TPoss np;

        if (tp == NULL)
                return NULL;

        np = tpossAlloc();
        np->possl = listCopy<TForm>(tp->possl);
        np->possc = tp->possc;
        np->refc  = 1;
        return np;
}

int
tpossCount(TPoss tp)
{
        if (tp)
                return tp->possc;
        else
                return 0;
}

int debugflag = 0; /* Don't want "clever" C compiler's dead coding this */
void
tpossFree(TPoss tp)
{
        if (tp == NULL)
                return;
        if (--tp->refc > 0) return;
if (!debugflag)
{
        listFree<TForm>(tp->possl);
        stoFree((void *) tp);
}
debugflag = 0;
}

int
tpossPrint(FILE *fout, TPoss tp)
{
#if EDIT_1_0_n1_07
        switch ((UAInt) tp)
        {
                case 0:
                        return fprintf(fout, "[]");
                case TUNI_INAPPROPRIATE_TPOSS_CODE:
                        return fprintf(fout, "(inappropriate)");
                case TUNI_NOVALUE_TPOSS_CODE:
                        return fprintf(fout, "(no value)");
                case TUNI_UNKNOWN_TPOSS_CODE:
                        return fprintf(fout, "(unknown)");
                case TUNI_ERROR_TPOSS_CODE:
                        return fprintf(fout, "(error)");
                default:
                        return listPrint<TForm>(fout, tp->possl, tfPrint);
        }
#else
        switch ((int)tp)
        {
                case 0:
                        return fprintf(fout, "[]");
                case (int)tuniInappropriateTPoss:
                        return fprintf(fout, "(inappropriate)");
                case (int)tuniNoValueTPoss:
                        return fprintf(fout, "(no value)");
                case (int)tuniUnknownTPoss:
                        return fprintf(fout, "(unknown)");
                case (int)tuniErrorTPoss:
                        return fprintf(fout, "(error)");
                default:
                        return listPrint<TForm>(fout, tp->possl, tfPrint);
        }
#endif
}

int
tpossPrintDb(TPoss tp)
{
        int rc = tpossPrint(dbOut, tp);
        fnewline(dbOut);
        return rc;
}

bool
tpossIsUnique(TPoss tp)
{
        if (tp)
                return tp->possc == 1;
        else
                return false;
}

#if 0
TPoss
tpossIntersect(TPoss tp1, TPoss tp2)
{
        List<TForm> l = 0;
        List<TForm> l1;

        if (tp1 == NULL || tp2 == NULL)
                return NULL;

        l1 = tp1->possl;

        for (; l1; l1 = cdr(l1)) {
                car(l1) = tfFollowOnly(car(l1));
                if (tpossHas(tp2, car(l1)))
                        l = listCons<TForm>(car(l1), l);
        }

        l = listNReverse<TForm>(l);
        return tpossFrTheList(l);
}
#endif


TPoss
tpossIntersect(TPoss S, TPoss T)
{
        List<TForm> LS, LT, l = 0;

        if (S == NULL || T == NULL)
                return NULL;

        /* If T is free of duplicates, then the result will also be. */
        for (LT = T->possl; LT; LT = cdr(LT)) {
                car(LT) = tfFollowOnly(car(LT));
                for (LS = S->possl; LS; LS = cdr(LS)) {
                        car(LS) = tfFollowOnly(car(LS));
                        if (tfSatisfies(car(LS), car(LT))) {
                                l = listCons<TForm>(car(LT), l);
                                break;
                        }
                        if (tfSatisfies(car(LT), car(LS))) {
                                if (!listMember<TForm>(l, car(LS), tfEqual))
                                        l = listCons<TForm>(car(LS), l);
                        }
                }
        }

        l = listNReverse<TForm>(l);
        return tpossFrTheList(l);
}

TPoss
tpossUnion(TPoss tp1, TPoss tp2)
{
        List<TForm> l1;
        List<TForm> l;

        if (tp1 == NULL)
                return tp2;
        else if (tp2 == NULL)
                return tp1;

        l1 = tp1->possl;
        l = listReverse<TForm>(tp2->possl);     /* Reversed copy */

        for (; l1; l1 = cdr(l1)) {
                car(l1) = tfFollowOnly(car(l1));
                if (!tpossHas(tp2, car(l1)))
                        l = listCons<TForm>(car(l1), l);
        }

        l = listNReverse<TForm>(l);
        return tpossFrTheList(l);
}

TForm
tpossUnique(TPoss tp)
{
        car(tp->possl) = tfFollowOnly(car(tp->possl));
        return car(tp->possl);
}

bool
tpossHas(TPoss tp, TForm t)
{
        if (tp == NULL)
                return false;
        if (listMemq<TForm>(tp->possl, t))
                return true;

        return listMember<TForm>(tp->possl, t, tfEqual);
}

local bool
tpossIsPending(TPoss tp, TForm t)
{
        bool    result = false;

        if (tpossIsUnique(tp)) {
                TForm   S = tpossUnique(tp);

                tcSatPush(S, t);

                if (tfIsPending(S)) {
                        tcNewSat(S, S, t, NULL);
                        result = true;
                }
                if (tfIsPending(t)) {
                        tcNewSat(t, S, t, NULL);
                        result = true;
                }

                tcSatPop();
        }

        return result;
}

bool
tpossHasSatisfier(TPoss tp, TForm t)
{
        List<TForm> l;

        if (tp == NULL)
                return false;

        if (tpossIsPending(tp, t))
                return true;

        for (l = tp->possl; l; l = cdr(l))
                if (tfSatValues(car(l), t))
                        return true;

        return false;
}

TForm
tpossSelectSatisfier(TPoss tp, TForm t)
{
        List<TForm> l;
        TForm     r = 0;

        if (tp == NULL)
                return 0;

        for (l = tp->possl; l; l = cdr(l)) {
/*              car(l) = tfFollowOnly(car(l));      */
                if (tfSatValues(car(l), t)) {
                        if (r) return 0;
                        r = car(l);
                }
        }
        return r;
}

local TForm
tpossJoin(TForm S, TForm T)
{
        return tfIsThird(S) ? S : T;
}

TPoss
tpossSatisfies(TPoss S, TPoss T)
{
        List<TForm> LS, LT, l = 0;

        if (S == NULL || T == NULL)
                return NULL;

        /* If T is free of duplicates, then the result will also be. */
        for (LT = T->possl; LT; LT = cdr(LT)) {
                car(LT) = tfFollowOnly(car(LT));
                for (LS = S->possl; LS; LS = cdr(LS)) {
                        TForm   s = car(LS), t = car(LT);
                        if (tfSatBit(tfSatBupMask(), s, t))
                                l = listCons<TForm>(tpossJoin(s, t), l);
                }
        }

        l = listNReverse<TForm>(l);
        return tpossFrTheList(l);
}

TPoss
tpossSatisfiesType(TPoss S, TForm T)
{
        List<TForm> LS, l = 0;

        if (S == NULL)
                return NULL;

        T = tfFollowOnly(T);

        if (tfIsUnknown(T) || tpossIsPending(S, T))
                return tpossRefer(S);

        for (LS = S->possl; LS; LS = cdr(LS))
                if (tfSatisfies(car(LS), T))
                        l = listCons<TForm>(car(LS), l);

        l = listNReverse<TForm>(l);
        return tpossFrTheList(l);
}

bool
tpossHasMapType(TPoss tp)
{
        TPossIterator   tit;

        if (tp == NULL)
                return false;

        for (tpossITER(tit, tp); tpossMORE(tit); tpossSTEP(tit)) {
                TForm tf = tpossELT(tit);
                tf = tfDefineeType(tf);
                if (tfIsAnyMap(tf) || tfIsMapSyntax(tf))
                        return true;
        }
        return false;
}

TPoss
tpossGeneratorArg(TPoss tpit)
{
        TPoss           tparg;
        TPossIterator   it;

        tparg = tpossEmpty();
        for (tpossITER(it,tpit); tpossMORE(it); tpossSTEP(it)) {
                TForm   t = tpossELT(it);
                tfFollow(t);
                if (tfIsGenerator(t))
                        tparg = tpossAdd1(tparg, tfGeneratorArg(t));
        }
        return tparg;
}

bool
tpossIsHaving(TPoss tp, TFormPredicate pred)
{
        TPossIterator tit;

        for (tpossITER(tit, tp); tpossMORE(tit); tpossSTEP(tit))
                if (pred(tpossELT(tit))) return true;

        return false;
}

