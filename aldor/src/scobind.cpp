///////////////////////////////////////////////////////////////////////////////
//
// scobind.cpp: Deduce the scopes of identifiers.
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

#include "axlphase.h"

bool    scoDebug                = false;
bool    scoStabDebug            = false;
bool    scoFluidDebug           = false;
bool    scoUndoDebug            = false;

#define scoDEBUG(s)             DEBUG_IF(scoDebug, s)
#define scoStabDEBUG(s)         DEBUG_IF(scoStabDebug, s)
#define scoFluidDEBUG(s)        DEBUG_IF(scoFluidDebug, s)
#define scoUndoDEBUG(s)         DEBUG_IF(scoUndoDebug, s)

/******************************************************************************
 *
 * :: Identifier contexts
 *
 *****************************************************************************/

enum idContext {
    SCO_Id_START,
        SCO_Id_InType = SCO_Id_START,
        SCO_Id_Label,
        SCO_Id_Used,
    SCO_Id_LIMIT
};

typedef enum idContext         IdContext;

static String IdContextNames[] = {
        "InType",
        "Label",
        "Used"
};

/******************************************************************************
 *
 * :: Declaration contexts
 *
 *****************************************************************************/

enum declContext {
    SCO_Sig_START,
        SCO_Sig_Assign = SCO_Sig_START,
        SCO_Sig_Builtin,
        SCO_Sig_Declare,
        SCO_Sig_Default,
        SCO_Sig_Define,
        SCO_Sig_DDefine,
        SCO_Sig_Export,
        SCO_Sig_Extend,
        SCO_Sig_Fluid,
        SCO_Sig_Foreign,
        SCO_Sig_Free,
        SCO_Sig_FreeRef,
        SCO_Sig_ImplicitLocal,
        SCO_Sig_Import,
        SCO_Sig_Inline,
        SCO_Sig_InType,
        SCO_Sig_Local,
        SCO_Sig_Loop,
        SCO_Sig_Param,
        SCO_Sig_Used,
        SCO_Sig_Reference,
        SCO_Sig_Value,
    SCO_Sig_LIMIT
};

typedef enum declContext       DeclContext;

static String DeclContextNames[] = {
        "Assign",
        "Builtin",
        "Declare",
        "Default",
        "Define",
        "DDefine",
        "Export",
        "Extend",
        "Fluid",
        "Foreign",
        "Free",
        "FreeReference",
        "ImplicitLocal",
        "Import",
        "Inline",
        "InType",
        "Local",
        "Loop",
        "Param",
        "Used",
        "Referenced",
        "Value"
};

/******************************************************************************
 *
 * :: Declaration information
 *
 *****************************************************************************/

typedef List<AInt>        DefnPos;


typedef struct decl_info {
        UShort          intStepNo;
        AbSyn           id;
        AbSyn           type;
        List<DefnPos>     defpos;
        Doc             doc;
        AbSyn           uses[SCO_Sig_LIMIT];
}      *DeclInfo;


/******************************************************************************
 *
 * :: Identifier information
 *
 *****************************************************************************/

typedef struct id_info {
        BPack(bool)     allFree;
        BPack(bool)     allLocal;
        BPack(bool)     allFluid;
        ULong           serialNo;
        UShort          intStepNo;
        Symbol          sym;
        AbSyn           uses[SCO_Id_LIMIT];
        List<DeclInfo>    declInfoList;
        AbSyn           defaultType;
        AbSyn           exampleId;
        List<AbSyn>       usePreDef;
}      *IdInfo;


/******************************************************************************
 *
 * :: Conditional Definitions
 *
 *****************************************************************************/
local DefnPos   defposTail      (DefnPos pos);
local bool      defposEqual     (DefnPos a, DefnPos b);
local bool      defposIsRoot    (DefnPos pos);

#define defposTop(posn) ((AbSyn)  (car(posn) & ~1))
#define defposTopIsThenPart(posn) (car(posn) & 1)

/******************************************************************************
 *
 * :: Static variables
 *
 *****************************************************************************/

static Stab             scoStab;
static bool             scoIsInType;
static bool             scoIsInAdd;
static bool             scoIsInExport;
static bool             scoIsInExtend;
static bool             scoIsInImport;
static List<AbSyn>        scoDefineList;
static List<AbSyn>        scoLambdaList;
static List<IdInfo>       scoIdInfoList;
static bool             scoUndoState;
static List<Syme>         scoUndoSymes;
static List<Syme>         scoRetiredSymes;
static List<TForm>        scoUndoTForms;
static List<TForm>        scoRetiredTForms;
static DefnPos          scoCondList;
static int              scoDefCounter;

/******************************************************************************
 *
 * :: Local operations
 *
 *****************************************************************************/

typedef void            (*ScoBindFun)           (AbSyn);

local void              scobindValue            (AbSyn);
local void              scobindContext          (AbSyn);
local void              scobindType             (AbSyn);
local void              scobindLevel            (AbSyn, ScoBindFun, ULong);

local void              scobindLambdaList       (void);
local void              scobindPushDefine       (AbSyn);
local void              scobindPopDefine        (AbSyn);
local void              scobindSetStab          (AbSyn, ULong);
local void              scobindStabHash         (AbSyn, Stab);

local void              scobindApply            (AbSyn);
local void              scobindAssign           (AbSyn);
local void              scobindBuiltin          (AbSyn);
local void              scobindDeclare          (AbSyn);
local void              scobindDefault          (AbSyn);
local void              scobindDefine           (AbSyn);
local void              scobindDDefine          (AbSyn);
local void              scobindExport           (AbSyn);
local void              scobindExtend           (AbSyn);
local void              scobindFluid            (AbSyn);
local void              scobindFor              (AbSyn);
local void              scobindForeign          (AbSyn);
local void              scobindFree             (AbSyn);
local void              scobindImport           (AbSyn);
local void              scobindInline           (AbSyn);
local void              scobindLocal            (AbSyn);
local void              scobindReference        (AbSyn);
local void              scobindParam            (AbSyn);
local void              scobindTry              (AbSyn);
local void              scobindIf               (AbSyn);

local void              scobindApplySelf        (AbSyn);
local bool              scobindApplyNeedsSelf   (AbSyn);
local bool              scobindApplyNeedsScope  (AbSyn);
local bool              scobindApplyArgNeedsScope(AbSyn);
local void              scobindApplyScope       (AbSyn);
local void              scobindApplyParam       (AbSyn);
local void              scobindApplyComma       (AbSyn);
local void              scobindApplyArg         (AbSyn);
local void              scobindAssignDeclare    (AbSyn);
local void              scobindAssignId         (AbSyn, AbSyn);
local void              scobindBuiltinDeclare   (AbSyn);
local void              scobindBuiltinId        (AbSyn, AbSyn);
local void              scobindDefaultDeclare   (AbSyn);
local void              scobindDefaultId        (AbSyn, AbSyn);
local void              scobindDefineDeclare    (AbSyn, AbSyn);
local void              scobindDefineId         (AbSyn, AbSyn, AbSyn);
local void              scobindDefineRhs        (AbSyn, AbSyn, DeclContext);
local void              scobindDDefineComma     (AbSyn);
local void              scobindDDefineDeclare   (AbSyn, AbSyn);
local void              scobindDDefineId        (AbSyn, AbSyn, AbSyn);
local void              scobindExportTo         (AbSyn, AbSyn);
local void              scobindExportFrom       (AbSyn, AbSyn);
local void              scobindExportWhat       (AbSyn);
local void              scobindExportDeclare    (AbSyn, AbSyn);
local void              scobindExportId         (AbSyn, AbSyn, AbSyn);
local void              scobindExtendDeclare    (AbSyn, AbSyn);
local void              scobindExtendId         (AbSyn, AbSyn, AbSyn);
local Syme              scobindGetExtend        (AbSyn, AbSyn);
local void              scobindFluidComma       (AbSyn);
local void              scobindFluidDeclare     (AbSyn);
local void              scobindFluidId          (AbSyn, AbSyn);
local void              scobindFor0             (AbSyn, bool);
local void              scobindForDeclare       (AbSyn, AbSyn);
local void              scobindForId            (AbSyn, AbSyn);
local void              scobindForeignDeclare   (AbSyn, ForeignOrigin);
local void              scobindForeignId        (AbSyn, AbSyn, ForeignOrigin);
local void              scobindParamDefine      (AbSyn, AbSyn);
local void              scobindParamDeclare     (AbSyn, AbSyn);
local void              scobindParamId          (AbSyn, AbSyn, AbSyn);

/*
 * scobindDeclare
 */
local DeclInfo  scobindDeclareId        (AbSyn, AbSyn, AbSyn, DeclContext);
local TForm     scobindDeclareTForm     (Stab,  AbSyn, AbSyn, AbSyn, DeclContext);
local void      scobindUniqifyDecl      (AbSyn, AbSyn);
local void      scobindIntroduceId      (AbSyn, AbSyn, DeclContext);
local AbSyn     scobindGuessType        (AbSyn);

/*
 * scobindFree
 * scobindLocal
 */
local void              scobindLOF              (AbSyn, DeclContext);
local void              scobindLOFComma         (AbSyn, AbSyn, DeclContext);
local void              scobindLOFDeclare       (AbSyn, AbSyn, DeclContext);
local void              scobindLOFId            (AbSyn, DeclContext);
local void              scobindLOFType          (AbSyn, AbSyn, AbSyn,
                                                 DeclContext);
local void              scobindLOFDeclInfo      (AbSyn, IdInfo, DeclInfo,
                                                 DeclContext);

/*
 * Properties of different lexical levels.
 */
#define                 scobindApplyFlags       0
#define                 scobindLambdaFlags      0
#define                 scobindAddFlags         STAB_LEVEL_LARGE
#define                 scobindWithFlags        STAB_LEVEL_LARGE
#define                 scobindWhereFlags       STAB_LEVEL_WHERE
#define                 scobindRepeatFlags      STAB_LEVEL_LOOP
#define                 scobindCollectFlags     STAB_LEVEL_LOOP

/*
 * scobindIf (and conditions)
 */
local void      scobindIf       (AbSyn);
local void      scoCondPush     (AInt);
local void      scoCondPop      (void);
local List<AInt>  scoConditions   (void);

/*
 * IdInfo
 */
local IdInfo            idInfoNew               (Stab, AbSyn);
local void              idInfoFree              (IdInfo);
local bool              idInfoIsNew             (IdInfo);
local void              scobindFreeIdInfo       (Stab);
local List<IdInfo>        scobindSaveIdInfo       (Stab);
local void              scobindRestoreIdInfo    (Stab, List<IdInfo>, bool);
local IdInfo            getIdInfoInAnyScope     (Stab, AbSyn);
local IdInfo            getIdInfoInThisScope    (Stab, Symbol);
local void              scobindSetIdUse         (IdInfo, IdContext, AbSyn);
local AbSyn             scobindDefaultType      (Stab, Symbol);

#define                 idInfoCell(x)           \
        ((List<IdInfo>) (symCoInfo(x)->phaseVal.generic))

#define                 setIdInfoCell(x,y)      \
        (symCoInfo(x)->phaseVal.generic = (void *) (y))

/*
 * DeclInfo
 */
local DeclInfo          declInfoNew             (AbSyn, AbSyn, DefnPos);
local void              declInfoFree            (DeclInfo);
local bool              declInfoIsNew           (DeclInfo);
local bool              declInfoUseIsNew        (AbSyn);
local bool              declInfoIsImplicitLocal (DeclInfo);
local void              scobindRestoreDeclInfo  (IdInfo);
local DeclInfo          idInfoHasType           (IdInfo, AbSyn);
local DeclInfo          idInfoAddType           (IdInfo, AbSyn, AbSyn, AbSyn, List<AInt>);
local void              scobindSetSigUse        (DeclInfo, DeclContext, AbSyn);
local bool              scobindCheckDefnPos     (DeclInfo, DefnPos);

/*
 * scobindAddMeaning
 */
local void              scobindAddMeaning       (AbSyn, Symbol, Stab,
                                                 SymeTag, TForm, AInt);
local bool              scobindNeedsMeaning     (AbSyn, TForm);
local void              scobindSetMeaning       (AbSyn, Syme);
local Syme              scobindDefMeaning       (Stab, SymeTag, Symbol,
                                                 TForm, AInt);

/*
 * scobindReconcile
 */
local void              scobindReconcile        (Stab,AbSynTag);
local void              scobindReconcileId      (Stab,AbSynTag,Symbol,IdInfo);
local void              scobindReconcileDecls   (Stab,AbSynTag,Symbol,IdInfo);
local void              scobindReconcileDecl    (Stab,AbSynTag,Symbol,IdInfo,
                                                 DeclInfo);

/*
 * scobindPrint
 */
extern void             scobindPrint            (Stab);
local void              scobindPrintStab        (Stab);
local void              scobindPrintId          (Symbol);
local void              scobindPrintIdInfo      (IdInfo);
local void              scobindPrintDeclInfo    (Length, DeclInfo);

/*
 * scobindUndo
 */
local void              scobindRestore          (void);
local void              scobindSave             (void);
local void              scobindUndo             (void);
local void              scoUndoStab             (Stab);
local void              scoUndoStabTree         (Stab);
local void              scoUndoStabLevel        (StabLevel);
local bool              scoUndoStabEntry        (StabEntry);
local void              scoUndoSyme             (Syme);
local void              scoUndoTForm            (TForm);
local void              scoUndoTFormUses        (TFormUses);
local void              scoUndoExtendExtendees  (StabLevel);
local void              scoRestoreExtendees     (List<Syme> *, Syme);
local List<Syme>        scoUndoBoundSymes       (List<Syme>);
local bool              isNewSyme               (Syme);
local bool              isNewTForm              (TForm);
local bool              isNewTFormUses          (TFormUses);


/*!! This needs to be cleaned up. */
local void        markOuterInstanceOfFree     (AbSyn, AbSyn, DeclContext);

local void        checkOuterUseOfImplicitLocal   (Stab, AbSyn, AbSyn);
local void        checkOuterUseOfLexicalConstant (Stab, AbSyn);
local bool        scobindCheckOuterUseOfFluid    (AbSyn, AbSyn);

local void        scobindUsedType             (Stab, AbSyn);
local void        scobindImportType           (Stab, AbSyn);

local void        scobindMatchWiths           (AbSyn, AbSyn, DeclContext);
local bool        scobindMatchParams          (AbSyn, AbSyn);
local bool        scobindMatchParam           (AbSyn, AbSyn);

local void        scobindAdd                  (AbSyn);
local void        scobindCollect              (AbSyn);
local void        scobindHas                  (AbSyn);
local void        scobindId                   (AbSyn, IdContext);
local void        scobindLabel                (AbSyn);
local void        scobindLambda               (AbSyn);
local void        scobindRepeat               (AbSyn);
local void        scobindWhere                (AbSyn);
local void        scobindWith                 (AbSyn);

local bool        scobindTFormMustBeUnique      (AbSyn ab);

#define abSetTFormCond(a,t) if (! abTForm(a)) abSetTForm((a), (t))

#if 1
#define scobindRetNeedsDefn(ab) \
        (abIsTheId(ab, ssymCategory) || abHasTag(ab, AB_With))

#define scobindMapNeedsDefn(ab) \
        (abIsAnyMap(ab) && scobindRetNeedsDefn(abMapRet(ab)))
#else
local bool
scobindRetNeedsDefn(AbSyn ab)
{
        if (abIsTheId(ab, ssymCategory) || abHasTag(ab, AB_With))
                return true;

        return false;
}

