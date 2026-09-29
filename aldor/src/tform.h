/*****************************************************************************
 *
 * tform.h: Type forms.
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

#ifndef _TFORM_H_
#define _TFORM_H_

#include "axlobs.h"
#include "foam.h"

extern SymeList tfSetSymesFn(TForm, SymeList);

/******************************************************************************
 *
 * :: Type Form kinds
 *
 *****************************************************************************/

enum tformTag {
    TF_START,
    TF_SYM_START = TF_START,
        TF_Unknown = TF_SYM_START,
        TF_Exit,
        TF_Literal,
        TF_Test,
        TF_Type,
        TF_Category,
    TF_SYM_LIMIT,
    TF_ABSYN_START = TF_SYM_LIMIT,
        TF_Syntax  = TF_ABSYN_START,
        TF_General,
    TF_ABSYN_LIMIT,
    TF_NODE_START = TF_ABSYN_LIMIT,
        TF_Add = TF_NODE_START,
        TF_Assign,
        TF_Cross,
        TF_Declare,
        TF_Default,
        TF_Define,
        TF_Enumerate,
        TF_Forward,
        TF_Generator,
        TF_If,
        TF_Instance,
        TF_Join,
        TF_Map,
        TF_Meet,
        TF_Multiple,
        TF_PackedMap,
        TF_Raw,
        TF_RawRecord,
        TF_Record,
        TF_Reference,
        TF_Subst,
        TF_Third,
        TF_Trigger,
        TF_TrailingArray,
        TF_Tuple,
        TF_Union,
        TF_Variable,
        TF_With,
        TF_Except,
    TF_NODE_LIMIT,
    TF_LIMIT = TF_NODE_LIMIT
};

typedef enum tformTag  TFormTag;

/******************************************************************************
 *
 * :: Table of information about type form kinds
 *
 *****************************************************************************/

struct tform_info {
        TFormTag                tag;
        String             str;            /* tag's string */
        String             syntax;         /* actual syntax string */
        Hash                    hash;           /* tag's hash */
        short                   argc;           /* -1 => N-ary */
};

extern struct tform_info        tformInfoTable[];

#define tformInfo(tag)          tformInfoTable[(int)(tag) - (int)TF_START]

#define tformStr(tag)           tformInfo(tag).str
#define tformSyntax(tag)        tformInfo(tag).syntax
#define tformArgc(tag)          tformInfo(tag).argc

/******************************************************************************
 *
 * :: Type Form state
 *
 *****************************************************************************/

enum tf_state {
        TF_State_TForm,         /* No semantics yet. */
        TF_State_Pending,       /* type form has been seen by tfPending. */
        TF_State_Meaning,       /* type form has been seen by tfMeaning. */
        TF_State_NeedsSefo,     /* type form needs processing by tiTfSefo. */
        TF_State_LIMIT
};

typedef enum tf_state  TfState;

/******************************************************************************
 *
 * :: Type Form structure
 *
 *****************************************************************************/

struct tform {
        BPack(TFormTag)         tag;            /* What kind of tform. */
        BPack(bool)             ownSyntax;      /* Do we own our syntax? */
        BPack(TfState)          state;          /* How much semantics? */
        BPack(bool)             hasSelf;        /* Do we have symes for %? */
        BPack(bool)             hasSelfSelf;    /* Do we have symes for %%? */
        BPack(bool)             hasCascades;    /* Is cascades complete? */
        BPack(FoamTag)          raw;            /* Raw Foam type. */
        Hash                    hash;
        AbSyn                   __absyn;        /* Owned abstract syntax. */
        UShort                  intStepNo;

        Length                  argc;           /* Length of argv. */
        TForm                   *argv;          /* Subordinate TForms. */

        Stab                    stab;           /* Symbol table. */
        SymeList                self;           /* Syme(s) for %. */
        SymeList                selfself;       /* Syme(s) for %%. */
        SymeList                parents;        /* Category parents. */
        SymeList                symes;          /* Representational symes. */

        SymeList                domExports;     /* Exports from this type. */
        SymeList                catExports;     /* Exports from categories. */
        SymeList                thdExports;     /* Exports from 3d-order. */

        SymeList                domImports;     /* Imports with % replaced. */

        TConstList              consts;         /* Promises of satisfaction. */
        TFormList               queries;        /* Questions asked: D has C. */
        TQualList               cascades;       /* Cascaded imports. */

        AbSub                   sigma;          /* Subst. for TF_Subst. */
        FreeVar                 fv;             /* Free vars for subst. */
        Length                  *rho;           /* Dep. multi. permutation. */

        SefoMark                __mark;         /* sefo traversal mark */
        TForm                   parent;         /* Parent for free vars. */
        ULong                   libNum;         /* Serial no w/in lib file. */
};

