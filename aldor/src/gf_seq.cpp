///////////////////////////////////////////////////////////////////////////////
//
// gf_seq.cpp: Generating code for add/default definition levels
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

#include "gf_seq.h"
#include "gf_util.h"
#include "gf_add.h"
#include "gf_prog.h"
#include "gf_implicit.h"

extern  bool genfExportDebug;   /* gf_add.c */
bool    genfImplicitDebug       = false;

#define genfExportDEBUG(s)      DEBUG_IF(genfExportDebug, s)
#define genfImplicitDEBUG(s)    DEBUG_IF(genfImplicitDebug, s)

typedef enum {
        DG_Const,
        DG_Lambda,
        DG_Cond,
        DG_Fix,
        DG_Type,
        DG_NonDefn
} DefGroupTag;

typedef struct defGroup *DefGroup;
typedef struct defSet   *DefSet;


struct defGroup {
        DefGroupTag     tag;
        AbSyn           ab;
        int             ordinal;
        List<Syme>        defines;
        List<Syme>        usedSymes;
        List<DefGroup>    usedDefs;
};

struct defSet {
        int             argc;
        List<DefGroup>    defs;
        List<Syme>        defines;
        List<Syme>        exports;
};

/*
 * A recursive type frame is a system of equations over all domain/category
 * valued bindings at this add/default definition level.  Every binding has a
 * stable placeholder (seen by references) and a separate RHS temporary (set
 * by its definition).  Once all RHSs have been evaluated, repeated generic
 * resolve operations establish the mutual fixed point.  No distinction is
 * made here between aliases and constructors: those are merely graph shapes.
 */
typedef struct typeFrameEntry {
        Syme            syme;
        Foam            placeholder;
        Foam            rhs;
        bool            isCategory;
        bool            generated;
} *TypeFrameEntry;

typedef struct typeFrame {
        GenFoamState       owner;
        List<TypeFrameEntry> entries;
        List<Syme>           deferredExports;
        int                  count;
        int                  generated;
        bool                 resolved;
} *TypeFrame;

static TypeFrame gen0TypeFrame = NULL;

#define dgTag(dg)               ((dg)->tag)
#define dgUses(dg)              ((dg)->usedSymes)
#define dgDefines(dg)           ((dg)->defines)
#define dgStmt(dg)              ((dg)->ab)

#define dgSetDefs(ds)           ((ds)->defs)

local void      gen0EnsureUsedSymes     (DefSet, DefGroup);
local void      gen0Fix                 (AbSyn, List<Syme>);
local void      gen0DefTypeCond         (AbSyn, List<Syme>);

local void              dgSortDefs              (DefSet);
local int               dgSortClassify          (DefGroup);
local DefSet            dgSeqToDefSet           (AbSyn, List<Syme>);
local DefGroup          dgMakeFixGroup          (List<DefGroup>);
local DefGroup          dgStmtToDef             (AbSyn, int);
local void              dgAbGetUsedSymes        (AbSyn, DefGroup);
local DefGroupTag       dgGetTag                (AbSyn);
local bool              dgSymeIsLocal           (Syme);
local DefGroup          dgNewGroup              (DefGroupTag, AbSyn);
local void              dgFreeGroup             (DefGroup);
local List<DefGroup>      dgProcessDependencies   (List<DefGroup>, bool);
local Syme              dgSymeImportToExport    (Syme);
local List<DefGroup>      dgSortDependencies      (DepDag, List<DepDag>, bool);
local void              dgCycleError            (DepDag, List<DepDag>);
local void              dgOrderError            (DepDag, List<DepDag>);
local MutString            dgPretty                (DefGroup);
local Syme              dgGetAbMeaning          (AbSyn);
local List<Syme>          dgFindDependencies      (AbSyn);

local TypeFrame         gen0TypeFrameNew        (DefSet);
local void              gen0TypeFrameFree       (TypeFrame);
local TypeFrameEntry    gen0TypeFrameFind       (Syme);
local bool              gen0TypeFrameHasGroup   (DefGroup);
local void              gen0TypeFrameNoteGroup  (DefGroup, AbSyn);
local void              gen0TypeFrameResolve    (AbSyn);
local void              gen0TypeFrameDeferExport(Syme);
local void              gen0TypeFrameFlushExports(void);
local Foam              gen0TypeFrameMakeDummy  (TypeFrameEntry);
local Foam              gen0TypeFrameResolveCall(TypeFrameEntry);


/******************************************************************************
 *
 * :: Generating sequences
 *
 *****************************************************************************/

