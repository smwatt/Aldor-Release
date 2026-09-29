///////////////////////////////////////////////////////////////////////////////
//
// ti_tdn.cpp: Type inference -- top down pass.
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

#include "ti_tdn.h"
#include "tinfer.h"
#include "terror.h"
#include "simpl.h"

/*
 * To do:
 * -- titdnApply: Should mark op as erroneous, if necessary.
 */

/*****************************************************************************
 *
 * :: Selective debug stuff
 *
 ****************************************************************************/

bool    condApplyDebug          = false;
bool    tipTdnDebug             = false;
#define condApplyDEBUG(s)       DEBUG_IF(condApplyDebug, s)
#define tipTdnDEBUG(s)          DEBUG_IF(tipTdnDebug, s)

/*****************************************************************************
 *
 * :: Fluids to unify multiple exit points
 *
 ****************************************************************************/

TForm           tuniYieldTForm  = 0;
TForm           tuniReturnTForm = 0;
TForm           tuniExitTForm   = 0;
TForm           tuniSelectTForm = 0;
AbSyn           tuniSelectObj   = 0;
/*****************************************************************************
 *
 * :: Fluids to keep trace of multiple exit points
 *
 ****************************************************************************/

List<AbSyn> abExitsList = 0;
List<AbSyn> abReturnsList = 0;
List<AbSyn> abYieldsList = 0;

/*****************************************************************************
 *
 * :: Declarations for top down pass
 *
 ****************************************************************************/

local bool      titdn0Generic   (Stab, AbSyn, TForm);
local bool      titdn0FarValue  (Stab, AbSyn, TForm, AbSyn,TForm *,
                                 List<AbSyn> *);
local bool      titdn0NoValue   (Stab, AbSyn, TForm, Msg);

local bool      titdn0ApplySymIfNeeded
                                (Stab, AbSyn, TForm, Symbol,
                                 Length, AbSynGetter, AbSyn, TFormPredicate);
local bool      titdn0ApplySym  (Stab, AbSyn, TForm, Symbol,
                                 Length, AbSynGetter, AbSyn);
local bool      titdn0ApplyFType(Stab, AbSyn, TForm, AbSyn,
                                 Length, AbSynGetter);
local bool      titdn0ApplyJoin (Stab, AbSyn, TForm, AbSyn,
                                 Length, AbSynGetter);

local bool      titdn0PLambdaArgs       (Stab, AbSyn);
local bool      titdn0PLambdaRets       (Stab, List<AbSyn>);

local bool      titdnId         (Stab, AbSyn, TForm);
local bool      titdn0IdCondition(AbSyn, Syme);
local bool      titdnIdSy       (Stab, AbSyn, TForm);
local bool      titdnBlank      (Stab, AbSyn, TForm);
local bool      titdnLitInteger (Stab, AbSyn, TForm);
local bool      titdnLitFloat   (Stab, AbSyn, TForm);
local bool      titdnLitString  (Stab, AbSyn, TForm);
local bool      titdnAdd        (Stab, AbSyn, TForm);
local bool      titdnAnd        (Stab, AbSyn, TForm);
local bool      titdnApply      (Stab, AbSyn, TForm);
local bool      titdnAssert     (Stab, AbSyn, TForm);
local bool      titdnAssign     (Stab, AbSyn, TForm);
local bool      titdnBreak      (Stab, AbSyn, TForm);
local bool      titdnBuiltin    (Stab, AbSyn, TForm);
local bool      titdnCoerceTo   (Stab, AbSyn, TForm);
local bool      titdnCollect    (Stab, AbSyn, TForm);
local bool      titdnComma      (Stab, AbSyn, TForm);
local bool      titdnDeclare    (Stab, AbSyn, TForm);
local bool      titdnDefault    (Stab, AbSyn, TForm);
local bool      titdnDefine     (Stab, AbSyn, TForm);
local bool      titdnDelay      (Stab, AbSyn, TForm);
local bool      titdnDo         (Stab, AbSyn, TForm);
local bool      titdnExcept     (Stab, AbSyn, TForm);
local bool      titdnRaise      (Stab, AbSyn, TForm);
local bool      titdnExit       (Stab, AbSyn, TForm);
local bool      titdnExport     (Stab, AbSyn, TForm);
local bool      titdnExtend     (Stab, AbSyn, TForm);
local bool      titdnFix        (Stab, AbSyn, TForm);
local bool      titdnFluid      (Stab, AbSyn, TForm);
local bool      titdnFor        (Stab, AbSyn, TForm);
local bool      titdnForeign    (Stab, AbSyn, TForm);
local bool      titdnFree       (Stab, AbSyn, TForm);
local bool      titdnGenerate   (Stab, AbSyn, TForm);
local bool      titdnGoto       (Stab, AbSyn, TForm);
local bool      titdnHas        (Stab, AbSyn, TForm);
local bool      titdnHide       (Stab, AbSyn, TForm);
local bool      titdnIf         (Stab, AbSyn, TForm);
local bool      titdnImport     (Stab, AbSyn, TForm);
local bool      titdnInline     (Stab, AbSyn, TForm);
local bool      titdnIterate    (Stab, AbSyn, TForm);
local bool      titdnLabel      (Stab, AbSyn, TForm);
local bool      titdnLambda     (Stab, AbSyn, TForm);
local bool      titdnLet        (Stab, AbSyn, TForm);
local bool      titdnLocal      (Stab, AbSyn, TForm);
local bool      titdnMacro      (Stab, AbSyn, TForm);
local bool      titdnMLambda    (Stab, AbSyn, TForm);
local bool      titdnNever      (Stab, AbSyn, TForm);
local bool      titdnNot        (Stab, AbSyn, TForm);
local bool      titdnNothing    (Stab, AbSyn, TForm);
local bool      titdnOr         (Stab, AbSyn, TForm);
local bool      titdnPretendTo  (Stab, AbSyn, TForm);
local bool      titdnQualify    (Stab, AbSyn, TForm);
local bool      titdnQuote      (Stab, AbSyn, TForm);
local bool      titdnReference  (Stab, AbSyn, TForm);
local bool      titdnRepeat     (Stab, AbSyn, TForm);
local bool      titdnRestrictTo (Stab, AbSyn, TForm);
local bool      titdnReturn     (Stab, AbSyn, TForm);
local bool      titdnSelect     (Stab, AbSyn, TForm);
local bool      titdnSequence   (Stab, AbSyn, TForm);
local bool      titdnSelect     (Stab, AbSyn, TForm);
local bool      titdnTest       (Stab, AbSyn, TForm);
local bool      titdnTry        (Stab, AbSyn, TForm);
local bool      titdnWhere      (Stab, AbSyn, TForm);
local bool      titdnWhile      (Stab, AbSyn, TForm);
local bool      titdnWith       (Stab, AbSyn, TForm);
local bool      titdnYield      (Stab, AbSyn, TForm);

local void      titdnError(Stab stab, AbSyn absyn, TForm type);
local void      titdn0Error(Stab stab, AbSyn absyn, TForm type);

local bool      titdnSequence0  (Stab, AbSyn, TForm);

/*****************************************************************************
 *
 * :: Top down pass
 *
 ****************************************************************************/

void
tiTopDown(Stab stab, AbSyn absyn, TForm type)
{

        Fluid fluidBinding_tuniReturnTForm(tuniReturnTForm);
        Fluid fluidBinding_tuniYieldTForm(tuniYieldTForm);
        Fluid fluidBinding_tuniExitTForm(tuniExitTForm);
        Fluid fluidBinding_abCondKnown(abCondKnown);

        tuniReturnTForm = tfNone();
        tuniYieldTForm  = tfNone();
        tuniExitTForm   = tfNone();
        abCondKnown     = abCondKnown ? ablogCopy(abCondKnown) : ablogTrue();

        titdn(stab, absyn, type);

        ablogFree(abCondKnown);

        return; /* (result); */
}

