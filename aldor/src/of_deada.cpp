///////////////////////////////////////////////////////////////////////////////
//
// of_deada.cpp: Dead Assignment Elimination
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
 * Dead assignment elimination:
 * Remove any assignments whose value is nowhere used:
 *
 * Example:
 *l0:
 * f(x)
 * x := 1;
 * if C then l1;
 * ...
 *l1:
 * x := 2;
 * goto l0;
 *
 * The first assignment to x can be killed.
 * Done using reverse dataflow, with
 * In(B)   == { v: v is live before B }
 * Out(B)  == { v: v is live after  B }
 * Gen(B)  == { v: v is used before set within B }
 * Kill(B) == { v: v is set before used within B }
 *
 * Eliminating within a BB is a matter of applying these rules
 * to the Out Set within the BB, starting from the last stmt.
 */
#include "axlobs.h"
#include "of_deada.h"
#include "of_util.h"
#include "opttools.h"
#include "optfoam.h"

bool      daDebug;

# define   daDEBUG(s)    DEBUG_IF(daDebug, s)
# define   trDEBUG(s)    DEBUG_IF(daDebug, s)

/****************************************************************************
 *
 * Type definitions
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Global Data Structures
 *
 ****************************************************************************/

static int              daFlogCutOff = 700;

static BitvClass        daBitvClass;
static bool             daChanged;
static LiveVars         daLiveVars;
static FlowGraph        daThisFlog;

/****************************************************************************
 *
 * Local declarations
 *
 ****************************************************************************/

local Foam      daProg          (Foam);
local bool      daFlog0         (int, FlowGraph);
local void      daFillDefRef    (FlowGraph, BBlock);
local void      daFillStmt      (Foam, BBlock);
local void      daFillExpr      (Foam, BBlock);
local void      daFixBBlock     (BBlock);
local Foam      daFixSet        (Bitv, Foam);
local void      daFixExpr       (Bitv, Foam);

local int daBitvSize    (Foam prog);
local int daBitvIndex   (Foam foam);

void    daReduceTemps   (BBlock bb);

local List<LiveInfo> daAddLiveChanges     (List<LiveInfo>, int, Bitv);
local LiveVars     daMakeInitLiveVars   (FlowGraph);

/****************************************************************************
 *
 * :: External functions
 *
 ****************************************************************************/

void
daSetCutOff(int n)
{
        daFlogCutOff = n;
}

void
deadAssign(Foam unit)
{
        Foam ddef, def, rhs;
        int  i;

        ddef = unit->foamUnit.defs;
        for (i = 0; i<foamArgc(ddef); i++) {
                def = ddef->foamDDef.argv[i];
                assert(foamTag(def) == FOAM_Def);

                rhs = def->foamDef.rhs;
                if (foamTag(rhs) == FOAM_Prog)
                        def->foamDef.rhs = daProg(rhs);
        }
        assert(foamAudit(unit));
}

void
daFlog(FlowGraph flog, LiveVars *pVars)
{
        int     nlocals = daBitvSize(flog->prog);

        if (pVars)
                daLiveVars = daMakeInitLiveVars(flog);
        else
                daLiveVars = NULL;

        daFlog0(nlocals, flog);

        if (pVars) {
                *pVars = daLiveVars;
                (*pVars)->bvClass = bitvClassCreate(nlocals);
        }
}


void
daFreeVars(LiveVars vars)
{
}

/****************************************************************************
 *
 * :: Simple accessors
 *
 ****************************************************************************/

local int
daBitvSize(Foam prog)
{
        return foamDDeclArgc(prog->foamProg.locals)
                + foamDDeclArgc(prog->foamProg.params);
}

local int
daBitvIndex(Foam foam)
{
        switch (foamTag(foam)) {
          case FOAM_Loc:
                return foam->foamLoc.index;
          case FOAM_Par:
                return foam->foamPar.index +
                        foamDDeclArgc(daThisFlog->prog->foamProg.locals);
          default:
                return -1;
        }
}

/****************************************************************************
 *
 * Compute and eliminate...
 *
 ****************************************************************************/