void
gen0DefTypeSequence(AbSyn ab, List<Syme> exports)
{
        DefSet          set;
        List<DefGroup>  lst;
        List<Syme>      symes, isymes;
        TypeFrame       savedTypeFrame = gen0TypeFrame;
        bool            ownTypeFrame = false;

        genfImplicitDEBUG({
                (void)fprintf(dbOut, "gen0DefTypeSequence():");
                fnewline(dbOut);
                for (symes = exports;symes;symes = cdr(symes)) {
                        Syme sy  = car(symes);
                        bool im  = symeIsImplicit(sy);
                        MutString s = symePretty(sy);

                        (void)fprintf(dbOut,"   [%c] %s",(im ? '*' : ' '),s);
                        fnewline(dbOut);
                }
                fnewline(dbOut);
        });

        if (abTag(ab) == AB_Nothing)
                return;

        set = dgSeqToDefSet(ab, exports);
        dgSortDefs(set);

        /*
         * Frames are scoped to one add/default ExportState.  A nested add is
         * generated while its parent's RHS is being built; it must neither
         * inherit nor enqueue exports into the parent's unresolved frame.
         * Recursive calls for conditionals in the *same* add do share it.
         */
        if (gen0TypeFrame && gen0TypeFrame->owner != gen0State)
                gen0TypeFrame = NULL;

        if (!gen0TypeFrame) {
                gen0TypeFrame = gen0TypeFrameNew(set);
                ownTypeFrame = gen0TypeFrame != NULL;
        }

        lst = dgSetDefs(set);

        while (lst) {
                DefGroup dg = car(lst);
                genfExportDEBUG({
                          printf("looking at:\n");
                          abWrSExpr(dbOut, dgStmt(dg),int0);
                          printf("defines:\n");
                          symeListPrintDb(dg->defines);
                          printf("uses:\n");
                          symeListPrintDb(dgUses(dg));
                          });
                gen0EnsureUsedSymes(set, dg);

                switch (dg->tag) {
                  case DG_Fix:
                        if (gen0TypeFrame && gen0TypeFrameHasGroup(dg)) {
                                int i;
                                AbSyn fix = dgStmt(dg);
                                for (i = 0; i < abArgc(fix); i++)
                                        genFoamStmt(abArgv(fix)[i]);
                        }
                        else
                                gen0Fix(dgStmt(dg), dg->defines);
                        break;
                  case DG_Cond:
                        gen0DefTypeCond(dgStmt(dg), exports);
                        break;
                  default:
                        genFoamStmt(dgStmt(dg));
                        break;
                }

                if (gen0TypeFrame)
                        gen0TypeFrameNoteGroup(dg, dgStmt(dg));

                for (symes = dg->defines; symes; symes = cdr(symes)) {
                        if (!symeIsExport(car(symes)))
                                continue;
                        if (gen0TypeFrame && !gen0TypeFrame->resolved)
                                gen0TypeFrameDeferExport(car(symes));
                        else
                                gen0TypeAddExportSlot(car(symes));
                }
                dgFreeGroup(dg);
                lst = cdr(lst);
        }

        /*
         * A well-formed frame resolves as soon as its last RHS is generated.
         * Keep this final call as a defensive completion for unusual grouping.
         */
        if (ownTypeFrame && gen0TypeFrame && !gen0TypeFrame->resolved)
                gen0TypeFrameResolve(ab);

        /* Collect implicit exports */
        isymes = listNil<Syme>;
        for (symes = exports;symes;symes = cdr(symes))
        {
                Syme syme = car(symes);

                if (symeIsImplicit(syme) && symeIsExport(syme)) {
                        /* Assume unconditional */
                        symeSetUnconditional(syme);
                        isymes = listCons<Syme>(syme, isymes);
                }
        }

        if (isymes != listNil<Syme>) {
                for (symes = isymes;symes;symes = cdr(symes))
                {
                        Syme syme = car(symes);

                        gen0ImplicitExport(syme, exports, ab);
                        gen0TypeAddExportSlot(syme);
                }
        }

        if (ownTypeFrame) {
                TypeFrame frame = gen0TypeFrame;
                gen0TypeFrame = savedTypeFrame;
                gen0TypeFrameFree(frame);
        }
        else if (gen0TypeFrame != savedTypeFrame && savedTypeFrame)
                gen0TypeFrame = savedTypeFrame;
}

/*!! Should be expanded to allow imports and inheritance
 *   from conditional symes
 */
local void
gen0DefTypeCond(AbSyn ab, List<Syme> exports)
{
        List<Foam> topLines;
        int     l1 = gen0State->labelNo++, l2 = gen0State->labelNo++;
        bool flag;

        /* COND-DEF */
        AbLogic saveCond;
        AbSyn   nTest;
        Stab    stab = abStab(ab) ? abStab(ab) : stabFile();

        flag = gen0AddImportPlace(&topLines);

        nTest = abExpandDefs(stab, (ab->abIf.test)); /* COND-DEF */

        gen0AddStmt(foamNewIf(genFoamBit(ab->abIf.test), l1), ab);
        ablogAndPush(&gfCondKnown, &saveCond, nTest, false); /* COND-DEF */
        gen0DefTypeSequence(ab->abIf.elseAlt, exports);
        ablogAndPop (&gfCondKnown, &saveCond); /* COND-DEF */
        gen0AddStmt(foamNewGoto(l2), ab);

        gen0AddStmt(foamNewLabel(l1), ab);
        ablogAndPush(&gfCondKnown, &saveCond, nTest, true); /* COND-DEF */
        gen0DefTypeSequence(ab->abIf.thenAlt, exports);
        ablogAndPop (&gfCondKnown, &saveCond); /* COND-DEF */
        gen0AddStmt(foamNewLabel(l2), ab);

        if (flag) gen0ResetImportPlace(topLines);
}


local void
gen0EnsureUsedSymes(DefSet set, DefGroup dg)
{
        List<Syme> symes = dgUses(dg);

        while (symes) {
                Syme syme = car(symes);
                if (symeIsExport(syme) && gen0SymeInit(syme) == NULL) {
                        gen0InitExport(syme);
                        gen0AddInit(foamNewSet(gen0ExpMapRef(syme),
                                               foamNewBool(false)));
                        gen0AddStmt(foamNewSet(gen0ExpMapRef(syme),
                                               foamNewBool(true)), NULL);
                        set->defines = listCons<Syme>(syme, set->defines);
                        gen0SymeSetInit(syme, gen0Syme(syme));
                }
                symes = cdr(symes);
        }
}

/******************************************************************************
 *
 * :: Fix-Pointing expressions...
 *
 *****************************************************************************/

typedef struct {
        List<Foam> inits;
        List<Foam> finis;
} FixInfoStruct, *FixInfo;

local void      gen0FixDummy            (Syme, AbSyn, FixInfo);
local Foam      gen0FixMakeDummyCat     (void);
local Foam      gen0FixMakeDummyDom     (void);
local Foam      gen0FixFillDom          (Foam, Foam);
local Foam      gen0FixFillCat          (Foam, Foam);
local void      gen0FixGenStmts         (List<Foam>, AbSyn);

local void
gen0Fix(AbSyn ab, List<Syme> dsymes )
{
        List<Syme> symes;
        FixInfoStruct info_s;
        FixInfo       info = &info_s;
        int i;

        info->inits = listNil<Foam>;
        info->finis = listNil<Foam>;
        for (symes = dsymes; symes ; symes = cdr(symes))
                gen0FixDummy(car(symes), ab, info);

        info->inits = listNReverse<Foam>(info->inits);
        info->finis = listNReverse<Foam>(info->finis);

        gen0FixGenStmts(info->inits, ab);

        for (i = 0; i< abArgc(ab) ; i++)
                genFoamStmt(abArgv(ab)[i]);

        gen0FixGenStmts(info->finis, ab);

        listFree<Foam>(info->inits);
        listFree<Foam>(info->finis);
        return ;
}