local bool
scobindMapNeedsDefn(AbSyn ab)
{
        AbSyn ret;
        if (!abIsAnyMap(ab))
                return false;
        ret = abMapRet(ab);

        if (abIsAnyMap(ret))
                return scobindMapNeedsDefn(ret);

        return scobindRetNeedsDefn(ret);

}

#endif
local bool
scobindTFormMustBeUnique(AbSyn ab)
{
        if (abIsAnyMap(ab))
                return scobindTFormMustBeUnique(abMapRet(ab));

        return abIsTheId(ab,ssymCategory);
}


/******************************************************************************
 *
 * :: scopeBind         Top-level external entry points.
 *
 *****************************************************************************/

void
scobindInitGlobal(void)
{
        scoRetiredSymes = listNil<Syme>;
        scoRetiredTForms = listNil<TForm>;
}

void
scobindFiniGlobal(void)
{
        /* Retired TForms contain non-owning Syme pointers.  Dispose of their
         * owned syntax/list cells before reclaiming the retired Symes. */
        listFreeDeeply<TForm>(scoRetiredTForms, tfFree);
        scoRetiredTForms = listNil<TForm>;
        listFreeDeeply<Syme>(scoRetiredSymes, symeFree);
        scoRetiredSymes = listNil<Syme>;
}

void
scobindInitFile(void)
{
        scoIdInfoList = listNil<IdInfo>;
        scoUndoState  = false;
        scoDefCounter = 1;
}

void
scobindFiniFile(void)
{
        listFreeDeeply<IdInfo>(scoIdInfoList, idInfoFree);
        scoIdInfoList = listNil<IdInfo>;
        scoUndoState = false;
}

int
scobindMaxDef()
{
        return scoDefCounter;
}

void
scoSetUndoState(void)
{
        scoUndoState = true;
}

void
scopeBind(Stab stab, AbSyn absyn)
{
        scoStab         = stab;
        scoIsInType     = false;
        scoIsInAdd      = false;
        scoIsInExtend   = false;
        scoIsInExport   = false;
        scoIsInImport   = false;
        scoDefineList   = listNil<AbSyn>;
        scoLambdaList   = listNil<AbSyn>;

        scoDEBUG({
                fprintf(dbOut, "Top-level Scope Begin");
                findent += 2;
                fnewline(dbOut);
        });

        scobindRestore();

        scobindValue(absyn);
        scobindLambdaList();
        scobindReconcile(scoStab, AB_Add);

        scobindSave();

        scoDEBUG({
                findent -= 2;
                fnewline(dbOut);
                scobindPrint(scoStab);
                scoStabDEBUG(stabPrint(dbOut, scoStab));
                fnewline(dbOut);
                fprintf(dbOut, "Top-level Scope End\n\n");
                fnewline(dbOut);
        });
}

local void
scobindRestore()
{
        if (scoIdInfoList) {
                scobindRestoreIdInfo(scoStab, scoIdInfoList, scoUndoState);
                scoIdInfoList = listNil<IdInfo>;

                if (scoUndoState) scobindUndo();
        }
}

local void
scobindSave()
{
        if (fintMode == FINT_LOOP)
                scoIdInfoList = scobindSaveIdInfo(scoStab);
        else
                scobindFreeIdInfo(scoStab);
}

/******************************************************************************
 *
 * :: scobindValue      Top-level recursive entry point.
 *
 *****************************************************************************/

local void
scobindValue(AbSyn absyn)
{
        switch (abTag(absyn)) {
        case AB_Nothing:
                break;

        case AB_Assign:
        case AB_Builtin:
        case AB_Declare:
        case AB_Default:
        case AB_Define:
        case AB_DDefine:
        case AB_Export:
        case AB_Extend:
        case AB_Fluid:
        case AB_For:
        case AB_Foreign:
        case AB_Free:
        case AB_Import:
        case AB_Inline:
        case AB_Local:
                scobindContext(absyn);
                break;

        case AB_Add:
                scobindLevel(absyn, scobindAdd, scobindAddFlags);
                break;

        case AB_Apply:
                scobindApply(absyn);
                break;

        case AB_CoerceTo:
                scobindValue(absyn->abCoerceTo.expr);
                scobindUsedType(scoStab, absyn->abCoerceTo.type);
                break;

        case AB_Collect:
                scobindLevel(absyn, scobindCollect, scobindCollectFlags);
                break;

        case AB_Fix:
                bugUnimpl("scoBind");

        case AB_Has:
                scobindHas(absyn);
                break;

        case AB_Hide:
                scobindUsedType(scoStab, absyn->abHide.type);
                abSetTFormCond(absyn, abTForm(absyn->abHide.type));
                break;

        case AB_Id:
                scobindId(absyn, SCO_Id_Used);
                break;

        case AB_Label:
                scobindLabel(absyn->abLabel.label);
                scobindValue(absyn->abLabel.expr);
                break;

        case AB_Lambda:
        case AB_PLambda:
                scobindLevel(absyn, scobindLambda, scobindLambdaFlags);
                break;

        case AB_PretendTo:
                scobindValue(absyn->abPretendTo.expr);
                scobindUsedType(scoStab, absyn->abPretendTo.type);
                break;

        case AB_Qualify:
                scobindValue(absyn->abQualify.what);
                scobindUsedType(scoStab, absyn->abQualify.origin);
                break;

        case AB_Reference:
                scobindReference(absyn->abReference.body);
                break;

        case AB_Repeat:
                scobindLevel(absyn, scobindRepeat, scobindRepeatFlags);
                break;

        case AB_RestrictTo:
                scobindValue(absyn->abRestrictTo.expr);
                scobindUsedType(scoStab, absyn->abRestrictTo.type);
                break;
        case AB_Where:
                scobindLevel(absyn, scobindWhere, scobindWhereFlags);
                break;
        case AB_Try:
                scobindTry(absyn);
                break;

        case AB_With:
                if (!abStab(absyn))      /* check if already done */
                        scobindLevel(absyn, scobindWith, scobindWithFlags);
                break;

        case AB_If:
                scobindIf(absyn);
                break;

        default:
                if (!abIsLeaf(absyn)) {
                        Length          i, argc = abArgc(absyn);
                        AbSyn           *argv = abArgv(absyn);

                        for (i = 0; i < argc; i += 1)
                                scobindValue(argv[i]);
                }
                break;
        }
}

/******************************************************************************
 *
 * :: scobindContext    Dispatch function for signature introducing contexts.
 *
 *****************************************************************************/

local void
scobindContext(AbSyn absyn)
{
        switch (abTag(absyn)) {
        case AB_Assign:
                scobindAssign(absyn);
                break;

        case AB_Builtin:
                scobindBuiltin(absyn);
                break;

        case AB_Declare:
                scobindDeclare(absyn);
                break;

        case AB_Default:
                scobindDefault(absyn);
                break;

        case AB_Define:
                scobindDefine(absyn);
                break;

        case AB_DDefine:
                scobindDDefine(absyn);
                break;

        case AB_Export:
                scobindExport(absyn);
                break;

        case AB_Extend:
                scobindExtend(absyn);
                break;

        case AB_Fluid:
                scobindFluid(absyn);
                break;

        case AB_For:
                scobindFor(absyn);
                break;

        case AB_Foreign:
                scobindForeign(absyn);
                break;

        case AB_Free:
                scobindFree(absyn);
                break;

        case AB_Import:
                scobindImport(absyn);
                break;

        case AB_Inline:
                scobindInline(absyn);
                break;

        case AB_Local:
                scobindLocal(absyn);
                break;

        default:
                bugBadCase(abTag(absyn));
                break;
        }
}

/******************************************************************************
 *
 * :: scobindType       Top-level entry point for types.
 *
 *****************************************************************************/

local void
scobindType(AbSyn type)
{
        bool    save = scoIsInType;

        scoIsInType = true;
        scobindValue(type);
        scoIsInType = save;
}

/******************************************************************************
 *
 * :: scobindLevel      Apply 'fun' within a new scope level.
 *
 *****************************************************************************/

local void
scobindLevel(AbSyn absyn, ScoBindFun fun, ULong flags)
{
        List<AbSyn>       savedLambdaList;
        List<AInt>        savedCondList;
        bool            saveInType = scoIsInType;


        assert(absyn);

        if (abIsNothing(absyn))
                return;

        savedLambdaList = scoLambdaList;
        savedCondList   = scoCondList;

        scoLambdaList = listNil<AbSyn>;
        scoCondList   = listNil<AInt>;

        /*
         * If we start a new scope level then we are no longer in a
         * type. This fixes bugs such as bug 988 where we used to give
         * spurious errors about variables being used in constants.
         * However, this hides some instances where a variable is used
         * in a type. The simplest example of this is as follows: given
         * a domain constructor Foo(X:SingleInteger),
         *
         *      local myVar:SingleInteger := 42;
         *      import from Foo(X == myVar);
         *
         * We compute the type Foo(X) before giving X the value of myVar.
         * This is probably one area where variables can be used in a
         * type because the definition nails the value into a constant.
         */
        scoIsInType   = false;

        scobindSetStab(absyn, flags);
        scoStab = abStab(absyn);

        scoDEBUG({
                fprintf(dbOut, "%s Scope Begin (# %lu)",
                        abInfo(abTag(absyn)).str, car(scoStab)->serialNo);
                findent += 2;
                fnewline(dbOut);
        });

        fun(absyn);
        scobindLambdaList();
        scobindReconcile(scoStab, abTag(absyn));

        scoDEBUG({
                scobindPrint(scoStab);
                scoStabDEBUG(stabPrintTo(dbOut, scoStab, -1));
                findent -= 2;
                fnewline(dbOut);
                fprintf(dbOut, "%s Scope End (# %lu)",
                        abInfo(abTag(absyn)).str, car(scoStab)->serialNo);
                fnewline(dbOut);
        });

        scobindFreeIdInfo(scoStab);

        scoStab       = stabPopLevel(scoStab);
        scoCondList   = savedCondList;
        scoLambdaList = savedLambdaList;
        scoIsInType   = saveInType;
}

local void
scobindLambdaList(void)
{
        scoLambdaList = listNReverse<AbSyn>(scoLambdaList);
        while (scoLambdaList) {
                AbSyn   lhs = car(scoLambdaList);
                AbSyn   rhs = car(cdr(scoLambdaList));

                scobindPushDefine(lhs);
                scobindLevel(rhs, scobindLambda, scobindLambdaFlags);
                scobindPopDefine(lhs);

                scoLambdaList = listFreeCons<AbSyn>(scoLambdaList);
                scoLambdaList = listFreeCons<AbSyn>(scoLambdaList);
        }
}

local void
scobindPushDefine(AbSyn lhs)
{
        if (!abHasTag(lhs, AB_Comma))
                listPush(AbSyn, abDefineeId(lhs), scoDefineList);
}

local void
scobindPopDefine(AbSyn lhs)
{
        if (!abHasTag(lhs, AB_Comma))
                scoDefineList = listFreeCons<AbSyn>(scoDefineList);
}

local void
scobindSetStab(AbSyn absyn, ULong flags)
{
        Stab    stab = stabPushLevel(scoStab, abPos(absyn), flags);
        scobindStabHash(absyn, stab);
        abSetStab(absyn, stab);
}

local bool
scobindStabHashIsUsed(Stab stab, Hash h)
{
        List<Stab>        l;

        if (car(stab)->hash == h)
                return true;

        for (l = car(stab)->children; l; l = cdr(l))
                if (scobindStabHashIsUsed(car(l), h))
                        return true;

        return false;
}

local void
scobindStabHash(AbSyn absyn, Stab stab0)
{
        Hash    h = abHashList(listCons<AbSyn>(absyn, scoDefineList));

        if (h && !scobindStabHashIsUsed(stabFile(), h))
                car(stab0)->hash = h;
}

/*
 * scopeBind functions called through scobindLevel
 */

local void
scobindAdd(AbSyn absyn)
{
        AbSyn   base    = absyn->abAdd.base;
        AbSyn   capsule = absyn->abAdd.capsule;
        bool    save = scoIsInAdd;

        scoIsInAdd = true;
        scobindValue(base);
        scobindValue(capsule);
        scoIsInAdd = save;

        stabDefLexVar(scoStab, ssymSelf, tfType);
}

local void
scobindLambda(AbSyn absyn)
{
        TForm   tf;
        AbSyn   ret = absyn->abLambda.rtype;
        bool    save = scoIsInAdd;

        scobindParam(absyn->abLambda.param);
        if (ret && abIsNotNothing(ret)) {
                AbSyn   *retv   = abArgvAs(AB_Comma, ret);
                Length  i, retc = abArgcAs(AB_Comma, ret);

                scobindType(ret);

                /* Save the rhs on the return type if needed. */
                if (scobindRetNeedsDefn(ret))
                        tf = tfSyntaxDefine(scoStab,ret,absyn->abLambda.body);

                /* Import from each of the return types. */
                for (i = 0; i < retc; i += 1) {
                        AbSyn   reti = retv[i];

                        tf = abTForm(ret);
                        if (tf == NULL) {
                                tf = tfSyntaxFrAbSyn(scoStab, reti);
                                abSetTForm(reti, tf);
                        }
                        if (!abHasTag(reti, AB_Hide))
                                stabImportTForm(scoStab, tf);
                }

                /* Make sure product types have a type form. */
                if (abTForm(ret) == NULL)
                        abSetTForm(ret, tfSyntaxFrAbSyn(scoStab, ret));
        }
        else
                comsgError(absyn, ALDOR_E_ChkMissingRetType);

        scoIsInAdd = false;
        scobindValue(absyn->abLambda.body);
        scoIsInAdd = save;
}

local void
scobindCollect(AbSyn absyn)
{
        int     i, itc = abCollectIterc(absyn);

        for (i = 0; i < itc; i++)
                scobindValue(absyn->abCollect.iterv[i]);
        stabLockLevel(scoStab);
        scobindValue(absyn->abCollect.body);
        stabUnlockLevel(scoStab);
}

local void
scobindRepeat(AbSyn absyn)
{
        int     i, itc = abRepeatIterc(absyn);

        stabLockLevel(scoStab);
        for (i = 0; i < itc; i++)
                scobindValue(absyn->abRepeat.iterv[i]);
        scobindValue(absyn->abRepeat.body);
        stabUnlockLevel(scoStab);
}

local void
scobindWhere(AbSyn absyn)
{
        scobindValue(absyn->abWhere.context);
        stabLockLevel(scoStab); /* 'for' does unlock locally */
        scobindValue(absyn->abWhere.expr);
        stabUnlockLevel(scoStab);
}

local void
scobindWith(AbSyn absyn)
{
        AbSyn   base   = absyn->abWith.base;
        AbSyn   within = absyn->abWith.within;
        TForm   tfbase, tf;
        Syme    syme;

        if (abIsNotNothing(base)) {
                tfbase = tfSyntaxFrAbSyn(scoStab, base);
                stabCategoricallyImportTForm(scoStab, tfbase);
        }
        tf = stabMakeUsedTForm(cdr(scoStab), absyn);
        syme = stabDefLexVar(scoStab, ssymSelf, tf);

        scobindValue(base);
        scobindExportWhat(within);
}

local void
scobindHas(AbSyn absyn)
{
        AbSyn   cat = absyn->abHas.property;
        AbSyn   dom = absyn->abHas.expr;
        TForm   tfdom, tfcat;

        scobindValue(cat);
        scobindValue(dom);

        tfcat = stabMakeUsedTForm(scoStab, cat);
        tfdom = stabMakeUsedTForm(scoStab, dom);

        stabAddTFormQuery(scoStab, tfdom, tfcat);
}