bool
titdn(Stab stab, AbSyn absyn, TForm type)
{
        TPoss           abtposs;
        static int      serialNo = 0, depthNo = 0;
        int             serialThis;
        bool            s;
        TForm           stype;

        if (abState(absyn) == AB_State_HasUnique)
                return true;

        if (abState(absyn) == AB_State_Error) {
                titdnError(stab, absyn, type);
                return false;
        }

        assert(abState(absyn) == AB_State_HasPoss);
#if AXL_EDIT_1_1_13_07
        /* MUST use tpossRefer() or abReferTPoss() */
        abtposs  = tpossRefer(abTPoss(absyn));
#else
        abtposs  = abTPoss(absyn);
#endif

        if (tpossCount(abtposs) == 0) {
                titdnError(stab, absyn, type);
                return false;
        }

        stype = tfFollowSubst(type);

        if (tfIsUnknown(stype) || tfIsNone(stype)) {
                if (tpossCount(abtposs) > 1) {
                        terrorNotUniqueType(ALDOR_E_TinExprMeans,
                                            absyn,type,abtposs);
                        return false;
                }
                if (tpossCount(abtposs) == 1)
                        type = tpossUnique(abtposs);
        }

        if (!abIsLeaf(absyn) && abStab(absyn))
                stab = abStab(absyn);

        serialNo += 1;
        depthNo  += 1;
        serialThis = serialNo;
        tipTdnDEBUG({
                fprintf(dbOut,"->Tdn: %*s%d= ", depthNo, "", serialThis);
                abPrettyPrint(dbOut, absyn);
                fprintf(dbOut," @ ");
                tfPrint(dbOut, type);
                fnewline(dbOut);
        });

        switch (abTag(absyn)) {
        case AB_Id:          s = titdnId         (stab, absyn, type); break;
        case AB_IdSy:        s = titdnIdSy       (stab, absyn, type); break;
        case AB_Blank:       s = titdnBlank      (stab, absyn, type); break;
        case AB_LitInteger:  s = titdnLitInteger (stab, absyn, type); break;
        case AB_LitFloat:    s = titdnLitFloat   (stab, absyn, type); break;
        case AB_LitString:   s = titdnLitString  (stab, absyn, type); break;
        case AB_Add:         s = titdnAdd        (stab, absyn, type); break;
        case AB_And:         s = titdnAnd        (stab, absyn, type); break;
        case AB_Apply:       s = titdnApply      (stab, absyn, type); break;
        case AB_Assert:      s = titdnAssert     (stab, absyn, type); break;
        case AB_Assign:      s = titdnAssign     (stab, absyn, type); break;
        case AB_Break:       s = titdnBreak      (stab, absyn, type); break;
        case AB_Builtin:     s = titdnBuiltin    (stab, absyn, type); break;
        case AB_CoerceTo:    s = titdnCoerceTo   (stab, absyn, type); break;
        case AB_Collect:     s = titdnCollect    (stab, absyn, type); break;
        case AB_Comma:       s = titdnComma      (stab, absyn, type); break;
        case AB_Declare:     s = titdnDeclare    (stab, absyn, type); break;
        case AB_Default:     s = titdnDefault    (stab, absyn, type); break;
        case AB_Define:      s = titdnDefine     (stab, absyn, type); break;
        case AB_Delay:       s = titdnDelay      (stab, absyn, type); break;
        case AB_Do:          s = titdnDo         (stab, absyn, type); break;
        case AB_Except:      s = titdnExcept     (stab, absyn, type); break;
        case AB_Raise:       s = titdnRaise      (stab, absyn, type); break;
        case AB_Exit:        s = titdnExit       (stab, absyn, type); break;
        case AB_Export:      s = titdnExport     (stab, absyn, type); break;
        case AB_Extend:      s = titdnExtend     (stab, absyn, type); break;
        case AB_Fix:         s = titdnFix        (stab, absyn, type); break;
        case AB_Fluid:       s = titdnFluid      (stab, absyn, type); break;
        case AB_For:         s = titdnFor        (stab, absyn, type); break;
        case AB_Foreign:     s = titdnForeign    (stab, absyn, type); break;
        case AB_Free:        s = titdnFree       (stab, absyn, type); break;
        case AB_Generate:    s = titdnGenerate   (stab, absyn, type); break;
        case AB_Goto:        s = titdnGoto       (stab, absyn, type); break;
        case AB_Has:         s = titdnHas        (stab, absyn, type); break;
        case AB_Hide:        s = titdnHide       (stab, absyn, type); break;
        case AB_If:          s = titdnIf         (stab, absyn, type); break;
        case AB_Import:      s = titdnImport     (stab, absyn, type); break;
        case AB_Inline:      s = titdnInline     (stab, absyn, type); break;
        case AB_Iterate:     s = titdnIterate    (stab, absyn, type); break;
        case AB_Label:       s = titdnLabel      (stab, absyn, type); break;
        case AB_Lambda:      s = titdnLambda     (stab, absyn, type); break;
        case AB_Let:         s = titdnLet        (stab, absyn, type); break;
        case AB_Local:       s = titdnLocal      (stab, absyn, type); break;
        case AB_Macro:       s = titdnMacro      (stab, absyn, type); break;
        case AB_MLambda:     s = titdnMLambda    (stab, absyn, type); break;
        case AB_Never:       s = titdnNever      (stab, absyn, type); break;
        case AB_Not:         s = titdnNot        (stab, absyn, type); break;
        case AB_Nothing:     s = titdnNothing    (stab, absyn, type); break;
        case AB_Or:          s = titdnOr         (stab, absyn, type); break;
        case AB_PLambda:     s = titdnLambda     (stab, absyn, type); break;
        case AB_PretendTo:   s = titdnPretendTo  (stab, absyn, type); break;
        case AB_Qualify:     s = titdnQualify    (stab, absyn, type); break;
        case AB_Quote:       s = titdnQuote      (stab, absyn, type); break;
        case AB_Reference:   s = titdnReference  (stab, absyn, type); break;
        case AB_Repeat:      s = titdnRepeat     (stab, absyn, type); break;
        case AB_RestrictTo:  s = titdnRestrictTo (stab, absyn, type); break;
        case AB_Return:      s = titdnReturn     (stab, absyn, type); break;
        case AB_Select:      s = titdnSelect     (stab, absyn, type); break;
        case AB_Sequence:    s = titdnSequence   (stab, absyn, type); break;
        case AB_Test:        s = titdnTest       (stab, absyn, type); break;
        case AB_Try:         s = titdnTry        (stab, absyn, type); break;
        case AB_Where:       s = titdnWhere      (stab, absyn, type); break;
        case AB_While:       s = titdnWhile      (stab, absyn, type); break;
        case AB_With:        s = titdnWith       (stab, absyn, type); break;
        case AB_Yield:       s = titdnYield      (stab, absyn, type); break;
        default:             bugBadCase(abTag(absyn)); NotReached(s = 0);
        }

        if (s) {
                /* The callee should have set abTUnique(absyn). */
                assert(abTPoss(absyn) != abtposs);

                /* All identifiers should have symes. */
                assert(abTag(absyn) != AB_Id || abSyme(absyn));

                abSetState(absyn, AB_State_HasUnique);
                tpossFree(abtposs);
        }
        else {
                /* The callee should not have changed the abState. */
                assert(abState(absyn) == AB_State_HasPoss);

                /* The caller should not have set abTUnique(absyn). */
                assert(abTPoss(absyn) == abtposs);
        }

        tipTdnDEBUG({
                fprintf(dbOut, "<-Tdn: %*s%d= ", depthNo, "", serialThis);
                abPrettyPrint(dbOut, absyn);
                fprintf(dbOut, " @ ");
                if (abState(absyn) == AB_State_HasUnique)
                        tfPrint(dbOut, abTUnique(absyn));
                else
                        tfPrint(dbOut, type);
                fnewline(dbOut);
        });
        depthNo -= 1;
        return s;
}

/****************************************************************************
 *
 * :: Generic:  abArgc(ab), abArgv(ab)
 *
 ***************************************************************************/

local bool
titdn0Generic(Stab stab, AbSyn absyn, TForm type)
{
        Length  i;
        if (!abIsLeaf(absyn))
                for (i = 0; i < abArgc(absyn); i += 1)
                        titdn(stab, abArgv(absyn)[i], tfUnknown);
        tfFollow(type);
        abTUnique(absyn) = type;
        return true;
}

/****************************************************************************
 *
 * :: Implied call:  apply, set!, test, generator, ...
 *
 ***************************************************************************/

/*
 * ab ==> m(i,...)       ->  tibup0Apply(stab, ab, 'apply,    n+1, [m,i,...])
 * ab ==> m(i,...) := x  ->  tibup0Apply(stab, ab, 'set!,     n+2, [m,i,...,x])
 * ab ==> if bb then ... ->  tibup0Apply(stab, bb, 'test,     1,   [bb.cond]);
 * ab ==> for i in l     ->  tibup0Apply(stab, ab, 'iterator, 1,   [l])
 * ab ==> x::T           ->  tibup0Apply(stab, ab, 'coerce,   1,   [x])
 *
 * Could also do...
 *
 * ab ==> 3              ->  tibup0Apply(stab, ab, 'integer,  0,   [])
 * ab ==> "3.14"         ->  tibup0Apply(stab, ab, 'float,    0,   [])
 * ab ==> "hello"        ->  tibup0Apply(stab, ab, 'string,   0,   [])
 */

local bool
titdn0ApplySymIfNeeded(Stab stab, AbSyn absyn, TForm type, Symbol fsym,
                Length argc, AbSynGetter argf,
                AbSyn implicitPart, TFormPredicate pred)
{
        AbSyn   part;
        TPoss   tp;

        assert(argc == 1);

        part = argf(absyn, int0);
        tp   = abReferTPoss(part);

        if (tpossIsHaving(tp, pred) || tpossCount(tp) == 0)
                titdn(stab, part, type);
        else
                return titdn0ApplySym(stab, absyn, type, fsym, argc, argf,
                                      implicitPart);

        tpossFree(tp);
        return true;
}

local bool
titdn0ApplySym(Stab stab, AbSyn absyn, TForm type,
               Symbol fsym, Length argc, AbSynGetter argf,
               AbSyn  implicitPart)
{
        AbSyn   implicitOp;

        if (!implicitPart) implicitPart = absyn;

        /* Get the implicit operation */
        implicitOp = abImplicit(implicitPart);
        assert(implicitOp);

        return titdn0ApplyFType(stab, absyn, type, implicitOp, argc, argf);
}

local bool
titdn0ApplyJoin(Stab stab, AbSyn absyn, TForm type,
                 AbSyn op, Length argc, AbSynGetter argf)
{
        TPoss   abtposs = abTPoss(absyn);
        TForm   opType, retType;

        opType = tpossUnique(abTPoss(op));
        titdn(stab, op, opType);
        assert(abState(op) == AB_State_HasUnique);
        abAddTContext(op, tfMapMultiArgEmbed(opType, argc));

        if (!tpossIsUnique(abtposs))
                return false;

        retType = tpossUnique(abtposs);
        if (!tfSatValues(retType, type))
                return false;

        abTUnique(absyn) = retType;
        return true;
}

/*
 * Filter the operations based on arg and ret types.
 * titdn is applied to the virtual arguments.
 */