local void
gen0FixDummy(Syme syme, AbSyn pos, FixInfo info)
{
        TForm   tf = symeType(syme);
        Foam    self = gen0Syme(syme);
        Foam    new_;

#if AXL_EDIT_1_1_13_01
        /* Record the initialisation, if necessary */
        if (symeIsExport(syme) && !gen0SymeInit(syme))
                gen0SymeSetInit(syme, foamCopy(self));
#endif

        if (tfSatDom(tf)) {
                new_ = gen0Temp(FOAM_Word);
                info->inits = listCons<Foam>
                        (foamNewSet(foamCopy(self), gen0FixMakeDummyDom()),
                         info->inits);
                info->finis = listCons<Foam>
                        (gen0FixFillDom(foamCopy(new_), foamCopy(self)),
                         info->finis);

        }
        else if (tfSatCat(tf)) {
                new_ = gen0Temp(FOAM_Word);
                info->inits = listCons<Foam>
                        (foamNewSet(foamCopy(self), gen0FixMakeDummyCat()),
                         info->inits);
                info->finis = listCons<Foam>
                        (gen0FixFillCat(foamCopy(new_), foamCopy(self)),
                         info->finis);

        }
#if AXL_EDIT_1_1_13_01
        else {
                foamFree(self);
                return;
        }
#else
        else {
                MutString  msg = strPrintf("bad syme (%s):(%s) passed to %s",
                                symePretty(syme), tfPretty(tf), "gen0FixDummy");
                comsgFatal(pos, ALDOR_F_Bug, msg);
        }
#endif
        info->inits = listCons<Foam>(foamNewDef(foamCopy(new_),
                                                foamCopy(self)), info->inits);
        info->finis = listCons<Foam>(foamNewSet(foamCopy(self),
                                                foamCopy(new_)), info->finis);

        foamFree(new_);
        foamFree(self);
}

local Foam
gen0FixMakeDummyCat()
{
        return gen0BuiltinCCall(FOAM_Word, "categoryMakeDummy", "runtime", int0);
}

local Foam
gen0FixFillCat(Foam new_, Foam self)
{
        return gen0BuiltinCCall(FOAM_NOp, "categoryFill!", "runtime", 2, new_, self);
}

local Foam
gen0FixMakeDummyDom()
{
        return gen0BuiltinCCall(FOAM_Word, "domainMakeDummy", "runtime", int0);
}

local Foam
gen0FixFillDom(Foam new_, Foam self)
{
        return gen0BuiltinCCall(FOAM_NOp, "domainFill!", "runtime", 2, new_, self);
}


local void
gen0FixGenStmts(List<Foam> lst, AbSyn pos)
{
        while (lst) {
                gen0AddStmt(car(lst), pos);
                lst = cdr(lst);
        }
}

/******************************************************************************
 *
 * :: DefGroup/Set manipulation
 *
 *****************************************************************************/

/*
 * Two stages in defining a top level export
 * 1. Defining
 * 2. Exporting
 *
 * Defining is safe providing all the symes used by the definition
 * have either been defined or imported, or the defn is a with, add or lambda
 *
 * Exporting is safe providing all the symes used in the type of the
 * definition have been defined.
 *
 * 'Used' here is a closure over the textual definition ignoring lambda-inducing
 * constructs. [NB this is not sufficient for absolute safety]
 *
 * We also must try to spot mutual recursion in definitions of types.
 * In this situation an AB_Fix node is created, which can then be dealt with by
 * gf_add.c
 *
 * Sort process:
 *   1) Find dependencies
 *   2) Compute dependencies, NB subsumption
 *   3) Sort
 *   4) transform to linear format
 *
 * We sort into the following groups:
 *   Tough maps         (list[0]) (domain/category DG_Const -> DG_Fix)
 *   Simple maps        (list[1]) (DG_Lambda)
 *   Types              (list[2]) (DG_Type)
 *   Other junk         (list[3]) (DG_Const, DG_Cond)
 *   Non-definitions    (list[4]) (DG_NonDefn)
 *
 * Locals used to be lumped together with "other junk" as DG_Const nodes.
 * Then local lambda's were split into a group of their own after DG_Type.
 * Simple locals (just one local being defined) are now placed in their
 * proper group (local lambdas go in DG_Lambda, local consts in "other junk"
 * etc); complicated locals stay in "other junk" unless they are DG_NonDefn.
 *
 * It would be nice if we could use dgProcessDependencies() to re-order
 * the non-lazy constants so that their values are correctly computed.
 * Unfortunately some people write code in add bodies in which non-lazy
 * constants depend on the values of local variables. Thus re-ordering
 * is not viable.
 *
 * Instead, we now place all members of "other junk" in with "non-defns"
 * for the overall sorting operation. We also take a separate copy of
 * "other junk" and perform dependency analysis to check for user bugs.
 */

#define DG_GROUP_TOUGH_MAP      (0)
#define DG_GROUP_SIMPLE_MAP     (1)
#define DG_GROUP_TYPE           (2)
#define DG_GROUP_OTHER_JUNK     (3)
#define DG_GROUP_NON_DEFN       (4)

#define DG_MAX                  (DG_GROUP_NON_DEFN+1)

static List<Syme> dgExports;