local void
scobindId(AbSyn id, IdContext context)
{
        if (id) {
                IdInfo  idInfo;
                bool    okayContext;

                assert(abHasTag(id, AB_Id));

                idInfo = getIdInfoInAnyScope(scoStab, id);
                scobindSetIdUse(idInfo, context, id);

                if (scoIsInType)
                        scobindSetIdUse(idInfo, SCO_Id_InType, id);

                /*
                 * Note use-before-definitions. We ought to be doing
                 * this using data-flow _after_ type inference (so
                 * that we can ignore lazy values such as domains).
                 * Until then we have to exclude certain contexts.
                 */
                okayContext = scoIsInAdd || scoIsInExport || scoIsInImport ||
                                scoIsInExtend;
                if (!idInfo->declInfoList && !okayContext) {
                        List<AbSyn>       uses = idInfo->usePreDef;
                        idInfo->usePreDef = listCons<AbSyn>(id, uses);
                }
        }
}

local void
scobindLabel(AbSyn lab)
{
        assert(lab);

        if (abIsNotNothing(lab)) {
                IdInfo  idInfo;
                assert(abHasTag(lab, AB_Id));

                idInfo = getIdInfoInAnyScope(scoStab, lab);
                scobindSetIdUse(idInfo, SCO_Id_Label, lab);
                stabAddLabel(scoStab, lab);
        }
}

/******************************************************************************
 *
 * :: scobind utility functions.
 *
 *****************************************************************************/

/*
 * Try to match up symbol table of 'with' forms in abType with those in
 * abLamb.  abType has already been analyzed.
 *
 * Future work: we need to be much better at detecting domains and
 * categories. We rely on finding a "with" for the type. This is
 * usually okay because abnorm tries hard to ensure that all domain
 * definitions get a "with" for their type. It fails when we have
 * non-obvious domain definitions such as:
 *
 *    MyInteger:Ring == Integer;
 *
 * The problem is that we don't recognise Integer as being a domain
 * or Ring as being a category until after tinfer. By then it might
 * be too late.
 */
local void
scobindMatchWiths(AbSyn abType, AbSyn abLamb, DeclContext context)
{
        AbSyn   lparam = abLamb->abLambda.param,
                lrtype = abLamb->abLambda.rtype;

        if (abIsAnyMap(abType) &&
            (context == SCO_Sig_Extend ||
             context == SCO_Sig_DDefine ||
             scobindRetNeedsDefn(lrtype))) {
                AbSyn   aparam = abArgv(abType)[1],
                        artype = abArgv(abType)[2];

                if (!abEqual(lrtype, artype))
                        return;
                if (abHasTag(aparam, AB_Comma) && 1 == abArgc(aparam))
                        aparam = abArgv(aparam)[0];
                if (abHasTag(lparam, AB_Comma) && 1 == abArgc(lparam))
                        lparam = abArgv(lparam)[0];

                if (scobindMatchParams(aparam, lparam)) {
                        abTransferSemantics(aparam, lparam);
                        abTransferSemantics(artype, lrtype);
                }
        }
}

/*
 * Do the map parameters from a type declaration match the
 * map parameters from the lambda definition?
 */
local bool
scobindMatchParams(AbSyn aparam, AbSyn lparam)
{
        AbSyn   *parv   = abArgvAs(AB_Comma, lparam);
        Length  parc    = abArgcAs(AB_Comma, lparam);
        AbSyn   *argv   = abArgvAs(AB_Comma, aparam);
        Length  i, argc = abArgcAs(AB_Comma, aparam);

        bool    result  = (parc == argc);

        for (i = 0; result && i < argc; i += 1) {
                AbSyn   par = parv[i];
                AbSyn   arg = argv[i];

                result &= scobindMatchParam(arg, par);
        }

        return result;
}

#if AXL_EDIT_1_1_13_19
/*
 * Does a parameter from a map type declaration match the corresponding
 * parameter from the lambda definition? Accepts declaration, definition
 * and identifier nodes as arguments.
 */
local bool
scobindMatchParam(AbSyn arg, AbSyn par)
{
        AbSynTag aTag = abTag(arg);
        AbSynTag pTag = abTag(par);
#if EDIT_1_0_n2_07
        const char *err = "parameter is not an identifier or a declaration";
#else
        char *err = "parameter is not an identifier or a declaration";
#endif

        /* Safety check */
        assert(aTag == AB_Define || aTag == AB_Declare || aTag == AB_Id);
        assert(pTag == AB_Define || pTag == AB_Declare || pTag == AB_Id);

        /* Extract declarations from default value definitions */
        if (aTag == AB_Define)
                return scobindMatchParam(arg->abDefine.lhs, par);
        if (pTag == AB_Define)
                return scobindMatchParam(arg, par->abDefine.lhs);

        /*
         * Argument and parameter are declarations or identifiers.
         * If only one is a declaration then ignore that declaration.
         * We could simply return abEqual(arg->abDeclare.id, par) or
         * abEqual(arg, par->abDeclare.id) but we play safe.
         */
        if (aTag != pTag) {
                /* Which is the declaration node? */
                if (aTag == AB_Declare) /* && (pTag == AB_Id) */
                        return scobindMatchParam(arg->abDeclare.id, par);
                if (pTag == AB_Declare) /* && (aTag == AB_Id) */
                        return scobindMatchParam(arg, par->abDeclare.id);

                /* Impossible */
                comsgFatal(arg, ALDOR_F_Bug, err);
                NotReached(return false);
        }

        /* Either two declarations or two identifiers */
        if (aTag == AB_Id) /* && (pTag == AB_Id) */
                return abEqual(arg, par);

        /* Must be two declarations */
        if (aTag != AB_Declare) {
                comsgFatal(arg, ALDOR_F_Bug, err);
                NotReached(return false);
        }

        /* Compare identifiers */
        if (!abEqual(arg->abDeclare.id, par->abDeclare.id)) return false;

        /* Identifiers match: check the types */
        arg = arg->abDeclare.type;
        par = par->abDeclare.type;

        /* Identifiers match: compare types if possible */
        if (abIsNothing(arg) || abIsNothing(par)) return true;

        /* Types are present: must be equal */
        return abEqual(arg, par);
}
#else
local bool
scobindMatchParam(AbSyn arg, AbSyn par)
{
        AbSyn tmp;

        assert(abTag(arg) == AB_Define || abTag(arg) == AB_Declare || abTag(arg) == AB_Id);
        assert(abTag(par) == AB_Define || abTag(par) == AB_Declare || abTag(par) == AB_Id);

        if (abTag(arg) == AB_Define)
                arg = arg->abDefine.lhs;

        if (abTag(par) == AB_Define)
                par = par->abDefine.lhs;

        if (abTag(par) == AB_Id) {
                tmp = par;
                par = arg;
                arg = tmp;
        }

        if (abTag(par) == AB_Id)
                return abEqual(arg, par);

        if (abTag(arg) == AB_Declare)
                return abEqual(par->abDeclare.id, arg->abDeclare.id) &&
                        (abIsNothing(par->abDeclare.type) ||
                         abIsNothing(arg->abDeclare.type) ||
                         abEqual(par->abDeclare.type, arg->abDeclare.type));

        return scobindMatchParam(arg->abDeclare.id, par);
}
#endif

local void
markOuterInstanceOfFree(AbSyn id, AbSyn type, DeclContext context)
{
        Stab    stab;
        Symbol  sym = id->abId.sym;
        bool    diffType = false;

        assert(type);

        for (stab = cdr(scoStab); stab; stab = cdr(stab)) {
                IdInfo          idInfo = getIdInfoInThisScope(stab, sym);
                DeclInfo        di;

                if (!idInfo) continue;

                if (idInfo->allFree || !idInfo->declInfoList)
                        continue;

                if (abIsUnknown(type))
                        di = car(idInfo->declInfoList);
                else if ((di = idInfoHasType(idInfo, type)) != NULL)
                        ;
                else if ((di = idInfoHasType(idInfo, abUnknown)) != NULL)
                        di->type = type;
                else
                        diffType = true;

                if (!di || di->uses[SCO_Sig_Free])
                        continue;

                scobindSetSigUse(di, context, id);
                return;
        }

        if (diffType)
                comsgError(id, ALDOR_E_ScoBadTypeFree, (sym).string());
        else
                comsgError(id, ALDOR_E_ScoUnknownFree, (sym).string());
}

local void
scobindImportType(Stab stab, AbSyn type)
{
        if (abHasTag(type, AB_Hide)) {
                scobindUsedType(stab, type->abHide.type);
                abSetTFormCond(type, abTForm(type->abHide.type));
        }
        else {
                scobindType(type);
                abSetTFormCond(type,
                        stabImportTForm(stab, tfSyntaxFrAbSyn(stab, type)));
        }
}

local void
scobindUsedType(Stab stab, AbSyn type)
{
        scobindType(type);
        stabMakeUsedTForm(stab, type);
}

local void
checkOuterUseOfImplicitLocal(Stab stab, AbSyn id, AbSyn type)
{
        /*
         * id is an implicit local. It is an error if it is local, free or a
         * parameter in an outer scope. If type != 0, the types must match
         * in an outer scope to generate the message.
         */

        Symbol sym = id->abId.sym;

        for (stab = cdr(stab); stab; stab = cdr(stab)) {
                IdInfo idInfo = getIdInfoInThisScope(stab, sym);
                List<DeclInfo> dil;

                if (! idInfo)
                        continue;

                for (dil = idInfo->declInfoList; dil; dil = cdr(dil)) {
                        DeclInfo outerDeclInfo = car(dil);

                        if ((abIsUnknown(type) ||
                             abIsUnknown(outerDeclInfo->type) ||
                             abEqualModDeclares(type, outerDeclInfo->type)) &&
                            (outerDeclInfo->uses[SCO_Sig_ImplicitLocal] ||
                             outerDeclInfo->uses[SCO_Sig_Local] ||
                             outerDeclInfo->uses[SCO_Sig_Free] ||
                             outerDeclInfo->uses[SCO_Sig_Param]))
                        {
                                comsgWarning(id, ALDOR_W_ScoBadLocal,
                                        (sym).string());
                                return;
                        }
                }
        }
}

local void
checkOuterUseOfLexicalConstant(Stab stab, AbSyn id)
{
        /*
         * id is an implicit or explicit lexical constant. It is illegal
         * to have a parameter or lexical variable in an outer scope with
         * the same name.
         */

        Symbol sym = id->abId.sym;

        for (stab = cdr(stab); stab; stab = cdr(stab)) {
                IdInfo idInfo = getIdInfoInThisScope(stab, sym);
                List<DeclInfo> dil;

                if (! idInfo)
                        continue;

                for (dil = idInfo->declInfoList; dil; dil = cdr(dil)) {
                        DeclInfo odi = car(dil); /* outer decl info */

                        if (odi->uses[SCO_Sig_Assign] ||
                            odi->uses[SCO_Sig_Param])
                        {
                                comsgNError(id,ALDOR_E_ScoBadLexConst);
                                if (odi->uses[SCO_Sig_Assign])
                                        comsgNote(odi->uses[SCO_Sig_Assign],
                                                  ALDOR_N_Here);
                                else
                                        comsgNote(odi->uses[SCO_Sig_Param],
                                                  ALDOR_N_Here);
                                return;
                        }
                }
        }
}

local bool
scobindCheckOuterUseOfFluid(AbSyn id, AbSyn type)
{
        Stab stab = scoStab;
        Symbol sym = id->abId.sym;
        bool foundOuter = false;
        for (stab = cdr(stab); stab; stab = cdr(stab)) {
                IdInfo idInfo = getIdInfoInThisScope(stab, sym);
                if (idInfo) {
                        DeclInfo declInfo;

                        if (type)
                                declInfo = idInfoHasType(idInfo, type);
                        else
                                declInfo = car(idInfo->declInfoList);
                        if (declInfo && !declInfo->uses[SCO_Sig_Fluid])
                                comsgError(id, ALDOR_E_ScoFluidShadow);
                        foundOuter = true;
                }
        }
        return foundOuter;
}

/******************************************************************************
 *
 * :: scobindApply
 *
 *****************************************************************************/

local void
scobindApply(AbSyn ab)
{
        int             i, argc = abArgc(ab);
        AbSyn           *argv = abArgv(ab);

        if (scobindApplyNeedsSelf(ab))
                scobindApplySelf(ab);

        if (scobindApplyNeedsScope(ab))
                scobindLevel(ab, scobindApplyScope, scobindApplyFlags);

        else
                for (i = 0; i < argc; i += 1)
                        scobindValue(argv[i]);
}

local void
scobindApplySelf(AbSyn ab)
{
        TForm           tf, tf0;
        Syme            syme;
        List<Syme>        self;

        tf = stabMakeUsedTForm(scoStab, ab);

        /* We don't want this self visible. */
        tf0 = abIsApplyOf(ab, ssymJoin) ? tf : tfType;
        syme = symeNewLexVar(ssymSelf, tf0, car(scoStab));
        self = listCons<Syme>(syme, listNil<Syme>);
        tfSetSelf(tf, self);
}

local bool
scobindApplyNeedsSelf(AbSyn ab)
{
        AbSyn   op;
        Symbol  sym;

        assert(abIsApply(ab));
        op = abApplyOp(ab);
        sym = abIsId(op) ? abIdSym(op) : NULL;

        return  sym == ssymJoin                 ||
                sym == ssymRawRecord            ||
                sym == ssymRecord               ||
                sym == ssymUnion                ||
                sym == ssymTrailingArray;
}

local bool
scobindApplyNeedsScope(AbSyn ab)
{
        int     i, argc = abArgc(ab);
        AbSyn  *argv = abArgv(ab);

        for (i = 0; i < argc; i++)
                if (scobindApplyArgNeedsScope(argv[i]))
                        return true;
        return false;
}

local bool
scobindApplyArgNeedsScope(AbSyn ab)
{
        AbSyn   *argv   = abArgvAs(AB_Comma, ab);
        Length  i, argc = abArgcAs(AB_Comma, ab);

        for (i = 0; i < argc; i += 1)
                switch (abTag(argv[i])) {
                case AB_Declare:
                case AB_Define:
                case AB_With:
                        return true;
                default:
                        break;
                }

        return false;
}

/******************************************************************************
 *
 * :: scobindApplyScope
 *
 *****************************************************************************/

local void
scobindApplyScope(AbSyn absyn)
{
        AbSyn   *argv   = abArgv(absyn);
        Length  i, argc = abArgc(absyn);

        for (i = 0; i < argc; i += 1) {
                AbSyn   arg = argv[i];
                if (abIsAnyMap(absyn) && i == 1)
                        scobindApplyParam(arg);
                else if (abHasTag(arg, AB_Comma))
                        scobindApplyComma(arg);
                else
                        scobindApplyArg(arg);
        }
}

/*
 * Scobind the parameter list of a function declaration. This examines
 * the parameter list from left-to-right adding new declarations as they
 * are encountered. Unfortunately this means that dependent maps must be
 * defined in a specific order with earlier parameters being available
 * as arguments to the types of later parameters.
 *
 * Ideally parameter declarations ought to be considered simultaneously
 * so that a wider class of dependent maps are possible. This requires
 * extra work during code generation to ensure that all definitions take
 * place in the correct order. This might happen automatically.
 */
local void
scobindApplyParam(AbSyn params)
{
        AbSyn   *argv   = abArgvAs(AB_Comma, params);
        Length  i, argc = abArgcAs(AB_Comma, params);

        for (i = 0; i < argc; i += 1) {
                AbSyn   arg = argv[i];
                if (abHasTag(arg, AB_Declare) || abHasTag(arg, AB_Define))
                        scobindParam(arg);
                else
                        scobindApplyArg(arg);
        }
}