local bool
titdn0ApplyFType(Stab stab, AbSyn absyn, TForm type, AbSyn op,
                 Length argc, AbSynGetter argf)
{
        SatMask         mask = tfSatBupMask();
        Length          nopc, popc, parmc;
        TForm           nopt, popt, opType;
        TPoss           opTypes, nopTypes, fopTypes;
        TPossIterator   it;
        bool            result;

        opTypes = abTPoss(op);
        if (abIsTheId(op, ssymJoin) && tpossIsUnique(opTypes) &&
            tfSatisfies(tfMapRet(tpossUnique(opTypes)), tfCategory))
                return titdn0ApplyJoin(stab, absyn, type, op, argc, argf);

        opTypes  = abReferTPoss(op);    /* Original list of possible types */
        nopTypes = tpossEmpty();        /* Possible (non-pending) types */
        fopTypes = tpossEmpty();        /* Possible unconditional types */
        nopc = 0;                       /* Number of non-pending matches */
        popc = 0;                       /* Number of all possible matches */
        nopt = tfUnknown;               /* Non-pending op type */
        popt = tfUnknown;               /* Any possible op type */
        opType = NULL;

        /* Filter opTypes based on the argument and return types. */
        for (tpossITER(it, opTypes); tpossMORE(it); tpossSTEP(it)) {
                TForm   opType = tpossELT(it);
                SatMask result;

                opType = tfDefineeType(opType);
                assert(tfIsAnyMap(opType));

                result = tfSatMap(mask, stab, opType, type, absyn, argc, argf);
                if (tfSatSucceed(result)) {
                        if (!tfSatPending(result)) {
                                nopc += 1;
                                nopt = opType;
                                nopTypes = tpossAdd1(nopTypes, opType);
                        }
                        popc += 1;
                        popt = opType;
                }
        }

        if (popc == 1) {
                opType = popt;
                result = true;
        }
        else if (nopc == 1) {
                opType = nopt;
                result = true;
        }
        else if (nopc == 0 && popc > 0) {
                terrorApplyNotAnalyzed(absyn, op, popt);
                result = false;
        }
        else {
                terrorApplyFType(absyn, type, nopTypes, op, stab, argc, argf);
                result = false;
        }

        tpossFree(opTypes);
        tpossFree(nopTypes);

        if (!result) return false;

        titdn(stab, op, opType);

        parmc = tfMapHasDefaults(opType) ? tfMapArgc(opType) : argc;
        abAddTContext(op, tfMapMultiArgEmbed(opType, parmc));

        mask = tfSatTdnMask();
        result = tfSatMap(mask, stab, opType, type, absyn, argc, argf);

        /* We return false rarely (eg titdn0FarValue failure). */
        return tfSatSucceed(result);
}

/****************************************************************************
 *
 * :: Far Values:  return x, yield x, a => x
 *
 ***************************************************************************/

local bool
titdn0FarValue(Stab stab,AbSyn absyn,TForm type,AbSyn farValue,TForm *pFarType,
               List<AbSyn> *pFarAbSynList)
{
        AbEmbed embed;
        tipFarDEBUG({
                fprintf(dbOut, "Computing far value as a ");
                tfPrint(dbOut, *pFarType);
                fnewline(dbOut);
        });
        titdn(stab, farValue, *pFarType);

        if (abState(farValue) != AB_State_HasUnique)
                return false;

        /*
         * Check for `return value' in contexts where no
         * value is expected and for `return'  in contexts
         * where a value _is_ expected.
         */
        if (abTag(absyn) == AB_Return)
        {
                bool    abnone = (abTag(farValue) == AB_Nothing);
                bool    tfnone = tfIsNone(*pFarType);

                if (abnone && !tfnone)
                        comsgError(absyn, ALDOR_E_TinReturnNoVal);
                else if (!abnone && tfnone)
                        comsgWarning(absyn, ALDOR_W_TinNoValReturn);
        }

        *pFarAbSynList = listCons<AbSyn>(farValue, *pFarAbSynList);
        if (tfIsUnknown(*pFarType)) {
                *pFarType = abTUnique(farValue);
                tipFarDEBUG({
                        fprintf(dbOut, "Converting far value to a ");
                        tfPrint(dbOut, *pFarType);
                        fnewline(dbOut);
                });
        }

        if (abTUnique(farValue)) {
                embed = tfSatEmbedType(abTUnique(farValue), *pFarType);
                if (!tfIsNone(*pFarType) && embed != AB_Embed_Identity)
                        abAddTContext(farValue, embed);
        }
        /* Calling program must set abTUnique(absyn). */
        return true;
}

/****************************************************************************
 *
 * :: No Values:  empty sequence, if w/o else, exit.
 *
 ***************************************************************************/

/*
 * This function tells the caller whether or not the void type
 * () satisfies the type provided. As a side-effect, the absyn
 * is given the void type if the type is satisfied otherwise the
 * specified error message is generated.
 */
local bool
titdn0NoValue(Stab stab, AbSyn absyn, TForm type, Msg msg)
{
        /*
         * If the context requires a value of type () then
         * this statement is type correct and has type ().
         *
         * We used to use tfSatValues(tfNone(), type) but
         * this is useless because () satisfies any tuple
         * type (it represents the empty tuple).
         */
        if (tfIsEmptyMulti(type))
        {
                /* Phew! The types match */
                abTUnique(absyn) = tfNone();
                return true;
        }
        else
        {
                /* Generate the required error message */
                comsgError(absyn, msg);
                return false;
        }
}


/****************************************************************************
 *
 * :: Id:  x, +, 1
 * X
 ***************************************************************************/

local bool
titdnId(Stab stab, AbSyn absyn, TForm type)
{
        Syme    syme = abSyme(absyn);

        tipIdDEBUG(fprintf(dbOut,"Entering titdnId\n"));


        /* If no meaning yet, find one */
        if (syme == NULL) {
                /*
                 * Use the default type if requested. This may be a
                 * source of bugs because we don't check conditions.
                 */
                if (abUse(absyn) == AB_Use_Default) {
                        TForm   tf = tpossUnique(abTPoss(absyn));
                        syme = symeNewLexVar(absyn->abId.sym, tf, car(stab));
                }
                else
                        syme = tiGetMeaning(stab, absyn, type);


                /*
                 * Unfortunately we can't tell if tiGetMeaning() failed
                 * because there are no symbols of the correct type or
                 * whether there were some but the conditions were wrong.
                 */
                if (syme == NULL) return false;
        }


        /* Check lazy conditions */
        if (!symeUseIdentifier(absyn, syme))
                comsgError(absyn, ALDOR_E_TinNoMeaningForId, symeString(syme));


        /* Check conditions imposed on local exports/constants */
        if (symeIsExport(syme) || symeIsLexConst(syme)) {
                bool    important = false;


                /*
                 * Some usages must be ignored: we don't want to
                 * complain that the redefinition of a conditional
                 * export (under a different condition) fails to
                 * satisfy the original condition!
                 */
                switch (abUse(absyn)) {
                        /* Ignore the following usages */
                        case AB_Use_Assign:             /* Fall through */
                        case AB_Use_Declaration:        /* Fall through */
                        case AB_Use_Define:             /* Fall through */
                        case AB_Use_Elided:             /* Fall through */
                                break;

                        /*
                         * All other usages are important:
                         *    AB_Use_Type:      conditional types are evil
                         *    AB_Use_Value:     normal usage
                         *    AB_Use_RetValue:  normal usage
                         *    AB_Use_NoValue:   don't really care
                         *    AB_Use_Label:     don't really care
                         *    AB_Use_Default:   rarely seen here
                         *    AB_Use_Iterator:  never seen here
                         *    AB_Use_Except:    not sure
                         *    (unspecified):    normal usage
                         */
                        default: important = true; break;
                }


                /* If important, ensure that the condition is satisfied */
                if (important) titdn0IdCondition(absyn, syme);
        }


        /* Give this leaf some meaning */
        stabSetSyme(stab, absyn, syme, abCondKnown);
        abTUnique(absyn) = symeType(syme);


        /* We return success even if an error was raised */
        return true;
}

local bool
titdn0IdCondition(AbSyn id, Syme syme)
{
        SImpl   impl;
        AbLogic cond;


        /* If it has a default implementation let it go */
        if (symeHasDefault(syme)) return true;


        /* Check to see if there are any implementation details. */
        impl = symeImpl(syme);


        /* If there aren't any then there are no conditions. */
        if (!impl) return true;


        /* At the moment we can only cope with simple conditions */
        if (implTag(impl) != SIMPL_Cond) return true;


        /* Check the implementation condition */
        cond = impl->implCond.cond;
        if (ablogImplies(abCondKnown, cond)) return true;


        /* Report the error */
        terrorIdCondition(symeType(syme), id, abCondKnown, cond);
        return false;
}


/****************************************************************************
 *
 * :: LitInteger:   32
 * :: LitFloat:     4.0
 * :: LitString:    "hello"
 * X
 * !! This stuff could be made to go through ti...0Apply.
 ***************************************************************************/

local bool      titdn0Literal (Symbol, Stab, AbSyn, TForm);

/*
 * Top down entry points.
 */

local bool
titdnLitInteger(Stab stab, AbSyn absyn, TForm tform)
{
        return titdn0Literal(ssymTheInteger, stab, absyn, tform);
}

local bool
titdnLitFloat(Stab stab, AbSyn absyn, TForm tform)
{
        return titdn0Literal(ssymTheFloat, stab, absyn, tform);
}

local bool
titdnLitString(Stab stab, AbSyn absyn, TForm tform)
{
        return titdn0Literal(ssymTheString, stab, absyn, tform);
}

/*
 * Functions which actually do the work.
 */

local bool
titdn0Literal(Symbol sym, Stab stab, AbSyn absyn, TForm type)
{
        SatMask         mask = tfSatBupMask(), result;
        Syme            syme = abSyme(absyn);
        TForm           retType = tfUnknown;

        if (syme) {
                assert(tfIsLitOpType(symeType(syme)));
                retType = tfMapRet(symeType(syme));
        }
        else {
                List<Syme> ml0, ml, okSymes = listNil<Syme>;
                int     n;

                ml0 = stabGetMeanings(stab, abCondKnown, sym);

                for (n = 0, ml = ml0; ml; ml = cdr(ml)) {
                        TForm   tf = symeType(car(ml));

                        tfFollow(tf);
                        if (!tfIsLitOpType(tf)) continue;

                        result = tfSat(mask, tfMapRet(tf), type);
                        if (tfSatPending(result) && cdr(ml0)) {
                                terrorApplyNotAnalyzed(absyn, absyn, tf);
                                return false;
                        }
                        if (tfSatSucceed(result)) {
                                if (n)
                                        okSymes= listCons<Syme>(syme, okSymes);

                                syme = car(ml);
                                retType = tfMapRet(tf);
                                if (!symeUseIdentifier(absyn, syme))
                                        comsgError(absyn,
                                                   ALDOR_E_TinNoMeaningForLit,
                                                   (sym).string(),
                                                   absyn->abLitString.str);
                                n++;
                        }
                }
                if (n != 1) {
                        if (n) okSymes = listCons<Syme>(syme, okSymes);
                        terrorNotUniqueMeaning(ALDOR_E_TinNMeanings,
                                               absyn, okSymes, ml0,
                                               abLeafStr(absyn), type);
                        listFree<Syme>(okSymes);
                        return false;
                }
        }

        mask = tfSatTdnMask();
        tfSat(mask, retType, type);

        abTUnique(absyn) = retType;
        stabSetSyme(stab, absyn, syme, abCondKnown);

        return true;
}