local void
dgSortDefs(DefSet set)
{
        int             i;
        List<DefGroup>    otherJunk = listNil<DefGroup>;
        List<DefGroup>    lists[DG_MAX];
        List<DefGroup>    lst = dgSetDefs(set);
        DefGroup        types;

        for (i=0; i<DG_MAX; i++)
                lists[i]=listNil<DefGroup>;


#if SORT_NON_LAZY_CONSTANTS
        /*
         * This code separates non-lazy constants from other
         * non-definitions in an add-body before sorting them
         * so that their dependencies will be satisfied when
         * code generation takes place. Note that this causes
         * severe problems when people write code in which
         * non-lazy constants depend on the value of variables.
         */
        while (lst) {
                DefGroup dg = car(lst);


                /* Classify into one of the sets */
                i = dgSortClassify(dg);


                /* Add to the chosen set */
                lists[i] = listCons<DefGroup>(dg, lists[i]);


                /* Get the next group */
                lst = cdr(lst);
        }


        /* The "other junk" group needs special care */
        tmp = lists[DG_GROUP_OTHER_JUNK];
        tmp = dgProcessDependencies(tmp, true);
        lists[DG_GROUP_OTHER_JUNK] = tmp;
#else
        /*
         * This code keeps non-lazy constants in the same
         * set as other non-definitions in an add-body and
         * does not sort them. However, these constants
         * are recorded separately and analysed for bad
         * dependencies.
         */
        while (lst) {
                DefGroup dg = car(lst);


                /* Classify into one of the sets */
                i = dgSortClassify(dg);


                /* DG_GROUP_OTHER_JUNK is treated differently */
                if (i == DG_GROUP_OTHER_JUNK) {
                        /* Record DG_GROUP_OTHER_JUNK separately */
                        otherJunk = listCons<DefGroup>(dg, otherJunk);


                        /* But also place them with DG_GROUP_NON_DEFN */
                        i = DG_GROUP_NON_DEFN;
                }


                /* Add to the relevent set */
                lists[i] = listCons<DefGroup>(dg, lists[i]);


                /* Get the next group */
                lst = cdr(lst);
        }


        /* Check for bad dependencies */
        assert(!lists[DG_GROUP_OTHER_JUNK]);
        otherJunk = dgProcessDependencies(otherJunk, false);
        listFree<DefGroup>(otherJunk);
#endif


        /* Concatenate the simple groups (assume !DG_GROUP_TOUGH_MAP) */
        assert(!lst);
        for (i = DG_MAX - 1; i; i--) {
                List<DefGroup>    tmp = listNReverse<DefGroup>(lists[i]);
                lst = listNConcat<DefGroup>(tmp, lst);
        }


        /* Tough maps require special care */
        if (lists[DG_GROUP_TOUGH_MAP]) {
                types = dgMakeFixGroup(lists[DG_GROUP_TOUGH_MAP]);
                lst = listCons<DefGroup>(types, lst);
                listFreeDeeply<DefGroup>(lists[DG_GROUP_TOUGH_MAP],dgFreeGroup);
        }

        dgSetDefs(set) = lst;
}


local int
dgSortClassify(DefGroup dg)
{
        List<Syme> lst;
        TForm    tf;
        int class_;

        if (dgTag(dg) == DG_Lambda)
                return DG_GROUP_SIMPLE_MAP;
        else if (dgTag(dg) == DG_Type)
                return DG_GROUP_TYPE;
        else if (dgTag(dg) == DG_NonDefn)
                return DG_GROUP_NON_DEFN;


        lst = dg->defines;
        class_ = DG_GROUP_OTHER_JUNK;

        while (lst) {
                tf = tfDefineeType(symeType(car(lst)));
                if (tfSatDom(tf) || tfSatCat(tf))
                        class_ = DG_GROUP_TOUGH_MAP;
                lst = cdr(lst);
        }
        return class_;
}

local List<DefGroup>
dgSeqToDefs(AbSyn ab)
{
        List<DefGroup> lst = listNil<DefGroup>;
        int i;

        switch(abTag(ab)) {
          case AB_Sequence:
          case AB_Default:
                for (i = 0; i < abArgc(ab); i++) {
                        AbSyn sub = ab->abSequence.argv[i];
                        lst = listNConcat<DefGroup>(dgSeqToDefs(sub), lst);
                }
                break;
          default:
                lst = listCons<DefGroup>(dgStmtToDef(ab, int0), lst);
        }


        /*
         * This function may be applied to nested sequences. To
         * ensure that all the definitions stay in the same order
         * as in the source code, it is essential that the list
         * is not reversed during recursive calls. Instead the
         * result of the top-level call must be reversed.
         */
        return lst;
}

local DefSet
dgSeqToDefSet(AbSyn ab, List<Syme> exports)
{
        DefSet new_ = (DefSet) stoAlloc(OB_Other, sizeof(*new_));

        dgExports       = exports;
        new_->argc       = 0;
        new_->defs       = listNReverse<DefGroup>(dgSeqToDefs(ab));
        new_->exports    = exports;
        dgExports       = NULL;

        return new_;
}


local DefGroup
dgMakeFixGroup(List<DefGroup> dlst)
{
        List<AbSyn>    stmts   = listNil<AbSyn>;
        List<Syme>     defines = listNil<Syme>;
        List<Syme>     uses    = listNil<Syme>;
        List<Syme>     lst;
        DefGroup     new_;
        int          ordinal = 0;

        while (dlst) {
                DefGroup dg = car(dlst);
                stmts       = listCons<AbSyn>(dg->ab, stmts);
                defines = listConcat<Syme>(dg->defines, defines);
                uses    = listConcat<Syme>(dg->usedSymes, uses);
                ordinal = ordinal < dg->ordinal ? dg->ordinal : ordinal;

                dlst = cdr(dlst);
        }
        new_ = dgNewGroup(DG_Fix, abNewOfList(AB_Fix, SrcPos::none,
                                        listNReverse<AbSyn>(stmts)));

        lst = uses;
        uses = listNil<Syme>;
        while (lst) {
                if (!listMemq<Syme>(defines, car(lst)))
                        uses = listCons<Syme>(car(lst), uses);
                lst = cdr(lst);
        }
        new_->defines = defines;
        new_->usedSymes = uses;
        new_->ordinal = ordinal;

        return new_;
}