local void
scobindApplyComma(AbSyn ab)
{
        AbSyn   *argv   = abArgvAs(AB_Comma, ab);
        Length  i, argc = abArgcAs(AB_Comma, ab);

        for (i = 0; i < argc; i += 1)
                scobindApplyArg(argv[i]);
}

local void
scobindApplyArg(AbSyn arg)
{
        if (abHasTag(arg, AB_Assign)) {
                Stab    ostab = scoStab;
                scoStab = cdr(scoStab);
                scobindValue(arg);
                scoStab = ostab;
        }
        else if (abHasTag(arg, AB_With)) {
                scobindValue(arg);
                abSetTFormCond(arg, tfSyntaxFrAbSyn(scoStab, arg));
        }
        else
                scobindValue(arg);
}

/******************************************************************************
 *
 * :: scobindAssign
 *
 *****************************************************************************/

local void
scobindAssign(AbSyn ab)
{
        AbSyn   lhs     = ab->abAssign.lhs;
        AbSyn   rhs     = ab->abAssign.rhs;
        AbSyn   *argv   = abArgvAs(AB_Comma, lhs);
        Length  i, argc = abArgcAs(AB_Comma, lhs);
        bool    isComma = abHasTag(lhs, AB_Comma);

        for (i = 0; i < argc; i += 1) {
                AbSyn   arg = argv[i];

                switch (abTag(arg)) {
                case AB_Id:
                        scobindIntroduceId(arg, (isComma ? NULL : rhs),
                                           SCO_Sig_Assign);
                        break;

                case AB_Declare:
                        scobindAssignDeclare(arg);
                        break;

                case AB_Apply:
                        scobindValue(arg);
                        break;

                default:
                        bugBadCase(abTag(arg));
                        break;
                }
        }

        scobindValue(rhs);
}

local void
scobindAssignDeclare(AbSyn decl)
{
        AbSyn   name    = decl->abDeclare.id;
        AbSyn   type    = decl->abDeclare.type;
        AbSyn   *argv   = abArgvAs(AB_Comma, name);
        Length  i, argc = abArgcAs(AB_Comma, name);

        for (i = 0; i < argc; i += 1)
                scobindAssignId(argv[i], type);
}

local void
scobindAssignId(AbSyn id, AbSyn type)
{
        DeclInfo        declInfo;

        declInfo = scobindDeclareId(id, type, NULL, SCO_Sig_Assign);
        scobindSetSigUse(declInfo, SCO_Sig_Assign, id);

        if (declInfo->uses[SCO_Sig_Free])
                markOuterInstanceOfFree(id, type, SCO_Sig_Assign);

        if (declInfoIsImplicitLocal(declInfo))
                scobindSetSigUse(declInfo, SCO_Sig_ImplicitLocal, id);
}

/******************************************************************************
 *
 * :: scobindBuiltin
 *
 *****************************************************************************/

local void
scobindBuiltin(AbSyn ab)
{
        AbSyn   what    = ab->abBuiltin.what;
        AbSyn   *argv   = abArgvAs(AB_Sequence, what);
        Length  i, argc = abArgcAs(AB_Sequence, what);
        bool    save    = scoIsInImport;

        scoIsInImport = true;
        for (i = 0; i < argc; i += 1)
                scobindBuiltinDeclare(argv[i]);
        scoIsInImport = save;
}

local void
scobindBuiltinDeclare(AbSyn decl)
{
        AbSyn   name    = decl->abDeclare.id;
        AbSyn   type    = decl->abDeclare.type;
        AbSyn   *argv   = abArgvAs(AB_Comma, name);
        Length  i, argc = abArgcAs(AB_Comma, name);

        for (i = 0; i < argc; i += 1)
                scobindBuiltinId(argv[i], type);
}

local void
scobindBuiltinId(AbSyn id, AbSyn type)
{
        Symbol          sym = id->abId.sym;
        AInt            bv;
        DeclInfo        declInfo;

        if (!(sym).info() || !symCoInfo(sym))
                symCoInfoInit(sym);

        bv = foamBValIdTag(sym);

        if (bv == FOAM_BVAL_LIMIT
            && sym != ssymArrNew
            && sym != ssymArrElt
            && sym != ssymArrSet
            && sym != ssymArrDispose
            && sym != ssymRecNew
            && sym != ssymRecElt
            && sym != ssymRecSet
            && sym != ssymRecDispose
            && sym != ssymBIntDispose)
        {
                comsgError(id, ALDOR_E_ScoNotBuiltin);
                return;
        }

        declInfo = scobindDeclareId(id, type, NULL, SCO_Sig_Builtin);
        scobindSetSigUse(declInfo, SCO_Sig_Builtin, id);
        scobindAddMeaning(id, sym, scoStab, SYME_Builtin, abTForm(type), bv);
}

/******************************************************************************
 *
 * :: scobindDeclare
 *
 *****************************************************************************/

local void
scobindDeclare(AbSyn ab)
{
        AbSyn   id      = ab->abDeclare.id;
        AbSyn   type    = ab->abDeclare.type;
        AbSyn   *argv   = abArgvAs(AB_Comma, id);
        Length  i, argc = abArgcAs(AB_Comma, id);

        for (i = 0; i < argc; i += 1)
                scobindDeclareId(argv[i], type, NULL, SCO_Sig_Declare);
}

local DeclInfo
scobindDeclareId(AbSyn id, AbSyn type, AbSyn val, DeclContext context)
{
        Symbol          sym   = id->abId.sym;
        AbSyn           dtype = scobindDefaultType(scoStab, sym);
        IdInfo          info  = getIdInfoInAnyScope(scoStab, id);
        DeclInfo        di    = idInfoAddType(info, id, type, val, scoConditions());
        AbSyn           oval  = di->uses[SCO_Sig_Value];
        TForm           tform;

        assert(abIsNotNothing(type));

        /* Check the type against any default type which was given. */
        if (dtype && !abEqualModDeclares(type, dtype))
                comsgWarning(id, ALDOR_W_ScoVarDefault, (sym).string());

        /* Check the value against any previous value which was given. */
        if (val && oval && !abEqual(val, oval)) {
                /*!! comsgError(id, ALDOR_E_ScoVal, (sym).string()); */
                return di;
        }

        /* Mark the identifier as declared. */
        scobindSetSigUse(di, SCO_Sig_Declare, id);
        if (val) scobindSetSigUse(di, SCO_Sig_Value, val);
        if (scoIsInType) scobindSetSigUse(di, SCO_Sig_InType, id);

        /* Construct the syntax type for the declaration. */
        scobindType(type);
        tform = scobindDeclareTForm(scoStab, id, type, val, context);
        tform = stabAddTFormDeclaree(scoStab, tform, id);
        if (!abHasTag(type, AB_Hide)) {
                stabImportTForm(scoStab, tform);
                if (context == SCO_Sig_Param)
                        stabParameterImportTForm(scoStab, tform);
        }

        abSetTForm(type, tform);
        return di;
}

local TForm
scobindDeclareTForm(Stab stab, AbSyn id, AbSyn type, AbSyn val, DeclContext context)
{
        TForm tf;

        if (val == NULL) {
                if (abTForm(type))
                        return abTForm(type);
                else
                        return tfSyntaxFrAbSyn(stab, type);
        }
        else if (abIsAnyLambda(val) &&
                 (context == SCO_Sig_Extend ||
                  context == SCO_Sig_DDefine ||
                  scobindMapNeedsDefn(type)))
                tf = tfSyntaxDefineMap(scoStab, type, val);

        else if (context == SCO_Sig_Extend ||
                 context == SCO_Sig_DDefine ||
                 (context == SCO_Sig_Define && abIsUnknown(type)) ||
                 scobindRetNeedsDefn(type))
                tf = tfSyntaxDefine(scoStab, type, val);
        else
                tf = tfSyntaxFrAbSyn(stab, type);

        if (scobindTFormMustBeUnique(type)) {
                scobindUniqifyDecl(tfExpr(tf), id);
        }
        return tf;
}

local void
scobindUniqifyDecl(AbSyn decl, AbSyn id)
{

        while (abIsAnyMap(decl))
                decl = abMapRet(decl);

        assert(abTag(id) == AB_Id);
        assert(abTag(decl) == AB_Define);
        assert(abTag(decl->abDefine.lhs) == AB_Declare);

        decl->abDefine.lhs->abDeclare.id = abNewLabel(SrcPos::none,
                                                      id,
                                                      abNewNothing(SrcPos::none));
}

local void
scobindIntroduceId(AbSyn id, AbSyn val, DeclContext context)
{
        Symbol          sym = id->abId.sym;
        AbSyn           type;
        TForm           tform;
        IdInfo          idInfo;
        DeclInfo        declInfo;

        /* Compute a type expression for the identifier. */

        idInfo = getIdInfoInAnyScope(scoStab, id);

        /* Try the default type. */
        type = scobindDefaultType(scoStab, sym);

        if (!type && val)
                /* Try to take the type from the syntax of the val. */
                type = scobindGuessType(val);

        if (!type && idInfo->declInfoList)
                /* Take the type from a previous declaration. */
                type = car(idInfo->declInfoList)->type;

        if (!type) {
                /* Give up. (Although later we may infer the type from val.) */
                type = abCopy(abUnknown);
                abSetTForm(type, tfUnknown);
        }

        if (context == SCO_Sig_Assign) val = NULL;
        declInfo = idInfoAddType(idInfo, id, type, val, scoConditions());

        /* Construct the syntax type for the identifier. */
        scobindType(type);
        tform = scobindDeclareTForm(scoStab, id, type, val, context);
        tform = stabAddTFormDeclaree(scoStab, tform, id);
        if (abIsUnknown(type)) abSetTForm(type, tform);
        if (!abHasTag(type, AB_Hide)) stabImportTForm(scoStab, tform);

        /* Determine if the identifier is free. */

        if (idInfo->allFree || (declInfo && declInfo->uses[SCO_Sig_Free])) {
                if (context == SCO_Sig_Define) {
                        comsgNError(id, ALDOR_E_ScoFreeConst, (sym).string());
                        if (declInfo && declInfo->uses[SCO_Sig_Free])
                                comsgNote(declInfo->uses[SCO_Sig_Free],
                                          ALDOR_N_Here);
                }
                else if (context == SCO_Sig_Assign)
                        markOuterInstanceOfFree(id, type, context);
        }

        scobindSetSigUse(declInfo, context, id);
        if (val) scobindSetSigUse(declInfo, SCO_Sig_Value, val);

        if (context == SCO_Sig_Default)
                scobindSetSigUse(declInfo, SCO_Sig_Define, id);

        if (scoIsInType)
                scobindSetSigUse(declInfo, SCO_Sig_InType, id);

        if (context == SCO_Sig_Loop)
                scobindSetSigUse(declInfo, SCO_Sig_Assign, id);

        if (context != SCO_Sig_Define && declInfoIsImplicitLocal(declInfo))
                scobindSetSigUse(declInfo, SCO_Sig_ImplicitLocal, id);
}

local AbSyn
scobindGuessType(AbSyn val)
{
        AbSyn   type, id, arg, ret;

        switch (abTag(val)) {
        case AB_PretendTo:
                type = val->abPretendTo.type;
                break;
        case AB_CoerceTo:
                type = val->abCoerceTo.type;
                break;
        case AB_RestrictTo:
                type = val->abRestrictTo.type;
                break;
        case AB_Lambda:
                id = abNewId(abPos(val), ssymArrow);
                arg = val->abLambda.param;
                ret = val->abLambda.rtype;
                type = abNewApply2(abPos(val), id, arg, ret);
                break;
        case AB_PLambda:
                id = abNewId(abPos(val), ssymPackedArrow);
                arg = val->abPLambda.param;
                ret = val->abPLambda.rtype;
                type = abNewApply2(abPos(val), id, arg, ret);
                break;
        default:
                type = NULL;
                break;
        }

        return type;
}

/******************************************************************************
 *
 * :: scobindDefault
 *
 *****************************************************************************/

local void
scobindDefault(AbSyn ab)
{
        AbSyn   body    = ab->abDefault.body;
        AbSyn   *argv   = abArgvAs(AB_Sequence, body);
        Length  i, argc = abArgcAs(AB_Sequence, body);

        for (i = 0; i < argc; i += 1) {
                if (abHasTag(argv[i], AB_Declare)) {
                        scobindDefaultDeclare(argv[i]);
                        /* argv[i] = abNewNothing(abPos(argv[i])); */
                }
                else
                        scobindValue(argv[i]);
        }
        /* ab->abDefault.body = body; */
}

local void
scobindDefaultDeclare(AbSyn decl)
{
        AbSyn   name    = decl->abDeclare.id;
        AbSyn   type    = decl->abDeclare.type;
        AbSyn   *argv   = abArgvAs(AB_Comma, name);
        Length  i, argc = abArgcAs(AB_Comma, name);

        for (i = 0; i < argc; i += 1)
                scobindDefaultId(argv[i], type);
}

local void
scobindDefaultId(AbSyn id, AbSyn type)
{
        IdInfo  idInfo = getIdInfoInAnyScope(scoStab, id);
        scobindImportType(scoStab, type);
        idInfo->defaultType = abCopy(type);
}

/******************************************************************************
 *
 * :: scobindDefine
 *
 *****************************************************************************/

local void
scobindDefine(AbSyn ab)
{
        AbSyn   lhs     = ab->abDefine.lhs;
        AbSyn   rhs     = ab->abDefine.rhs;
        AbSyn   *argv   = abArgvAs(AB_Comma, lhs);
        Length  i, argc = abArgcAs(AB_Comma, lhs);
        bool    isComma = abHasTag(lhs, AB_Comma);
        bool    isKey   = abUse(ab) == AB_Use_Value;
        AbSyn   val     = isComma ? NULL : rhs;
        DeclContext     context = isKey ? SCO_Sig_Default : SCO_Sig_Define;

        scobindPushDefine(lhs);

        for (i = 0; i < argc; i += 1) {
                AbSyn   arg = argv[i];

                switch (abTag(arg)) {
                case AB_Id:
                        scobindIntroduceId(arg, val, context);
                        break;

                case AB_Declare:
                        scobindDefineDeclare(arg, val);
                        break;

                default:
                        bugBadCase(abTag(arg));
                        break;
                }
        }

        scobindDefineRhs(lhs, rhs, context);
        scobindPopDefine(lhs);
}

local void
scobindDefineDeclare(AbSyn decl, AbSyn val)
{
        AbSyn   name    = decl->abDeclare.id;
        AbSyn   type    = decl->abDeclare.type;
        AbSyn   *argv   = abArgvAs(AB_Comma, name);
        Length  i, argc = abArgcAs(AB_Comma, name);

        for (i = 0; i < argc; i += 1)
                scobindDefineId(argv[i], type, val);
}

local void
scobindDefineId(AbSyn id, AbSyn type, AbSyn val)
{
        DeclInfo        declInfo;

        declInfo = scobindDeclareId(id, type, val, SCO_Sig_Define);
        scobindSetSigUse(declInfo, SCO_Sig_Define, id);

        abSetDefineIdx(id, scoDefCounter++);

        if (declInfo->uses[SCO_Sig_Free])
                comsgError(id, ALDOR_E_ScoFreeConst, (id->abId.sym).string());
}

local void
scobindDefineRhs(AbSyn lhs, AbSyn rhs, DeclContext context)
{
        if (abIsAnyLambda(rhs)) {
                listPush(AbSyn, lhs, scoLambdaList);
                listPush(AbSyn, rhs, scoLambdaList);
                if (abHasTag(lhs, AB_Declare))
                        scobindMatchWiths(lhs->abDeclare.type, rhs, context);
        }
        else
                scobindValue(rhs);
}