local Foam
daProg(Foam prog)
{
        FlowGraph flog;
        Foam      locals, ret;
        int       nbits;

        daDEBUG({
                printf("(Before:\n");
                foamWrSExpr(dbOut, prog,int0);
        });
        locals = prog->foamProg.locals;

        nbits  = daBitvSize(prog);

        if (nbits == 0)
                return prog;

        flog = flogFrProg(prog, FLOG_UniqueExit);

        daLiveVars = NULL;
        daFlog0(nbits, flog);

        ret = flogToProg(flog);
        daDEBUG({
                foamWrSExpr(dbOut, ret,int0);
                printf("After)\n");
        });
        daBitvClass = NULL;

        return ret;
}

local bool
daFlog0(int nLocals, FlowGraph flog)
{
        BBlock bb;
        int    i, count;
        bool    changed = false;

        daBitvClass = bitvClassCreate(nLocals);
        flogBitvClass(flog) = daBitvClass;
        daThisFlog = flog;

        daChanged = true;

        while (daChanged) {
                daChanged   = false;

                for (i=0; i<flogBlockC(flog); i++) {
                        bb = flogBlock(flog, i);
                        dflowNewBlockInfo(bb, nLocals, daFillDefRef);
                }

                i = dflowRevIterate(flog, DFLOW_Union,
                                    daFlogCutOff, &count, NULL);

                if (i != 0)
                        return false;

                for (i=0; i<flogBlockC(flog); i++) {
                        if (!flogBlock(flog,i)) continue;
                        if (flogBlock(flog, i))
                                daFixBBlock(flogBlock(flog, i));
                }

                if (daChanged) changed = true;
        }

        /* Killing assignments may simplify cse/flow/cprop analysis */
        if (changed) {
                optSetCsePending(flog->prog);
                optSetJFlowPending(flog->prog);
                optSetCPropPending(flog->prog);
        }

        daThisFlog = NULL;
        return true;
}

local void
daFillDefRef(FlowGraph flog, BBlock bb)
{
        Foam      seq = bb->code;
        Foam      stmt;
        BitvClass class_ = flogBitvClass(flog);
        int       i;

        /* clear the vectors */
        bitvClearAll(class_, dfRevKill(bb));
        bitvClearAll(class_, dfRevGen(bb));

        for (i=0; i<foamArgc(seq); i++) {
                stmt = seq->foamSeq.argv[i];
                daFillStmt(stmt, bb);
        }
}

local void
daFillStmt(Foam stmt, BBlock bb)
{
        Foam *argv;
        int   idx, argc, i;

        if (!otIsDef(stmt)) {
                daFillExpr(stmt, bb);
                return;
        }

        daFillExpr(stmt->foamSet.rhs, bb);

        switch(foamTag(stmt->foamSet.lhs)) {
          case FOAM_Values:
                argv = stmt->foamSet.lhs->foamValues.argv;
                argc = foamArgc(stmt->foamSet.lhs);
                break;
          default:
                argv = &stmt->foamSet.lhs;
                argc = 1;
                break;
        }

        for (i=0; i<argc; i++) {
                idx = daBitvIndex(argv[i]);
                if (idx != -1) {
                        if (!bitvTest(daBitvClass, dfRevGen(bb), idx))
                                bitvSet(daBitvClass, dfRevKill(bb), idx);
                } else
                        daFillExpr(argv[i], bb);
        }
}

local void
daFillExpr(Foam foam, BBlock bb)
{
        int idx;

        idx = daBitvIndex(foam);
        if (idx != -1) {
                if (!bitvTest(daBitvClass, dfRevKill(bb), idx))
                        bitvSet(daBitvClass, dfRevGen(bb), idx);
        }
        else
                foamIter(foam, arg, daFillExpr(*arg, bb));
}