/***************************************************************************
 *
 * :: Comma: (a, b)
 *
 ***************************************************************************/

local bool
titdnComma(Stab stab, AbSyn absyn, TForm type)
{
        AbSyn           *argv   = abArgv(absyn);
        Length          i, argc = abArgc(absyn);
        Length          n = 0;
        TPoss           tp = abTPoss(absyn);
        TPossIterator   it;
        TForm           rtype = NULL;
        AbEmbed         embed;

        if (abUse(absyn) == AB_Use_Declaration ||
            abUse(absyn) == AB_Use_Default) {
                for (i = 0; i < argc; i++)
                        titdn(stab, argv[i], tfUnknown);
                rtype = type;
        }
        else {
                for (tpossITER(it,tp); tpossMORE(it); tpossSTEP(it)) {
                        TForm   tt = tpossELT(it);
                        if (tfSatReturn(tt, type)) {
                                n++;
                                rtype = tt;
                        }
                }
                if (n != 1) {
                        /* Note we haven't done titdn on descendents. */
                        terrorNotUniqueType(ALDOR_E_TinExprMeans, absyn, type, tp);
                        return false;
                }
                for (i = 0; i < argc; i++) {
                        TForm   rti = tfMultiArgN(rtype,i);
                        if (tfIsTypeTuple(type)) rti = tfType;
                        titdn(stab, argv[i], rti);
                }
                embed = tfSatEmbedType(rtype, type);
                if (!tfIsNone(type) && embed != AB_Embed_Identity)
                        abAddTContext(absyn, embed);
        }

        abTUnique(absyn) = rtype;
        return true;
}

/****************************************************************************
 *
 * :: Apply:   f(a, b, ...)
 *
 ***************************************************************************/

local bool
titdnApply(Stab stab, AbSyn absyn, TForm type)
{
        AbSyn           op = abApplyOp(absyn);
        TPoss           tp;

        tipApplyDEBUG(fprintf(dbOut, "Entering titdnApply\n"));

        if (abState(op) == AB_State_Error)
                return false;

        tp = abTPoss(op);

        if (tpossHasMapType(tp) || tpossCount(tp) == 0)
                return titdn0ApplyFType(stab, absyn, type, op,
                                        abApplyArgc(absyn), abApplyArgf);
        else
                return titdn0ApplySym(stab, absyn, type, ssymApply,
                                      abArgc(absyn), abArgf, NULL);
}

/****************************************************************************
 *
 * :: Define:   a == e
 * X
 ***************************************************************************/

local bool
titdnDefine(Stab stab, AbSyn absyn, TForm type)
{
        AbSyn           lhs = absyn->abDefine.lhs;
        AbSyn           rhs = absyn->abDefine.rhs;
        TForm           rtype, ltype, idtype;
        TPoss           idtposs;

        if (tfIsNone(type)) type = tfUnknown;

        idtype = tiDefineFilter(absyn, type);
        idtposs = tiDefineTPoss(absyn);

        rtype = tpossSelectSatisfier(idtposs, idtype);
        if (!rtype) {
                terrorNotUniqueType(ALDOR_E_TinDefnMeans, absyn, idtype, idtposs);
                return false;
        }

        /* If the r.h. type satisfies the constraint, relax on the l.h. */
        if (abState(lhs) == AB_State_HasUnique &&
            tfSatValues(abTUnique(lhs), rtype))
                ltype = abTUnique(lhs);
        else {
                assert(abState(lhs) == AB_State_HasPoss);
                if (tpossIsUnique(abTPoss(lhs)) &&
                    tfSatValues(tpossUnique(abTPoss(lhs)), rtype))
                        ltype = tpossUnique(abTPoss(lhs));
                else
                        ltype = rtype;
        }

        tipDefineDEBUG({
                fprintf(dbOut, "************** Defining: ");
                abPrettyPrint(dbOut, lhs);
                fnewline(dbOut);
        });

        titdn(stab, lhs, ltype);
        titdn(stab, rhs, rtype);

        if (abTag(lhs) == AB_Declare) {
                rtype = tpossSelectSatisfier(abTPoss(absyn), type);
                if (!rtype) {
                        terrorNotUniqueType(ALDOR_E_TinDefnMeans, absyn,
                                            type, abTPoss(absyn));
                        return false;
                }
        }


        abTUnique(absyn) = rtype;

        tipDefineDEBUG({
                fprintf(dbOut,"Tdn: Define of ");
                abPrint(dbOut, lhs);
                fprintf(dbOut," has type ");
                tfPrint(dbOut, rtype);
                fnewline(dbOut);
        });
        return true;
}

/****************************************************************************
 *
 * :: Assign:   a := e
 * X
 ***************************************************************************/

/*!! To do: (v.i,v.j) := (v.j, v.i)      */

local bool
titdnAssign(Stab stab, AbSyn absyn, TForm type)
{
        AbSyn           rhs = absyn->abAssign.rhs;
        AbSyn           lhs = absyn->abAssign.lhs;
        TPoss           abtposs = abTPoss(absyn);
        TForm           rtype;
        AbEmbed         embed;

        if (tfIsNone(type)) type = tfUnknown;
        rtype = tpossSelectSatisfier(abtposs, type);
        if (!rtype) {
                terrorNotUniqueType(ALDOR_E_TinAssMeans, absyn, type, abtposs);
                return false;
        }

        if (abTag(lhs) == AB_Apply)
                return titdn0ApplySym(stab, absyn, rtype, ssymSetBang,
                                      abArgc(lhs) + 1, abSetArgf, lhs);

        titdn(stab, rhs, rtype);
        titdn(stab, lhs, rtype);
        abTUnique(absyn) = rtype;

        embed = tfSatEmbedType(abTUnique(rhs), rtype);
        if (!tfIsNone(rtype) && embed != AB_Embed_Identity)
                abAddTContext(rhs, embed);


        tipAssignDEBUG({
                fprintf(dbOut,"Tdn: Assignment to ");
                abPrint(dbOut, lhs);
                fprintf(dbOut," has type ");
                tfPrint(dbOut, rtype);
                fnewline(dbOut);
        });
        return true;
}

/****************************************************************************
 *
 * :: Declare:   a: A
 *
 ***************************************************************************/

local bool
titdnDeclare(Stab stab, AbSyn absyn, TForm type)
{
        AbSyn id     = absyn->abDeclare.id;
        AbSyn idtype = absyn->abDeclare.type;
        TForm tf, rtype;
        Syme  syme;

        tipDeclareDEBUG({
                fprintf(dbOut, "In the declaration ");
                abPrettyPrint(dbOut, absyn);
                fprintf(dbOut, ", the semantics field is ");
                if (abTForm(absyn))
                        tfPrint(dbOut, abTForm(absyn));
                else
                        fprintf(dbOut, "_");
                fnewline(dbOut);
        });

        if (abUse(absyn) == AB_Use_Define || abUse(absyn) == AB_Use_Assign)
                tf = (tfIsUnknown(type) ? tiGetTForm(stab, idtype) : type);
        else
                tf = tiGetTForm(stab, idtype);

        /* Prevent stabUseMeaning during titdn(stab, id, tf) */
        if (abUse(absyn) != AB_Use_Assign) stabUseMeaningShadow(id);

        syme = abSyme(id);
        if (syme && tfIsMulti(type)) {
                comsgError(absyn, ALDOR_E_TinBadDeclare);
                return false;
        }

        titdn(stab, id, tf);
        stabUseMeaningUnshadow();

        /* idtype has been handled already. */

        rtype = tpossSelectSatisfier(abTPoss(absyn), type);
        if (!rtype) rtype = tfUnknown;
        abTUnique(absyn) = rtype;

        tipDeclareDEBUG({
                fprintf(dbOut,"Tdn: Declare of ");
                abPrint(dbOut, id);
                fprintf(dbOut," has type ");
                tfPrint(dbOut, rtype);
                fnewline(dbOut);
        });
        return true;
}

/****************************************************************************
 *
 * :: Label:   @@ x @@ [e]
 *
 ***************************************************************************/

local bool
titdnLabel(Stab stab, AbSyn absyn, TForm type)
{
        AbSyn   expr = absyn->abLabel.expr;

        if (titdn(stab, expr, type))
                abTUnique(absyn) = abTUnique(expr);
        else
                abTUnique(absyn) = tfUnknown;

        return true;
}

/****************************************************************************
 *
 * :: Goto:   goto id
 *
 ***************************************************************************/

local bool
titdnGoto(Stab stab, AbSyn absyn, TForm type)
{
#if EDIT_1_0_n2_05
        AbSyn label = absyn->abGoto.label;

        if (stabLabelExistsInThisStab(stab, abIdSym(label))) {
                abTUnique(absyn) = tfExit;
                return true;
        }
        else {
                List<AbSyn> abl = stabGetLabels(stab, abIdSym(label));
                while (abl) {
                        AbSyn label = car(abl);
                        Syme syme = label->abHdr.seman->syme;
                        ULong labelLamLev = symeDefLambdaLevelNo(syme);
                        ULong labelLexLev = symeDefLevelNo(syme);
                        ULong gotoLamLev = stabLambdaLevelNo(stab);
                        ULong gotoLexLev = stabLevelNo(stab);

                        if (labelLamLev == gotoLamLev && labelLexLev <= gotoLexLev) {
                                abTUnique(absyn) = tfExit;
                                return true;
                        }
                        abl = cdr(abl);
                }

                comsgError(absyn->abGoto.label, ALDOR_E_TinFarGoto);
                return false;
        }
#else
        /* Need to be more careful than this (see titdnAssert) */
        abTUnique(absyn) = tfExit;
        return true;
#endif
}

/****************************************************************************
 *
 * :: Lambda:   (a: A): B +-> b
 * :: PLambda:  (a: A): B +->* b
 *
 ***************************************************************************/