/******************************************************************************
 *
 * :: scobindDDefine
 *
 *****************************************************************************/

local void
scobindDDefine(AbSyn absyn)
{
        AbSyn   body    = absyn->abDDefine.body;
        AbSyn   *argv   = abArgvAs(AB_Sequence, body);
        Length  i, argc = abArgcAs(AB_Sequence, body);

        for (i = 0; i < argc; i += 1)
                scobindDDefineComma(argv[i]);
}

local void
scobindDDefineComma(AbSyn defn)
{
        AbSyn   lhs     = defn->abDefine.lhs;
        AbSyn   rhs     = defn->abDefine.rhs;
        AbSyn   *argv   = abArgvAs(AB_Comma, lhs);
        Length  i, argc = abArgcAs(AB_Comma, lhs);
        bool    isComma = abHasTag(lhs, AB_Comma);

        scobindPushDefine(lhs);

        for (i = 0; i < argc; i += 1) {
                AbSyn   arg = argv[i];

                switch (abTag(arg)) {
                case AB_Id:
                        scobindIntroduceId(arg, (isComma ? NULL : rhs),
                                           SCO_Sig_DDefine);
                        break;

                case AB_Declare:
                        scobindDDefineDeclare(arg, rhs);
                        break;

                default:
                        bugBadCase(abTag(arg));
                        break;
                }
        }

        scobindDefineRhs(lhs, rhs, SCO_Sig_DDefine);
        scobindPopDefine(lhs);
}

local void
scobindDDefineDeclare(AbSyn decl, AbSyn val)
{
        AbSyn   name    = decl->abDeclare.id;
        AbSyn   type    = decl->abDeclare.type;
        AbSyn   *argv   = abArgvAs(AB_Comma, name);
        Length  i, argc = abArgcAs(AB_Comma, name);

        for (i = 0; i < argc; i += 1)
                scobindDDefineId(argv[i], type, val);
}

local void
scobindDDefineId(AbSyn id, AbSyn type, AbSyn val)
{
        DeclInfo        declInfo;

        declInfo = scobindDeclareId(id, type, val, SCO_Sig_DDefine);
        scobindSetSigUse(declInfo, SCO_Sig_DDefine, id);

        if (declInfo->uses[SCO_Sig_Free])
                comsgError(id, ALDOR_E_ScoFreeConst, (id->abId.sym).string());
}

/******************************************************************************
 *
 * :: scobindExtend
 *
 *****************************************************************************/

local void
scobindExtend(AbSyn absyn)
{
        AbSyn   body    = absyn->abExtend.body;
        AbSyn   *argv   = abArgvAs(AB_Sequence, body);
        Length  i, argc = abArgcAs(AB_Sequence, body);

        for (i = 0; i < argc; i += 1) {
                if (abHasTag(argv[i], AB_Define)) {
                        AbSyn   lhs = argv[i]->abDefine.lhs;
                        AbSyn   rhs = argv[i]->abDefine.rhs;

                        scobindExtendDeclare(lhs, rhs);
                        scobindDefineRhs(lhs, rhs, SCO_Sig_Extend);
                }
                else
                        scobindExtendDeclare(argv[i], NULL);
        }
}

local void
scobindExtendDeclare(AbSyn lhs, AbSyn rhs)
{
        bool    save = scoIsInExtend;

        scoIsInExtend = true;
        switch (abTag(lhs)) {
        case AB_Declare:
                scobindExtendId(lhs->abDeclare.id, lhs->abDeclare.type, rhs);
                break;

        case AB_Id:
                scobindExtendId(lhs, NULL, rhs);
                break;

        default:
                bugBadCase(abTag(lhs));
                break;
        }
        scoIsInExtend = save;
}

local void
scobindExtendId(AbSyn id, AbSyn type, AbSyn val)
{
        Symbol          sym = id->abId.sym;
        Doc             doc = abComment(id);
        DeclInfo        declInfo;
        Syme            syme, ext;
        TForm           typeTf;

        if (doc == NULL) doc = Doc::none;

        /* Create the syme for the extendee.  The declaration step may bind
         * parameters in `type` and attach its proper TForm, so do not snapshot
         * abTForm(type) before scobindDeclareId. */
        declInfo = scobindDeclareId(id, type, val, SCO_Sig_Extend);
        scobindSetSigUse(declInfo, SCO_Sig_Extend, id);
        typeTf = type ? abTForm(type) : NULL;
        if (fintMode == FINT_LOOP && type && !typeTf)
                typeTf = tfSyntaxFrAbSyn(scoStab, type);
        syme = stabDefExtendee(scoStab, sym, typeTf, doc);
        abSetSyme(id, syme);

        /* Create the syme for the extension. */
        ext = scobindGetExtend(id, type);
        if (ext == NULL) {
                TForm   template_ = tfSyntaxExtend(scoStab, id, type);
                ext = symeNewExtend(sym, template_, car(scoStab));
                car(scoStab)->extendSymes =
                        listCons<Syme>(ext, car(scoStab)->extendSymes);
        }

        /* Associate the extendee with the extension. */
        symeSetExtension(syme, ext);
        symeAddExtendee(ext, syme);
        stabAddTFormDeclaree(scoStab, symeType(ext), id);
        stabAddTFormExtendees(scoStab, symeType(ext), id);
        if (typeTf)
                stabAddTFormExtension(scoStab, typeTf, id);
}

local Syme
scobindGetExtend(AbSyn id, AbSyn type)
{
        Stab    stab;
        Symbol  sym = id->abId.sym;

        for (stab = scoStab; stab; stab = cdr(stab)) {
                IdInfo          idInfo = getIdInfoInThisScope(stab, sym);
                List<DeclInfo>    dil;

                dil = idInfo ? idInfo->declInfoList : listNil<DeclInfo>;

                for (; dil; dil = cdr(dil)) {
                        DeclInfo        di = car(dil);
                        AbSyn           abuse = di->uses[SCO_Sig_Extend];
                        Syme            ext;

                        ext = abuse ? symeExtension(abSyme(abuse)) : NULL;

                        if (ext) {
                                TForm typeTf = type ? abTForm(type) : NULL;
                                if (type && !typeTf)
                                        typeTf = tfSyntaxFrAbSyn(scoStab, type);
                                if (typeTf && tfCanExtend(typeTf, symeType(ext))) {
                                        TForm extTf = symeType(ext);

                                        /* Reuse only an extension that is still
                                         * being assembled in this semantic step.
                                         * A completed extension is committed state;
                                         * a later `extend` must form a fresh layer
                                         * with the committed extension as extendee.
                                         */
                                        if (tfIsExtendTemplate(extTf) ||
                                            !tfExtendDone(extTf))
                                                return ext;
                                }
                        }
                }
        }


        return NULL;
}

/******************************************************************************
 *
 * :: scobindExport
 *
 *****************************************************************************/

local void
scobindExport(AbSyn ab)
{
        AbSyn   what    = ab->abExport.what;
        AbSyn   from    = ab->abExport.origin;
        AbSyn   dest    = ab->abExport.destination;
        bool    save    = scoIsInExport;

        assert(abIsNothing(from) || abIsNothing(dest));

        scoIsInExport = true;

        if (abIsNotNothing(dest))
                scobindExportTo(what, dest);

        else if (abIsNotNothing(from))
                scobindExportFrom(what, from);

        else
                scobindExportWhat(what);

        scoIsInExport = save;
}

local void
scobindExportTo(AbSyn what, AbSyn dest)
{
        scobindType(dest);
        abSetTFormCond(dest, tfSyntaxFrAbSyn(scoStab, dest));
        scobindLOF(what, SCO_Sig_Local);
}

local void
scobindExportFrom(AbSyn what, AbSyn origin)
{
        TForm   tf;

        scobindType(origin);
        scobindValue(what);

        tf = tfSyntaxFrAbSyn(scoStab, origin);
        if (abIsNothing(what))
                tf = stabExportTForm(scoStab, tf);
        else
                tf = stabQualifiedExportTForm(scoStab, what, tf);

        abSetTFormCond(origin, tf);
}

local void
scobindExportWhat(AbSyn what)
{
        Length  i;

        switch (abTag(what)) {
        case AB_Nothing:
                break;

        case AB_Comma:
        case AB_Sequence:
                for (i = 0; i < abArgc(what); i += 1)
                        scobindExportWhat(abArgv(what)[i]);
                break;

        case AB_Define: {
                AbSyn   lhs = what->abDefine.lhs;
                AbSyn   rhs = what->abDefine.rhs;

                scobindExportDeclare(lhs, rhs);
                scobindDefineRhs(lhs, rhs, SCO_Sig_Export);
                break;
        }

        case AB_Declare:
                scobindExportDeclare(what, NULL);
                break;

        case AB_Default:
                /* 'default' statement within 'with' body */

                scobindValue(what);
                break;

        case AB_If:
                /* 'if' statement within 'with' body */

                scobindValue(what->abIf.test);
                scobindExportWhat(what->abIf.thenAlt);
                scobindExportWhat(what->abIf.elseAlt);
                break;

        case AB_Import:
                /* 'import' within 'with' body */

                scobindValue(what);
                break;

        default:
                scobindValue(what);
                break;
        }
}

local void
scobindExportDeclare(AbSyn decl, AbSyn val)
{
        AbSyn   name    = decl->abDeclare.id;
        AbSyn   type    = decl->abDeclare.type;
        AbSyn   *argv   = abArgvAs(AB_Comma, name);
        Length  i, argc = abArgcAs(AB_Comma, name);

        for (i = 0; i < argc; i += 1)
                scobindExportId(argv[i], type, val);
}

local void
scobindExportId(AbSyn id, AbSyn type, AbSyn val)
{
        Symbol          sym = id->abId.sym;
        Doc             doc = abComment(id);
        DeclInfo        declInfo;

        if (doc == NULL) doc = Doc::none;

        declInfo = scobindDeclareId(id, type, val, SCO_Sig_Export);
        scobindSetSigUse(declInfo, SCO_Sig_Export, id);
        scobindAddMeaning(id, sym, scoStab, SYME_Export, abTForm(type),
                          doc.packed());
}

/******************************************************************************
 *
 * :: scobindFluid
 *
 *****************************************************************************/

local void
scobindFluid(AbSyn ab)
{
        AbSyn   body    = ab->abFluid.argv[0];
        AbSyn   *argv   = abArgvAs(AB_Sequence, body);
        Length  i, argc = abArgcAs(AB_Sequence, body);

        for (i = 0; i < argc; i += 1) {
                if (abHasTag(argv[i], AB_Assign)) {
                        scobindAssign(argv[i]);
                        scobindFluidComma(argv[i]->abAssign.lhs);
                }
                else
                        scobindFluidComma(argv[i]);
        }
}

local void
scobindFluidComma(AbSyn lhs)
{
        AbSyn   *argv   = abArgvAs(AB_Comma, lhs);
        Length  i, argc = abArgcAs(AB_Comma, lhs);

        for (i = 0; i < argc; i += 1) {
                AbSyn   arg = argv[i];

                switch (abTag(arg)) {
                case AB_Id:
                        scobindFluidId(arg, NULL);
                        break;

                case AB_Declare:
                        scobindFluidDeclare(arg);
                        break;

                default:
                        bugBadCase(abTag(arg));
                        break;
                }
        }
}

local void
scobindFluidDeclare(AbSyn decl)
{
        AbSyn   name    = decl->abDeclare.id;
        AbSyn   type    = decl->abDeclare.type;
        AbSyn   *argv   = abArgvAs(AB_Comma, name);
        Length  i, argc = abArgcAs(AB_Comma, name);

        for (i = 0; i < argc; i += 1)
                scobindFluidId(argv[i], type);
}

local void
scobindFluidId(AbSyn id, AbSyn type)
{
        IdInfo          idInfo = getIdInfoInAnyScope(scoStab, id);

        if (type) {
                DeclInfo        declInfo;
                declInfo = scobindDeclareId(id, type, NULL, SCO_Sig_Fluid);
                scobindSetSigUse(declInfo, SCO_Sig_Fluid, id);
        }
        else {
                List<DeclInfo>    dil;
                idInfo->allFluid = true;
                for (dil = idInfo->declInfoList; dil; dil = cdr(dil))
                        scobindSetSigUse(car(dil), SCO_Sig_Fluid, id);
        }
}

/******************************************************************************
 *
 * :: scobindFor
 *
 *****************************************************************************/

local void
scobindFor(AbSyn ab)
{
        AbSyn   lhs     = ab->abFor.lhs;
        AbSyn   whole   = ab->abFor.whole;
        AbSyn   test    = ab->abFor.test;
        Stab    ostab   = scoStab;
        bool    stabWasLocked = stabLevelIsLocked(scoStab);

        /* whole should be analyzed in the enclosing symbol table. */
        scoStab = stabPopLevel(scoStab);
        scobindValue(whole);
        scoStab = ostab;

        if (stabWasLocked)
                stabUnlockLevel(scoStab);

        scobindFor0(lhs, true);
        scobindValue(test);

        if (stabWasLocked)
                stabLockLevel(scoStab);
}

local void
scobindFor0(AbSyn var, bool check)
{
        Length  i;

        switch (abTag(var)) {
        case AB_Comma:
        case AB_Sequence:
                for (i = 0; i < abArgc(var); i += 1)
                        scobindFor0(abArgv(var)[i], check);
                break;

        case AB_Free:
                scobindFree(var);
                scobindFor0(abArgv(var)[0], false);
                break;

        case AB_Local:
                scobindLocal(var);
                scobindFor0(abArgv(var)[0], false);
                break;

        case AB_Id:
                if (check) {
                        scobindLOF(var, SCO_Sig_Local);
                        checkOuterUseOfImplicitLocal(scoStab, var, abUnknown);
                }
                scobindIntroduceId(var, NULL, SCO_Sig_Loop);
                break;

        case AB_Declare:
                if (check) {
                        scobindLOF(var, SCO_Sig_Local);
                        checkOuterUseOfImplicitLocal(scoStab, abDefineeId(var),
                                                     var->abDeclare.type);
                }
                scobindForDeclare(var->abDeclare.id, var->abDeclare.type);
                break;

        default:
                bugBadCase(abTag(var));
                break;
        }
}

local void
scobindForDeclare(AbSyn name, AbSyn type)
{
        Length  i, argc = abArgcAs(AB_Comma, name);
        AbSyn   *argv = abArgvAs(AB_Comma, name);

        for (i = 0; i < argc; i += 1)
                scobindForId(argv[i], type);
}

local void
scobindForId(AbSyn id, AbSyn type)
{
        DeclInfo        declInfo;

        declInfo = scobindDeclareId(id, type, NULL, SCO_Sig_Loop);
        scobindSetSigUse(declInfo, SCO_Sig_Loop, id);
        scobindSetSigUse(declInfo, SCO_Sig_Assign, id);

        if (declInfo->uses[SCO_Sig_Free])
                markOuterInstanceOfFree(id, type, SCO_Sig_Assign);

        if (declInfoIsImplicitLocal(declInfo))
                scobindSetSigUse(declInfo, SCO_Sig_ImplicitLocal, id);
}

/******************************************************************************
 *
 * :: scobindForeign
 *
 *****************************************************************************/