local void
daFixBBlock(BBlock bb)
{
        List<LiveInfo> lst = listNil<LiveInfo>;
        Foam seq = bb->code;
        Foam stmt, newstmt;
        Bitv bitv;
        int i;

        /* Do dataflow by hand backwards through the BB */
        /* Start with the 'out' set and work upwards    */
        /* when complete, out will be a subset of "in" */
        daDEBUG({
                fprintf(dbOut, "(Fixing: Out\n");
                bitvPrint(dbOut, daBitvClass, dfRevOut(bb));
                fprintf(dbOut, "\nIn\n");
                bitvPrint(dbOut, daBitvClass, dfRevIn(bb));
                fprintf(dbOut, "\nFoam\n");
                foamWrSExpr(dbOut, seq, int0);
        });
        bitv = bitvNew(daBitvClass);
        bitvCopy(daBitvClass, bitv, dfRevOut(bb));

        for (i=foamArgc(seq) - 1; i >= 0 ; i--) {
                stmt = seq->foamSeq.argv[i];

                if (daLiveVars != NULL)
                        lst = daAddLiveChanges(lst, i, bitv);

                if (!otIsDef(stmt))
                        daFixExpr(bitv, stmt);
                else {
                        newstmt = daFixSet(bitv, stmt);
                        seq->foamSeq.argv[i] = newstmt;
                }

        }

        if (daLiveVars != NULL) {
                lst = daAddLiveChanges(lst, i, bitv);
                daLiveVars->vars[bb->label] = lst;
        }

#if 0
        daReduceTemps(bb);
#endif
        daDEBUG({
                bitvPrint(dbOut, daBitvClass, bitv);
                fprintf(dbOut, "\nDone fix)\n");
        });

}

local Foam
daFixSet(Bitv bitv, Foam set)
{
        Foam lhs = set->foamSet.lhs;
        Foam rhs = set->foamSet.rhs;
        Foam *argv;
        int   argc;
        bool  isLive;
        int   i, idx;

        switch(foamTag(lhs)) {
          case FOAM_Values:
                argv = lhs->foamValues.argv;
                argc = foamArgc(lhs);
                break;
          default:
                argv = &lhs;
                argc = 1;
                break;
        }

        isLive = false;
        for (i=0; i<argc; i++) {
                idx = daBitvIndex(argv[i]);
                if (idx == -1) {
                        isLive = true;
                        daFixExpr(bitv, argv[i]);
                }
                else {
                        isLive |= bitvTest(daBitvClass, bitv, idx);
                        bitvClear(daBitvClass, bitv, idx);
                }
        }

        if (isLive) {
                daFixExpr(bitv, rhs);
                return set;
        }

        /* !! GenC should deal with MFmt without a Set */
        if (foamTag(rhs) == FOAM_MFmt) {
                daFixExpr(bitv, rhs);
                return set;
        }

        if (foamHasSideEffect(rhs)) {
                daFixExpr(bitv, rhs);
                foamFree(lhs);
                foamFreeNode(set);
                return rhs;
        }

        foamFree(set);
        daChanged = true;
        return foamNewNOp();
}

local void
daFixExpr(Bitv bitv, Foam expr)
{
        int idx;

        idx = daBitvIndex(expr);
        if (idx != -1)
                bitvSet(daBitvClass, bitv, idx);
        else
                foamIter(expr, arg, daFixExpr(bitv, *arg));
}


/****************************************************************************
 *
 * :: Temporary Reduction
 *
 ****************************************************************************/

typedef struct {
        List<AInt> vars;
        Foam     temps;
        int      lastStmt;
} *FreeTempSet;

local void      trRenameBlock           (BBlock);
local Foam      trRenameExpr            (int, Foam);
local Foam      trRenameSet             (int, Foam);
local AInt      trRenameId              (int, AInt);
local void      trAddTemporary          (FreeTempSet, int, AInt);
local AInt      trAllocateTemporary     (int, AInt);
local void      trSetName               (AInt, AInt);
local AInt      trGetName               (AInt);
local void      trInitTempSets          (BBlock);
local void      trFillSetsExpr          (int, Foam);
local void      trFillKillSet           (int, Foam);
local void      trFiniTempSets          (void);
local void      trInitFreeVars          (BBlock);
local List<AInt>  trKillEventsAfter       (int, AInt);
local List<AInt>  trUseEventsAfter        (int, AInt);
local List<AInt>  trEventsAfter           (int, List<AInt> l);
local void      trCollectNonLocalTemps  (FreeTempSet, FlowGraph, BBlock);
local bool      trVarsCompatible        (FreeTempSet, AInt, AInt);
local void      trUseName               (AInt);
local bool      trIsUseable             (int stmt, AInt newVar);

static Table<AInt, List<AInt>> trKillTbl;
static Table<AInt, List<AInt>> trUseTbl;
static Table<AInt, AInt> trRenameTbl;
static Bitv     trLive;
static Bitv     trOut;

static FreeTempSet trFreeTemps;

void
daReduceTemps(BBlock bb)
{
        trInitTempSets(bb);

        trRenameBlock(bb);

        trFiniTempSets();
}