local bool
titdnLambda(Stab stab, AbSyn absyn, TForm type)
{
        AbSyn   param = absyn->abLambda.param;
        AbSyn   body  = absyn->abLambda.body;
        AbSyn   ret   = absyn->abLambda.rtype;
        TPoss   abtposs;
        TForm   rtype;
        bool    result = true;

        Fluid fluidBinding_tuniReturnTForm(tuniReturnTForm);
        Fluid fluidBinding_tuniYieldTForm(tuniYieldTForm);
        Fluid fluidBinding_tuniExitTForm(tuniExitTForm);
        Fluid fluidBinding_abReturnsList(abReturnsList);

        tuniReturnTForm = tfNone();
        tuniYieldTForm  = tfNone();
        tuniExitTForm   = tfNone();
        abReturnsList = listNil<AbSyn>;

        tuniReturnTForm = tfFullFrAbSyn(stab, ret);

        titdn(stab, body, tuniReturnTForm);

        abtposs = abTPoss(absyn);
        if (tiCheckLambdaType(type) || !tfIsAnyMap(type))
                rtype = tpossSelectSatisfier(abtposs, type);
        else
                rtype = type;

        if (!rtype) {
                terrorNotUniqueType(ALDOR_E_TinExprMeans, absyn, type, abtposs);
                result = false;
        }

        else if (abHasTag(absyn, AB_PLambda)) {
                bool tres;

                while (abTag(body) == AB_Label)
                        body = body->abLabel.expr;
                abReturnsList = listCons<AbSyn>(body, abReturnsList);
                tres = titdn0PLambdaArgs(stab, param) &&
                         titdn0PLambdaRets(stab, abReturnsList);

                /* Return false if either result or tres are false */
                result = result ? tres : result;
        }

#if AXL_EDIT_1_1_12p6_21
        /* Only allowed to set abTUnique if successful */
        if (result) abTUnique(absyn) = rtype;

        listFree<AbSyn>(abReturnsList);
        return result;
#else
        listFree<AbSyn>(abReturnsList);
        abTUnique(absyn) = rtype;
        return result;
#endif
}

local bool
titdn0PLambdaArgs(Stab stab, AbSyn param)
{
        AbSyn   *argv;
        Length  i, argc;
        bool    result = true;

        switch (abTag(param)) {
        case AB_Nothing:
                argc = 0;
                argv = 0;
                break;
        case AB_Comma:
                argc = abArgc(param);
                argv = abArgv(param);
                break;
        default:
                argc = 1;
                argv = &param;
                break;
        }

        for (i = 0; result && i < argc; i += 1) {
                AbSyn   argi = abDefineeId(argv[i]);
                assert(abState(argi) == AB_State_HasUnique);
                result = tiRawToUnary(stab, argi, abTUnique(argi));
        }

        return result;
}

local bool
titdn0PLambdaRets(Stab stab, List<AbSyn> vals)
{
        bool    result = true;

        for (; result && vals; vals = cdr(vals)) {
                AbSyn   val = car(vals);
                assert(abState(val) == AB_State_HasUnique);
                result = tiUnaryToRaw(stab, val, tfUnknown);
        }

        return result;
}

/****************************************************************************
 *
 * :: Sequence:   (a; b; c)
 *
 ***************************************************************************/

local bool
titdnSequence(Stab stab, AbSyn absyn, TForm type)
{
        Fluid fluidBinding_tuniExitTForm(tuniExitTForm);
        Fluid fluidBinding_abExitsList(abExitsList);
        Fluid fluidBinding_tuniSelectObj(tuniSelectObj);
        bool            result;

        tuniExitTForm = type;
        abExitsList   = listNil<AbSyn>;
        tuniSelectObj = NULL;

        result = titdnSequence0(stab, absyn, type);

        return result;
}

local bool
titdnSequence0(Stab stab, AbSyn absyn, TForm type)
{
        int             i, n = abArgc(absyn);
        bool            result;

        if (abState(absyn) == AB_State_Error)
                abSetState(absyn, AB_State_HasPoss);

        if (n == 0)
                result = titdn0NoValue(stab, absyn, type, ALDOR_E_TinContextSeq);
        else
        {
                TForm none = tfNone();

                for (i = 0; i < n-1; i++)
                        titdn(stab, abArgv(absyn)[i], none);

                titdn0FarValue(stab,absyn, type, abArgv(absyn)[n-1],
                               &tuniExitTForm, &abExitsList);

                if (abState(absyn) == AB_State_Error) {
                        terror(stab, absyn, type);
                        result = false;
                }
                else {
                        abTUnique(absyn) = tuniExitTForm;
                        result = true;
                }
        }
        listFree<AbSyn>(abExitsList);

        return result;
}

/****************************************************************************
 *
 * :: Exit:  (...; b => x ; ...)
 *
 ***************************************************************************/

local bool
titdnExit(Stab stab, AbSyn absyn, TForm type)
{
        AbSyn   test  = absyn->abExit.test;
        AbSyn   value = absyn->abExit.value;
        AbLogic saveCond;

        titdn(stab, test, tfUnknown);

        if (!tuniSelectObj)
        {
                /* See tibupExit for comments */
                AbSyn nTest = abExpandDefs(stab, test);
                ablogAndPush(&abCondKnown, &saveCond, nTest, true);
        }

        titdn0FarValue(stab, absyn, type, value, &tuniExitTForm, &abExitsList);

        if (!tuniSelectObj)
                ablogAndPop (&abCondKnown, &saveCond);

        return titdn0NoValue(stab, absyn, type, ALDOR_E_TinContextExit);
}

/***************************************************************************
 *
 * :: return: return x,  return;
 *
 ***************************************************************************/

local bool
titdnReturn(Stab stab, AbSyn absyn, TForm type)
{
        titdn0FarValue(stab,absyn,type,absyn->abReturn.value,
                       &tuniReturnTForm, &abReturnsList);
        abTUnique(absyn) = tfExit;
        return true;
}

/****************************************************************************
 *
 * :: Generate:  generate [N] of (... yield ...)
 *
 ***************************************************************************/

local bool
titdnGenerate(Stab stab, AbSyn absyn, TForm type)
{

        Fluid fluidBinding_tuniReturnTForm(tuniReturnTForm);
        Fluid fluidBinding_tuniYieldTForm(tuniYieldTForm);
        Fluid fluidBinding_tuniExitTForm(tuniExitTForm);
        Fluid fluidBinding_abYieldsList(abYieldsList);
        AbSyn           body = absyn->abGenerate.body;
        AbSyn           count = absyn->abGenerate.count;
        bool            result;

        tuniReturnTForm = tfNone();
        tuniYieldTForm  = tfNone();
        tuniExitTForm   = tfNone();
        abYieldsList = listNil<AbSyn>;

        if (tfIsGenerator(type))
                tuniYieldTForm = tfGeneratorArg(type);
        else
                tuniYieldTForm = tfUnknown;

        titdn(stab, count, tfUnknown);
        titdn(stab, body,  tfNone());

        if (abState(absyn) == AB_State_Error) {
                /*
                 * The problem must be in the body. We hope that
                 * errors in the count expression will be handled
                 * by someone else. The trouble is that this kind
                 * of thinking caused bugs like #1144 which we are
                 * trying to fix here ...
                 *
                 * Anyway, since we have fully analysed the body of
                 * the generator, it probably has a unique type now.
                 * If so we turn it into a tposs for terror().
                 */
                if (abState(body) == AB_State_HasUnique) {
                        abSetState(body, AB_State_HasPoss);
                        abTPoss(body) = tpossSingleton(abTUnique(body));
                }


                /*
                 * If errors were detected then they will have been
                 * reported by titdn and the unique type associated
                 * with the body will satisfy the context of the
                 * generator. This means that we won't repeat these
                 * error messages when we call terror().
                 *
                 * If we didn't detect any errors when we dealt with
                 * the body then this node must be bad because its
                 * inferred type does not satisfy the context type.
                 * Hopefully terror() will report the problem for us.
                 */
                terror(stab, body, type);
        }

#if AXL_EDIT_1_1_12p6_21
        /* Were we successful? */
        result = (abState(absyn) != AB_State_Error) ? true : false;

        /* Only allowed to set abTUnique if returning true */
        if (result) abTUnique(absyn) = tfGenerator(tuniYieldTForm);

        listFree<AbSyn>(abYieldsList);
        return result;
#else
        abTUnique(absyn) = tfGenerator(tuniYieldTForm);
        listFree<AbSyn>(abYieldsList);

        return abState(absyn) != AB_State_Error;
#endif
}

/***************************************************************************
 *
 * :: Yield:  yield x
 *
 ***************************************************************************/

local bool
titdnYield(Stab stab, AbSyn absyn, TForm type)
{
        titdn0FarValue(stab, absyn, type,
                       absyn->abYield.value,
                       &tuniYieldTForm,
                       &abYieldsList);
        abTUnique(absyn) = tfExit;
        return true;
}

/****************************************************************************
 *
 * :: Add:  [D] add (a: A == ...)
 *
 ***************************************************************************/

local bool
titdnAdd(Stab stab, AbSyn absyn, TForm type)
{
        AbSyn           base    = absyn->abAdd.base;
        AbSyn           capsule = absyn->abAdd.capsule;

        titdn(stab, base,    tfUnknown);        /* !! cd push Type */
        titdn(stab, capsule, tfUnknown);

        /* !! Should infer type of body. */
        abTUnique(absyn) = type;
        return true;
}

/****************************************************************************
 *
 * :: With:  [C] with (a: A; ...)
 *
 ***************************************************************************/

local bool
titdnWith(Stab stab, AbSyn absyn, TForm type)
{
        TPoss   abtposs = abTPoss(absyn);
        TForm   rtype;

        rtype = tpossSelectSatisfier(abtposs, type);
        if (!rtype) return false;

        abTUnique(absyn) = rtype;
        return true;
}

/****************************************************************************
 *
 * :: Where:  e where d
 *
 ***************************************************************************/

local bool
titdnWhere(Stab stab, AbSyn absyn, TForm type)
{
        TPoss   abtposs = abTPoss(absyn);
        AbSyn   context = absyn->abWhere.context;
        AbSyn   expr    = absyn->abWhere.expr;

        titdn(stab, context, tfNone());
        titdn(stab, expr,    type);

        type = tpossSelectSatisfier(abtposs, type);
        if (!type) return false;

        abTUnique(absyn) = type;
        return true;
}