local void
scobindForeign(AbSyn ab)
{
        AbSyn   origin  = ab->abForeign.origin;
        AbSyn   what    = ab->abForeign.what;
        AbSyn   *argv   = abArgvAs(AB_Sequence, what);
        Length  i, argc = abArgcAs(AB_Sequence, what);

        if (abHasTag(what, AB_Id) || abHasTag(what, AB_With))
        {
                TForm           tf;
                AbSyn           abTf;
                TFormUses       tfu;

                /* Treat as a traditional import */
                scobindType(origin);
                scobindValue(what);
                tf = tfSyntaxFrAbSyn(scoStab, origin);
                tf = stabQualifiedImportTForm(scoStab, what, tf);
                abSetTFormCond(origin, tf);


                /* Find the use of this tform */
                abTf = tfExpr(tf);
                tfu = stabFindTFormUses(scoStab, abTf);


                /* Mark as qualified foreign or builtin import */
                if (abIsTheId(origin, ssymBuiltin))
                        tqSetStatus(tfu->imports, TQUAL_Builtin);
                else
                        tqSetStatus(tfu->imports, TQUAL_Foreign);
        }
        else
        {
                bool    save    = scoIsInImport;
                scoIsInImport = true;
                for (i = 0; i < argc; i += 1)
                {
                        AbSyn   arg = argv[i];

                        if (abHasTag(argv[i], AB_Declare))
                        {
                                ForeignOrigin   forg;

                                forg = forgFrAbSyn(origin);
                                scobindForeignDeclare(arg, forg);
                        }
                }
                scoIsInImport = save;
        }
}

local void
scobindForeignDeclare(AbSyn decl, ForeignOrigin forg)
{
        AbSyn   name    = decl->abDeclare.id;
        AbSyn   type    = decl->abDeclare.type;
        AbSyn   *argv   = abArgvAs(AB_Comma, name);
        Length  i, argc = abArgcAs(AB_Comma, name);

        for (i = 0; i < argc; i += 1)
                scobindForeignId(argv[i], type, forg);
}

local void
scobindForeignId(AbSyn id, AbSyn type, ForeignOrigin forg)
{
        Symbol          sym = id->abId.sym;
        DeclInfo        declInfo;

        declInfo = scobindDeclareId(id, type, NULL, SCO_Sig_Foreign);
        scobindSetSigUse(declInfo, SCO_Sig_Foreign, id);
        scobindAddMeaning(id, sym, scoStab,SYME_Foreign,abTForm(type),
                          (AInt) forg);
}

/******************************************************************************
 *
 * :: scobindImport
 *
 *****************************************************************************/

local void
scobindImport(AbSyn ab)
{
        AbSyn   what    = ab->abImport.what;
        AbSyn   from    = ab->abImport.origin;
        TForm   tf;
        bool    save    = scoIsInImport;

        scobindType(from);

        scoIsInImport = true;
        scobindValue(what);
        scoIsInImport = save;

        tf = tfSyntaxFrAbSyn(scoStab, from);
        if (abIsNothing(what))
                tf = stabExplicitlyImportTForm(scoStab, tf);
        else
                tf = stabQualifiedImportTForm(scoStab, what, tf);

        abSetTFormCond(from, tf);
}

/******************************************************************************
 *
 * :: scobindInline
 *
 *****************************************************************************/

local void
scobindInline(AbSyn ab)
{
        AbSyn   what    = ab->abInline.what;
        AbSyn   from    = ab->abInline.origin;
        TForm   tf;

        scobindType(from);
        scobindValue(what);

        tf = tfSyntaxFrAbSyn(scoStab, from);
        if (abIsNothing(what))
                tf = stabInlineTForm(scoStab, tf);
        else
                tf = stabQualifiedInlineTForm(scoStab, what, tf);

        abSetTFormCond(from, tf);
}

/******************************************************************************
 *
 * :: scobindParam
 *
 *****************************************************************************/

local void
scobindParam(AbSyn ab)
{
        AbSyn   *argv   = abArgvAs(AB_Comma, ab);
        Length  i, argc = abArgcAs(AB_Comma, ab);

        for (i = 0; i < argc; i += 1) {
                if (abHasTag(argv[i], AB_Define)) {
                        AbSyn   lhs = argv[i]->abDefine.lhs;
                        AbSyn   rhs = argv[i]->abDefine.rhs;

                        scobindParamDefine(lhs, rhs);
                        scobindDefineRhs(lhs, rhs, SCO_Sig_Param);
                }
                else
                        scobindParamDefine(argv[i], NULL);
        }
}

local void
scobindParamDefine(AbSyn lhs, AbSyn rhs)
{
        switch (abTag(lhs)) {
        case AB_Declare:
                scobindParamDeclare(lhs, rhs);
                break;

        default:
                /*!! abcheck */
                comsgError(lhs, ALDOR_E_ScoBadParameter);
                break;
        }
}

local void
scobindParamDeclare(AbSyn decl, AbSyn val)
{
        AbSyn   name    = decl->abDeclare.id;
        AbSyn   type    = decl->abDeclare.type;
        AbSyn   *argv   = abArgvAs(AB_Comma, name);
        Length  i, argc = abArgcAs(AB_Comma, name);

        for (i = 0; i < argc; i += 1) {
                AbSyn   id = argv[i];

                /*!! This is almost certainly wrong. */
                Symbol  sym = id->abId.sym;
                AbSyn   idType = type;
                if (abIsNothing(idType)) {
                        idType = scobindDefaultType(scoStab, id->abId.sym);
                        if (idType) decl->abDeclare.type = abCopy(idType);
                }
                if (!idType) {
                        comsgError(id, ALDOR_E_ScoParmType, (sym).string());
                        continue;
                }

                scobindParamId(id, idType, val);
        }
}

local void
scobindParamId(AbSyn id, AbSyn type, AbSyn val)
{
        Symbol          sym = id->abId.sym;
        DeclInfo        declInfo;

        declInfo = scobindDeclareId(id, type, val, SCO_Sig_Param);
        scobindSetSigUse(declInfo, SCO_Sig_Param, id);

        if (abSyme(id))
                stabAddMeaning(scoStab, abSyme(id));
        else
                scobindAddMeaning(id, sym, scoStab, SYME_Param, abTForm(type),
                                  (AInt) NULL);
}

/******************************************************************************
 *
 * :: scobindReference
 *
 *****************************************************************************/

local void
scobindReference(AbSyn ab)
{
        /* We ought to continue down the tree */
        scobindValue(ab);


        /* Ignore complicated things */
        if (!abHasTag(ab, AB_Id))
                return;

        /* Mark the thing being referenced */
        scobindIntroduceId(ab, NULL, SCO_Sig_Reference);
}

/******************************************************************************
 *
 * :: scobindFree
 * :: scobindLocal
 *
 *****************************************************************************/

local void
scobindFree(AbSyn ab)
{
        scobindLOF(ab->abFree.argv[0], SCO_Sig_Free);
}

local void
scobindLocal(AbSyn ab)
{
        scobindLOF(ab->abLocal.argv[0], SCO_Sig_Local);
}

local void
scobindLOF(AbSyn body, DeclContext context)
{
        AbSyn   *argv   = abArgvAs(AB_Sequence, body);
        Length  i, argc = abArgcAs(AB_Sequence, body);

        for (i = 0; i < argc; i += 1) {
                AbSyn   arg = argv[i];

                switch (abTag(arg)) {
                case AB_Id:
                case AB_Declare:
                case AB_Comma:
                        scobindLOFComma(arg, NULL, context);
                        break;

                case AB_Assign:
                        scobindLOFComma(arg->abAssign.lhs, NULL, context);
                        scobindAssign(arg);
                        break;

                case AB_Define:
                        scobindLOFComma(arg->abDefine.lhs,
                                        arg->abDefine.rhs, context);
                        scobindDefine(arg);
                        break;

                default:
                        bugBadCase(abTag(arg));
                        break;
                }
        }
}

local void
scobindLOFComma(AbSyn lhs, AbSyn rhs, DeclContext context)
{
        AbSyn   *argv   = abArgvAs(AB_Comma, lhs);
        Length  i, argc = abArgcAs(AB_Comma, lhs);
        bool    isComma = abHasTag(lhs, AB_Comma);
        AbSyn   val     = isComma ? NULL : rhs;

        for (i = 0; i < argc; i += 1) {
                AbSyn   arg = argv[i];

                switch (abTag(arg)) {
                case AB_Id:
                        scobindLOFId(arg, context);
                        break;

                case AB_Declare:
                        scobindLOFDeclare(arg, val, context);
                        break;

                default:
                        bugBadCase(abTag(arg));
                        break;
                }
        }
}

local void
scobindLOFDeclare(AbSyn decl, AbSyn rhs, DeclContext context)
{
        AbSyn   name    = decl->abDeclare.id;
        AbSyn   type    = decl->abDeclare.type;
        AbSyn   *argv   = abArgvAs(AB_Comma, name);
        Length  i, argc = abArgcAs(AB_Comma, name);
        bool    isComma = abHasTag(name, AB_Comma);
        AbSyn   val     = isComma ? NULL : rhs;

        for (i = 0; i < argc; i += 1)
                scobindLOFType(argv[i], type, val, context);
}

local void
scobindLOFId(AbSyn id, DeclContext context)
{
        IdInfo          idInfo = getIdInfoInAnyScope(scoStab, id);
        List<DeclInfo>    dil = idInfo->declInfoList;

        if (context == SCO_Sig_Local)
                idInfo->allLocal = true;
        else
                idInfo->allFree = true;

        if (!dil && context == SCO_Sig_Free)
                markOuterInstanceOfFree(id, abUnknown, SCO_Sig_FreeRef);

        for (; dil; dil = cdr(dil))
                scobindLOFDeclInfo(id, idInfo, car(dil), context);
}

local void
scobindLOFType(AbSyn id, AbSyn type, AbSyn val, DeclContext context)
{
        IdInfo          idInfo = getIdInfoInAnyScope(scoStab, id);
        DeclInfo        declInfo;

        scobindDeclareId(id, type, val, context);
        declInfo = idInfoHasType(idInfo, type);

        if (declInfo)
                scobindLOFDeclInfo(id, idInfo, declInfo, context);
        else if (context == SCO_Sig_Free)
                markOuterInstanceOfFree(id, type, SCO_Sig_FreeRef);
}

local void
scobindLOFDeclInfo(AbSyn id, IdInfo idInfo, DeclInfo di, DeclContext context)
{
        AbSyn   abNote = NULL;

        if (idInfo->uses[SCO_Id_Used])
                abNote = idInfo->uses[SCO_Id_Used];
        else if (di->uses[SCO_Sig_Used])
                abNote = di->uses[SCO_Sig_Used];
        else if (di->uses[SCO_Sig_Assign])
                abNote =  di->uses[SCO_Sig_Assign];
        else if (di->uses[SCO_Sig_Define])
                abNote = di->uses[SCO_Sig_Define];

        if (abNote) {
                comsgNError(id, ALDOR_E_ScoLateFreeLocal);
                comsgNote(abNote, ALDOR_N_Here);
        }

        scobindSetSigUse(di, context, id);

        if (di->uses[SCO_Sig_Param])
                comsgError(id, ALDOR_E_ScoParmLocFree);

        if (di->uses[SCO_Sig_Free] && di->uses[SCO_Sig_Local])
                comsgError(id, ALDOR_E_ScoFreeAndLoc, (id->abId.sym).string());

        if (context == SCO_Sig_Free)
                markOuterInstanceOfFree(id, di->type, SCO_Sig_FreeRef);
}

/******************************************************************************
 *
 * :: scobindTry
 *
 *****************************************************************************/

local void
scobindTry(AbSyn ab)
{
        AbSyn    id = ab->abTry.id;

        if (!abIsNothing(id)) {
                AbSyn    with, base, within;
                base  = abNewNothing(abPos(id));
                abPutUse(base, AB_Use_Type);
                within  = abNewNothing(abPos(id));
                abPutUse(within, AB_Use_Declaration);
                with = abNewWith(abPos(id), base, within);

                id = abDefineeId(ab->abTry.id);
                scobindDeclareId(id, with,
                                 NULL, SCO_Sig_Define);
        }
        scobindValue(ab->abTry.expr);
        scobindValue(ab->abTry.except);
        scobindValue(ab->abTry.always);
}

/******************************************************************************
 *
 * :: scobindIf
 *
 *****************************************************************************/

local void
scobindIf(AbSyn ab)
{
        AbSyn test = ab->abIf.test;
        scobindValue(test);
        scoCondPush((AInt) test);
        scobindValue(ab->abIf.thenAlt);
        scoCondPop();
        scoCondPush(((AInt) test) | 1); /* ooo, you bad boy */
        scobindValue(ab->abIf.elseAlt);
        scoCondPop();
}



local void
scoCondPush(AInt ab)
{
        scoCondList = listCons<AInt>(ab, scoCondList);
}

local void
scoCondPop()
{
        scoCondList = cdr(scoCondList);
}

local List<AInt>
scoConditions()
{
        return scoCondList;
}

local DefnPos
defposTail(DefnPos pos)
{
        return cdr(pos);
}

local bool
defposEqInner(AInt a, AInt b)
{
        return a == b;
}

local bool
defposEqual(DefnPos a, DefnPos b)
{
        return listEqual<AInt>(a, b, defposEqInner);
}

local bool
defposIsRoot(DefnPos pos)
{
        return pos == listNil<AInt>;
}

/******************************************************************************
 *
 * :: IdInfo
 *
 *****************************************************************************/

local IdInfo
idInfoNew(Stab stab, AbSyn id)
{
        Symbol          sym = id->abId.sym;
        IdInfo          idInfo;
        List<IdInfo>      iil;
        Length          c;

        if (!(sym).info() || !symCoInfo(sym))
                symCoInfoInit(sym);

        car(stab)->idsInScope = listCons<Symbol>(sym, car(stab)->idsInScope);

        idInfo = (IdInfo) stoAlloc(OB_Other, sizeof(*idInfo));

        idInfo->allFree         = false;
        idInfo->allLocal        = false;
        idInfo->allFluid        = false;
        idInfo->serialNo        = car(stab)->serialNo;
        idInfo->intStepNo       = intStepNo;
        idInfo->sym             = sym;
        idInfo->declInfoList    = listNil<DeclInfo>; /* NULL; */
        idInfo->defaultType     = NULL;
        idInfo->exampleId       = id;
        idInfo->usePreDef       = listNil<AbSyn>;

        for (c = 0; c < SCO_Id_LIMIT; c += 1)
                idInfo->uses[c] = NULL;

        /*
         * Insert the new id info so that the id info structures are sorted
         * from innermost to outermost.  (Inner stabs have higher serialNo's.)
         */

        iil = listCons<IdInfo>(idInfo, idInfoCell(sym));
        setIdInfoCell(sym, iil);

        while (cdr(iil) && idInfo->serialNo < car(cdr(iil))->serialNo) {
                car(iil) = car(cdr(iil));
                iil = cdr(iil);
        }
        setcar(iil, idInfo);

        return idInfo;
}

local void
idInfoFree(IdInfo idInfo)
{
        listFreeDeeply<DeclInfo>(idInfo->declInfoList, declInfoFree);
        listFree<AbSyn>(idInfo->usePreDef);
        stoFree(idInfo);
}

local bool
idInfoIsNew(IdInfo idInfo)
{
        return idInfo->intStepNo == intStepNo - 1;
}

local void
scobindFreeIdInfo(Stab stab)
{
        List<Symbol>      ids = car(stab)->idsInScope;

        for (; ids; ids = listFreeCons<Symbol>(ids)) {
                Symbol          id = car(ids);
                List<IdInfo>      il = idInfoCell(id);
                IdInfo          info = car(il);

                idInfoFree(info);
                setIdInfoCell(id, listFreeCons<IdInfo>(il));
        }
        car(stab)->idsInScope = ids;
}