typedef bool    (*TFormPredicate)       (TForm);

#define         TYPE_NUMBER_UNASSIGNED  (0x7FFFFFFF)
#define         TYPE_NUMBER_NEEDS_SEFO  (0x7FFFFFFE)

/******************************************************************************
 *
 * :: Basic operations
 *
 *****************************************************************************/

/*
 * Macros for generic treatment of structures.
 */
#define                 tfTag(tf)               ((TFormTag) ((tf)->tag))
#define                 tfSetTag(tf,t)          ((tf)->tag = (UByte) (t))
#define                 tfState(tf)             ((TfState) ((tf)->state))
#define                 tfSetState(tf,s)         ((tf)->state = (UByte) (s))
#define                 tfFoamType(tf)          ((FoamTag) ((tf)->raw))
#define                 tfSetFoamType(tf,t)      ((tf)->raw = (UByte) (t))
#define                 tfArgc(tf)              ((tf)->argc)
#define                 tfArgv(tf)              ((tf)->argv)
#define                 tfExpr(tf)              tfToAbSyn(tf)

#define                 tfStab(tf)              ((tf)->stab)
#define                 tfSelf(tf)              ((tf)->self)
#define                 tfSelfSelf(tf)          ((tf)->selfself)
#define                 tfParents(tf)           ((tf)->parents)
#define                 tfSymes(tf)             ((tf)->symes)

#define                 tfDomExports(tf)        ((tf)->domExports)
#define                 tfCatExports(tf)        ((tf)->catExports)
#define                 tfThdExports(tf)        ((tf)->thdExports)

#define                 tfDomImports(tf)        ((tf)->domImports)

#define                 tfSetStab(tf,st)        ((tf)->stab = (st))
#define                 tfSetSelf(tf,sl)        ((tf)->self = (sl))
#define                 tfSetSelfSelf(tf,sl)    ((tf)->selfself = (sl))
#define                 tfSetParents(tf,sl)     ((tf)->parents = (sl))
#define                 tfSetSymes(tf,sl)       ((tf)->symes = (sl))

#define                 tfSetDomExports(tf,sl)  (tfDomExports(tf) = (sl))
#define                 tfSetCatExports(tf,sl)  (tfCatExports(tf) = (sl))
#define                 tfSetThdExports(tf,sl)  (tfThdExports(tf) = (sl))

#define                 tfSetDomImports(tf,sl)  (tfDomImports(tf) = (sl))

#define                 tfConsts(tf)            ((tf)->consts)

#define                 tfQueries(tf)           ((tf)->queries)
#define                 tfSetQueries(tf,tl)     (tfQueries(tf) = (tl))

#define                 tfCascades(tf)          ((tf)->cascades)
#define                 tfHasCascades(tf)       ((tf)->hasCascades)
#define                 tfSetCascades(tf,ql)    (tfCascades(tf) = (ql))

#define                 tfFVars(tf)             ((tf)->fv)
#define                 tfSetFVars(tf,fv)       (tfFVars(tf) = (fv))
#define                 tfClrFVars(tf)          (tfFVars(tf) = NULL)

#define                 tformMark(tf)           ((tf)->__mark)
#define                 tformSetMark(tf,n)      ((tf)->__mark = (n))

#define                 tfNeedsSefo(tf)         \
        ((tf)->state == TF_State_NeedsSefo)
#define                 tfSetNeedsSefo(tf)      \
        ((tf)->state = tfIsMeaning(tf) ? TF_State_NeedsSefo : (tf)->state)
#define                 tfClrNeedsSefo(tf)      \
        ((tf)->state = tfNeedsSefo(tf) ? TF_State_Meaning : (tf)->state)

/*
 * Debugging functions.
 */
extern void             tformDump               (TForm);
extern TForm            tfFollowFully           (TForm);

/*
 * Basic TForm operations.
 */
extern void             tfInit                  (void);
extern TForm            tfNewEmpty              (TFormTag, Length);
extern TForm            tfNewAbSyn              (TFormTag, AbSyn);
extern void             tfFree                  (TForm);
extern int              tfPrint                 (FILE *, TForm);
extern int              tfPrintDb               (TForm);
extern bool             tfEqual                 (TForm, TForm);
extern Hash             tfHash                  (TForm);

extern TForm            tfFrSyme                (Stab, Syme);
extern TForm            tfFrSelf                (Stab, TForm);

/*
 * Pending TForms.
 */
#define                 tfIsTForm(tf)           (tfState(tf)==TF_State_TForm)
#define                 tfIsPending(tf)         (tfState(tf)==TF_State_Pending)
#define                 tfIsMeaning(tf)         (tfState(tf)>=TF_State_Meaning)