local void
trRenameBlock(BBlock bb)
{
        Foam seq, stmt;
        int i;

        seq = bb->code;
        for (i=0; i<foamArgc(seq); i++) {
                stmt = seq->foamSeq.argv[i];

                switch (foamTag(stmt)) {
                  case FOAM_Def:
                  case FOAM_Set:
                        seq->foamSeq.argv[i] = trRenameSet(i, stmt);
                        break;
                  default:
                        trRenameExpr(i, stmt);
                }
        }
}

local Foam
trRenameExpr(int stmtId, Foam expr)
{
        List<AInt> kills, uses;
        bool     liveAfter;
        int      id;

        foamIter(expr, arg, *arg = trRenameExpr(stmtId, *arg));

        if (foamTag(expr) != FOAM_Loc)
                return expr;

        id = daBitvIndex(expr);

        if (bitvTest(daBitvClass, trLive, id)) {
                /* Determine if this is the last use of this local */
                uses  = trUseEventsAfter(stmtId + 1, id);
                kills = trKillEventsAfter(stmtId, id);

                liveAfter = bitvTest(daBitvClass, trOut, id);

                trDEBUG(fprintf(dbOut,
                        "Checking %d var: %d next use: %d next kill: %d  used later: %s\n",
                                stmtId, id,
                                uses ? (int)car(uses): 999999,
                                kills ? (int)car(kills): 999999,
                                liveAfter ? "Yes" : "No"));

                if (uses == listNil<AInt>) {
                        if (!liveAfter)
                                trAddTemporary(trFreeTemps, stmtId, id);
                }
                else if (kills != listNil<AInt> && (car(uses) > car(kills)))
                        trAddTemporary(trFreeTemps, stmtId, id);

                bitvClear(daBitvClass, trLive, id);
        }

        id = trGetName(id);

        expr->foamLoc.index = id;

        return expr;

}

local Foam
trRenameSet(int stmtId, Foam set)
{
        Foam     lhs = set->foamSet.lhs;
        Foam     rhs = set->foamSet.rhs;
        AInt     lhsId;

        rhs = trRenameExpr(stmtId, rhs);

        if (foamTag(lhs) != FOAM_Loc) {
                lhs = trRenameExpr(stmtId, lhs);
                return set;
        }

        lhsId = trRenameId(stmtId, lhs->foamLoc.index);
        lhs->foamLoc.index = lhsId;

        return set;
}

local AInt
trRenameId(int stmtId, AInt id)
{
        bool liveAfter;

        /* Mark as live (could tighten this up) */
        bitvSet(daBitvClass, trLive, id);
        /* Determine if it is safe to rename */

        liveAfter = bitvTest(daBitvClass, trOut, id);
        if (liveAfter) {
                List<AInt> kills;
                kills = trKillEventsAfter(stmtId+1, id);
                if (kills == listNil<AInt>) {
                        /* forget the previous name */
                        trUseName(id);
                        return id;
                }
        }

        return trAllocateTemporary(stmtId, id);
}


local void
trAddTemporary(FreeTempSet set, int stmt, AInt id)
{
        assert(stmt >= set->lastStmt);

        if (set->lastStmt < stmt) {
                set->vars = listCons<AInt>((AInt) -1, set->vars);
                set->lastStmt = stmt;
        }
        if (id == -1)
                return;

        set->vars = listCons<AInt>(id, set->vars);

        trDEBUG(fprintf(dbOut, "Stmt: %d free: %d\n", stmt, (int)id));
}