local List<IdInfo>
scobindSaveIdInfo(Stab stab)
{
        List<Symbol>      ids = car(stab)->idsInScope;
        List<IdInfo>      iil = listNil<IdInfo>;

        for (; ids; ids = listFreeCons<Symbol>(ids)) {
                Symbol          id = car(ids);
                List<IdInfo>      il = idInfoCell(id);
                IdInfo          info = car(il);

                iil = listCons<IdInfo>(info, iil);
                setIdInfoCell(id, listFreeCons<IdInfo>(il));
        }
        car(stab)->idsInScope = ids;

        return iil;
}

local void
scobindRestoreIdInfo(Stab stab, List<IdInfo> iil, bool undo)
{
        List<Symbol>      ids = listNil<Symbol>;

        for (; iil; iil = listFreeCons<IdInfo>(iil)) {
                IdInfo          info = car(iil);
                Symbol          id = info->sym;
                List<IdInfo>      il = idInfoCell(id);

                if (undo && idInfoIsNew(info))
                        idInfoFree(info);
                else {
                        ids = listCons<Symbol>(id, ids);
                        setIdInfoCell(id, listCons<IdInfo>(info, il));
                        if (undo) scobindRestoreDeclInfo(info);
                }
        }
        car(stab)->idsInScope = ids;
}

/*
 * Get the id info from the innermost level which can have it.
 * A lexical level can have info for an id if the id is already present,
 * or if the level is unlocked.
 */
local IdInfo
getIdInfoInAnyScope(Stab stab, AbSyn id)
{
        Symbol          sym = id->abId.sym;
        IdInfo          info = NULL;

        for (; stab && !info; stab = cdr(stab)) {
                info = getIdInfoInThisScope(stab, sym);
                if (!info && !stabLevelIsLocked(stab))
                        info = idInfoNew(stab, id);
        }

        assert(info);
        return info;
}

/*
 * Get the id info for sym if it exists in the given scope.
 * Return NULL if no id info exists for sym in the given scope.
 */
local IdInfo
getIdInfoInThisScope(Stab stab, Symbol sym)
{
        ULong           serialNo = car(stab)->serialNo;
        List<IdInfo>      info;

        if (!(sym).info() || !symCoInfo(sym))
                symCoInfoInit(sym);

        for (info = idInfoCell(sym); info; info = cdr(info)) {
                if (car(info)->serialNo == serialNo)
                        return car(info);
                else if (car(info)->serialNo < serialNo)
                        return NULL;
        }
        return NULL;
}

local void
scobindSetIdUse(IdInfo idInfo, IdContext context, AbSyn use)
{
        idInfo->uses[context] = use;
}

local AbSyn
scobindDefaultType(Stab stab, Symbol sym)
{
        AbSyn   dtype = NULL;

        for (; stab && !dtype; stab = cdr(stab)) {
                IdInfo  info = getIdInfoInThisScope(stab, sym);
                dtype = info ? info->defaultType : NULL;
        }

        return dtype;
}

/******************************************************************************
 *
 * :: DeclInfo
 *
 *****************************************************************************/

local DeclInfo
declInfoNew(AbSyn id, AbSyn type, DefnPos cond)
{
        DeclInfo        di;
        Length          c;

        di = (DeclInfo) stoAlloc(OB_Other, sizeof(*di));

        di->intStepNo   = intStepNo;
        di->id          = id;
        di->type        = type;
        di->defpos      = listSingleton<DefnPos>(cond);
        di->doc         = id ? abComment(id) : Doc::none;

        for (c = 0; c < SCO_Sig_LIMIT; c += 1)
                di->uses[c] = NULL;

        return di;
}

local void
declInfoFree(DeclInfo di)
{
        stoFree(di);
}

local bool
declInfoIsNew(DeclInfo di)
{
        return di->intStepNo == intStepNo - 1;
}

local bool
declInfoUseIsNew(AbSyn ab)
{
        return ab && (!abSyme(ab) || isNewSyme(abSyme(ab)));
}

local bool
declInfoIsImplicitLocal(DeclInfo di)
{
        return !(di->uses[SCO_Sig_Local] ||
                 di->uses[SCO_Sig_Free]  ||
                 di->uses[SCO_Sig_Param] ||
                 di->uses[SCO_Sig_Export]);
}

local void
scobindRestoreDeclInfo(IdInfo ido)
{
        List<DeclInfo>    dil = ido->declInfoList;
        dil = listFreeIfSat<DeclInfo>(dil, declInfoFree, declInfoIsNew);
        ido->declInfoList = dil;

        for (; dil; dil = cdr(dil)) {
                DeclInfo        di = car(dil);
                Length          i;
                for (i = SCO_Sig_START; i < SCO_Sig_LIMIT; i += 1)
                        if (declInfoUseIsNew(di->uses[i]))
                                di->uses[i] = NULL;
        }
}

local DeclInfo
idInfoHasType(IdInfo ido, AbSyn type)
{
        List<DeclInfo>    dil;

        for (dil = ido->declInfoList; dil; dil = cdr(dil))
                if (abEqualModDeclares(car(dil)->type, type))
                        return car(dil);
        return NULL;
}

/*
 * Augment list of types and symbol documentation.
 */
local DeclInfo
idInfoAddType(IdInfo ido, AbSyn id, AbSyn type, AbSyn val, List<AInt> cond)
{
        DeclInfo        di;

        /* If this id has been declared with this type,
         * use the old decl info.
         */
        if ((di = idInfoHasType(ido, type)) != NULL)
                ;

        /* If this id has been declared with type Unknown,
         * substitute the new type in the old decl info.
         */
        else if ((di = idInfoHasType(ido, abUnknown)) != NULL &&
                 (val == NULL || di->uses[SCO_Sig_Value] == NULL))
                di->type = type;

        /* Otherwise create a new decl info structure. */
        else {
                di = declInfoNew(id, type, cond);

                if (ido->allFree)
                        scobindSetSigUse(di, SCO_Sig_Free, id);
                if (ido->allLocal)
                        scobindSetSigUse(di, SCO_Sig_Local, id);

                ido->declInfoList = listCons<DeclInfo>(di, ido->declInfoList);
        }

        return di;
}

local void
scobindSetSigUse(DeclInfo declInfo, DeclContext context, AbSyn use)
{
        /* check for double definitions */

        /*!! This shouldn't be here. */
        if (context == SCO_Sig_Define && declInfo->uses[context]) {
                if (fintMode == FINT_LOOP &&
                    fintYesOrNo("Redefine? (y/n): ")) {
                        comsgFPrintf(stdout, ALDOR_M_FintRedefined,
                                     use->abId.sym.string());
                }
                else if (!scobindCheckDefnPos(declInfo, scoCondList)) {
                        comsgNError(use, ALDOR_E_ScoDupDefine,
                                     (use->abId.sym).string());
                        comsgNote(declInfo->uses[context], ALDOR_N_Here);
                }
        }

        declInfo->uses[context] = use;
}

/*
 * Returns true iff a definition is found with the same
 * conditionalisation as the current symbol.
 */
local bool
scobindCheckDefnPos(DeclInfo declInfo, DefnPos posn)
{
        List<DefnPos> lpos = declInfo->defpos;
#if NoDupDefs
        return false;
#endif
        while (lpos) {
                if (defposEqual(car(lpos), posn))
                        return false;
                lpos = cdr(lpos);
        }

        if (defposIsRoot(posn)) return true;

        return scobindCheckDefnPos(declInfo, defposTail(posn));
}

/******************************************************************************
 *
 * :: scobindAddMeaning
 *
 *****************************************************************************/

local void
scobindAddMeaning(AbSyn ab, Symbol sym, Stab stab, SymeTag kind,
                  TForm tf, AInt data)
{
        if (scobindNeedsMeaning(ab, tf)) {
                Syme    syme = scobindDefMeaning(stab,kind,sym,tf,data);
                scobindSetMeaning(ab, syme);
        }
}

local bool
scobindNeedsMeaning(AbSyn ab, TForm tf)
{
        return  ab == NULL || abSyme(ab) == NULL ||
                symeIsLazy(abSyme(ab)) || symeType(abSyme(ab)) != tf;
}

local void
scobindSetMeaning(AbSyn ab, Syme syme)
{
        if (ab) {
                if (abSyme(ab) == NULL) {
                        MutString  s = abPrettyClippedIn(tfExpr(symeType(syme)),
                                                      60, ABPP_NOINDENT);

                        comsgRemark(ab, ALDOR_R_ScoMeaning,
                                    comsgString(symeTagToDescrMsgId(symeKind(syme))),
                                    symeString(syme), s);
                        strFree(s);
                }
                abSetSyme(ab, syme);
        }
}

local Syme
scobindDefMeaning(Stab stab, SymeTag kind, Symbol sym, TForm tf, AInt data)
{
        Syme    syme = NULL;

        switch (kind) {
        case SYME_Param:
                syme = stabDefParam(stab, sym, tf);
                break;
        case SYME_LexVar:
                syme = stabDefLexVar(stab, sym, tf);
                break;
        case SYME_LexConst:
                syme = stabDefLexConst(stab, sym, tf);
                break;
        case SYME_Fluid:
                syme = stabDefFluid(stab, sym, tf);
                break;
        case SYME_Export:
                syme = stabDefExport(stab, sym, tf, Doc::fromPacked(data));
                break;
        case SYME_Builtin:
                syme = stabDefBuiltin(stab, sym, tf, (FoamBValTag) data);
                break;
        case SYME_Foreign:
                syme = stabDefForeign(stab, sym, tf, (ForeignOrigin) data);
                break;
        default:
                bugBadCase(kind);
                break;
        }

        return syme;
}

/******************************************************************************
 *
 * :: scobindReconcile
 *
 *****************************************************************************/

local void
scobindReconcile(Stab stab, AbSynTag context)
{
        List<Symbol>      ids = car(stab)->idsInScope;

        for (; ids; ids = cdr(ids)) {
                Symbol          id = car(ids);
                List<IdInfo>      il = idInfoCell(id);
                IdInfo          info = car(il);


                /* The standard checks */
                scobindReconcileId(stab, context, id, info);
        }
}

local void
scobindReconcileId(Stab stab, AbSynTag context, Symbol sym, IdInfo idInfo)
{
        AbSyn   abuse;

        if (idInfo->declInfoList)
                scobindReconcileDecls(stab, context, sym, idInfo);

        /* Check for bad uses or non-uses of locals. */
        else if (idInfo->allLocal) {
                AbSyn   ab = idInfo->exampleId;
                String str = (sym).string();

                if (idInfo->allFree)
                        comsgError(ab, ALDOR_E_ScoFreeAndLoc, str);
                else if ((abuse = idInfo->uses[SCO_Id_Used]) != NULL)
                        comsgWarning(abuse, ALDOR_W_ScoBadUse, str);
                else
                        comsgWarning(ab, ALDOR_W_ScoLocalNoUse, str);

                if (idInfo->allFluid)
                        bug("local, free, fluid, wassa matta?");
        }
        else {
                /*
                 * The identifier is not defined in this scope
                 * level and so it must either be defined in an
                 * outer level or it has been imported. Create
                 * new usage information in the level outside
                 * this one to allow the symbol meaning to be
                 * bound in that level, if appropriate.
                 */
                abuse = idInfo->uses[SCO_Id_InType];
                if (abuse && stab != stabFile()) {
                        idInfo = getIdInfoInAnyScope(cdr(stab), abuse);
                        idInfo->uses[SCO_Id_InType] = abuse;
                }
                abuse = idInfo->uses[SCO_Id_Used];
                if (abuse && stab != stabFile()) {
                        idInfo = getIdInfoInAnyScope(cdr(stab), abuse);
                        idInfo->uses[SCO_Id_Used] = abuse;
                }
        }
}

local void
scobindReconcileDecls(Stab stab, AbSynTag context, Symbol sym, IdInfo idInfo)
{
        bool            lazy = false;
        List<AbSyn>       earlyUse;
        List<DeclInfo>    declInfoList;
        AbSyn           isAssigned = NULL, isDefined = NULL, isReffed = NULL;

        /* Reverse list so we see signatures in correct order. */
        idInfo->declInfoList = listNReverse<DeclInfo>(idInfo->declInfoList);
        declInfoList = idInfo->declInfoList;

        for ( ; declInfoList; declInfoList = cdr(declInfoList)) {
                DeclInfo        declInfo = car(declInfoList);
                AbSyn           abuse;

                abuse = declInfo->uses[SCO_Sig_Assign];
                if (abuse) {
                        if (isAssigned) {
                                comsgNError(isAssigned, ALDOR_E_ScoVarOverload);
                                comsgNote(abuse, ALDOR_N_Here);
                                abSetState(declInfo->id, AB_State_Error);
                        }
                        isAssigned = abuse;
                        if (fintMode == FINT_LOOP && isDefined)
                                abSetState(declInfo->id, AB_State_Error);
                }

                abuse = declInfo->uses[SCO_Sig_Define];
                if (abuse && !isDefined) {
                        isDefined = abuse;
                        if (fintMode == FINT_LOOP && isAssigned)
                                abSetState(declInfo->id, AB_State_Error);
                }

                abuse = declInfo->uses[SCO_Sig_Reference];
                if (abuse)
                        isReffed = abuse;


                /* Does it look like a lazy value? */
                if (!lazy && abIsAnyMap(declInfo->type))
                        lazy = true;

                scobindReconcileDecl(stab, context, sym, idInfo, declInfo);
        }


        /* Ensure id is not assigned and defined. */
        if (isAssigned && isDefined) {
                comsgNError(isAssigned, ALDOR_E_ScoAssAndDef, (sym).string());
                comsgNote(isDefined, ALDOR_N_Here);
        }


        /* Ensure id is not referenced and defined */
        if (isReffed && isDefined) {
                comsgNError(isReffed, ALDOR_E_ScoAssAndRef, (sym).string());
                comsgNote(isDefined, ALDOR_N_Here);
        }


        /* Ensure id is not library or archive identifier and assigned  */
        /* or defined.                                                  */
        if (isAssigned || isDefined) {
                AbSyn   ab = isAssigned ? isAssigned : isDefined;
                if (stabGetLibrary(sym) || stabGetArchive(sym))
                        comsgError(ab, ALDOR_E_ScoLibrary, (sym).string());
        }


        /*
         * Ensure that non-lazy constants aren't used before their
         * definition outside an `add'. We don't seem to need to
         * watch for domains and categories as they never seem to
         * generate use-before-definition errors. If we do, try
         * extending the range of cases in which `lazy' is true.
         */
        earlyUse = idInfo->usePreDef;
        if (!lazy && isDefined && earlyUse) {
                /*
                 * Display an error for each use-before-definition of
                 * non-lazy constants used outside and `add' body.
                 * We don't need to reverse `earlyUse' because the
                 * comsg system sorts messages by line number.
                 */
                for (;earlyUse; earlyUse = cdr(earlyUse)) {
                        AbSyn   id = car(earlyUse);
                        comsgNError(id, ALDOR_E_ScoEarlyUse, (sym).string());
                        comsgNote(isDefined, ALDOR_N_Here);
                }
        }
}

/*
 * This function processes the data for a given signature.
 */