#define                 tfSetTForm(tf)          tfSetState(tf, TF_State_TForm)
#define                 tfSetPending(tf)        tfSetState(tf, TF_State_Pending)
#define                 tfSetMeaning(tf)        tfSetState(tf, TF_State_Meaning)

#define                 tfCopyState(ntf,otf)    tfSetState(ntf, tfState(otf))

extern TForm            tfPending               (Stab, AbSyn);
extern TForm            tfMeaning               (Stab, AbSyn, TForm);
extern void             tfSetMeaningArgs        (TForm);

extern Syme             tfpOpSyme               (Stab, Symbol, Length);

/*
 * Type form syntax.
 */
#define                 tfHasExpr(tf)           ((tf)->__absyn != 0)
#define                 tfGetExpr(tf)           ((tf)->__absyn)
extern void             tfOwnExpr               (TForm);
extern void             tfReleaseExpr           (TForm, AbSyn);
extern void             tfTransferSemantics     (TForm, TForm);

extern TForm            tfSyntaxFrAbSyn         (Stab, AbSyn);
extern TForm            tfSyntaxMap             (Stab, AbSyn, TForm);
extern TForm            tfSyntaxDefine          (Stab, AbSyn, AbSyn);
extern TForm            tfSyntaxDefineMap       (Stab, AbSyn, AbSyn);
extern TForm            tfSyntaxExtend          (Stab, AbSyn, AbSyn);
extern TForm            tfSyntaxExtendMap       (Stab, AbSyn, TForm);

extern TForm            tfFullFrAbSyn           (Stab, Sefo);
extern TForm            tfForwardFrSyntax       (TForm, TForm);
extern TForm            tfPendingFrSyntax       (Stab, AbSyn, TForm);
extern TForm            tfMeaningFrSyntax       (Stab, Sefo,  TForm);

extern AbSyn            tfSymesToWith           (SymeList);
extern AbSyn            tfWithFrAdd             (AbSyn);
extern AbSyn            tfToAbSyn               (TForm);
extern AbSyn            tfToAbSynPretty         (TForm);

extern Stab             tfFloat                 (Stab, TForm);

/*
 * Import/export selection
 */
extern  Syme            tfGetDomImport(TForm, String, bool (*f)(TForm));
extern  Syme            tfGetDomExport(TForm, String, bool (*f)(TForm));


/*
 * Type form semantics.
 */
extern TFormList        tfCopyQueries           (TForm, TForm);
extern TForm            tfAddQuery              (TForm, TForm);

extern TForm            abGetCategory           (Sefo);
extern SymeList         abGetCatExports         (Sefo);
extern SymeList         abGetCatSelf            (Sefo);

extern Stab             tfDefStab               (Stab, TForm);
extern Stab             tfGetStab               (TForm);
extern TForm            tfGetCategory           (TForm);

#define                 tfHasSelf(tf)           ((tf)->hasSelf)
extern SymeList         tfCopySelf              (TForm, TForm);
extern SymeList         tfAddSelf               (TForm, SymeList);
extern SymeList         tfGetSelfFrStab         (Stab);
extern SymeList         tfGetSelf               (Stab, TForm);
extern SymeList         tfDefSelf               (Stab, TForm);
extern SymeList         tfGetDomSelf            (TForm);
extern SymeList         tfGetCatSelf            (TForm);
extern SymeList         tfGetThdSelf            (TForm);

#define                 tfHasSelfSelf(tf)       ((tf)->hasSelfSelf)
extern SymeList         abGetCatSelfSelf        (Sefo);
extern SymeList         tfGetCatSelfSelf        (TForm);
extern SymeList         tfGetThdSelfSelf        (TForm);
extern SymeList         tfGetCatParents         (TForm, bool);
extern SymeList         tfGetThdParents         (TForm);

extern void             tfAuditExportList       (SymeList);
extern SymeList         tfJoinExportLists       (SymeList, SymeList, SymeList,
                                                 Sefo);

extern bool             tfHasCatExports         (TForm);
extern bool             tfHasThdExports         (TForm);
extern bool             tfHasCatExportsFrWith   (Sefo);

extern TForm            tfCatExportsPending     (TForm);
extern TForm            tfThdExportsPending     (TForm);

extern SymeList         tfGetDomExports         (TForm);
extern SymeList         tfGetCatExports         (TForm);
extern SymeList         tfGetThdExports         (TForm);

extern SymeList         tfGetDomConstants       (TForm);
extern SymeList         tfGetCatConstants       (TForm);

extern TQualList        tfGetDomCascades        (TForm);
extern TQualList        tfGetCatCascades        (TForm);
extern TQualList        tfGetThdCascades        (TForm);