local DefGroup
dgStmtToDef(AbSyn absyn, int i)
{
        DefGroup dg = dgNewGroup(dgGetTag(absyn), absyn);

        dgAbGetUsedSymes(absyn, dg);
        if (dg->tag == DG_Cond) {
                listFree<Syme>(dg->defines);
                dg->defines = listNil<Syme>;
        }
        dg->ordinal = i;
        return dg;
}


local void
dgAbGetUsedSymes(AbSyn ab, DefGroup dg)
{
        Syme    syme;
        int     i, argc;
        AbSyn   *argv;

        switch(abTag(ab)) {
          case AB_Add:
          case AB_With:
          case AB_Generate:
          case AB_Lambda:
          case AB_PLambda:
                return;

          case AB_Define:
#if AXL_EDIT_1_1_13_01
                /* Treat all definitions as a multi */
                argc = abArgcAs(AB_Comma, ab->abDefine.lhs);
                argv = abArgvAs(AB_Comma, ab->abDefine.lhs);


                /* Find out all the things being defined */
                for (i = 0; i < argc; i++) {
                        syme = abSyme(abDefineeId(argv[i]));
                        if (!syme) continue;
                        dg->defines = listCons<Syme>(syme, dg->defines);
                }


                /* Now find all the symes used from the rhs */
                dgAbGetUsedSymes(ab->abDefine.rhs, dg);
                break;
#else
                syme = abSyme(abDefineeId(ab));
                dg->defines = listCons<Syme>(syme, dg->defines);
                dgAbGetUsedSymes(ab->abDefine.rhs, dg);
                break;
#endif

          case AB_LitInteger:
          case AB_LitString:
          case AB_LitFloat:
          case AB_Id:
                if ((syme = abSyme(ab)) != NULL && dgSymeIsLocal(syme)) {
                        dg->usedSymes = listCons<Syme>(syme, dgUses(dg));
                }
                break;
          case AB_CoerceTo:
          case AB_Test:
          case AB_For:
                if ( (syme = abImplicitSyme(ab)) != NULL && dgSymeIsLocal(syme))
                        dg->usedSymes = listCons<Syme>(syme, dgUses(dg));
                for (i=0; i<abArgc(ab); i++)
                        dgAbGetUsedSymes(abArgv(ab)[i], dg);
                break;
          default:
                if ((syme = abSyme(ab)) != NULL && dgSymeIsLocal(syme)) {
                        dg->usedSymes = listCons<Syme>(syme, dgUses(dg));
                }
                for (i=0; i<abArgc(ab); i++)
                        dgAbGetUsedSymes(abArgv(ab)[i], dg);
                break;
        }

}

local DefGroupTag
dgGetTag(AbSyn absyn)
{
        switch(abTag(absyn)) {
          case AB_Fix:
                return DG_Fix;
          case AB_Define:
                switch (abTag(absyn->abDefine.rhs)) {
                  case AB_Lambda:
                  case AB_PLambda:
                        return DG_Lambda;
                  case AB_Add:
                  case AB_With:
                        return DG_Type;
                  default:
                        return DG_Const;
                }
          case AB_If:
                return DG_Cond;
          case AB_Local: {
                /* Can only handle simple locals */
                if (abArgc(absyn) != 1)
                        return DG_Const; /* can do better than this ... */

                /* Return the tag of the local object */
                return dgGetTag(absyn->abLocal.argv[0]);
          }
          default:
                return DG_NonDefn;
        }
}

local bool
dgSymeIsLocal(Syme syme)
{
        if (symeIsParam(syme) || symeIsImport(syme))
                return false;
        if (symeIsExport(syme) && !listMemq<Syme>(dgExports, syme))
                return false;

        return true;

}

local DefGroup
dgNewGroup(DefGroupTag tag, AbSyn ab)
{
        DefGroup new_;

        new_ = (DefGroup) stoAlloc(OB_Other, sizeof(*new_));

        new_->tag        = tag;
        new_->ab         = ab;
        new_->usedSymes  = NULL;
        new_->defines    = NULL;
        new_->ordinal    = -1;

        return new_;
}

local void
dgFreeGroup(DefGroup dg)
{
        listFree<Syme>(dg->defines);
        listFree<Syme>(dg->usedSymes);

        stoFree(dg);
}


/******************************************************************************
 *
 * :: Dependency analysis
 *
 *****************************************************************************/

/*
 * symeHash is a macro and we need a function.
 */
local Hash
symeHashFn(Syme syme)
{
        return symeHash(syme);
}


/*
 * Analyse the dependencies between a set of DG_Const and DG_Cond
 * groups and return them correctly sorted. If `reOrder' is false
 * then it is an error if a defgroup depends on one that has a
 * higher ordinal (was defined later in the code).
 */
local List<DefGroup>
dgProcessDependencies(List<DefGroup> defs, bool reOrder)
{
        int             ord = -10;
        DepDag          root;
        Table<Syme, DepDag> symeToDegDag;
        List<DefGroup>    result = listNil<DefGroup>;


        /* Create a new mapping table */
        symeToDegDag = Table<Syme, DepDag>::create<symeHashFn, symeEqual>();


        /* Root dag node with no dependencies */
        root = DepDag::leaf((void *) NULL);


        /*
         * Phase 1: compute the mapping from symes to defgroups
         * and add the dependencies of the root node. Each group
         * is numbered such that the first defgroup in the source
         * has the lowest number.
         */
        listIter(DefGroup, dg, defs, {
                AbSyn   absyn = dgStmt(dg);
                Syme    syme = dgGetAbMeaning(absyn);
                DepDag  node = DepDag::leaf((void *)dg);


                /* Number the defgroup */
                dg->ordinal = --ord;


                /* Add root dependency */
                root = root.addDependency(node);


                /*
                 * Only index meaningful dgs. The side-effect of this
                 * is that we only find dependencies between dgs that
                 * have meaning.
                 */
                if (syme) symeToDegDag.set(syme, node);
        });


        /* Phase 2: compute dependencies */
        listIter(DepDag, dd, root.dependencies(), {
                DefGroup        dg = (DefGroup)dd.label();
                AbSyn           absyn = dgStmt(dg);
                List<Syme>        deps = dgFindDependencies(absyn);


                /* Skip if no dependencies found */
                if (!deps) continue;


                /* Add each dependency */
                listIter(Syme, syme, deps, {
                        DepDag  dag;


                        /* Search for the defgroup */
                        dag = symeToDegDag.get(syme, nullptr);


                        /* Skip if not found */
                        if (!dag) continue;


                        /* Add the dependency */
                        dd = dd.addDependency(dag);
                });


                /* Release storage */
                listFree<Syme>(deps);
        });


        /* Phase 3: sort */
        result = dgSortDependencies(root, listNil<DepDag>, reOrder);


        /*
         * Phase 4: clean up. Note that the peculiar nature of
         * this dependency graph is that all nodes are children
         * of the root node. We don't have to walk the graph
         * to find nodes to free.
         */
        listFree<DepDag>(root.dependencies());
        root.free();
        symeToDegDag.free();


        /* Return the sorted list */
        return listNReverse<DefGroup>(result);
}