/****************************************************************************
 *
 * :: If:  if b then t [else e]
 *
 ***************************************************************************/

local bool
titdnIf(Stab stab, AbSyn absyn, TForm type)
{
        AbSyn   test    = absyn->abIf.test;
        AbSyn   thenAlt = absyn->abIf.thenAlt;
        AbSyn   elseAlt = absyn->abIf.elseAlt;
        AbSyn   nTest   = test;
        AbLogic saveCond;

        TPoss   abtposs = abTPoss(absyn);
        abtposs = tpossSatisfiesType(abtposs, type);

        if (tpossCount(abtposs) > 1) {
                terrorNotUniqueType(ALDOR_E_TinIfMeans, absyn, type, abtposs);
                return false;
        }

        /*
         * An unfixed compiler bug means that parts of Salli programs
         * tinfered with (tfBoolean == tfUnknown). We want to catch
         * this problem as soon as possible.
         */
        assert(tfBoolean != tfUnknown);


        /* This ought to do nothing since we tinfered test during tibup */
        titdn(stab, test, tfBoolean);


        if (tfIsCategoryContext(type, absyn))
                type = tpossUnique(abtposs);
        else    /* Normalise the test for other contexts */
                nTest = abExpandDefs(stab, test);

        ablogAndPush(&abCondKnown, &saveCond, nTest, true); /* test, true); */
        titdn(stab, thenAlt, type);
        ablogAndPop (&abCondKnown, &saveCond);

        ablogAndPush(&abCondKnown, &saveCond, nTest, false); /* test, false); */
        titdn(stab, elseAlt, type);
        ablogAndPop (&abCondKnown, &saveCond);

        /*
         * We can't use tpossUnique(abtposs) here because otherwise we
         * will end up performing embeddings on ourself in addition to
         * the same embeddings that we performed on the branches.
         */
        abTUnique(absyn) = type;
        return true;
}

/****************************************************************************
 *
 * :: Test:  implied test
 *
 ***************************************************************************/

AbSyn
titdnSelectArgf(AbSyn ab, Length i)
{
        if (i==0) return tuniSelectObj;
        else return abArgv(ab)[i-1];
}

local bool
titdnTest(Stab stab, AbSyn absyn, TForm type)
{
        /*
         * An unfixed compiler bug means that parts of Salli programs
         * tinfered with (tfBoolean == tfUnknown). We want to catch
         * this problem as soon as possible.
         */
        assert(tfBoolean != tfUnknown);

        if (tuniSelectObj != NULL) {
                titdn0ApplySym(stab, absyn,
                               tfBoolean,
                               ssymTheCase, 2,
                               titdnSelectArgf, NULL);
        }
        else {
                titdn0ApplySymIfNeeded(stab, absyn, tfBoolean,
                                       ssymTheTest, 1, abArgf, NULL,
                                       tfIsBooleanFn);
        }
        abTUnique(absyn) = tfBoolean;
        return true;
}

/***************************************************************************
 *
 * :: Collect:  e <iter>*
 *
 ***************************************************************************/

local bool
titdnCollect(Stab stab, AbSyn absyn, TForm type)
{
        AbSyn   body      = absyn->abCollect.body;
        AbSyn   *iterv    = absyn->abCollect.iterv;
        Length  i, iterc  = abCollectIterc(absyn);
        TForm   rtype;

        Fluid fluidBinding_tuniReturnTForm(tuniReturnTForm);
        Fluid fluidBinding_tuniYieldTForm(tuniYieldTForm);
        Fluid fluidBinding_tuniExitTForm(tuniExitTForm);

        tuniReturnTForm = tfNone();
        tuniYieldTForm  = tfNone();
        tuniExitTForm   = tfNone();

        for (i = 0; i < iterc; i++)
                titdn(stab, iterv[i], tfUnknown);

        if (tfIsGenerator(type))
                rtype = tfGeneratorArg(type);
        else
                rtype = tfUnknown;

        titdn(stab, body, rtype);
        abTUnique(absyn) = type;

#if EDIT_1_0_n1_05
        {
                AbEmbed embed = tfSatEmbedType(abTUnique(body), rtype);
                if (!tfIsNone(rtype) && embed != AB_Embed_Identity)
                        abAddTContext(body, embed);
        }
#endif

        return true;
}

/****************************************************************************
 *
 * :: Repeat:  <iter>* repeat e
 *
 ***************************************************************************/

local bool
titdnRepeat(Stab stab, AbSyn absyn, TForm type)
{
        AbSyn   body      = absyn->abRepeat.body;
        AbSyn   *iterv    = absyn->abRepeat.iterv;
        Length  i, iterc  = abRepeatIterc(absyn);
        bool    result;

        for (i = 0; i < iterc; i++)
                titdn(stab, iterv[i], tfUnknown);

        titdn(stab, body, tfNone());
        if (tfIsNone(tpossUnique(abTPoss(absyn))))
        {
                result = titdn0NoValue(stab,absyn,type,ALDOR_E_TinContextRepeat);
        }
        else
        {
                abTUnique(absyn) = type;
                result = true;
        }

        return result;
}

/***************************************************************************
 *
 * :: Never
 * X
 ***************************************************************************/

local bool
titdnNever(Stab stab, AbSyn absyn, TForm type)
{
        abTUnique(absyn) = tfExit;
        return true;
}

/***************************************************************************
 *
 * :: Iterate
 * X
 ***************************************************************************/

local bool
titdnIterate(Stab stab, AbSyn absyn, TForm type)
{
        abTUnique(absyn) = tfExit;
        return true;
}

/***************************************************************************
 *
 * :: Break
 * X
 ***************************************************************************/

local bool
titdnBreak(Stab stab, AbSyn absyn, TForm type)
{
        abTUnique(absyn) = tfExit;
        return true;
}

/****************************************************************************
 *
 * :: While:  while c
 *
 ***************************************************************************/

local bool
titdnWhile(Stab stab, AbSyn absyn, TForm type)
{
        /*
         * An unfixed compiler bug means that parts of Salli programs
         * tinfered with (tfBoolean == tfUnknown). We want to catch
         * this problem as soon as possible.
         */
        assert(tfBoolean != tfUnknown);

        return titdn0Generic(stab, absyn, tfBoolean);
}

/***************************************************************************
 *
 * :: For:  for x in l | c
 *
 ***************************************************************************/

local bool
titdnFor(Stab stab, AbSyn absyn, TForm type)
{
        AbSyn   lhs   = absyn->abFor.lhs;
        AbSyn   test  = absyn->abFor.test;
        TForm   twhole;

        if (tfIsUnknown(type) && tpossIsUnique(abTPoss(lhs)))
                twhole = tfGenerator(tpossUnique(abTPoss(lhs)));
        else
                twhole = type;

        /*
         * Subtle note: the generator in a for-iterator lies
         * outside the scope level of the repeat. This means
         * that we have to use cdr(stab) whenever we tinfer
         * absyn->abFor.whole or via abForIterArgf().
         */
        titdn0ApplySymIfNeeded(cdr(stab), absyn, twhole,
                        ssymTheGenerator, 1, abForIterArgf,
                        NULL, tfIsGeneratorFn);


        /*
         * The for-variable and test lie within the scope
         * of the repeat clause.
         */
        titdn(stab, lhs,  tfUnknown);
        titdn(stab, test, tfUnknown);

        abTUnique(absyn) = twhole;
        return true;
}

/****************************************************************************
 *
 * :: Foreign:  import ... from Foreign(...)
 *
 ***************************************************************************/

local bool
titdnForeign(Stab stab, AbSyn absyn, TForm type)
{
        titdn(stab, absyn->abForeign.what, tfUnknown);
        abTUnique(absyn) = type;
        return true;
}

/****************************************************************************
 *
 * :: Import:  import ... from D
 *
 ***************************************************************************/

local bool
titdnImport(Stab stab, AbSyn absyn, TForm type)
{
        abTUnique(absyn) = tfNone();
        return true;
}

/****************************************************************************
 *
 * :: Inline:  inline .. from D
 *
 ***************************************************************************/

local bool
titdnInline(Stab stab, AbSyn absyn, TForm type)
{
        abTUnique(absyn) = type;
        return true;
}

/****************************************************************************
 *
 * :: Qualify:   A $ B
 *
 * ToDo: 3$Integer
 *
 ***************************************************************************/

local bool
titdnQualify(Stab stab, AbSyn absyn, TForm type)
{
        AbSyn           origin = absyn->abQualify.origin;
        AbSyn           what   = absyn->abQualify.what;
        Symbol          sym    = what->abId.sym;

        TForm           tforg;
        List<Syme>        symes, msymes, fsymes, allSymes, okSymes;
        Syme            syme, osyme;

        tforg = tiGetTForm(stab, origin);

        if (tfIsForeign(tforg)) {
                msymes = stabGetMeanings(stab, abCondKnown, what->abId.sym);
                symes = listNil<Syme>;

                for ( ; msymes; msymes = cdr(msymes))
                        if (symeId(car(msymes)) == sym && symeIsForeign(car(msymes)))
                                symes = listCons<Syme>(car(msymes), symes);
                fsymes = symes;
        }
        else if (tfIsSelf(tforg)) {
                symes  = tfGetDomImports(tforg);
                fsymes = listNil<Syme>;

                if (symes == listNil<Syme>) {
                        msymes = stabGetMeanings(stab, abCondKnown, what->abId.sym);
                        for ( ; msymes; msymes = cdr(msymes))
                                if (symeId(car(msymes)) == sym && symeIsExport(car(msymes)))
                                        symes = listCons<Syme>(car(msymes),
                                                               symes);
                        fsymes = symes;
                }
        }
        else {
                symes  = listNil<Syme>;
                msymes = tfGetDomImports(tforg);
                for ( ; msymes; msymes = cdr(msymes))
                        if (symeId(car(msymes)) == sym
                            && ablogIsListKnown(symeCondition(car(msymes))))
                                symes = listCons<Syme>(car(msymes), symes);
                fsymes = symes;
        }

        allSymes = symes;
        okSymes  = listNil<Syme>;
        syme     = NULL;

        for (symes = allSymes; symes; symes = cdr(symes)) {
                syme = car(symes);

                if (symeId(syme) == sym
                    && tfSatReturn(symeType(syme), type)) {
                        osyme = symeListHasExtendee(okSymes, syme);
                        if (osyme) {
                                symeSetExtension(osyme, syme);
                                okSymes = symeListExtend(okSymes, syme);
                                continue;
                        }

                        osyme = symeListHasExtension(okSymes, syme);
                        if (osyme) continue;
                        if (tiMergeSyme(syme, okSymes))
                            okSymes = listCons<Syme>(syme, okSymes);
                }
        }

        if (okSymes == NULL || cdr(okSymes) != NULL) {
                terrorNotUniqueMeaning(ALDOR_E_TinNMeanings, absyn, okSymes,
                                       allSymes, (sym).string(), type);
                listFree<Syme>(okSymes);
                listFree<Syme>(fsymes);
                return false;
        }

        syme = car(okSymes);
        listFree<Syme>(okSymes);
        listFree<Syme>(fsymes);

        if (abState(what) == AB_State_HasPoss)
                tpossFree(abTPoss(what));

        stabSetSyme(stab, what,  syme, abCondKnown);
        stabSetSyme(stab, absyn, syme, abCondKnown);
        abTUnique(what)  = symeType(syme);
        abTUnique(absyn) = symeType(syme);
        abSetState(what, AB_State_HasUnique);
        return true;
}