extern SymeList         tfStabGetDomImports     (Stab, TForm);
extern SymeList         tfGetDomImports         (TForm);
extern SymeList         tfGetCatImportsFrWith   (Sefo, SymeList);
extern Syme             tfHasDomExportMod       (TForm,SymeList,Symbol,TForm);
extern Syme             tfHasDomImport          (TForm, Symbol, TForm);

extern bool             tfTagHasSymes           (TFormTag);
extern bool             tfSymeInducesDependency (Syme, TForm);
extern void             tfGetSymes              (Stab, TForm, AbSyn);

#define                 tfUseCatExports(tf)     \
             (tfIsNone(tfWithBase(tf)) && tfIsNone(tfWithWithin(tf)))
#define                 tfUseThdExports(tf)     \
             (tfIsNone(tfThirdRestrictions(tf)))

/******************************************************************************
 *
 * :: Special type forms
 *
 *****************************************************************************/

/*
 * Type form utility functions
 */

#define tfIsSymTag(t)   (/*TF_SYM_START   <= (t) &&*/ (t) < TF_SYM_LIMIT)
#define tfIsAbSynTag(t) (  TF_ABSYN_START <= (t) &&   (t) < TF_ABSYN_LIMIT)
#define tfIsNodeTag(t)  (  TF_NODE_START  <= (t) &&   (t) < TF_NODE_LIMIT)

#define                 tfIsSym(tf)             tfIsSymTag(tfTag(tf))
#define                 tfIsAbSyn(tf)           tfIsAbSynTag(tfTag(tf))
#define                 tfIsNode(tf)            tfIsNodeTag(tfTag(tf))

#define                 tfIsId(tf)              \
        (tfIsGeneral(tf) && abTag(tfGetExpr(tf)) == AB_Id)
#define                 tfIsApply(tf)           \
        (tfIsGeneral(tf) && abTag(tfGetExpr(tf)) == AB_Apply)
#define                 tfIsTheId(tf,id)        \
        (tfIsId(tf) && tfIdSym(tf) == (id))
#define                 tfIsSelf(tf)            tfIsTheId(tf, ssymSelf)
#define                 tfIsLazyExporter(tf)    \
        (tfIsLibrary(tf) || tfIsArchive(tf) || tfIsSelf(tf))

#define                 tfIdSym(tf)             (tfGetExpr(tf)->abId.sym)
#define                 tfIdSyme(tf)            abSyme(tfGetExpr(tf))
#define                 tfApplyOp(tf)           (tfGetExpr(tf)->abApply.op)

extern Symbol           tfDefineeSymbol         (TForm);
extern Syme             tfDefineeSyme           (TForm);
extern TForm            tfDefineeType           (TForm);
extern TForm            tfDefineeTypeSubst      (TForm);
extern TForm            tfDefineeBaseType       (TForm);
extern TForm            tfDefineeMaybeType      (TForm);

#define                 tfHasPoss(tf)           \
        (tfHasExpr(tf) && abState(tfGetExpr(tf)) == AB_State_HasPoss)
#define                 tfTPoss(tf)             abTPoss(tfGetExpr(tf))

#define                 tfHasUnique(tf)         \
        (tfHasExpr(tf) && abState(tfGetExpr(tf)) == AB_State_HasUnique)
#define                 tfTUnique(tf)           abTUnique(tfGetExpr(tf))

extern bool             tfIsDomainType          (TForm);
extern bool             tfIsDomainMap           (TForm);
extern bool             tfIsCategoryType        (TForm);
extern bool             tfIsCategoryMap         (TForm);

#define tfIsValue(tf)   (!tfIsUnknown(tf) && !tfIsNone(tf))

/*
 * tfUnknown            Type of an expression which is not yet known.
 */
extern AbSyn            abUnknown;
extern TForm            tfUnknown;
#define                 tfIsUnknown(tf)         (tfTag(tf) == TF_Unknown)
#define                 tfIsUnknownMap(tf)      \
        (tfIsAnyMap(tf) && tfIsUnknown(tfMapRet(tf)))

/*
 * tfExit               Type of an expression which does not return.
 */
extern TForm            tfExit;
#define                 tfIsExit(tf)            (tfTag(tf) == TF_Exit)

/*
 * tfLiteral            Type of an expression which is a literal.
 */
extern TForm            tfLiteral;
#define                 tfIsLiteral(tf)         (tfTag(tf) == TF_Literal)

/*
 * tfTest               Type of an expression used as a condition.
 */
extern TForm            tfTest;
#define                 tfIsTest(tf)            (tfTag(tf) == TF_Test)

/*
 * tfType               Type of an expression which is a type.
 */
extern TForm            tfType;
extern TForm            tfTypeTuple;
#define                 tfIsType(tf)            (tfTag(tf) == TF_Type)
#define                 tfIsTypeSyntax(tf)      \
        (tfIsSyntax(tf) && abIsTheId(tfGetExpr(tf), ssymType))

