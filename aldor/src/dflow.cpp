///////////////////////////////////////////////////////////////////////////////
//
// dflow.cpp: Data-flow analysis
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


/*****************************************************************************
 *
 * :: Setup/Teardown
 *
 ****************************************************************************/

#define bbDfC(bb)       ((bb)->graph->dfIsPerExit ? bbExitC(bb) : 1)

void
dflowNewGraphInfo(FlowGraph flog, Length nbits, DFlowFillGKFun gkf)
{
        int     i;

        for (i = 0; i < flogBlockC(flog); i++)
                dflowNewBlockInfo(flogBlock(flog, i), nbits, gkf);
}

void
dflowNewBlockInfo(BBlock bb, Length nbits, DFlowFillGKFun gkf)
{
        DFlowInfo       dfi=(DFlowInfo) 0;
        int             sz;
        BitvClass       class_;

        if (!bb || bb->dfinfo) return;

        class_ = bbBitvClass(bb);
        
        if (!class_) {
                class_ = bitvClassCreate(nbits);
                bbBitvClass(bb) = class_;
        }

        sz = sizeof(*dfi)-sizeof(dfi->exit) + bbDfC(bb)*sizeof(dfi->exit[0]);
        bb->dfinfo = (DFlowInfo) bitvManyNew(class_, sz/sizeof(Bitv));

        (*gkf)(bb->graph, bb);
}

void
dflowFreeGraphInfo(FlowGraph flog)
{
        int     i;

        for (i = 0; i < flogBlockC(flog); i++)
                dflowFreeBlockInfo(flogBlock(flog, i));
}

void
dflowFreeBlockInfo(BBlock bb)
{
        if (!bb || !bb->dfinfo) return;

        bitvManyFree((Bitv *) bb->dfinfo);
        bb->dfinfo = 0;
}

int
dflowPrintBlockInfo(FILE *fout, BBlock bb)
{
        int     i, cc = 0;
        BitvClass       class_ = bbBitvClass(bb);

        if (!bb->dfinfo) return cc;

        cc += fprintf  (fout,   "  In:     ");
        cc += bitvPrint(fout, class_, dfFwdIn(bb));
        cc += fprintf  (fout, "\n  Gen:    ");
        cc += bitvPrint(fout, class_, dfFwdGen(bb));

        for (i = 0; i < bbDfC(bb); i++) {
                cc += fprintf(fout, "\n  Kill %d: ", i);
                cc += bitvPrint(fout, class_, dfFwdKill(bb,i));
                cc += fprintf(fout, "\n  Out  %d: ", i);
                cc += bitvPrint(fout, class_, dfFwdOut (bb,i));
        }
        cc += fprintf(fout, "\n");

        return cc;
}

/*****************************************************************************
 *
 * :: Forward Dataflow
 *
 ****************************************************************************/

/*
 * Initialize the "in" sets.
 */
local void
dflowFwdInit(FlowGraph flog, DFlowType inFrOut)
{
        int             i;
        BBlock          bb;
        BitvClass       class_ = flogBitvClass(flog);

        for (i = 0; i < flogBlockC(flog); i++) {
                bb = flogBlock(flog, i);
                if (!bb) continue;
                if (bbEntryC(bb) == 0 && !bb->isblock0) continue;

                if (inFrOut == DFLOW_Union)
                        bitvClearAll(class_, dfFwdIn(bb));
                else  /* DFLOW_Intersection */
                        bitvSetAll(class_, dfFwdIn(bb));
        }
        
        /* Insure that IN[B0] = {} */
        if (flog->block0)
                bitvClearAll(class_, dfFwdIn(flog->block0));
}

int
dflowFwdStep(FlowGraph flog, DFlowType inFrOut)
{
        int             i, j, jj, nchanged;
        BBlock          bb, dd;
        BitvClass       class_ = flogBitvClass(flog);


        /* Propagate information from "in" to "out". */
        for (i = 0; i < flogBlockC(flog); i++) {
                bb = flogBlock(flog, i);
                if (!bb) continue;
                if (bbEntryC(bb) == 0 && !bb->isblock0) continue;

                for (j = 0; j < bbDfC(bb); j++) {
                        /* outj = (in \/ gen) - killj */
                        bitvOr   (class_, dfFwdOut(bb,j), dfFwdIn(bb),   dfFwdGen(bb));
                        bitvMinus(class_, dfFwdOut(bb,j),dfFwdOut(bb,j), dfFwdKill(bb,j));
                }
        }

        /* Clear "in" sets so we can via |=.  Remember old values. */
        for (i = 0; i < flogBlockC(flog); i++) {
                bb = flogBlock(flog, i);
                if (!bb) continue;
                if (bbEntryC(bb) == 0 && !bb->isblock0) continue;

                bitvCopy (class_, dfFwdTemp(bb), dfFwdIn(bb));

                if (bb == flog->block0) continue;

                if (inFrOut == DFLOW_Union)
                        bitvClearAll(class_, dfFwdIn(bb));
                else
                        bitvSetAll(class_, dfFwdIn(bb));
        }

        /* Propagate information from "out" to "in". */
        for (i = 0; i < flogBlockC(flog); i++) {
                bb = flogBlock(flog, i);
                if (!bb) continue;
                if (bbEntryC(bb) == 0 && !bb->isblock0) continue;

                for (j = 0; j < bbExitC(bb); j++) {
                        dd = bbExit(bb, j);
                        jj = flog->dfIsPerExit ? j : 0;

                        if (inFrOut == DFLOW_Union)
                            bitvOr(class_, dfFwdIn(dd), dfFwdIn(dd), dfFwdOut(bb,jj));
                        else /* DFLOW_Intersect */
                            bitvAnd(class_, dfFwdIn(dd), dfFwdIn(dd), dfFwdOut(bb,jj));
                }
        }

        /* Count changed "in" sets. */
        nchanged = 0;
        for (i = 0; i < flogBlockC(flog); i++) {
                bb = flogBlock(flog, i);
                if (!bb) continue;
                if (bbEntryC(bb) == 0 && !bb->isblock0) continue;

                if (!bitvEqual(class_, dfFwdIn(bb), dfFwdTemp(bb))) nchanged++;
        }

        return nchanged;
}