/*
 * dgSortDependencies(dag, deps, reOrder) produces a sorted list of
 * def-groups from the dependency DAG "dag". If the caller doesn't
 * want dependencies re-ordered then "reOrder" must be false and this
 * function generates an error if a valid sort cannot be computed
 * without re-ordering definitions. The "deps" parameter is a list
 * of dependencies which are pending and is used simply for telling
 * the user which nodes occur in a cycle in the graph if one is found.
 */
local List<DefGroup>
dgSortDependencies(DepDag dag, List<DepDag> depends, bool reOrder)
{
        List<DefGroup>    tmp;
        List<DefGroup>    result = listNil<DefGroup>;
        DefGroup        us = (DefGroup)dag.label();
        List<DepDag>      deps = depends;
        List<DepDag>      orders = listNil<DepDag>;


        /* Have we been here before and finished? */
        if (dag.isFinished()) return result;


        /* Have we been here before and not finished? */
        if (dag.isPending()) {
                /* Cycle in dependency graph: report the error */
                dgCycleError(dag, depends);


                /* Ignore this dependency */
                return result;
        }


        /* Add ourselves to the dependency chain (if not the root) */
        if (us) deps = listCons<DepDag>(dag, deps);


        /* Indicate that we are processing this node */
        dag.setPending(true);


        /* Process our dependencies first */
        listIter(DepDag, d, dag.dependencies(), {
                /* Convert into a defgroup */
                DefGroup dg = (DefGroup)d.label();


                /* Check for dependency ordering errors */
                if (!reOrder && us && (dg->ordinal > us->ordinal))
                        orders = listCons<DepDag>(d, orders);


                /* Sort the dependencies */
                tmp = dgSortDependencies(d, deps, reOrder);


                /* Add to the result list if found */
                if (tmp) result = listNConcat<DefGroup>(result, tmp);
        });


        /* Emit ordering errors in one go */
        if (orders) {
                dgOrderError(dag, orders);
                listFree<DepDag>(orders);
        }


        /* Remove ourselves from the dependency chain (if not the root) */
        if (us) deps = listFreeCons<DepDag>(deps);


        /* Add ourselves to the list (if not the root) */
        us = (DefGroup)dag.label();
        if (us) {
                tmp = listSingleton<DefGroup>(us);
                result = listNConcat<DefGroup>(result, tmp);
        }


        /*
         * Indicate that we have finished processing this node. We
         * clear the pending flag for tidiness but don't have to.
         */
        dag.setFinished(true);
        dag.setPending(false);


        /* Return the sorted list */
        return result;
}


local void
dgCycleError(DepDag dag, List<DepDag> deps)
{
        Buffer          buf = Buffer::create();
        MutString          pretty;
        DefGroup        dg = (DefGroup)dag.label();
        List<DepDag>      ptr, cycle = listSingleton<DepDag>(dag);


        /* Root cannot be pending */
        assert(deps);
        assert(dg);


        /* Compute the dependency cycle */
        for (;deps && (car(deps) != dag);deps = cdr(deps)) {
                DepDag  d = car(deps);
                cycle = listCons<DepDag>(d, cycle);
        }


        /* Start with the bad dependency source/target */
        pretty = dgPretty(dg);
        buf.printf("<%s, ", pretty);
        strFree(pretty);


        /* Display the dependency cycle */
        for (ptr = cycle; ptr; ptr = cdr(ptr)) {
                DepDag          d = car(ptr);
                DefGroup        dg = (DefGroup)d.label();


                /* Format this node */
                pretty = dgPretty(dg);
                buf.printf("%s", pretty);
                strFree(pretty);


                /* Any more nodes in this cycle? */
                if (cdr(ptr)) buf.printf(", ");
        }
        buf.printf(">");


        /* Release storage used */
        listFree<DepDag>(cycle);


        /* Report the error */
        comsgWarning(dgStmt(dg), ALDOR_W_GenBadDefCycle, buf.chars());


        /* Free the buffer */
        buf.liberate();
}


local void
dgOrderError(DepDag dag, List<DepDag> deps)
{
        Buffer          buf = Buffer::create();
        MutString          pretty, s;
        DefGroup        dg = (DefGroup)dag.label();
        List<DepDag>      ptr, order = listNil<DepDag>;


        /* Root cannot be pending */
        assert(deps);
        assert(dg);


        /* Re-order the dependency list */
        for (;deps;deps = cdr(deps)) {
                DepDag  d = car(deps);
                order = listCons<DepDag>(d, order);
        }


        /* Display the dependencies */
        for (ptr = order; ptr; ptr = cdr(ptr)) {
                DepDag          d = car(ptr);
                DefGroup        dg = (DefGroup)d.label();


                /* Format this node */
                pretty = dgPretty(dg);
                buf.printf("`%s'", pretty);
                strFree(pretty);


                /* Any more nodes in this cycle? */
                if (cdr(ptr)) {
                        if (cdr(cdr(ptr)))
                                buf.printf(", ");
                        else
                                buf.printf(" and ");
                }
        }


        /* Release storage used */
        listFree<DepDag>(order);


        /* Name of the problem definition */
        s = dgPretty(dg);


        /* Report the error */
        comsgWarning(dgStmt(dg), ALDOR_W_GenBadDefOrder, s, buf.chars(), s);


        /* Free the buffer and other storage */
        buf.liberate();
        strFree(s);
}