/*
 * tfDomain             Type of an expression which is a domain.
 */
extern TForm            tfDomain;

/*
 * tfCategory           Type of an expression which is a category.
 */
extern TForm            tfCategory;
#define                 tfIsCategory(tf)        (tfTag(tf) == TF_Category)
#define                 tfIsCategorySyntax(tf)  \
        (tfIsSyntax(tf) && abIsTheId(tfGetExpr(tf), ssymCategory))

/*
 * tfBoolean            Type of the test expression of a conditional.
 */
extern TForm            tfBoolean;
extern bool             tfIsBooleanFn           (TForm);

/*
 * tfTextWriter         Type used in the special print operation (<<)
 *                      exported by compound types.
 */
extern TForm            tfTextWriter;

/*
 * tfSingleInteger      Type used in trailing arrays (and elsewhere)
 */
extern TForm tfSingleInteger;

/*
 * tfGeneral            Type form with no special structure.
 */
#define                 tfIsGeneral(tf)         (tfTag(tf) == TF_General)

/*
 * tfLibrary            Type of a library file.
 */
extern TForm            tfLibrary               (Syme);
extern TForm            tfGetLibrary            (Syme);
extern bool             tfIsLibrary             (TForm);
extern bool             tfIsBasicLib            (TForm);
extern Syme             tfLibrarySyme           (TForm);
extern Lib              tfLibraryLib            (TForm);
extern MutString           tfLibraryName           (TForm);

/*
 * tfArchive            Type of a archive file.
 */
extern TForm            tfArchive               (Syme);
extern TForm            tfGetArchive            (Syme);
extern bool             tfIsArchive             (TForm);
extern Syme             tfArchiveSyme           (TForm);
extern Archive          tfArchiveAr             (TForm);
extern TForm            tfArchiveLib            (TForm, Syme);

/*
 * tfDeclare            Type with label. (for dependent types)
 */
extern TForm            tfDeclare               (AbSyn, TForm);
#define                 tfIsDeclare(tf)         (tfTag(tf) == TF_Declare)
extern Symbol           tfDeclareId             (TForm);
extern Syme             tfDeclareSyme           (TForm);
#define                 tfDeclareType(tf)       tfFollowArg(tf, 0)

/*
 * tfDefine             Type with initialization. (for default arguments)
 */
extern TForm            tfDefine                (TForm, AbSyn);
#define                 tfIsDefine(tf)          (tfTag(tf) == TF_Define)
#define                 tfDefineDecl(tf)        tfFollowArg(tf, 0)
#define                 tfDefineVal(tf)         tfFollowArg(tf, 1)
extern TForm            tfDefineOfType          (TForm);
extern bool             tfIsDefineOfType        (TForm);
extern bool             tfIsDefinedType         (TForm);
extern TForm            tfDefinedVal            (TForm);

/*
 * tfAssign             Type with initialization (for default arguments)
 */
extern TForm            tfAssign                (TForm, AbSyn);
#define                 tfIsAssign(tf)          (tfTag(tf) == TF_Assign)
#define                 tfAssignDecl(tf)        tfFollowArg(tf, 0)
#define                 tfAssignVal(tf)         tfFollowArg(tf, 1)

/*
 * tfMap                Type of an expression which is a function.
 */
extern TForm            tfAnyMap                (TForm, TForm, bool);
#define                 tfIsAnyMap(tf)          \
        (tfIsMap(tf) || tfIsPackedMap(tf))

extern TForm            tfPackedMap             (TForm arg, TForm ret);
#define                 tfIsPackedMap(tf)       (tfTag(tf) == TF_PackedMap)

extern TForm            tfMap                   (TForm arg, TForm ret);
#define                 tfIsMap(tf)             (tfTag(tf) == TF_Map)
extern bool             tfIsDependentMap        (TForm);
#define                 tfIsMapSyntax(tf)       \
        (tfIsSyntax(tf) && abIsAnyMap(tfGetExpr(tf)))
extern bool             tfMapHasDefaults        (TForm);

#define                 tfMapArg(tf)            tfFollowArg(tf, 0)
extern Length           tfMapArgc               (TForm);
extern TForm            tfMapArgN               (TForm, Length);
extern TForm            tfMapMultiArgN          (TForm, Length, Length);
extern AbEmbed          tfMapMultiArgEmbed      (TForm, Length);

#define                 tfMapRet(tf)            tfFollowArg(tf, 1)
extern Length           tfMapRetc               (TForm);
extern TForm            tfMapRetN               (TForm, Length);
extern TForm            tfMapMultiRetN          (TForm, Length, Length);
extern AbEmbed          tfMapMultiRetEmbed      (TForm, Length);