/* "inFrOut" is the type of equations we want to use; it must be
 * DFLOW_Intersection or DFLOW_Union (see header file).
 * "dflowInit" is a pointer to an initialization function that is called
 * after dflowFwdInit (standard initializer). It may be NULL.
 * "cutoff" is the max num. of iterations.
 * Upon exit, if *pcount == 0 => the algorithm didn't converge with "cutoff"
 * limit.
 */
int
dflowFwdIterate(FlowGraph flog, DFlowType inFrOut, int cutoff, int *pcount,
                DFlowInitFun dflowInit)
{
        int     i, n=-1;
        flogFixEntries(flog);

        if (inFrOut != DFLOW_Intersection && inFrOut != DFLOW_Union)
                bug("dflowFwdIterate: bad equation type...");

        dflowFwdInit(flog, inFrOut);

        if (dflowInit)  dflowInit(flog);

        for (i = 0; i < cutoff; i++) {
                n = dflowFwdStep(flog, inFrOut);
                if (n == 0) break;
        }
        if (pcount) *pcount = i;

        return n;
}

/*****************************************************************************
 *
 * :: Reverse Dataflow
 *
 ****************************************************************************/

local void
dflowRevInit(FlowGraph flog, DFlowType inFrOut)
{
        int             i;
        BBlock          bb;
        BitvClass       class_ = flogBitvClass(flog);

        assert( !flog->dfIsPerExit );
        flogFixEntries(flog);

        for (i = 0; i < flogBlockC(flog); i++) {
                bb = flogBlock(flog, i);
                if (!bb) continue;
                if (bbEntryC(bb) == 0 && !bb->isblock0) continue;
                if (inFrOut == DFLOW_Union)
                        bitvClearAll(class_, dfRevIn(bb));
                else  /* DFLOW_Intersection */
                        bitvSetAll(class_, dfRevIn(bb));
        }
}

int
dflowRevIterate(FlowGraph flog, DFlowType inFrOut, 
                int cutoff, int *pcount,
                DFlowInitFun dfInit)
{
        int i, n=-1;

        dflowRevInit(flog, inFrOut);
        
        if (dfInit) dfInit(flog);

        for (i = 0; n != 0 && i < cutoff ; i++)
                n = dflowRevStep(flog, inFrOut);
        
        if (pcount) *pcount = i;

        return n;
}

int
dflowRevStep(FlowGraph flog, DFlowType inFrOut)
{
        BitvClass class_ = flogBitvClass(flog);
        BBlock    bb; 
        int       i, j, changed;
        
        changed = 0;
        for (i = 0; i < flogBlockC(flog); i++) {
                /* Create the new "out" set */
                bb = flogBlock(flog, i);
                if (!bb) continue;
                if (bbEntryC(bb) == 0 && !bb->isblock0) continue;

                if (inFrOut == DFLOW_Union) {
                        bitvClearAll(class_, dfRevOut(bb));
                        for (j = 0; j < bbExitC(bb); j++)
                                bitvOr(class_, dfRevOut(bb),
                                       dfRevOut(bb), dfRevIn(bbExit(bb, j)));
                }
                else { /* Intersection */
                        bitvSetAll(class_, dfRevOut(bb));
                        for (j = 0; j < bbExitC(bb); j++)
                                bitvAnd(class_, dfRevOut(bb),
                                        dfRevOut(bb), dfRevIn(bbExit(bb, j)));
                }

                /* In[B] := use[B] op (out[B] - def[B]) */
                bitvCopy(class_, dfRevTemp(bb), dfRevIn(bb));
                bitvMinus(class_, dfRevIn(bb), dfRevOut(bb), dfRevKill(bb));
                bitvOr(class_, dfRevIn(bb), dfRevGen(bb), dfRevIn(bb));

                if (!bitvEqual(class_, dfRevTemp(bb), dfRevIn(bb)))
                        changed++;
        }
        return changed;
}
                