/*
 * Pretty-print the symbol associated with a def-group for use in
 * error messages about cyclic dependencies. We assume that this
 * node has meaning: if it didn't we would not have been able to
 * work out that there is a cycle involved.
 */
local MutString
dgPretty(DefGroup dg)
{
        /* name: type */
        AbSyn   absyn;
        Syme    syme;
        String name;
        MutString  type, result;


        /* Extract the syme for this definition */
        absyn = dgStmt(dg);
        syme = dgGetAbMeaning(absyn);


        /* Paranoia */
        assert(syme);


        /* Create the text "name:Type" */
        name = (symeId(syme)).string();
        type = tfPretty(symeType(syme));
        result = strlConcat(name, ":", (String) type, (String) NULL);


        /* Free the text associated with the type */
        strFree(type);


        /* Return the pretty text */
        return result;

}


/*
 * Try to find the meaning of a piece of absyn. The absyn
 * will probably be associated with a def-group and will
 * most likely be a constant definition. We want the syme
 * for the symbol being defined or NULL if not found.
 */
local Syme
dgGetAbMeaning(AbSyn absyn)
{
        AbSyn   id;

        /* Search for the identifier in this absyn */
        id = abDefineeIdOrElse(absyn, (AbSyn) NULL);


        /* Return the meaning of the absyn, if known */
        return id ? abSyme(id) : (Syme) NULL;
}


/*
 * Convert an import syme into the corresponding export
 * syme used as indices into the dependency graph.
 */
local Syme
dgSymeImportToExport(Syme syme)
{
        List<Syme>        symes;
        TForm           exporter, tf = symeType(syme);


        /* Get the exporter of this syme */
        exporter = symeExporter(syme);


        /* If can't find exporter, return untouched */
        if (!exporter) return syme;


        /* Get the constants defined in the exporter */
        symes = tfGetDomConstants(exporter);


        /* If no constants, return untouched */
        if (!symes) return syme;


        /* Search constants for this import */
        for (;symes;symes = cdr(symes)) {
                Syme    esyme = car(symes);


                /* Do the symbols match? */
                if (symeId(esyme) != symeId(syme)) continue;


                /* Check the types match */
                if (!tfEqual(symeType(esyme), tf)) continue;


                /* Return the matching syme */
                return esyme;
        }


        /* Not found: return untouched */
        return syme;
}


/*
 * Recursive type-frame implementation.
 */
local TypeFrameEntry
gen0TypeFrameFind(Syme syme)
{
        List<TypeFrameEntry> entries;

        if (!gen0TypeFrame || !syme)
                return NULL;

        if (symeIsImport(syme))
                syme = dgSymeImportToExport(syme);

        for (entries = gen0TypeFrame->entries; entries; entries = cdr(entries)) {
                TypeFrameEntry entry = car(entries);
                if (entry->syme == syme)
                        return entry;
                if (symeOriginal(entry->syme) == symeOriginal(syme))
                        return entry;
        }
        return NULL;
}

Foam
gen0TypeFrameRef(Syme syme)
{
        TypeFrameEntry entry = gen0TypeFrameFind(syme);
        return entry ? foamCopy(entry->placeholder) : NULL;
}

Foam
gen0TypeFrameRhs(Syme syme)
{
        TypeFrameEntry entry = gen0TypeFrameFind(syme);
        return entry ? foamCopy(entry->rhs) : NULL;
}

bool
gen0TypeFrameMember(Syme syme)
{
        return gen0TypeFrameFind(syme) != NULL;
}

local TypeFrame
gen0TypeFrameNew(DefSet set)
{
        List<DefGroup> defs;
        TypeFrame frame;

        frame = (TypeFrame) stoAlloc(OB_Other, sizeof(*frame));
        frame->owner = gen0State;
        frame->entries = listNil<TypeFrameEntry>;
        frame->deferredExports = listNil<Syme>;
        frame->count = 0;
        frame->generated = 0;
        frame->resolved = false;

        for (defs = dgSetDefs(set); defs; defs = cdr(defs)) {
                List<Syme> symes;
                DefGroup dg = car(defs);

                /* Conditional branches are generated by nested sequences. */
                if (dg->tag == DG_Cond)
                        continue;

                for (symes = dg->defines; symes; symes = cdr(symes)) {
                        Syme syme = car(symes);
                        TForm tf = symeType(syme);
                        bool isDom = tfSatDom(tf);
                        bool isCat = tfSatCat(tf);
                        TypeFrameEntry entry;
                        Foam self;

                        if (!isDom && !isCat)
                                continue;

                        /* Avoid duplicate entries from grouped definitions. */
                        {
                                List<TypeFrameEntry> es;
                                bool seen = false;
                                for (es = frame->entries; es; es = cdr(es))
                                        if (car(es)->syme == syme) {
                                                seen = true;
                                                break;
                                        }
                                if (seen)
                                        continue;
                        }

                        entry = (TypeFrameEntry)
                                stoAlloc(OB_Other, sizeof(*entry));
                        entry->syme = syme;
                        entry->rhs = gen0Temp(FOAM_Word);
                        entry->isCategory = isCat;
                        entry->generated = false;

                        self = gen0Syme(syme);
                        entry->placeholder = foamCopy(self);

                        /* Stable identity is visible before any RHS runs. */
                        gen0AddInit(foamNewSet(foamCopy(self),
                                              gen0TypeFrameMakeDummy(entry)));

                        if (symeIsExport(syme))
                                gen0SymeSetInit(syme,
                                               foamCopy(entry->placeholder));

                        foamFree(self);
                        frame->entries = listCons<TypeFrameEntry>
                                (entry, frame->entries);
                        frame->count++;
                }
        }

        if (!frame->count) {
                listFree<TypeFrameEntry>(frame->entries);
                stoFree(frame);
                return NULL;
        }

        frame->entries = listNReverse<TypeFrameEntry>(frame->entries);
        return frame;
}