/*
 * tfCross              Type of cross products.  Possibly heterogeneous.
 */
extern TForm            tfCross                 (Length, ...);
extern TForm            tfCrossFrMulti          (TForm);
#define                 tfIsCross(tf)           (tfTag(tf) == TF_Cross)
extern bool             tfIsDependentCross      (TForm);

extern Length           tfCrossArgc             (TForm);
#define                 tfCrossArgN(tf,n)       tfFollowArg(tf, n)
#define                 tfCrossArgv(tf)         tfArgv(tf)

/*
 * tfMultiple           Type of a sequence of argument/return values.
 */
extern TForm            tfMulti                 (Length, ...);
#define                 tfIsMulti(tf)           (tfTag(tf) == TF_Multiple)
extern bool             tfIsDependentMulti      (TForm);
#define                 tfIsEmptyMulti(tf)      \
        (tfIsMulti(tf) && tfMultiArgc(tf) == 0)

extern TForm            tfMultiFrList           (TFormList);
extern TForm            tfMultiFrTUnique        (Sefo);
extern Length           tfMultiArgc             (TForm);
#define                 tfMultiArgN(tf,n)       tfFollowArg(tf, n)
#define                 tfMultiArgv(tf)         tfArgv(tf)
extern bool             tfMultiHasDefaults      (TForm);

extern AbEmbed          tfAsMultiEmbed          (TForm, Length argc);
extern Length           tfAsMultiArgc           (TForm);
extern TForm            tfAsMultiArgN           (TForm, Length argc, Length n);
extern AbSyn            tfAsMultiSelectArg      (AbSyn, Length argc, Length n,
                                                 AbSynGetter, TForm, bool *,
                                                 Length *);
extern AbSyn            tfMapSelectArg          (TForm, AbSyn, Length);
/*
 * tfTuple              Type of homogeneous tuples.  Kleene closure.
 */
extern TForm            tfTuple                 (TForm);
#define                 tfIsTuple(tf)           (tfTag(tf) == TF_Tuple)
#define                 tfTupleArg(tf)          tfFollowArg(tf, 0)

/*
 * tfNone               Type of an expression which has no value.
 */
#define                 tfNone()                tfMulti(0)
#define                 tfIsNone(tf)            tfIsEmptyMulti(tf)

/*
 * tfEnumerate          Type of an explicit set.
 */
extern TForm            tfEnum                  (Stab, AbSyn);
#define                 tfIsEnum(tf)            (tfTag(tf) == TF_Enumerate)
extern Length           tfEnumArgc              (TForm);
extern Symbol           tfEnumId                (TForm, Length);
extern Symbol           tfCompoundId            (TForm, Length);
#define                 tfEnumArgN(tf,n)        tfFollowArg(tf, n)

/*
 * tfRawRecord          Type of a raw record object.
 */
#define                 tfIsRawRecord(tf)       (tfTag(tf) == TF_RawRecord)
extern                  Length tfRawRecordArgc  (TForm);
#define                 tfRawRecordArgN(tf,n)   tfFollowArg(tf, n)

/*
 * tfRecord             Type of a record object.
 */
#define                 tfIsRecord(tf)          (tfTag(tf) == TF_Record)
extern Length           tfRecordArgc            (TForm);
#define                 tfRecordArgN(tf,n)      tfFollowArg(tf, n)

/*
 * tfTrailingArray      Type of a trailing array object.
 */
#define                 tfIsTrailingArray(tf)           (tfTag(tf) == TF_TrailingArray)
#define                 tfTrailingIArg(tf)      tfFollowArg(tf,int0)
#define                 tfTrailingAArg(tf)      tfFollowArg(tf,1)

/*
 * tfUnion              Type of a union object.
 */
#define                 tfIsUnion(tf)           (tfTag(tf) == TF_Union)
extern Length           tfUnionArgc             (TForm);
#define                 tfUnionArgN(tf,n)       tfFollowArg(tf, n)

/*
 * tfAdd
 */
#define                 tfIsAdd(tf)             (tfTag(tf) == TF_Add)
#define                 tfAddBase(tf)           tfFollowArg(tf, 0)

/*
 * tfWith               Type of a domain object.
 */
extern TForm            tfWith                  (TForm, TForm);
extern TForm            tfWithFrSymes           (SymeList);
extern TForm            tfWithFrAbSyn           (AbSyn);
#define                 tfIsWith(tf)            (tfTag(tf) == TF_With)
#define                 tfWithBase(tf)          tfFollowArg(tf, 0)
#define                 tfWithWithin(tf)        tfFollowArg(tf, 1)
#define                 tfWithBody(tf)          tfGetExpr(tfWithWithin(tf))
#define                 tfIsWithSyntax(tf)      \
        (tfIsSyntax(tf) && abHasTag(tfGetExpr(tf), AB_With))