local AInt
trAllocateTemporary(int stmtId, AInt oldId)
{
        List<AInt> *vars = &trFreeTemps->vars;
        AInt      actual, id = -1;

        trDEBUG({
                List<AInt> lst;
                lst = trFreeTemps->vars;
                fprintf(dbOut, "Before: [");
                while (lst) {
                        fprintf(dbOut, "%d", (int)car(lst));
                        lst = cdr(lst);
                        if (lst) fprintf(dbOut, " ");
                }
                fprintf(dbOut, "]\n");
        });
        actual = trGetName(oldId);

        while (*vars != listNil<AInt>) {
                List<AInt> *fallback = NULL;
                AInt      var;

                while ( (var = car(*vars)) != (AInt) -1) {
                        if ( var == actual) {
                                id = actual;
                                break;
                        }
                        if (fallback == NULL
                            && trVarsCompatible(trFreeTemps, oldId, var)
                            && trIsUseable(stmtId, var))
                                fallback = vars;
                        vars = &cdr(*vars);
                }

                if (id != -1)
                        break;
                if (fallback != NULL) {
                        vars = fallback;
                        id   = car(*fallback);
                        break;
                }
                vars = &cdr(*vars);
        }

        assert(*vars);
        /* Remove this element from the list [memleak!]*/
        *vars = cdr(*vars);
        trDEBUG(fprintf(dbOut, "Rename: %d -> %d, ", (int)oldId, (int)id));
        trDEBUG({
                List<AInt> lst;
                lst = trFreeTemps->vars;
                fprintf(dbOut, "[");
                while (lst) {
                        fprintf(dbOut, "%d", (int)car(lst));
                        lst = cdr(lst);
                        if (lst) fprintf(dbOut, " ");
                }
                fprintf(dbOut, "]\n");
        });

        trSetName(oldId, id);
        return id;
}

local void
trUseName(AInt id)
{
        List<AInt> *vars = &trFreeTemps->vars;
        while (*vars != listNil<AInt> && car(*vars) != id)
                vars = &cdr(*vars);

        if (*vars != listNil<AInt>)
                *vars = cdr(*vars);

}

local bool
trIsUseable(int stmtId, AInt newVar)
{
        bool liveAfter;

        /* Mark as live (could tighten this up) */
        bitvSet(daBitvClass, trLive, newVar);
        /* Determine if it is safe to rename */

        liveAfter = bitvTest(daBitvClass, trOut, newVar);
        if (liveAfter) {
                List<AInt> kills;
                kills = trKillEventsAfter(stmtId+1, newVar);
                if (kills == listNil<AInt>) {
                        return false;
                }
        }

        return true;
}

local bool
trVarsCompatible(FreeTempSet set, AInt old, AInt new_)
{
        Foam odecl, ndecl;
        bool ok;

        odecl = set->temps->foamDDecl.argv[old];
        ndecl = set->temps->foamDDecl.argv[new_];

        if (odecl->foamDecl.type != ndecl->foamDecl.type)
                return false;

        switch(odecl->foamDecl.type) {
          case FOAM_Rec:
          case FOAM_Arr:
                ok = (odecl->foamDecl.format == ndecl->foamDecl.format);
                break;
          default:
                ok = true;
        }
        return ok;
}


local void
trSetName(AInt old, AInt new_)
{
        trRenameTbl.set(old, new_);
}

local AInt
trGetName(AInt id)
{
        return trRenameTbl.get(id, id);
}


local void
trInitTempSets(BBlock bb)
{
        Foam seq, stmt;
        int i;

        trKillTbl = Table<AInt, List<AInt>>::create();
        trUseTbl = Table<AInt, List<AInt>>::create();
        trRenameTbl = Table<AInt, AInt>::create();

        seq = bb->code;
        /* First find all uses and kills */
        for (i=foamArgc(seq)-1; i>=0; i--) {
                stmt = seq->foamSeq.argv[i];
                trFillSetsExpr(i, stmt);
        }

        trInitFreeVars(bb);

        /* Current live variables */
        trLive = dfRevIn(bb);
        trOut  = dfRevOut(bb);
}

local void
trInitFreeVars(BBlock bb)
{
        FreeTempSet fvs = (FreeTempSet) stoAlloc(OB_Other,sizeof(*fvs));

        fvs->vars  = listNil<AInt>;
        fvs->lastStmt  = -3;
        fvs->temps = daThisFlog->prog->foamProg.locals;

        /* Mark initially killed variables as free */
        for (Table<AInt, List<AInt>>::Iterator it(trKillTbl);
             it.more(); it.step()) {
                AInt id = it.key();
                if (!bitvTest(daBitvClass, dfRevIn(bb), id))
                        trAddTemporary(fvs, -2, id);
        }
        trCollectNonLocalTemps(fvs, daThisFlog, bb);
        trAddTemporary(fvs, int0, -1);

        trFreeTemps = fvs;
}