/***************************************************************************
 *
 * :: CoerceTo:  x :: T
 *
 ***************************************************************************/

local bool
titdnCoerceTo(Stab stab, AbSyn absyn, TForm type)
{
        TForm   tf = tiGetTForm(stab, absyn->abCoerceTo.type);

        if (!tfSatReturn(tf, type)) {
                terrorNotUniqueType(ALDOR_E_TinExprMeans,
                                    absyn, type, abTPoss(absyn));
                return false;
        }
        titdn0ApplySym(stab, absyn, tf, ssymCoerce, 1, abArgf, NULL);
        abTUnique(absyn) = tf;
        return true;
}

/****************************************************************************
 *
 * :: RestrictTo:   A @ B
 *
 ***************************************************************************/


local bool
titdnRestrictTo(Stab stab, AbSyn absyn, TForm type)
{
        TForm tf = tiGetTForm(stab, absyn->abRestrictTo.type);

        if (!tfSatReturn(tf, type)) {
                terrorNotUniqueType(ALDOR_E_TinExprMeans,
                                    absyn, type, abTPoss(absyn));
                return false;
        }
        titdn(stab, absyn->abRestrictTo.expr, tf);
        abTUnique(absyn) = tf;
        return true;
}


/****************************************************************************
 *
 * :: PretendTo:   A pretend B
 *
 ***************************************************************************/

local bool
titdnPretendTo(Stab stab, AbSyn absyn, TForm type)
{
        TForm tf = tiGetTForm(stab, absyn->abPretendTo.type);

        if (!tfSatReturn(tf, type)) {
                terrorNotUniqueType(ALDOR_E_TinExprMeans,
                                    absyn, type, abTPoss(absyn));
                return false;
        }
        titdn(stab, absyn->abPretendTo.expr, tfUnknown);
        abTUnique(absyn) = tf;
        return true;
}

/***************************************************************************
 *
 * :: Not: not a
 *
 ***************************************************************************/

local bool
titdnNot(Stab stab, AbSyn absyn, TForm type)
{
        /*
         * An unfixed compiler bug means that parts of Salli programs
         * tinfered with (tfBoolean == tfUnknown). We want to catch
         * this problem as soon as possible.
         */
        assert(tfBoolean != tfUnknown);

        if (!tfSatReturn(tfBoolean, type)) {
                terrorNotUniqueType(ALDOR_E_TinExprMeans,
                                    absyn, type, abTPoss(absyn));
                return false;
        }
        titdn(stab, absyn->abNot.expr, tfBoolean);
        abTUnique(absyn) = tfBoolean;
        return true;
}

/***************************************************************************
 *
 * :: And: a and b and c ...
 *
 ***************************************************************************/

local bool
titdnAnd(Stab stab, AbSyn absyn, TForm type)
{
        int     i;

        /*
         * An unfixed compiler bug means that parts of Salli programs
         * tinfered with (tfBoolean == tfUnknown). We want to catch
         * this problem as soon as possible.
         */
        assert(tfBoolean != tfUnknown);

        if (!tfSatReturn(tfBoolean, type)) {
                terrorNotUniqueType(ALDOR_E_TinExprMeans,
                                    absyn, type, abTPoss(absyn));
                return false;
        }

        for (i = 0; i < abArgc(absyn); i++)
                titdn(stab, abArgv(absyn)[i], tfBoolean);
        abTUnique(absyn) = tfBoolean;
        return true;
}

/***************************************************************************
 *
 * :: Or: a or b or c ...
 *
 ***************************************************************************/


local bool
titdnOr(Stab stab, AbSyn absyn, TForm type)
{
        int     i;

        /*
         * An unfixed compiler bug means that parts of Salli programs
         * tinfered with (tfBoolean == tfUnknown). We want to catch
         * this problem as soon as possible.
         */
        assert(tfBoolean != tfUnknown);

        if (!tfSatReturn(tfBoolean, type)) {
                terrorNotUniqueType(ALDOR_E_TinExprMeans,
                                    absyn, type, abTPoss(absyn));
                return false;
        }
        for (i = 0; i < abArgc(absyn); i++)
                titdn(stab, abArgv(absyn)[i], tfBoolean);
        abTUnique(absyn) = tfBoolean;
        return true;
}

/***************************************************************************
 *
 * :: Assert:
 *
 ***************************************************************************/

local bool
titdnAssert(Stab stab, AbSyn absyn, TForm type)
{
        bool ok;

        /* Ensure that the context requires no value */
        ok = titdn0NoValue(stab, absyn, type, ALDOR_E_TinContextAssert);

        /*
         * An unfixed compiler bug means that parts of Salli programs
         * tinfered with (tfBoolean == tfUnknown). We want to catch
         * this problem as soon as possible.
         */
        assert(!ok || (tfBoolean != tfUnknown));

        if (ok) titdn(stab, absyn->abAssert.test, tfBoolean);
        return ok;
}

/***************************************************************************
 *
 * :: Blank:
 *
 ***************************************************************************/