#define                 tfIsNotDomain(tf)       \
        (tfIsType(tf) || tfIsWith(tf) || tfIsWithSyntax(tf) || \
         tfIsJoin(tf) || tfIsIf(tf) || tfIsThird(tf))

/*
 * tfThird              Type of a category object.
 */
extern TForm            tfThird                 (SymeList);
extern TForm            tfThirdFrTForm          (TForm);
#define                 tfIsThird(tf)           (tfTag(tf) == TF_Third)
#define                 tfThirdRestrictions(tf) tfFollowArg(tf, 0)

/*
 * tfGenerator          Type of a generator object.
 */
extern TForm            tfGenerator             (TForm);
#define                 tfIsGenerator(tf)       (tfTag(tf) == TF_Generator)
extern bool             tfIsGeneratorFn         (TForm);
#define                 tfGeneratorArg(tf)      tfFollowArg(tf, 0)

/*
 * tfSubst              Type which has a pending substitution.
 *
 *      Substitution is allowed to create lazy type forms whose ultimate
 *      value is determined by performing the substitution specified by
 *      tf->sigma on tf->argv[0].
 */
extern TForm            tfSubst                 (AbSub, TForm);
#define                 tfIsSubst(tf)           (tfTag(tf) == TF_Subst)
#define                 tfSubstSigma(tf)        ((tf)->sigma)
#define                 tfSubstArg(tf)          tfFollowArg(tf, 0)
#define                 tfIsSubstOf(tf,fin)     \
        (tfIsSubst(fin) && tfSubstArg(fin) == (tf))

/*
 * tfSyntax             Type which has not been semantically analyzed.
 *
 *      Undescended, not semantically analyzed. Will most likely be
 *      destructively modified into a forwarding tform, so argc == 1.
 */
#define                 tfIsSyntax(tf)          (tfTag(tf) == TF_Syntax)

/*
 * tfTrigger            Type of a symbol in the process of loading.
 */
extern TForm            tfTrigger               (Syme);
#define                 tfIsTrigger(tf)         (tfTag(tf) == TF_Trigger)
extern Lib              tfTriggerLib            (TForm);
extern TForm            tfForwardFrTrigger      (TForm, TForm);

#define                 tfTriggerSyme(tf)       car(tfSymes(tf))
#define                 tfIsTriggerSyme(tf,s)   (tfIsTrigger(tf) && \
                                                 tfTriggerSyme(tf) == (s))
#define                 tfSetTriggerSyme(tf,s)  (tfTriggerSyme(tf) = (s))

/*
 * tfForward            Type which points to another type.
 *
 * tfFollow(tf)         Resets tf after following forwarding pointers
 *                      tf should be an lvalue.
 */
#define                 tfIsForward(tf)         (tfTag(tf) == TF_Forward)
#define                 tfFollow(tf)            ((tf) = tfFollowFn(tf))
extern TForm            tfFollowOnly            (TForm);
extern TForm            tfFollowSubst           (TForm);
extern TForm            tfFollowFn              (TForm);
extern TForm            tfFollowArg             (TForm, Length);

/*
 * tfRaw                Type which gives the type-dependent format for values
 *                      from another type.
 */
#define                 tfIsRaw(tf)             (tfTag(tf) == TF_Raw)
#define                 tfRawArg(tf)            tfFollowArg(tf, 0)
extern TForm            tfRawType               (TForm);

/*
 * tfVariable           Type which represents another type.
 *
 *      Will most likely be destructively modified into a forwarding
 *      tform, so argc == 1.  Probably created from an AB_Blank and
 *      that is the __absyn value.
 */
#define                 tfIsVariable(tf)        (tfTag(tf) == TF_Variable)
extern TForm            tfSetVariable           (TForm, TForm);

/*
 * tfIf                 Type used to construct conditional categories.
 */
#define                 tfIsIf(tf)              (tfTag(tf) == TF_If)
extern TForm            tfIf                    (TForm, TForm, TForm);
#define                 tfIfTest(tf)            tfFollowArg(tf, 0)
#define                 tfIfThen(tf)            tfFollowArg(tf, 1)
#define                 tfIfElse(tf)            tfFollowArg(tf, 2)
#define                 tfIfCond(tf)            tfExpr(tfIfTest(tf))

/*
 * tfJoin               Type used to construct compound categories.
 */
#define                 tfIsJoin(tf)            (tfTag(tf) == TF_Join)
extern TForm            tfJoin                  (Length, ...);
extern TForm            tfJoinFrList            (TFormList);
#define                 tfJoinArgc(tf)          tfArgc(tf)
#define                 tfJoinArgN(tf,n)        tfFollowArg(tf, n)
#define                 tfJoinArgv(tf)          tfArgv(tf)