local void
scobindReconcileDecl(Stab stab, AbSynTag context, Symbol sym, IdInfo idInfo,
                     DeclInfo declInfo)
{
        AbSyn   assigned = declInfo->uses[SCO_Sig_Assign];

        /* Error if name used in type is a variable. */

        if ((idInfo->uses[SCO_Id_InType] || declInfo->uses[SCO_Sig_InType])
            && assigned)
        {
                comsgError(assigned, ALDOR_E_ScoAssTypeId, (sym).string());
        }

        /*
         * Error if a loop variable is assigned to. The initial creation
         * of the loop variable will be an assignment, but if the use is
         * not the same then there has been a later assignment.
         */

        if (declInfo->uses[SCO_Sig_Loop] &&
            declInfo->uses[SCO_Sig_Loop] != assigned)
        {
                comsgError(assigned, ALDOR_E_ScoBadLoopAss);
        }

        /* Leave early if we don't require symbol meaning generation */

        if (declInfo->uses[SCO_Sig_Builtin] && declInfo->uses[SCO_Sig_Foreign])
                comsgError(declInfo->uses[SCO_Sig_Builtin],
                        ALDOR_E_ScoSameSig);

        if (declInfo->uses[SCO_Sig_Builtin] || declInfo->uses[SCO_Sig_Foreign])
        {
                if (declInfo->uses[SCO_Sig_Free])
                        comsgError(declInfo->uses[SCO_Sig_Free],ALDOR_E_ScoNoFree);
                if (declInfo->uses[SCO_Sig_Local])
                        comsgError(declInfo->uses[SCO_Sig_Local],ALDOR_E_ScoNoFree);
                if (declInfo->uses[SCO_Sig_Param])
                        comsgError(declInfo->uses[SCO_Sig_Free],ALDOR_E_ScoNoParm);
                if (assigned)
                        comsgError(assigned, ALDOR_E_ScoNoSet);
                if (declInfo->uses[SCO_Sig_Define])
                        comsgError(declInfo->uses[SCO_Sig_Define],ALDOR_E_ScoNoSet);
                return;
        }

        if (declInfo->uses[SCO_Sig_Free] || declInfo->uses[SCO_Sig_Param])
                return;

        /* Process explicit locals */

        if (declInfo->uses[SCO_Sig_Local]) {
                TForm   tf = tfSyntaxFrAbSyn(stab, declInfo->type);

                if (declInfo->uses[SCO_Sig_Define]) {
                        checkOuterUseOfLexicalConstant(stab, declInfo->id);
                        scobindAddMeaning(declInfo->id,sym,stab,SYME_LexConst,
                                tf, (AInt) NULL);
                        return;
                }

                if (assigned) {
                        scobindAddMeaning(declInfo->id,sym,stab,SYME_LexVar,
                            tf, (AInt) NULL);
                        return;
                }

                /* we seem to need to make the syme to keep tinfer happy */

                scobindAddMeaning(declInfo->id,sym,stab,SYME_LexConst,
                                  tf, (AInt) NULL);

                /* check for use without assignment or definition */

                if (idInfo->uses[SCO_Id_Used])
                         comsgWarning(idInfo->uses[SCO_Id_Used],
                                      ALDOR_W_ScoBadUse, (sym).string());
                else if (declInfo->uses[SCO_Sig_Used])
                         comsgWarning(declInfo->uses[SCO_Sig_Used],
                                      ALDOR_W_ScoBadUse, (sym).string());
                else if (! declInfo->uses[SCO_Sig_FreeRef])
                         comsgWarning(declInfo->uses[SCO_Sig_Local],
                                      ALDOR_W_ScoLocalNoUse, (sym).string());
                return;
        }
        if (declInfo->uses[SCO_Sig_Fluid]) {
                TForm tf;
                if (abIsNothing(declInfo->type))
                        tf = tfUnknown;
                else
                        tf = tfSyntaxFrAbSyn(stab,declInfo->type);

                scoFluidDEBUG(printf("Adding fluid: %s", (sym).string()));
                scoFluidDEBUG(tfPrintDb(tf));

                if (!scobindCheckOuterUseOfFluid(declInfo->id, declInfo->type)) {
                        scoFluidDEBUG(printf(" New\n"));
                        scobindAddMeaning(declInfo->id, sym, stab, SYME_Fluid,
                                          tf, (AInt) NULL);
                }
                else
                        scoFluidDEBUG(printf(" See'd it before.\n"));
                return;
        }

        /* Signature without 'local' declaration. */

        if (assigned) {
                TForm tf;

                if (context != AB_Apply)
                        checkOuterUseOfImplicitLocal(stab,
                                assigned,
                                declInfo->type);

                tf = tfSyntaxFrAbSyn(stab, declInfo->type);

                if (fintMode == FINT_LOOP) {
                        if (abSyme(declInfo->id))
                                return;
                        if (tfIsUnknown(tf) && stabGetLex(stab, sym))
                                return;
                }

                scobindAddMeaning(declInfo->id, sym, stab, SYME_LexVar,
                                  tf, (AInt) NULL);
                return;
        }

        if (declInfo->uses[SCO_Sig_Define]) {
                TForm   tf = tfSyntaxFrAbSyn(stab, declInfo->type);
                if (context == AB_Add || context == AB_With) {
                        if (!abSyme(declInfo->id))
                                scobindAddMeaning(declInfo->id,
                                          sym, stab, SYME_Export,
                                          tf, declInfo->doc.packed());
                }
                else {
                        checkOuterUseOfLexicalConstant(stab, declInfo->id);
                        scobindAddMeaning(declInfo->id, sym, stab,
                                          SYME_LexConst, tf, (AInt) NULL);

                        if (context != AB_Apply)
                                checkOuterUseOfImplicitLocal(stab,
                                        declInfo->uses[SCO_Sig_Define],
                                        declInfo->type);

                        scobindSetSigUse(declInfo, SCO_Sig_ImplicitLocal,
                                declInfo->uses[SCO_Sig_Define]);
                }
                return;
        }

        if (abSyme(declInfo->id))
                return;

        /* make anything else into a lexical constant */

        scobindAddMeaning(declInfo->id, sym, stab, SYME_LexConst,
                          tfSyntaxFrAbSyn(stab, declInfo->type), (AInt) NULL);
}

/******************************************************************************
 *
 * :: scobindPrint
 *
 *****************************************************************************/

void
scobindPrint(Stab stab)
{
        scobindPrintStab(stab);
}

local void
scobindPrintStab(Stab stab)
{
        List<Symbol>      ids;

        for (ids = car(stab)->idsInScope; ids; ids = cdr(ids)) {
                scobindPrintId(car(ids));
                if (cdr(ids)) fnewline(dbOut);
        }
}

local void
scobindPrintId(Symbol id)
{
        List<IdInfo>      il = idInfoCell(id);
        IdInfo          info = (il ? car(il) : NULL);
        List<DeclInfo>    dil;
        Length          i;

        if (!info) return;

        fprintf(dbOut, "        %-14s: ", (id).string());
        scobindPrintIdInfo(info);
        for (i = 1, dil = info->declInfoList; dil; i += 1, dil = cdr(dil))
                scobindPrintDeclInfo(i, car(dil));
}

local void
scobindPrintIdInfo(IdInfo info)
{
        Length          c;

        if (info->allFree)
                fprintf(dbOut, "AllFree ");
        if (info->allLocal)
                fprintf(dbOut, "AllLocal ");
        if (info->allFluid)
                fprintf(dbOut, "AllFluid ");

        for (c = SCO_Id_START; c < SCO_Id_LIMIT; c += 1)
                if (info->uses[c])
                        fprintf(dbOut, "%s ", IdContextNames[c]);

        if (info->defaultType) {
                fprintf(dbOut, "Default (");
                abPrettyPrintClippedIn(dbOut, info->defaultType,
                                       ABPP_UNCLIPPED, 1);
                fprintf(dbOut, ")");
        }
}

local void
scobindPrintDeclInfo(Length i, DeclInfo declInfo)
{
        Length          c;

        fnewline(dbOut);
#if EDIT_1_0_n1_07
        fprintf(dbOut, "        %-14s: [%d] ", " ", (int) i);
#else
        fprintf(dbOut, "        %-14s: [%d] ", " ", i);
#endif

        if (declInfo->type) {
                fprintf(dbOut, "<");
                abPrettyPrintClippedIn(dbOut, declInfo->type,
                                       ABPP_UNCLIPPED, 1);
                fprintf(dbOut, "> ");
        }

        for (c = SCO_Sig_START; c < SCO_Sig_LIMIT; c += 1)
                if (declInfo->uses[c])
                        fprintf(dbOut, "%s ", DeclContextNames[c]);
}

/******************************************************************************
 *
 * :: scobindUndo
 *
 *****************************************************************************/

local void
scobindUndo()
{
        scoUndoSymes = listNil<Syme>;
        scoUndoTForms = listNil<TForm>;

        scoUndoStab(scoStab);

        /*
         * A failed semantic step can leave non-owning Syme references in
         * TForms and related caches which survive the rollback.  Remove the
         * failed Symes from live lookup structures immediately, but defer
         * their physical reclamation until compiler shutdown, after no
         * semantic state can be consulted again.
         */
        scoRetiredSymes = listNConcat<Syme>(scoUndoSymes, scoRetiredSymes);
        scoUndoSymes = listNil<Syme>;
        scoRetiredTForms = listNConcat<TForm>(scoUndoTForms, scoRetiredTForms);
        scoUndoTForms = listNil<TForm>;

        scoUndoState = false;
}

local void
scoUndoStab(Stab stab)
{
        Stab ancestor;

        /*
         * The top-level Stab is a stack whose cdr is the ancestor chain
         * (file -> global).  Each such level may acquire semantic state during
         * a failed step.  Visit every ancestor exactly once; recursive child
         * traversal deliberately ignores the child's repeated ancestor tail.
         */
        for (ancestor = stab; ancestor; ancestor = cdr(ancestor))
                scoUndoStabTree(ancestor);
}

local void
scoUndoStabTree(Stab stab)
{
        StabLevel       stabLev;
        List<Stab>      stabl;

        if (!stab) return;

        stabLev = car(stab);
        scoUndoStabLevel(stabLev);
        for (stabl = stabLev->children; stabl; stabl = cdr(stabl))
                scoUndoStabTree(car(stabl));
}

local void
scoUndoStabLevel(StabLevel stabLev)
{
        stabLev->tbl.removeIf<__stoFree, scoUndoStabEntry>();

        scoUndoExtendExtendees(stabLev);
        stabLev->extendSymes = listFreeIfSat<Syme>
                (stabLev->extendSymes, scoUndoSyme, isNewSyme);

        stabLev->boundSymes = scoUndoBoundSymes(stabLev->boundSymes);

        stabLev->tformsUsed.list = listFreeIfSat<TFormUses>
                (stabLev->tformsUsed.list, scoUndoTFormUses, isNewTFormUses);

        if (stabLev->tformsUsed.table)
                stabLev->tformsUsed.table.removeIf<scoUndoTFormUses, isNewTFormUses>();

        stabLev->tformsUnused = listFreeIfSat<TForm>
                (stabLev->tformsUnused, scoUndoTForm, isNewTForm);
}

local List<Syme>
scoUndoBoundSymes(List<Syme> src)
{
        List<Syme> dst = listNil<Syme>;

        for (; src; src = cdr(src)) {
                Syme syme = car(src);

                if (isNewSyme(syme)) {
                        scoRestoreExtendees(&dst, syme);
                        scoUndoSyme(syme);
                }
                else if (!listMemq<Syme>(dst, syme))
                        dst = listCons<Syme>(syme, dst);
        }

        return listNReverse<Syme>(dst);
}

local void
scoRestoreExtendees(List<Syme> *dst, Syme extension)
{
        List<Syme> extendees;

        if (!symeIsExtend(extension)) return;
        for (extendees = symeExtendee(extension); extendees;
             extendees = cdr(extendees)) {
                Syme extendee = car(extendees);

                if (isNewSyme(extendee)) continue;
                if (symeExtension(extendee) == extension)
                        symeSetExtension(extendee, NULL);
                if (!listMemq<Syme>(*dst, extendee))
                        *dst = listCons<Syme>(extendee, *dst);
        }
}

local bool
scoUndoStabEntry(StabEntry stent)
{
        Length          i;
        bool            any = false;

        if (!stent) return false;

        /*
         * A StabEntry may contain the same logical meanings in several cache
         * slots, and slots 0/1 can share tails.  Rebuild changed slots rather
         * than destructively freeing their cells.  On an aborted extension,
         * the extension syme had replaced its committed extendees in the
         * entry; restore those extendees as the inverse of stabExtendMeanings.
         */
        for (i = 0; i < stent->argc; i += 1) {
                List<Syme> src, dst = listNil<Syme>;
                bool changed = false;

                for (src = stent->symev[i]; src; src = cdr(src)) {
                        Syme syme = car(src);
                        if (isNewSyme(syme)) {
                                scoRestoreExtendees(&dst, syme);
                                scoUndoSyme(syme);
                                changed = true;
                        }
                        else if (!listMemq<Syme>(dst, syme))
                                dst = listCons<Syme>(syme, dst);
                }

                if (changed) {
                        stent->symev[i] = listNReverse<Syme>(dst);
                        tpossFree(stent->possv[i]);
                        stent->possv[i] = NULL;
                }
                else
                        listFree<Syme>(dst);

                if (stent->symev[i]) any = true;
        }

        return !any;
}

local void
scoUndoSyme(Syme syme)
{
        /* Retired semantic objects remain memory-valid because older TForms
         * and import caches may still contain non-owning pointers to them.
         * Mark them semantically inert before placing them on the deferred
         * reclamation list so stale caches cannot reactivate them. */
        symeSetRetired(syme);
        if (!listMemq<Syme>(scoUndoSymes, syme))
                scoUndoSymes = listCons<Syme>(syme, scoUndoSymes);
}

local void
scoUndoTForm(TForm tf)
{
        if (!listMemq<TForm>(scoUndoTForms, tf))
                scoUndoTForms = listCons<TForm>(tf, scoUndoTForms);
}

local void
scoUndoTFormUses(TFormUses tfu)
{
        scoUndoTForm(tfu->tf);
        /* stoFree(tfu); $$ USE tfUsesPool or intStepNo or refCounter */
}

local void
scoUndoExtendExtendees(StabLevel stabLev)
{
        List<Syme> extensions;

        for (extensions = stabLev->extendSymes; extensions;
             extensions = cdr(extensions)) {
                Syme extension = car(extensions);
                SymeList old, keep = listNil<Syme>;
                bool changed = false;

                if (!symeIsExtend(extension))
                        continue;

                /*
                 * A new interactive extension layer may have attached itself
                 * to a committed extension/domain before the failed statement
                 * reached stabExtendMeanings.  In that case there is no Stab
                 * entry replacement for scoUndoStabEntry to invert, but the
                 * committed extendee still carries a back-pointer to the
                 * aborted extension.  Detach it before retiring the new layer.
                 */
                if (isNewSyme(extension)) {
                        for (old = symeExtendee(extension); old; old = cdr(old)) {
                                Syme extendee = car(old);
                                if (!isNewSyme(extendee) &&
                                    symeExtension(extendee) == extension)
                                        symeSetExtension(extendee, NULL);
                        }
                        continue;
                }

                for (old = symeExtendee(extension); old; old = cdr(old)) {
                        Syme extendee = car(old);
                        if (isNewSyme(extendee)) {
                                scoUndoSyme(extendee);
                                changed = true;
                        }
                        else
                                keep = listCons<Syme>(extendee, keep);
                }
                if (changed)
                        symeSetExtendee(extension, listNReverse<Syme>(keep));
                else
                        listFree<Syme>(keep);
        }
}

local bool
isNewSyme(Syme syme)
{
        return symeIntStepNo(syme) == intStepNo - 1;
}

local bool
isNewTForm(TForm tf)
{
        return tf->intStepNo == intStepNo - 1;
}

local bool
isNewTFormUses(TFormUses tfu)
{
        return tfu->tf->intStepNo == intStepNo - 1;
}