local bool
titdnBlank(Stab stab, AbSyn absyn, TForm type)
{
        return titdn0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Builtin:
 *
 ***************************************************************************/

local bool
titdnBuiltin(Stab stab, AbSyn absyn, TForm type)
{
        return titdn0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Default:
 *
 ***************************************************************************/

local bool
titdnDefault(Stab stab, AbSyn absyn, TForm type)
{
        TPoss   abtposs = abTPoss(absyn);
        TForm   rtype;

        rtype = tpossSelectSatisfier(abtposs, type);
        if (!rtype) return false;

        abTUnique(absyn) = rtype;
        return true;
}

/***************************************************************************
 *
 * :: Delay:
 *
 ***************************************************************************/

local bool
titdnDelay(Stab stab, AbSyn absyn, TForm type)
{
        return titdn0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Do:
 *
 ***************************************************************************/

local bool
titdnDo(Stab stab, AbSyn absyn, TForm type)
{
        bool ok;

        /* Ensure that the context requires no value */
        ok = titdn0NoValue(stab, absyn, type, ALDOR_E_TinContextDo);
        return titdn0Generic(stab, absyn, tfNone());
}

/***************************************************************************
 *
 * :: Except:
 *
 ***************************************************************************/

local bool
titdnExcept(Stab stab, AbSyn absyn, TForm type)
{
        titdn(stab, absyn->abExcept.except, tfTuple(tfCategory));
        if (!titdn(stab, absyn->abExcept.type, type))
                return false;
        if (abState(absyn->abExcept.type) != AB_State_HasUnique)
          return false;
        abTUnique(absyn) = abTUnique(absyn->abExcept.type);
        return true;
}

/***************************************************************************
 *
 * :: Raise:
 *
 ***************************************************************************/

local bool
titdnRaise(Stab stab, AbSyn absyn, TForm type)
{
        TForm tf;
        Sefo sef;
        titdn(stab, absyn->abRaise.expr, tfDomain);
#if AXL_EDIT_1_1_13_10
        tf = tiGetTForm(stab, absyn->abRaise.expr);
        sef = tfGetExpr(tf);
        assert(sef);
        tf = tfExcept(tfExit, abGetCategory(sef));
#else
        tf = tfExcept(tfExit, tiGetTForm(stab, absyn->abRaise.expr));
#endif
        if (!tfSatReturn(tf, type)) {
                /* !!This is the _wrong_ routine to call */
                terrorNotUniqueType(ALDOR_E_TinExprMeans, absyn, type,
                                    tpossSingleton(tf));
                return false;
        }
        abTUnique(absyn) = tf;
        return true;
}

/***************************************************************************
 *
 * :: Export:
 *
 ***************************************************************************/

local bool
titdnExport(Stab stab, AbSyn absyn, TForm type)
{
        abTUnique(absyn) = tfNone();
        return true;
}

/***************************************************************************
 *
 * :: Extend:
 *
 ***************************************************************************/

local bool
titdnExtend(Stab stab, AbSyn absyn, TForm type)
{
        return titdn0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Fix:
 *
 ***************************************************************************/

local bool
titdnFix(Stab stab, AbSyn absyn, TForm type)
{
        return titdn0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Fluid:
 *
 ***************************************************************************/

local bool
titdnFluid(Stab stab, AbSyn absyn, TForm type)
{
        return titdn0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Free:
 *
 ***************************************************************************/

local bool
titdnFree(Stab stab, AbSyn absyn, TForm type)
{
        return titdn0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Has:
 *
 ***************************************************************************/

local bool
titdnHas(Stab stab, AbSyn absyn, TForm type)
{
        return titdn0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Hide:
 *
 ***************************************************************************/

local bool
titdnHide(Stab stab, AbSyn absyn, TForm type)
{
        return titdn0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: IdSy:
 *
 ***************************************************************************/

local bool
titdnIdSy(Stab stab, AbSyn absyn, TForm type)
{
        return titdn0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Local:
 *
 ***************************************************************************/

local bool
titdnLocal(Stab stab, AbSyn absyn, TForm type)
{
        return titdn0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Macro:
 *
 ***************************************************************************/

local bool
titdnMacro(Stab stab, AbSyn absyn, TForm type)
{
        return titdn0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: MLambda:
 *
 ***************************************************************************/

local bool
titdnMLambda(Stab stab, AbSyn absyn, TForm type)
{
        return titdn0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Nothing:
 *
 ***************************************************************************/

local bool
titdnNothing(Stab stab, AbSyn absyn, TForm type)
{
        abTUnique(absyn) = tfNone();
        return true;
}

/***************************************************************************
 *
 * :: Quote:
 *
 ***************************************************************************/

local bool
titdnQuote(Stab stab, AbSyn absyn, TForm type)
{
        return titdn0Generic(stab, absyn, type);
}

/***************************************************************************
 *
 * :: Reference: ref id
 *
 ***************************************************************************/

local bool
titdnReference(Stab stab, AbSyn absyn, TForm type)
{
        AbSyn   body = absyn -> abReference.body;
        TForm   inner;


        /* What is the argument to the reference? */
        if (tfIsReference(type))
                inner = tfReferenceArg(type);
        else
                inner = tfUnknown;


        /* Continue type inference to the leaves */
        titdn(stab, body, tfUnknown);


#if AXL_EDIT_1_1_12p6_21
        /* Return now if an error has occurred */
        if (abState(absyn) == AB_State_Error) return false;


        /* Fix our type */
        abTUnique(absyn) = tfReference(inner);
        return true;
#else
        /* Fix our type */
        abTUnique(absyn) = tfReference(inner);
        return (abState(absyn) != AB_State_Error);
#endif
}

/***************************************************************************
 *
 * :: Select:
 *
 ***************************************************************************/

local bool
titdnSelect(Stab stab, AbSyn absyn, TForm type)
{
        Fluid fluidBinding_tuniExitTForm(tuniExitTForm);
        Fluid fluidBinding_abExitsList(abExitsList);
        Fluid fluidBinding_tuniSelectObj(tuniSelectObj);
        AbSyn   seq;
        bool result;

        tuniExitTForm = type;
        abExitsList   = listNil<AbSyn>;
        tuniSelectObj = absyn->abSelect.testPart;

        titdn(stab, absyn->abSelect.testPart, tfUnknown);

        seq = absyn->abSelect.alternatives;

        result = titdnSequence0(stab, absyn->abSelect.alternatives, type);

        if (result)
                abTUnique(absyn) = abTUnique(absyn->abSelect.alternatives);
        return result;
}

/***************************************************************************
 *
 * :: Try:
 *
 ***************************************************************************/

local bool
titdnTry(Stab stab, AbSyn absyn, TForm type)
{
        AInt    i, argc;
        TForm   inner;
        AbEmbed embed;
        AbSyn   expr, seq, nuttin, *argv;

        extern void tibup(Stab, AbSyn, TForm);


        /* Try-blocks with no value are tricky */
        if (tfIsNone(type)) {
                /* (void)fprintf(dbOut, "*** Ick: empty multi.\n"); */
                expr = absyn->abTry.expr;


                /* We want to force a void return value */
                if(abHasTag(expr, AB_Sequence)) {
                        argc = abArgc(expr);
                        argv = abArgv(expr);
                }
                else {
                        argc = 1;
                        argv = &expr;
                }


                /* Create a new sequence */
                seq = abNewEmpty(AB_Sequence, argc + 1);


                /* Copy across the old members */
                for (i = 0;i < argc; i++)
                        abArgv(seq)[i] = argv[i];


                /* Create a void value and type infer */
                nuttin = abNewNothing(abPos(expr));
                tibup(stab, nuttin, tfNone());


                /* Append the void to the sequence */
                abArgv(seq)[argc] = nuttin;


                /* We require the type to be () */
                abUse(seq)   = abUse(expr);
                abSetState(seq, AB_State_HasPoss);
                abTPoss(seq) = tpossSingleton(tfNone());


                /* Update the try-expression */
                absyn->abTry.expr = seq;
        }
        else if (tfIsMulti(type)) {
                /* (void)fprintf(dbOut, "*** Yum: hot-cross multi!\n"); */
                type = tfCrossFrMulti(type);
        }


        /* Finish the type inference on this node */
        titdn(stab, absyn->abTry.id, tfUnknown);
        titdn(stab, absyn->abTry.expr, tfIgnoreExceptions(type));
        titdn(stab, absyn->abTry.always, tfNone());
        if (!abIsNothing(absyn->abTry.except))
                titdn(stab, absyn->abTry.except, type);


        /* Embed multi-valued try-blocks in a Cross */
        if (abState(absyn->abTry.expr) == AB_State_HasUnique) {
                inner = abTUnique(absyn->abTry.expr);

                if (tfAsMultiArgc(inner) > 1) {
                        embed = tfSatEmbedType(inner, type);
                        if (!tfIsNone(type) && (embed != AB_Embed_Identity))
                                abAddTContext(absyn->abTry.expr, embed);
                }
        }

        abTUnique(absyn) = type;
        return true;
}

/***************************************************************************
 *
 * :: Let:
 *
 ***************************************************************************/

local bool
titdnLet(Stab stab, AbSyn absyn, TForm type)
{
        return titdn0Generic(stab, absyn, type);
}


/**************************************************************************
 * titdnError: permforms a recursive discendent visit of parse tree
 * looking for nodes with state == AB_State_Error and giving error
 * message.
 * Note: every error find in bup process should be handled here.
 **************************************************************************/

local void      titdn0ErrorSequence(Stab stab, AbSyn ab, TForm type);

/* Call titdnError on each subtree using .type. as constraint type */
local void
titdn0Error(Stab stab, AbSyn absyn, TForm type)
{
        int     i;
        int argc = abArgc(absyn);

        for (i = 0; i < argc; i++) {
                AbSyn   argi = abArgv(absyn)[i];
                titdnError(stab, argi, type);
        }
}

local void
titdnError(Stab stab, AbSyn absyn, TForm type)
{
        tfFollow(type);

        if (!abIsLeaf(absyn) && abStab(absyn))
                stab = abStab(absyn);

        if (abState(absyn) == AB_State_Error) {
                bool    exit = true;
                bool    result = false;

                if (abTag(absyn) == AB_Sequence)
                        result = titdnSequence(stab, absyn, type);
                else if (abIsAnyLambda(absyn))
                        result = titdnLambda(stab, absyn, type);
                else if (abTag(absyn) == AB_Generate)
                        result = titdnGenerate(stab, absyn, type);
                else
                        exit = !terror(stab, absyn, type);

                /*assert(!result);*/
                if (result) abSetState(absyn, AB_State_HasUnique);

                if (exit) return;
        }

        if (!abIsLeaf(absyn)) {

                        /* Determine the constraint types */
                switch(abTag(absyn)) {
                case AB_Declare:
                        {
                        TForm tf = abTForm(absyn->abDeclare.type);
                        titdnError(stab, absyn->abDeclare.id, tf);
                }
                        break;
                case AB_Lambda:
                case AB_PLambda: {
                        TForm   tf    = tiGetTForm(stab,absyn->abLambda.rtype);
                        titdnError(stab, absyn->abLambda.param, tfUnknown);
                        titdnError(stab, absyn->abLambda.body,  tf);
                }
                        break;
                case AB_With:
                        titdnError(stab,absyn->abWith.base, tfCategory);
                        titdnError(stab,absyn->abWith.within, tfUnknown);
                        break;
                case AB_Where:
                        titdnError(stab,absyn->abWhere.context, tfUnknown);
                        titdnError(stab,absyn->abWhere.expr, type);
                        break;
                case AB_If:
                        assert(tfBoolean != tfUnknown);
                        titdnError(stab, absyn->abIf.test,     tfBoolean);
                        titdnError(stab, absyn->abIf.thenAlt,  type);
                        titdnError(stab, absyn->abIf.elseAlt, type);
                        break;
                case AB_RestrictTo: {
                        TForm tf = tiGetTForm(stab, absyn->abRestrictTo.type);
                        titdnError(stab, absyn->abRestrictTo.expr, tf);
                }
                        break;
                case AB_For:
                        titdnError(stab, absyn->abFor.whole, tfUnknown);
                        assert(tfBoolean != tfUnknown);
                        titdnError(stab, absyn->abFor.test, tfBoolean);
                        titdnError(stab, absyn->abFor.lhs, tfUnknown);
                        break;
                case AB_Foreign:
                        titdnError(stab, absyn->abForeign.what, tfUnknown);
                        break;
                case AB_Import:
                        titdnError(stab, absyn->abImport.what, tfUnknown);
                        break;
                case AB_Inline:
                        titdnError(stab, absyn->abInline.what, tfUnknown);
                        break;
                case AB_Sequence:
                        titdn0ErrorSequence(stab, absyn, type);
                        break;
                case AB_PretendTo:
                case AB_Do:
                case AB_Repeat:
                case AB_Collect:
                case AB_Generate:
                case AB_Return:
                case AB_Exit:
                case AB_Comma:
                case AB_Apply:
                        titdn0Error(stab, absyn, tfUnknown);
                        break;
                case AB_Not:
                case AB_And:
                case AB_Or:
                case AB_Has:
                case AB_While:
                        assert(tfBoolean != tfUnknown);
                        titdn0Error(stab, absyn, tfBoolean);
                        break;
                default:
                        titdn0Error(stab, absyn, type);
                        break;
                }
        }
        return;
}


/* NB: We should also handle 'exit' cases, and similar with
 * generate and return (probably)
 */
local void
titdn0ErrorSequence(Stab stab, AbSyn absyn, TForm type)
{
        TForm  none = tfNone();
        int     i;
        int argc = abArgc(absyn);

        if (argc == 0)
                return;

        for (i = 0; i < argc-1; i++) {
                AbSyn   argi = absyn->abSequence.argv[i];
                titdnError(stab, argi, none);
        }
        titdnError(stab, absyn->abSequence.argv[i], type);
}