/*
 * tfMeet               Type used to construct compound categories.
 */
#define                 tfIsMeet(tf)            (tfTag(tf) == TF_Meet)
extern TForm            tfMeet                  (Length, ...);
#define                 tfMeetArgc(tf)          tfArgc(tf)
#define                 tfMeetArgN(tf,n)        tfFollowArg(tf, n)
#define                 tfMeetArgv(tf)          tfArgv(tf)

/*
 * tfHas                Type used to represent conditions in conditional types.
 */
extern AbSyn            abHas                   (Syme, TForm);
extern TForm            tfHas                   (Syme, TForm);

/*
 * tfExcept             Type used to represent potential exceptions in types.
 */

extern TForm            tfExcept                (TForm, TForm);
#define                 tfIsExcept(tf)          (tfTag(tf) == TF_Except)
#define                 tfExceptType(tf)        tfFollowArg(tf, 0)
#define                 tfExceptExcept(tf)      tfFollowArg(tf, 1)
extern TForm            tfIgnoreExceptions      (TForm);

/*
 * tfExtend             Operations to construct the types of extensions.
 */
#define                 tfExtendArgc(tf)        \
        (tfIsAnyMap(tf) ? tfArgc(tfMapRet(tf)) : tfArgc(tf))
#define                 tfExtendDone(tf)        \
        (tfExtendNext(tf) == tfExtendArgc(tf))
extern bool             tfIsExtendTemplate      (TForm);
extern bool             tfCanExtend             (TForm, TForm);
extern TForm            tfExtendEmpty           (TForm, Length);
extern Length           tfExtendNext            (TForm);
extern void             tfExtendFill            (TForm, Length, TForm);
extern TForm            tfExtendeeSubst         (Stab, TForm, TForm);
extern void             tfExtendSubst           (Stab, TForm);
extern void             tfExtendFinish          (TForm);

/*
 * tfReference          Type of a reference object.
 */
extern TForm            tfReference             (TForm);
#define                 tfIsReference(tf)       (tfTag(tf) == TF_Reference)
extern bool             tfIsReferenceFn         (TForm);
#define                 tfReferenceArg(tf)      tfFollowArg(tf, 0)

/* These two assume that the symbols have unique tforms (ie types) */
extern TForm            tfFrSymbol(Symbol);
extern TForm            tfFrSymbolPair(Symbol, Symbol);

/*****************************************************************************
 *
 * :: Types of operations used in syntactic sugar
 *
 ****************************************************************************/

/*
 * integer: Literal -> X
 * float:   Literal -> X
 * string:  Literal -> X
 *
 * Used for abstract literals, e.g. 22.3e22 => [Apply float "22.3e22"]
 * tfIsLitOpType(tf)  verifies that tf is of the form Literal -> X.
 * tfLitOpTypeRet(tf) returns X.
 */
extern bool             tfIsLitOpType           (TForm);
extern TForm            tfLitOpTypeRet          (TForm);

/*
 * test: X -> Test
 *
 * Used for condition context, e.g. while c => [While [Apply test c]]
 * tfIsTestOpType(tf)  verifies that tf is of the form X -> Test.
 * tfTestOpTypeArg(tf) returns X.
 */
extern bool             tfIsTestOpType          (TForm);
extern TForm            tfTestOpTypeArg         (TForm);

/*
 * generator: X -> Generator E
 *
 * Used for abstract iteration, e.g. for i in x => [For i [Apply generator x] .]
 * tfIsGeneratorOpType(tf)  verifies that tf is of the form X -> Generator E.
 * tfGeneratorOpTypeArg(tf) returns X.
 * tfGeneratorOpTypeRet(tf) returns E.
 */
extern bool             tfIsGeneratorOpType     (TForm);
extern TForm            tfGeneratorOpTypeArg    (TForm);
extern TForm            tfGeneratorOpTypeRet    (TForm);

extern bool             tfIsTypeTuple           (TForm);
extern bool             tfIsCategoryContext     (TForm, AbSyn);
extern bool             tfIsNoValueContext      (TForm, AbSyn);

extern bool             tfHasPrint              (Stab, TForm);


/*****************************************************************************
 *
 * :: Implicit category stuff
 *
 ****************************************************************************/

extern TForm            tfImplPAOps;
extern void             tfInitImplict(void);
extern TForm            tfCatFrDom(TForm);
extern bool             tfDomHasImplicit(TForm);
extern bool             tfCatHasImplicit(TForm);
extern Syme             tfImplicitExport(Stab, SymeList, Syme);

#endif /* !_TFORM_H_ */