local void
gen0TypeFrameFree(TypeFrame frame)
{
        List<TypeFrameEntry> entries;

        if (!frame)
                return;

        for (entries = frame->entries; entries; entries = cdr(entries)) {
                TypeFrameEntry entry = car(entries);
                foamFree(entry->placeholder);
                foamFree(entry->rhs);
                stoFree(entry);
        }
        listFree<TypeFrameEntry>(frame->entries);
        listFree<Syme>(frame->deferredExports);
        stoFree(frame);
}

local Foam
gen0TypeFrameMakeDummy(TypeFrameEntry entry)
{
        return gen0BuiltinCCall(FOAM_Word,
                                entry->isCategory
                                ? "categoryMakeDummy"
                                : "domainMakeDummy",
                                "runtime", int0);
}

local Foam
gen0TypeFrameResolveCall(TypeFrameEntry entry)
{
        return gen0BuiltinCCall(FOAM_NOp,
                                entry->isCategory
                                ? "categoryResolve!"
                                : "domainResolve!",
                                "runtime", 2,
                                foamCopy(entry->placeholder),
                                foamCopy(entry->rhs));
}

local bool
gen0TypeFrameHasGroup(DefGroup dg)
{
        List<Syme> symes;
        for (symes = dg->defines; symes; symes = cdr(symes))
                if (gen0TypeFrameMember(car(symes)))
                        return true;
        return false;
}

local void
gen0TypeFrameNoteGroup(DefGroup dg, AbSyn pos)
{
        List<Syme> symes;

        if (!gen0TypeFrame || gen0TypeFrame->resolved)
                return;

        for (symes = dg->defines; symes; symes = cdr(symes)) {
                TypeFrameEntry entry = gen0TypeFrameFind(car(symes));
                if (!entry || entry->generated)
                        continue;
                entry->generated = true;
                gen0TypeFrame->generated++;
        }

        if (gen0TypeFrame->generated == gen0TypeFrame->count)
                gen0TypeFrameResolve(pos);
}

local void
gen0TypeFrameResolve(AbSyn pos)
{
        int pass;
        List<TypeFrameEntry> entries;

        if (!gen0TypeFrame || gen0TypeFrame->resolved)
                return;

        /*
         * With N bindings, N monotone propagation passes suffice for every
         * finite forwarding chain.  Cycles containing a concrete constructor
         * become concrete as soon as that constructor's RHS is available.
         */
        for (pass = 0; pass < gen0TypeFrame->count; pass++)
                for (entries = gen0TypeFrame->entries;
                     entries; entries = cdr(entries))
                        gen0AddStmt(gen0TypeFrameResolveCall(car(entries)), pos);

        gen0TypeFrame->resolved = true;
        gen0TypeFrameFlushExports();
}

local void
gen0TypeFrameDeferExport(Syme syme)
{
        if (!listMemq<Syme>(gen0TypeFrame->deferredExports, syme))
                gen0TypeFrame->deferredExports = listCons<Syme>
                        (syme, gen0TypeFrame->deferredExports);
}

local void
gen0TypeFrameFlushExports()
{
        List<Syme> symes;

        if (!gen0TypeFrame)
                return;

        gen0TypeFrame->deferredExports =
                listNReverse<Syme>(gen0TypeFrame->deferredExports);
        for (symes = gen0TypeFrame->deferredExports; symes; symes = cdr(symes))
                gen0TypeAddExportSlot(car(symes));
        listFree<Syme>(gen0TypeFrame->deferredExports);
        gen0TypeFrame->deferredExports = listNil<Syme>;
}


local List<Syme>
dgFindDependencies(AbSyn absyn)
{
        List<Syme>        result = listNil<Syme>;

        /*
         * Check the absyn: we want to find all dependencies
         * but don't want to find false ones. For example, we
         * need to examine the type of a declaration but we
         * don't want to examine the thing being declared: it
         * might be the node whose dependency is being checked!
         */
        if (abIsId(absyn)) {
                Syme    syme = dgGetAbMeaning(absyn);

                if (syme) {
                        /* Imports need to be converted to exports */
                        if (symeIsImport(syme))
                                syme = dgSymeImportToExport(syme);


                        /*
                         * Add to the dependency list: at the moment
                         * we add all symes to the list. Ideally we
                         * ought to filter out those which aren't
                         * nodes of the dependency graph. They get
                         * filtered out later but if we remove them
                         * down here then there are fewer store
                         * operations via listCons/listNConcat.
                         */
                        result = listCons<Syme>(syme, result);
                }
                return result;
        }
        else if (abTag(absyn) < AB_NODE_START)
                return result;


        /* Most cases are not very special */
        switch (abTag(absyn)) {
                /* Some nodes must not be fully checked */
                case AB_Add:
                        result = dgFindDependencies(absyn->abAdd.base);
                        break;

                case AB_With:
                        result = dgFindDependencies(absyn->abWith.base);
                        break;

                case AB_Declare:
                        result = dgFindDependencies(absyn->abDeclare.type);
                        break;

                case AB_Define:
                        result = dgFindDependencies(absyn->abDefine.rhs);
                        break;

                case AB_PLambda: /* Fall through */
                case AB_Lambda: {
                        List<Syme>        plst, rlst;

                        /* Dependencies in types */
                        plst = dgFindDependencies(absyn->abLambda.param);
                        rlst = dgFindDependencies(absyn->abLambda.rtype);

                        result = listNConcat<Syme>(plst, rlst);
                        break;
                }

                /* Check everything else completely */
                default: {
                        int     i;

                        for (i = 0; i < abArgc(absyn); i++) {
                                List<Syme>        tmp;

                                tmp = dgFindDependencies(abArgv(absyn)[i]);
                                if (tmp)
                                        result = listNConcat<Syme>(result, tmp);
                        }

                        break;
                }
        }


        /* Return the list of dependencies */
        return result;
}