local void
trCollectNonLocalTemps(FreeTempSet fvs, FlowGraph flog, BBlock bb)
{
        BBlock tbb;
        Bitv   lset;
        int    i;
        /*
         * Find all locals s.t. bb != thisBB,
         * and In(bb_i).x == Out(bb_i) == false;
         */
        lset = bitvNew(daBitvClass);
        bitvClearAll(daBitvClass, lset);
        for (i=0; i<flogBlockC(flog); i++) {
                tbb = flogBlock(flog, i);
                if (!tbb || bb == tbb) continue;

                bitvOr(daBitvClass, lset, dfRevIn(tbb), lset);
                bitvOr(daBitvClass, lset, dfRevOut(tbb), lset);
        }

        for (i=0; i<bitvClassSize(daBitvClass); i++) {
                if (!bitvTest(daBitvClass, lset, i))
                        trAddTemporary(fvs, -1, (AInt) i);
        }
}

local void
trFillSetsExpr(int stmtId, Foam foam)
{
        List<AInt> ids;
        AInt     loc;

        switch (foamTag(foam)) {
          case FOAM_Set:
          case FOAM_Def:
                trFillKillSet(stmtId, foam->foamDef.lhs);
                foam = foam->foamDef.rhs;
          default:
                break;
        }

        foamIter(foam, arg, trFillSetsExpr(stmtId, *arg));


        if (foamTag(foam) != FOAM_Loc)
                return;

        loc = foam->foamLoc.index;

        ids = trUseTbl.get(loc, listNil<AInt>);

        trUseTbl.set(loc, listCons<AInt>(stmtId, ids));
}

local void
trFillKillSet(int stmtId, Foam foam)
{
        Foam *argv;
        List<AInt> ids;
        AInt     loc;
        int   i, argc;

        switch (foamTag(foam)) {
          case FOAM_Values:
                argv = foam->foamValues.argv;
                argc = foamArgc(foam);
                break;
          default:
                argv = &foam;
                argc = 1;
        }

        for (i = 0; i<argc; i++) {
                if (foamTag(argv[i]) != FOAM_Loc)
                        trFillSetsExpr(stmtId, argv[i]);
                else {
                        loc = argv[i]->foamLoc.index;
                        ids = trKillTbl.get(loc, listNil<AInt>);
                        trKillTbl.set(loc, listCons<AInt>(stmtId, ids));
                }
        }
}

local void
trFiniTempSets()
{

}


local List<AInt>
trKillEventsAfter(int stmt, AInt id)
{
        List<AInt> kills;
        kills = trKillTbl.get(id, listNil<AInt>);
        return trEventsAfter(stmt, kills);
}

local List<AInt>
trUseEventsAfter(int stmt, AInt id)
{
        List<AInt> uses;
        uses = trUseTbl.get(id, listNil<AInt>);

        uses = trEventsAfter(stmt, uses);
        return uses;
}


local List<AInt>
trEventsAfter(int id, List<AInt> l)
{
        while (l && car(l) < (AInt) id)
                l = cdr(l);
        return l;
}

/****************************************************************************
 *
 * Live variables
 *
 ****************************************************************************/


local List<LiveInfo>
daAddLiveChanges(List<LiveInfo> lst, int stmt, Bitv bitv)
{
        LiveInfo info;

        if (lst != listNil<LiveInfo>
            && bitvEqual(daBitvClass, car(lst)->live, bitv))
                return lst;

        info = (LiveInfo) stoAlloc(OB_Other, sizeof(*info));
        info->stmtId = stmt;
        info->live   = bitvNew(daBitvClass);
        bitvCopy(daBitvClass, info->live, bitv);

        return listCons<LiveInfo>(info, lst);
}

local LiveVars
daMakeInitLiveVars(FlowGraph flog)
{
        LiveVars vars = (LiveVars) stoAlloc(OB_Other, sizeof(*vars));
        vars->bvClass = NULL;
        vars->vars    = (List<LiveInfo>*)
                stoAlloc(OB_Other, flogBlockC(flog)*sizeof(List<LiveInfo>));

        return vars;
}

Bitv
daGetLiveVars(BBlock bb, LiveVars vars, int stmt)
{
        List<LiveInfo> lst  = vars->vars[bb->label];
        Bitv         bits = NULL;

        assert(lst);
        while (stmt < car(lst)->stmtId) {
                bits = car(lst)->live;
                lst = cdr(lst);
        }
        return bits;
}

void
daKillLiveVars(LiveVars vars)
{
}

