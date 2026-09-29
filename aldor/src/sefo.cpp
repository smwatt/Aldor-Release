///////////////////////////////////////////////////////////////////////////////
//
// sefo.cpp: Semantic form maniuplation
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
# include "genfoam.h"
# include "of_util.h"
# include "srcpos.h"

bool    sstDebug                = false;
bool    sstMarkDebug            = false;

bool    sefoPrintDebug          = false;
bool    sefoEqualDebug          = false;
bool    sefoFreeDebug           = false;
bool    sefoSubstDebug          = false;
bool    sefoUnionDebug          = false;
bool    sefoInterDebug          = false;
bool    sefoCloseDebug          = false;

#define sstDEBUG(s)             DEBUG_IF(sstDebug,       s)
#define sstMarkDEBUG(s)         DEBUG_IF(sstMarkDebug,   s)

#define sefoPrintDEBUG(s)       DEBUG_IF(sefoPrintDebug, s)
#define sefoEqualDEBUG(s)       DEBUG_IF(sefoEqualDebug, s)
#define sefoFreeDEBUG(s)        DEBUG_IF(sefoFreeDebug,  s)
#define sefoSubstDEBUG(s)       DEBUG_IF(sefoSubstDebug, s)
#define sefoUnionDEBUG(s)       DEBUG_IF(sefoUnionDebug, s)
#define sefoInterDEBUG(s)       DEBUG_IF(sefoInterDebug, s)
#define sefoCloseDEBUG(s)       DEBUG_IF(sefoCloseDebug, s)

#define SefoSubstTUnique
#undef  SefoSubstShare
#define MarkScheme3

#ifndef NDEBUG
static ULong * SubstDebugTable = NULL;

void
substDebugInit(void)
{
        Length  i;
        SubstDebugTable = (ULong *) stoAlloc((unsigned) OB_Other,
                                             (TF_LIMIT + 1) * sizeof(ULong));
        for (i = 0; i <= TF_LIMIT; i += 1)
                SubstDebugTable[i] = 0;
}

void
substDebugReport(void)
{
        Length  i;
        for (i = 0; i <= TF_LIMIT; i += 1)
                fprintf(dbOut, "\t%12s:\t\t%12ld\n",
                        tformStr(i), SubstDebugTable[i]);
}
#endif

/*****************************************************************************
 *
 * :: Local function declarations.
 *
 ****************************************************************************/

extern void             genGetConstNums         (List<Syme>);
extern List<Syme>         tfGetDomConstants       (TForm);

local int               sstPrSefo               (FILE *, int, Sefo);
local int               sstPrSyme               (FILE *, int, Syme);
local int               sstPrTForm              (FILE *, int, TForm);
local int               sstPrAbSub              (FILE *, int, AbSub);
local int               sstPrSefoList           (FILE *, int, List<Sefo>);
local int               sstPrSymeList           (FILE *, int, List<Syme>);
local int               sstPrTFormList          (FILE *, int, List<TForm>);
local int               sstPrBool               (FILE *, int, bool);
local int               sstPrLib                (FILE *, int, Lib);

local int               sefoPrint0              (FILE *, bool, Sefo);
local int               symePrint0              (FILE *, bool, Syme);
local int               tformPrint0             (FILE *, bool, TForm);
local int               sefoListPrint0          (FILE *, bool, List<Sefo>);
local int               symeListPrint0          (FILE *, bool, List<Syme>);
local int               tformListPrint0         (FILE *, bool, List<TForm>);

local bool              sefoEqual0              (List<Syme>, Sefo, Sefo);
local bool              symeEqual0              (List<Syme>, Syme, Syme);
local bool              tformEqual0             (List<Syme>, TForm, TForm);
local bool              sefoListEqual0          (List<Syme>, List<Sefo>, List<Sefo>);
local bool              symeListEqual0          (List<Syme>, List<Syme>, List<Syme>);
local bool              tformListEqual0         (List<Syme>,List<TForm>,List<TForm>);

local bool              symeTypeEqual0          (List<Syme>, Syme, Syme);
local bool              symeExtendEqual0        (List<Syme>, Syme, Syme);
local bool              symeOriginEqual0        (List<Syme>, Syme, Syme);

local bool              tformEqualCheckSymes    (TForm);
local Sefo              sefoEqualMods           (Sefo);

local void              sfvInitTable            (void);
local void              sfvFiniTable            (void);
local void              sfvPushTable            (void);
local void              sfvConsTable            (Syme);
local FreeVar           sfvPopTable             (bool);

local void              sfvAddSyme              (Syme);
local void              sfvAddSymes             (List<Syme>);
local void              sfvDelSyme              (Syme);
local void              sfvDelSymes             (List<Syme>);
local void              sfvDelStab              (Stab);

local bool              sfvIsAncestor           (TForm, TForm);
local TForm             sfvCommonAncestor       (TForm, TForm);

local void              sefoFreeVars0           (TForm *, TForm, Sefo);
local void              symeFreeVars0           (TForm *, TForm, Syme);
local void              tformFreeVars0          (TForm *, TForm, TForm);
local void              tqualFreeVars0          (TForm *, TForm, TQual);
local void              abSubFreeVars0          (TForm *, TForm, AbSub);
local void              sefoListFreeVars0       (TForm *, TForm, List<Sefo>);
local void              symeListFreeVars0       (TForm *, TForm, List<Syme>);
local void              tformListFreeVars0      (TForm *, TForm, List<TForm>);
local void              tqualListFreeVars0      (TForm *, TForm, List<TQual>);

local void              sefoUnboundVars0        (TForm *, TForm, Sefo);
local void              symeUnboundVars0        (TForm *, TForm, Syme);
local void              tformUnboundVars0       (TForm *, TForm, TForm);
local void              tqualUnboundVars0       (TForm *, TForm, TQual);
local void              sefoListUnboundVars0    (TForm *, TForm, List<Sefo>);
local void              symeListUnboundVars0    (TForm *, TForm, List<Syme>);
local void              tformListUnboundVars0   (TForm *, TForm, List<TForm>);
local void              tqualListUnboundVars0   (TForm *, TForm, List<TQual>);

local List<TQual>         tqualListSubst          (AbSub, List<TQual>);

local Sefo              sefoSubst0              (AbSub, Sefo);
local Syme              symeSubst0              (AbSub, Syme);
local TForm             tformSubst0             (AbSub, TForm);
local TQual             tqualSubst0             (AbSub, TQual);
local FreeVar           freeVarSubst0           (AbSub, FreeVar);
local List<Syme>          parentListSubst0        (AbSub, List<Syme>);
local List<Sefo>          condListSubst0          (AbSub, List<Sefo>);
local List<Syme>          symeListSubst0          (AbSub, List<Syme>);
local List<TForm>         tformListSubst0         (AbSub, List<TForm>);
local List<TQual>         tqualListSubst0         (AbSub, List<TQual>);

local void              symeListMarkTwins       (AbSub, Syme, Syme);
local void              symeListSubstUseConstants(AbSub, List<Syme>, List<Syme>,
                                                 List<Syme>);
local bool              sefoDependsOnRuntimeValue(AbSub, Sefo);
local Syme              sefoHeadSyme           (AbSub, Sefo);
local bool              symeImplOwnedByExporter(AbSub, Syme, Syme);

local bool              symeWillPush            (Syme);
local bool              symeWillSubst           (AbSub, Syme);
local bool              tformWillSubst          (AbSub, TForm);

local void              slcAddSyme              (Lib, Syme);
local void              slcAddType              (Lib, TForm);

local void              sefoClosure0            (Lib, Sefo);
local void              symeClosure0            (Lib, Syme);
local void              symeClosure1            (Lib, Syme);
local void              tformClosure0           (Lib, TForm);
local void              tqualClosure0           (Lib, TQual);

local void              sefoListClosure0        (Lib, List<Sefo>);
local void              symeListClosure0        (Lib, List<Syme>);
local void              tformListClosure0       (Lib, List<TForm>);
local void              tqualListClosure0       (Lib, List<TQual>);

local int               freeVarToBuffer         (Lib, Buffer, FreeVar);

local int               tformToBuffer1          (Lib, Buffer, TForm);
local TForm             tformFrBuffer1          (Lib, Buffer);

local void              sefoFrBuffer0           (Buffer);
local void              symeFrBuffer0           (Buffer);
local void              tformFrBuffer0          (Buffer);
local void              tqualFrBuffer0          (Buffer);
local void              tformListFrBuffer0      (Buffer);
local void              tqualListFrBuffer0      (Buffer);

/*****************************************************************************
 *
 * :: Sefo/Syme/TForm traversal stack
 *
 ****************************************************************************/

union  sstArg {
        void *         ptr;
        bool *          pbool;
        Sefo            sefo;
        List<Sefo>        sefos;
        Syme            syme;
        List<Syme>        symes;
        TForm           tform;
        List<TForm>       tforms;
        AbSub           sigma;
        Lib             lib;
};

struct sstFrame {
        MutString          where;
        union sstArg    arg1;
        union sstArg    arg2;
        struct sstFrame *prev;
};

static struct sstFrame  *sstStack = 0;

void
sstStackPush(MutString where, void * arg1, void * arg2)
{
        struct sstFrame *frame;

        frame = (struct sstFrame *) stoAlloc(OB_Other, sizeof(*frame));
        frame->where    = where;
        frame->arg1.ptr = arg1;
        frame->arg2.ptr = arg2;
        frame->prev     = sstStack;
        sstStack        = frame;
}

void
sstStackPop(void)
{
        struct sstFrame *frame;

        if (!sstStack)
                bug("sstStackPop: popping empty stack");

        frame = sstStack->prev;
        stoFree((void *) sstStack);
        sstStack = frame;
}

int
sstStackPrint(FILE *fout)
{
        int             n, cc = 0;
        struct sstFrame *frame;
        MutString          where;

        cc += fputcTimes('v', 70, fout);
        cc += fprintf(fout, "\n");

        /* Compute stack depth. */
        for (n = 0, frame = sstStack; frame; frame = frame->prev)
                n++;

        /* Display the stack. */
        for (frame = sstStack; frame; frame = frame->prev, n--) {
                where = frame->where;
                cc += fprintf(fout, "Level %d. %s: \n", n, where);

                /*
                 * Equal
                 */
                if (strEqual(where, "sefoEqual")) {
                        cc += sstPrSefo(fout, 1, frame->arg1.sefo);
                        cc += sstPrSefo(fout, 2, frame->arg2.sefo);
                }
                else if (strEqual(where, "symeIsTwin")) {
                        cc += sstPrSyme(fout, 1, frame->arg1.syme);
                        cc += sstPrSyme(fout, 2, frame->arg2.syme);
                }
                else if (strEqual(where, "symeEqual")) {
                        cc += sstPrSyme(fout, 1, frame->arg1.syme);
                        cc += sstPrSyme(fout, 2, frame->arg2.syme);
                }
                else if (strEqual(where, "tformEqual")) {
                        cc += sstPrTForm(fout, 1, frame->arg1.tform);
                        cc += sstPrTForm(fout, 2, frame->arg2.tform);
                }
                else if (strEqual(where, "sefoListEqual")) {
                        cc += sstPrSefoList(fout, 1, frame->arg1.sefos);
                        cc += sstPrSefoList(fout, 2, frame->arg2.sefos);
                }
                else if (strEqual(where, "symeListEqual")) {
                        cc += sstPrSymeList(fout, 1, frame->arg1.symes);
                        cc += sstPrSymeList(fout, 2, frame->arg2.symes);
                }
                else if (strEqual(where, "tformListEqual")) {
                        cc += sstPrTFormList(fout, 1, frame->arg1.tforms);
                        cc += sstPrTFormList(fout, 2, frame->arg2.tforms);
                }

                /*
                 * EqualMod
                 */
                else if (strEqual(where, "sefoEqualMod")) {
                        cc += sstPrSefo(fout, 1, frame->arg1.sefo);
                        cc += sstPrSefo(fout, 2, frame->arg2.sefo);
                }
                else if (strEqual(where, "symeEqualMod")) {
                        cc += sstPrSyme(fout, 1, frame->arg1.syme);
                        cc += sstPrSyme(fout, 2, frame->arg2.syme);
                }
                else if (strEqual(where, "tformEqualMod")) {
                        cc += sstPrTForm(fout, 1, frame->arg1.tform);
                        cc += sstPrTForm(fout, 2, frame->arg2.tform);
                }
                else if (strEqual(where, "sefoListEqualMod")) {
                        cc += sstPrSefoList(fout, 1, frame->arg1.sefos);
                        cc += sstPrSefoList(fout, 2, frame->arg2.sefos);
                }
                else if (strEqual(where, "symeListEqualMod")) {
                        cc += sstPrSymeList(fout, 1, frame->arg1.symes);
                        cc += sstPrSymeList(fout, 2, frame->arg2.symes);
                }
                else if (strEqual(where, "tformListEqualMod")) {
                        cc += sstPrTFormList(fout, 1, frame->arg1.tforms);
                        cc += sstPrTFormList(fout, 2, frame->arg2.tforms);
                }

                /*
                 * FreeVars
                 */
                else if (strEqual(where, "abSubFreeVars")) {
                        cc += sstPrBool (fout, 1, *(frame->arg1.pbool));
                        cc += sstPrAbSub(fout, 2, frame->arg2.sigma);
                }
                else if (strEqual(where, "tformFreeVars")) {
                        cc += sstPrBool (fout, 1, *(frame->arg1.pbool));
                        cc += sstPrTForm(fout, 2, frame->arg2.tform);
                }

                /*
                 * ListClosure
                 */
                else if (strEqual(where, "symeListClosure")) {
                        cc += sstPrLib     (fout, 1, frame->arg1.lib);
                        cc += sstPrSymeList(fout, 2, frame->arg2.symes);
                }

                else {
                        cc += fprintf(fout, "\tArg1: ???\n");
                }
        }
        cc += fputcTimes('^', 70, fout);
        cc += fprintf(fout, "\n");

        return cc;
}

local int
sstPrSefo(FILE *fout, int n, Sefo sefo)
{
        int     cc = 0;
        cc += fprintf(fout, "\tArg%d: ", n);
        cc += sefoPrint(fout, sefo);
        cc += fprintf(fout, "\n");
        return cc;
}

local int
sstPrSyme(FILE *fout, int n, Syme syme)
{
        int     cc = 0;
        cc += fprintf(fout, "\tArg%d: ", n);
        cc += symePrint(fout, syme);
        cc += fprintf(fout, "\n");
        return cc;
}

local int
sstPrTForm(FILE *fout, int n, TForm tform)
{
        int     cc = 0;
        cc += fprintf(fout, "\tArg%d: ", n);
        cc += tformPrint(fout, tform);
        cc += fprintf(fout, "\n");
        return cc;
}

local int
sstPrAbSub(FILE *fout, int n, AbSub sigma)
{
        int     cc = 0;
        cc += fprintf (fout, "\tArg%d: ", n);
        cc += absPrint(fout, sigma);
        cc += fprintf (fout, "\n");
        return cc;
}

local int
sstPrSefoList(FILE *fout, int n, List<Sefo> sefos)
{
        int     i, cc = 0;
        cc += fprintf(fout, "\tArg%d:\n", n);
        for (i = 0; sefos; sefos = cdr(sefos), i++) {
                cc += fprintf(fout, "%d. ", i);
                cc += sefoPrint(fout, car(sefos));
                cc += fprintf(fout, "\n");
        }
        return cc;
}

local int
sstPrSymeList(FILE *fout, int n, List<Syme> symes)
{
        int     i, cc = 0;
        cc += fprintf(fout, "\tArg%d:\n", n);

        for (i = 0; symes; symes = cdr(symes), i++) {
                cc += fprintf(fout, "%d. ", i);
                cc += symePrint(fout, car(symes));
                cc += fprintf(fout, "\n");
        }
        return cc;
}

local int
sstPrTFormList(FILE *fout, int n, List<TForm> tforms)
{
        int     i, cc = 0;
        cc += fprintf(fout, "\tArg%d:\n", n);

        for (i = 0; tforms; tforms = cdr(tforms), i++) {
                cc += fprintf(fout, "%d. ", i);
                cc += tformPrint(fout, car(tforms));
                cc += fprintf(fout, "\n");
        }
        return cc;
}

local int
sstPrBool(FILE *fout, int n, bool flag)
{
        return fprintf(fout, "\tArg%d: %s\n", n, flag ? "true" : "false");
}

local int
sstPrLib(FILE *fout, int n, Lib lib)
{
        int     cc = 0;
        cc += fprintf(fout, "\tArg%d: ", n);
        cc += fprintf(fout, "%s", (lib->name).unparseStatic());
        cc += fprintf(fout, "\n");
        return cc;
}

/*****************************************************************************
 *
 * :: Sefo/Syme/TForm traversal control (General)
 *
 ****************************************************************************/

static int      sstSerialDebug  = 0;    /* debugging counter */

#define sstNext(wh,a1,a2) \
        { sstDEBUG(sstNextDB(wh, (void *)(a1), (void *)(a2))); sstNext0(); }
#define sstDone() \
        { sstDone0(); sstDEBUG(sstDoneDB()); }

#define sstDoneSefo(sefo)       { sefoClear(sefo); sstDone(); }
#define sstDoneSyme(syme)       { symeClear(syme); sstDone(); }
#define sstDoneTForm(tform)     { tformClear(tform); sstDone(); }
#define sstDoneAbSub(sigma)     { abSubClear(sigma); sstDone(); }
#define sstDoneSefoList(sl)     { sefoListClear(sl); sstDone(); }
#define sstDoneSymeList(sl)     { symeListClear(sl); sstDone(); }
#define sstDoneTFormList(tl)    { tformListClear(tl); sstDone(); }

/*****************************************************************************
 *
 * :: Sefo/Syme/TForm traversal control (Scheme 1)
 *
 ****************************************************************************/

#if defined(MarkScheme1)
/* To handle simultaneous sst traversals, interpret the mark bits
 * as two fields:  the round number and the traversal number.
 * Increment the round number only after the current traversals complete.
 * Each round can handle up to eight simultaneous traversals.
 * We can have a lot of rounds.  (2^24 for 32 bit longs)
 */

/* A more complete implementation would use a stack of mark bits,
 * but since we do so many traversals, it would be better to have
 * a way that doesn't allocate memory.
 */
/* Where this scheme fails the fastest is when we have a need to
 * do repeated instances of traversal "B" within a traversal "A".
 * Being in traversal "A" fixes the round, and then the repeated
 * instances of traversal "B" use up the bits.
 */

static ULong    sstSerialRound          = 1;    /* sst round counter */
static ULong    sstSerialTraversal      = 0;    /* sst traversal counter */
static ULong    sstMarkMask             = 0;    /* active traversal mask */
static ULong    sstSerialNext           = 0;    /* next available mask bit */

/* sstSerialTraversal always points to the high bit of sstMarkMask. */

#define         SST_TR_NBITS    (8)
#define         SST_RD_NBITS    (bitsizeof(SefoMark) - SST_TR_NBITS)

#define         SST_TR_SHIFT    (0)
#define         SST_TR_MASK     ((1 << SST_TR_NBITS) - 1)

#define         SST_RD_SHIFT    (SST_TR_SHIFT + SST_TR_NBITS)
#define         SST_RD_MASK     (((1 << SST_RD_NBITS) - 1) << SST_RD_SHIFT)

#define         sstRound(m)     (((m) & SST_RD_MASK) >> SST_RD_SHIFT)
#define         sstSet(r, t)    (((r) << SST_RD_SHIFT) | (t) << SST_TR_SHIFT)
#define         sstBit()        (1 << sstSerialTraversal)

local void
sstNext0(void)
{
        /* If all the traversals for the current round are complete,
         * go to the next round, otherwise, go to the next traversal.
         */
        if (sstMarkMask == 0) {
                sstSerialRound += 1;
                sstSerialNext = 0;
        }
        else
                sstSerialNext += 1;

        if (sstSerialNext == SST_TR_NBITS)
                bug("sstNext:  too many simultaneous traversals");

        sstSerialTraversal = sstSerialNext;
        sstMarkMask |= sstBit();
}

local void
sstDone0(void)
{
        ULong   mask;

        sstMarkMask &= ~sstBit();
        sstSerialTraversal = 0;

        for (mask = sstMarkMask >> 1; mask; mask >>= 1)
                sstSerialTraversal += 1;

        assert(sstMarkMask == 0 || sstMarkMask >> sstSerialTraversal == 1);
}

local ULong
sstSetMark(ULong mark)
{
        if (sstRound(mark) == sstSerialRound)
                return (mark | sstBit());
        else
                return sstSet(sstSerialRound, sstBit());
}

local bool
sstIsMarked(ULong mark)
{
        return (sstRound(mark) == sstSerialRound) && (mark & sstBit());
}

#define sstMarkSyme(syme)       symeSetMark(syme, sstSetMark(symeMark(syme)))
#define sstMarkTForm(tf)        tformSetMark(tf, sstSetMark(tformMark(tf)))

#define sstSymeIsMarked(syme)   sstIsMarked(symeMark(syme))
#define sstTFormIsMarked(tf)    sstIsMarked(tformMark(tf))

#endif /* defined(MarkScheme1) */


/*****************************************************************************
 *
 * :: Sefo/Syme/TForm traversal control (Scheme 2)
 *
 ****************************************************************************/

#if defined(MarkScheme2)

/*
 * This scheme uses each mark bit individually and relies on being able
 * to clear all the bits which are set.
 */

#define SST_NMASK_BITS  bitsizeof(SefoMark)

static int      sstMarkBitNo    = -1;
static SefoMark sstMarkMask     = 0;
static int      sstMarkCount    = 0;
static int      sstMarkCountSave[SST_NMASK_BITS];

local void
sstNext0(void)
{
        /* Save state. */
        if (sstMarkBitNo >= 0) {
                sstMarkCountSave[sstMarkBitNo] = sstMarkCount;
                sstMarkDEBUG(fprintf(dbOut, "sstNext: saving count %d\n",
                                     sstMarkCount));
        }

        /* Increment level. */
        sstMarkBitNo++;
        if (sstMarkBitNo >= SST_NMASK_BITS) {
                sstStackPrint(stderr);
                bug("sstNext:  too many simultaneous traversals");
        }

        /* Initialize new state. */
        sstMarkMask = 1L << sstMarkBitNo;
        sstMarkCount = sstMarkCountSave[sstMarkBitNo] = 0;

        sstMarkDEBUG(fprintf(dbOut, "sstNext: BitNo = %d\n", sstMarkBitNo));
}

local void
sstDone0(void)
{
        /* Verify current state. */
        sstMarkDEBUG(fprintf(dbOut, "sstDone: BitNo = %d\n", sstMarkBitNo));
        assert(sstMarkBitNo >= 0);
        if (sstMarkCount != 0) {
                sstStackPrint(stderr);
                bug("sstDone:  lost %d marks.", sstMarkCount);
        }

        /* Decrement level. */
        sstMarkBitNo--;

        /* Restore previous state. */
        if (sstMarkBitNo >= 0) {
                sstMarkMask  = 1L << sstMarkBitNo;
                sstMarkCount = sstMarkCountSave[sstMarkBitNo];
                sstMarkDEBUG(fprintf(dbOut, "sstDone: restoring count %d\n",
                                     sstMarkCount));
        }
}

#define sstSymeIsMarked(syme)   (symeMark(syme) & sstMarkMask)
#define sstTFormIsMarked(tf)    (tformMark(tf)  & sstMarkMask)

#if EDIT_1_0_n1_07
#define sstMarkSyme(syme) \
        { if (!sstSymeIsMarked(syme)) { \
                sstMarkDEBUG(fprintf(dbOut, "++ marking %p\n", syme)); \
                sstMarkCount++; \
                symeSetMark((syme), symeMark(syme) | sstMarkMask); }}

#define sstMarkTForm(tf) \
        { if (!sstTFormIsMarked(tf)) { \
                sstMarkDEBUG(fprintf(dbOut, "++ marking %p\n", tf)); \
                sstMarkCount++; \
                tformSetMark((tf), tformMark(tf) | sstMarkMask); }}

#define sstClearSyme(syme) \
        { if (sstSymeIsMarked(syme)) { \
                sstMarkDEBUG(fprintf(dbOut, "-- clearing %p\n", syme)); \
                sstMarkCount--; \
                symeSetMark((syme), symeMark(syme) & ~sstMarkMask); }}

#define sstClearTForm(tf) \
        { if (sstTFormIsMarked(tf)) { \
                sstMarkDEBUG(fprintf(dbOut, "-- clearing %p\n", tf)); \
                sstMarkCount--; \
                tformSetMark((tf), tformMark(tf) & ~sstMarkMask); }}

#else /* EDIT_1_0_n1_07 */

#define sstMarkSyme(syme) \
        { if (!sstSymeIsMarked(syme)) { \
                sstMarkDEBUG(fprintf(dbOut, "++ marking %x\n", syme)); \
                sstMarkCount++; \
                symeSetMark((syme), symeMark(syme) | sstMarkMask); }}

#define sstMarkTForm(tf) \
        { if (!sstTFormIsMarked(tf)) { \
                sstMarkDEBUG(fprintf(dbOut, "++ marking %x\n", tf)); \
                sstMarkCount++; \
                tformSetMark((tf), tformMark(tf) | sstMarkMask); }}

#define sstClearSyme(syme) \
        { if (sstSymeIsMarked(syme)) { \
                sstMarkDEBUG(fprintf(dbOut, "-- clearing %x\n", syme)); \
                sstMarkCount--; \
                symeSetMark((syme), symeMark(syme) & ~sstMarkMask); }}

#define sstClearTForm(tf) \
        { if (sstTFormIsMarked(tf)) { \
                sstMarkDEBUG(fprintf(dbOut, "-- clearing %x\n", tf)); \
                sstMarkCount--; \
                tformSetMark((tf), tformMark(tf) & ~sstMarkMask); }}

#endif /* EDIT_1_0_n1_07 */

#endif /* defined(MarkScheme2) */


/*****************************************************************************
 *
 * :: Sefo/Syme/TForm traversal control (Scheme 3)
 *
 ****************************************************************************/

#if defined(MarkScheme3)

#ifndef NDEBUG
static ULong     sstMarkMask    =  0;   /* For the generic debug fns. */
#endif

/*
 * Not Traversing:        sstMarkDepth=0; sstMarkTable=0; sstMarkStack=0;
 * 1st Level(uses Round): sstMarkDepth=1; sstMarkTable=0; sstMarkStack=[0];
 * 2nd Level(uses Table): sstMarkDepth=2; sstMarkTable=T; sstMarkStack=[0,0];
 * 3rd Level(uses Table): sstMarkDepth=3; sstMarkTable=U; sstMarkStack=[T,0,0];
 * ...
 */

static int       sstMarkDepth   = 0;    /* current depth of marking. */
static long      sstMarkRound   = 0;    /* round counter        (level 1) */
using SefoMarkTable = Table<void *, void *>;
static List<SefoMarkTable> sstMarkStack = nullptr;    /* mark table stack  (levels > 1) */
static SefoMarkTable sstMarkTable = nullptr;    /* current markees   (levels > 1) */

local void
sstNext0(void)
{
        sstMarkDepth++;

        if (sstMarkDepth == 1)
                sstMarkRound++;
        else {
                sstMarkStack = listCons<SefoMarkTable>(sstMarkTable, sstMarkStack);
                sstMarkTable = 0;       /* Will be allocated on first use. */
        }
}


local void
sstDone0(void)
{

        if (sstMarkDepth == 1)
                ; /* Nothing */
        else {
                if (sstMarkTable) sstMarkTable.free();
                sstMarkTable = car(sstMarkStack);
                sstMarkStack = listFreeCons<SefoMarkTable>(sstMarkStack);
        }

        sstMarkDepth--;
}

local void
sstMarkSyme(Syme syme)
{
        if (sstMarkDepth == 1)
                symeSetMark(syme, sstMarkRound);
        else {
                if (!sstMarkTable)
                        sstMarkTable = SefoMarkTable::create();
                sstMarkTable.set((void *) syme, (void *) syme);
        }
}

local void
sstMarkTForm(TForm tf)
{
        if (sstMarkDepth == 1)
                tformSetMark(tf, sstMarkRound);
        else {
                if (!sstMarkTable)
                        sstMarkTable = SefoMarkTable::create();
                sstMarkTable.set((void *) tf, (void *) tf);
        }
}

#define sstSymeIsMarked(syme) \
        (sstMarkDepth == 1 ? symeMark(syme) == sstMarkRound \
         : sstMarkTable && sstMarkTable.get((void *) syme, nullptr))

#define sstTFormIsMarked(tf) \
        (sstMarkDepth == 1 ? tformMark(tf) == sstMarkRound \
         : sstMarkTable && sstMarkTable.get((void *) tf, nullptr))

#endif /* defined(MarkScheme3) */

/*****************************************************************************
 *
 * :: Sefo/Syme/TForm unmarking
 *
 ****************************************************************************/

#if defined(MarkScheme1)

#define sefoClear(sefo)         Nothing
#define symeClear(syme)         Nothing
#define tformClear(tform)       Nothing
#define abSubClear(sigma)       Nothing
#define sefoListClear(sl)       Nothing
#define symeListClear(sl)       Nothing
#define tformListClear(tl)      Nothing

#endif /* defined(MarkScheme1) */


#if defined(MarkScheme2)

local void      sefoClear       (Sefo);
local void      symeClear       (Syme);
local void      tformClear      (TForm);
local void      tqualClear      (TQual);
local void      abSubClear      (AbSub);
local void      sefoListClear   (List<Sefo>);
local void      symeListClear   (List<Syme>);
local void      tformListClear  (List<TForm>);
local void      tqualListClear  (List<TQual>);

local void
sefoClear(Sefo sefo)
{
        if (abIsLeaf(sefo)) {
                if (abSyme(sefo))
                        symeClear(abSyme(sefo));
                if (abImplicit(sefo))
                        sefoClear(abImplicit(sefo));
        }
        else {
                Length  i;
                for (i = 0; i < abArgc(sefo); i += 1)
                        sefoClear(abArgv(sefo)[i]);
        }
}

local void
symeClear(Syme syme)
{
        if (!sstSymeIsMarked(syme))
                return;

        sstClearSyme(syme);

        if (!symeIsLazy(syme))
                tformClear(symeType(syme));

        if (symeIsImport(syme))
                tformClear(symeExporter(syme));
        if (symeIsExtend(syme))
                symeListClear(symeExtendee(syme));
        if (symeExtension(syme))
                symeClear(symeExtension(syme));
        if (symeCondition(syme))
                sefoListClear(symeCondition(syme));
}

local void
tformClear(TForm tf)
{
        assert (tf != 0);
        tf = tfFollowOnly(tf);

        if (!sstTFormIsMarked(tf))
                return;

        sstClearTForm(tf);

        if (tfIsSym(tf))
                ;
        else if (tfIsAbSyn(tf))
                sefoClear(tfGetExpr(tf));
        else if (tfIsNode(tf)) {
                Length  i;
                for (i = 0; i < tfArgc(tf); i += 1)
                        tformClear(tfFollowArg(tf, i));
        }
        else
                bugBadCase(tfTag(tf));

        if (tfSelf(tf))
                symeListClear(tfSelf(tf));
        if (tfSelfSelf(tf))
                symeListClear(tfSelfSelf(tf));
        if (tfParents(tf))
                symeListClear(tfParents(tf));
        if (tfSymes(tf))
                symeListClear(tfSymes(tf));

        /* Extra stuff */
        if (tfDomExports(tf))
                symeListClear(tfDomExports(tf));
        if (tfCatExports(tf))
                symeListClear(tfCatExports(tf));
        if (tfThdExports(tf))
                symeListClear(tfThdExports(tf));

        if (tfDomImports(tf))
                symeListClear(tfDomImports(tf));

        if (tfQueries(tf))
                tformListClear(tfQueries(tf));
        if (tfCascades(tf))
                tqualListClear(tfCascades(tf));
}

local void
tqualClear(TQual tq)
{
        assert(tq != 0);

        tformClear(tqBase(tq));
        if (tqIsQualified(tq))
                tformListClear(tqQual(tq));
}

local void
abSubClear(AbSub sigma)
{
        List<AbBind>      l0;
        for (l0 = sigma->l; l0; l0 = cdr(l0))
                sefoClear(car(l0)->val);
}

local void
sefoListClear(List<Sefo> sefos)
{
        for ( ; sefos; sefos = cdr(sefos))
                sefoClear(car(sefos));
}

local void
symeListClear(List<Syme> symes)
{
        for ( ; symes; symes = cdr(symes))
                symeClear(car(symes));
}

local void
tformListClear(List<TForm> tforms)
{
        for ( ; tforms; tforms = cdr(tforms))
                tformClear(car(tforms));
}

local void
tqualListClear(List<TQual> tquals)
{
        for ( ; tquals; tquals = cdr(tquals))
                tqualClear(car(tquals));
}

#endif /* defined(MarkScheme2) */


#if defined(MarkScheme3)

#define sefoClear(sefo)         Nothing
#define symeClear(syme)         Nothing
#define tformClear(tform)       Nothing
#define abSubClear(sigma)       Nothing
#define sefoListClear(sl)       Nothing
#define symeListClear(sl)       Nothing
#define tformListClear(tl)      Nothing

#endif /* defined(MarkScheme3) */


/*****************************************************************************
 *
 * :: Sefo/Syme/TForm traversal debugging
 *
 ****************************************************************************/

#ifndef NDEBUG

void
sstNextDB(MutString where, void * arg1, void * arg2)
{
        int i;

        sstStackPush(where, arg1, arg2);

        if (sstMarkMask <= 1) return;

        fprintf(dbOut, "-> sstNext (%s) [%d]\n", where, sstSerialDebug);

        fprintf(dbOut, "sstNext +");
        for (i = 0; i < bitsizeof(SefoMark); i++)
                fprintf(dbOut, (sstMarkMask & (1<<i)) ? "1" : "0");
        fprintf(dbOut, "\n");

        sstStackPrint(dbOut);
}

void
sstDoneDB(void)
{
        int i;

        sstStackPop();

        if (sstMarkMask <= 1) return;

        fprintf(dbOut, "<- sstDone\n");

        fprintf(dbOut, "sstDone -");
        for (i = 0; i < bitsizeof(SefoMark); i++)
                fprintf(dbOut, (sstMarkMask & (1<<i)) ? "1" : "0");
        fprintf(dbOut, "\n");
}

#endif /* !defined(NDEBUG) */

/*****************************************************************************
 *
 * :: sstPrint
 *
 ****************************************************************************/

/*
 * Within debugging code, we should avoid using the sst marking code
 * and avoid pushing any lazy symes.
 */

/*
 * sstPrint external entry points.
 */

int
sefoPrint(FILE *fout, Sefo sefo)
{
        return sefoPrint0(fout, true, sefo);
}

int
symePrint(FILE *fout, Syme syme)
{
        return symePrint0(fout, true, syme);
}

int
tformPrint(FILE *fout, TForm tf)
{
        return tformPrint0(fout, true, tf);
}

int
sefoListPrint(FILE *fout, List<Sefo> sefos)
{
        return sefoListPrint0(fout, true, sefos);
}

int
symeListPrint(FILE *fout, List<Syme> symes)
{
        return symeListPrint0(fout, true, symes);
}

int
tformListPrint(FILE *fout, List<TForm> tforms)
{
        return tformListPrint0(fout, true, tforms);
}

/*
 * sstPrintDb external entry points.
 */

int
sefoPrintDb(Sefo sefo)
{
        int     cc = sefoPrint0(dbOut, false, sefo);
        fnewline(dbOut);
        return cc;
}

int
symePrintDb(Syme syme)
{
        int     cc = symePrint0(dbOut, false, syme);
        fnewline(dbOut);
        return cc;
}
int
symePrintDb2(Syme syme)
{
        int     cc = symePrint0(dbOut, false, syme);
        return cc;
}

int
tformPrintDb(TForm tf)
{
        int     cc = tformPrint0(dbOut, false, tf);
        fnewline(dbOut);
        return cc;
}

int
sefoListPrintDb(List<Sefo> sefos)
{
        int     cc = sefoListPrint0(dbOut, false, sefos);
        fnewline(dbOut);
        return cc;
}

int
symeListPrintDb(List<Syme> symes)
{
        int     cc = symeListPrint0(dbOut, false, symes);
        fnewline(dbOut);
        return cc;
}

int
tformListPrintDb(List<TForm> tforms)
{
        int     cc = tformListPrint0(dbOut, false, tforms);
        fnewline(dbOut);
        return cc;
}

/*
 * Local functions.
 */

/* The deep argument is currently unused. */
local int
sefoPrint0_Inner(FILE *fout, bool deep, Sefo sefo, int depth)
{
        int             cc = 0;

        if (!sefo) {
                cc += fprintf(fout, "(* NULL *)");
                return cc;
        }


        if (abIsLeaf(sefo)) {
                cc += fprintf(fout, " ");
                cc += abPrint(fout, sefo);
                sefoPrintDEBUG({
                        if (abSyme(sefo)) {
                                cc += fprintf(fout, "|");
                                cc += symePrint0(fout, false, abSyme(sefo));
                                cc += fprintf(fout, "|");
                        }
                });
        }

        else {
                cc += fprintf(fout, "\n%*s(*", 2*depth, "");
                Length i;
                for (i = 0; i < abArgc(sefo); i += 1) {
                        if (i) cc += fprintf(fout, " ");
                        cc += sefoPrint0_Inner(fout, deep, abArgv(sefo)[i], depth+1);
                }
                cc += fprintf(fout, "%*s*)\n", 2*depth, "");
        }


        return cc;
}


local int
sefoPrint0(FILE *fout, bool deep, Sefo sefo)
{
    return sefoPrint0_Inner(fout, deep, sefo, (int) 0);
#ifdef ANCIENT_SEFOPRINT0
        int             cc = 0;

        if (!sefo)
                return fprintf(fout, "(* NULL *)");

        cc += fprintf(fout, "(*");

        if (abIsLeaf(sefo)) {
                cc += fprintf(fout, " ");
                cc += abPrint(fout, sefo);
                sefoPrintDEBUG({
                        if (abSyme(sefo)) {
                                cc += fprintf(fout, " ");
                                cc += symePrint0(fout, false, abSyme(sefo));
                        }
                });
        }

        else {
                Length i;
                for (i = 0; i < abArgc(sefo); i += 1) {
                        cc += fprintf(fout, " ");
                        cc += sefoPrint0(fout, deep, abArgv(sefo)[i]);
                }
        }

        cc += fprintf(fout, " *)");

        return cc;
#endif
}

local int
symePrint0(FILE *fout, bool deep, Syme syme)
{
        int             cc = 0;

        if (!syme)
                return fprintf(fout, "{* NULL *}");

        cc += fprintf(fout, "{* (%s) ",
                      symeInfo[symeKind(syme)].str + sizeof("SYME_") - 1);

        cc += fprintf(fout, "%s", symeString(syme));

        sefoPrintDEBUG({
                cc += fprintf(fout, " [cn:%ld,", symeConstNum(syme));
                cc += fprintf(fout, " dn:%d,", symeDefnNum(syme));
                cc += fprintf(fout, " ad:%p,", syme);
                cc += fprintf(fout, " mk:%ld]", symeMark(syme));
        });

        if (deep) {
                cc += fprintf(fout, " : ");
                if (symeIsLazy(syme))
                        cc += fprintf(fout, "[lazy] %ld", symeHash(syme));
                else
                        cc += tformPrint0(fout, deep, symeType(syme));

                if (symeIsImport(syme)) {
                        cc += fprintf(fout, " from ");
                        cc += tformPrint0(fout, deep, symeExporter(syme));
                }
        }

        cc += fprintf(fout, " *}");

        return cc;
}

local int
tformPrint0_Inner(FILE *fout, bool deep, TForm tf, int depth)
{
        int             cc = 0;

        cc += fprintf(fout, "%*s", 2*depth, "");
        if (!tf) {
                cc += fprintf(fout, "<* NULL *>\n");
                return cc;
        }

        cc += fprintf(fout, "<* %s\n", tformStr(tfTag(tf)));

        if (tfIsSym(tf))
                ;
        else if (tfIsAbSyn(tf)) {
                cc += fprintf(fout, " <+ SEFO ");
                cc += sefoPrint0(fout, deep, tfGetExpr(tf));
                cc += fprintf(fout, "+>");
        }
        else if (tfIsNode(tf)) {
                Length  i;
                for (i = 0; i < tfArgc(tf); i += 1) {
                        if (i) cc += fprintf(fout, "\n");
                        cc += tformPrint0_Inner(fout,deep,tfArgv(tf)[i], depth+1);
                }
        }
        else
                bugBadCase(tfTag(tf));

        if (tfSymes(tf)) {
                cc += fprintf(fout, "%*s[ SYMES ", 2*depth, "");
                cc += symeListPrint0(fout, deep, tfSymes(tf));
                cc += fprintf(fout, " ]\n");
        }

        cc += fprintf(fout, "%*s*>\n", 2*depth, "");

        return cc;
}

local int
tformPrint0(FILE *fout, bool deep, TForm tf)
{
    return tformPrint0_Inner(fout, deep, tf, (int) 0);
#ifdef ANCIENT_TFORMPRINT0
        int             cc = 0;

        if (!tf)
                return fprintf(fout, "<* NULL *>");

        cc += fprintf(fout, "<* %s", tformStr(tfTag(tf)));

        if (tfIsSym(tf))
                ;
        else if (tfIsAbSyn(tf)) {
                cc += fprintf(fout, " ");
                cc += sefoPrint0(fout, deep, tfGetExpr(tf));
        }
        else if (tfIsNode(tf)) {
                Length  i;
                for (i = 0; i < tfArgc(tf); i += 1) {
                        cc += fprintf(fout, " ");
                        cc += tformPrint0(fout, deep, tfArgv(tf)[i]);
                }
        }
        else
                bugBadCase(tfTag(tf));

        if (tfSymes(tf))
                cc += symeListPrint0(fout, deep, tfSymes(tf));

        cc += fprintf(fout, " *>");

        return cc;
#endif
}

/* The deep argument is currently unused. */
local int
sefoListPrint0(FILE *fout, bool deep, List<Sefo> sefos)
{
        int             cc = 0;

        cc += fprintf(fout, " (");
        for (; sefos; sefos = cdr(sefos)) {
                cc += fprintf(fout, " ");
                cc += sefoPrint0(fout, false, car(sefos));
        }
        cc += fprintf(fout, " )");

        return cc;
}

/* The deep argument is currently unused. */
local int
symeListPrint0(FILE *fout, bool deep, List<Syme> symes)
{
        int             cc = 0;

        cc += fprintf(fout, " (");
        for (; symes; symes = cdr(symes)) {
                cc += fprintf(fout, " ");
                cc += symePrint0(fout, false, car(symes));
        }
        cc += fprintf(fout, " )");

        return cc;
}

/* The deep argument is currently unused. */
local int
tformListPrint0(FILE *fout, bool deep, List<TForm> tforms)
{
        int             cc = 0;

        cc += fprintf(fout, " (");
        for (; tforms; tforms = cdr(tforms)) {
                cc += fprintf(fout, " ");
                cc += tformPrint0(fout, false, car(tforms));
        }
        cc += fprintf(fout, " )");

        return cc;
}

/*****************************************************************************
 *
 * :: symeIsTwin
 *
 ****************************************************************************/

local void              symeMarkTwins           (Syme);
local bool              symeFindTwins           (Syme);
local void              symeClearTwins          (Syme);

bool
symeIsTwin(Syme syme1, Syme syme2)
{
        bool    eq;

        if (syme1 == syme2) return false;

        symeMarkTwins(syme2);
        eq = symeFindTwins(syme1);
        symeClearTwins(syme2);

        return eq;
}

local void
symeMarkTwins(Syme syme)
{
        List<Syme>        symes = symeTwins(syme);

        symeSetMarkBit(syme);
        for (; symes; symes = cdr(symes)) {
                Syme    twin = car(symes);
                if (twin != syme) symeMarkTwins(twin);
        }
}

local bool
symeFindTwins(Syme syme)
{
        List<Syme>        symes = symeTwins(syme);
        bool            result = false;

        result = symeMarkBit(syme);
        for (; !result && symes; symes = cdr(symes)) {
                Syme    twin = car(symes);
                if (twin != syme) result = symeFindTwins(twin);
        }

        return result;
}

local void
symeClearTwins(Syme syme)
{
        List<Syme>        symes = symeTwins(syme);

        symeClrMarkBit(syme);
        for (; symes; symes = cdr(symes)) {
                Syme    twin = car(symes);
                if (twin != syme) symeClearTwins(twin);
        }
}

/*****************************************************************************
 *
 * :: sstEqual
 *
 ****************************************************************************/

/*
 * sstEqual external entry points.
 */

bool
sefoEqual(Sefo sefo1, Sefo sefo2)
{
        bool    eq;

        sstNext("sefoEqual", sefo1, sefo2);
        eq = sefoEqual0(NULL, sefo1, sefo2);
        sstDoneSefo(sefo1);

        return eq;
}

bool
symeEqual(Syme syme1, Syme syme2)
{
        bool    eq;

        sstNext("symeEqual", syme1, syme2);
        eq = symeEqual0(NULL, syme1, syme2);
        sstDoneSyme(syme1);

        return eq;
}

bool
tformEqual(TForm tf1, TForm tf2)
{
        bool    eq;

        sstNext("tformEqual", tf1, tf2);
        eq = tformEqual0(NULL, tf1, tf2);
        sstDoneTForm(tf1);

        return eq;
}

bool
sefoListEqual(List<Sefo> sefos1, List<Sefo> sefos2)
{
        bool    eq;

        sstNext("sefoListEqual", sefos1, sefos2);
        eq = sefoListEqual0(NULL, sefos1, sefos2);
        sstDoneSefoList(sefos1);

        return eq;
}

bool
symeListEqual(List<Syme> symes1, List<Syme> symes2)
{
        bool    eq;

        sstNext("symeListEqual", symes1, symes2);
        eq = symeListEqual0(NULL, symes1, symes2);
        sstDoneSymeList(symes1);

        return eq;
}

bool
tformListEqual(List<TForm> tforms1, List<TForm> tforms2)
{
        bool    eq;

        sstNext("tformListEqual", tforms1, tforms2);
        eq = tformListEqual0(NULL, tforms1, tforms2);
        sstDoneTFormList(tforms1);

        return eq;
}

/*
 * sstEqualMod external entry points.
 */

bool
sefoEqualMod(List<Syme> mods, Sefo sefo1, Sefo sefo2)
{
        bool    eq;

        sstNext("sefoEqualMod", sefo1, sefo2);
        eq = sefoEqual0(mods, sefo1, sefo2);
        sstDoneSefo(sefo1);

        return eq;
}

bool
symeEqualMod(List<Syme> mods, Syme syme1, Syme syme2)
{
        bool    eq;

        sstNext("symeEqualMod", syme1, syme2);
        eq = symeEqual0(mods, syme1, syme2);
        sstDoneSyme(syme1);

        return eq;
}

bool
tformEqualMod(List<Syme> mods, TForm tf1, TForm tf2)
{
        bool    eq;

        sstNext("tformEqualMod", tf1, tf2);
        eq = tformEqual0(mods, tf1, tf2);
        sstDoneTForm(tf1);

        return eq;
}

bool
sefoListEqualMod(List<Syme> mods, List<Sefo> sefos1, List<Sefo> sefos2)
{
        bool    eq;

        sstNext("sefoListEqualMod", sefos1, sefos2);
        eq = sefoListEqual0(mods, sefos1, sefos2);
        sstDoneSefoList(sefos1);

        return eq;
}

bool
symeListEqualMod(List<Syme> mods, List<Syme> symes1, List<Syme> symes2)
{
        bool    eq;

        sstNext("symeListEqualMod", symes1, symes2);
        eq = symeListEqual0(mods, symes1, symes2);
        sstDoneSymeList(symes1);

        return eq;
}

bool
tformListEqualMod(List<Syme> mods, List<TForm> tforms1, List<TForm> tforms2)
{
        bool    eq;

        sstNext("tformListEqualMod", tforms1, tforms2);
        eq = tformListEqual0(mods, tforms1, tforms2);
        sstDoneTFormList(tforms1);

        return eq;
}

bool
symeEqualModConditions(List<Syme> mods, Syme syme1, Syme syme2)
{
        bool    eq;

        symeSetPopConds(syme1);
        symeSetPopConds(syme2);
        eq = symeEqualMod(mods, syme1, syme2);
        symeClrPopConds(syme2);
        symeClrPopConds(syme1);

        return eq;
}

/*
 * Local functions.
 */

#define                 symeModsAllowAlpha(mods)                \
        ((mods) && cdr(mods) && !symeIsSelf(car(mods)) &&       \
         symeId(car(mods)) != symeId(car(cdr(mods))))

local bool
sefoIsDefinedType(Sefo sefo)
{
        TForm   cat = abGetCategory(sefo);
        return tfIsDefineOfType(cat);
}

local Sefo
sefoDefinedVal(Sefo sefo)
{
        TForm   cat = abGetCategory(sefo);
        return tfExpr(tfDefineVal(cat));
}

local bool
sefoEqual0(List<Syme> mods, Sefo sefo1, Sefo sefo2)
{
        bool    result = true;
        Length  serial = 0;

        sstSerialDebug += 1;
        serial = sstSerialDebug;

        sefo1 = sefoEqualMods(sefo1);
        sefo2 = sefoEqualMods(sefo2);

        sefoEqualDEBUG({
                fprintf(dbOut, "-> sefoEqual[%d]:", (int) serial);
                fnewline(dbOut);
                sefoPrintDb(sefo1);
                sefoPrintDb(sefo2);
        });

        if (sefo1 == sefo2)
                result = true;

        else if (!sefo1 || !sefo2)
                result = false;

        else if (abTag(sefo1) != abTag(sefo2))
                result = false;

        else if (abArgc(sefo1) != abArgc(sefo2))
                result = false;

        else if (abIsId(sefo1))
                result = symeEqual0(mods, abSyme(sefo1), abSyme(sefo2));

        else if (abIsLeaf(sefo1))
                result = abEqual(sefo1, sefo2) &&
                         symeEqual0(mods, abSyme(sefo1), abSyme(sefo2));

        else {
                Length  i;
                for (i = 0; result && i < abArgc(sefo1); i += 1)
                        result = sefoEqual0(mods, abArgv(sefo1)[i],
                                            abArgv(sefo2)[i]);
        }

        if (!result) {
                if (sefoIsDefinedType(sefo1))
                        result = sefoEqual0(mods, sefoDefinedVal(sefo1), sefo2);

                else if (sefoIsDefinedType(sefo2))
                        result = sefoEqual0(mods, sefo1, sefoDefinedVal(sefo2));
        }

        sefoEqualDEBUG({
                fprintf(dbOut, "<- sefoEqual[%d] = %s",
                        (int) serial, boolToString(result));
                fnewline(dbOut);
        });

        return result;
}

local bool
symeEqual0(List<Syme> mods, Syme syme1, Syme syme2)
{
        bool    result = true;
        Length  serial = 0;

        sstSerialDebug += 1;
        serial = sstSerialDebug;

        if (syme1 == syme2)
                return true;

        else if (!syme1 || !syme2)
                return false;

        sefoEqualDEBUG({
                fprintf(dbOut, "-> symeEqual[%d]:", (int) serial);
                fnewline(dbOut);
                symePrintDb(syme1);
                symePrintDb(syme2);
        });

        if (symeIsArchive(syme1) && symeIsLibrary(syme2))
                result = arLibraryIsMember(symeArchive(syme1),
                                           symeLibrary(syme2));

        else if (symeIsArchive(syme2) && symeIsLibrary(syme1))
                result = arLibraryIsMember(symeArchive(syme2),
                                           symeLibrary(syme1));

        else if (symeId(syme1) != symeId(syme2)) {
                if (symeModsAllowAlpha(mods))
                        result = listMember<Syme>(mods, syme1, symeEq) &&
                                 listMember<Syme>(mods, syme2, symeEq);
                else
                        result = false;
        }

        else if (sstSymeIsMarked(syme1))
                result = true;

        else if (symeExtendEqual0(mods, syme1, syme2))
                result = true;

        else if (listMember<Syme>(mods, syme1, symeEq) &&
                 listMember<Syme>(mods, syme2, symeEq))
                return true;

        else if (symeIsSelf(syme1))
                return false;

        else if (symeIsSelfSelf(syme1)) {
                TForm   tf1;
                TForm   tf2;

                if (symeHash(syme1) && symeHash(syme2) &&
                    symeHash(syme1) != symeHash(syme2))
                        return false;

                tf1 = symeType(syme1);
                tf2 = symeType(syme2);

                tfFollow(tf1);
                tfFollow(tf2);

                assert (tfIsGeneral(tf1) && tfIsGeneral(tf2));
                result = (sefoListEqual0(mods, symeCondition(syme1),
                                         symeCondition(syme2)) &&
                          abEqualModDeclares(tfGetExpr(tf1), tfGetExpr(tf2)));
        }

        else if (symeIsTwin(syme1, syme2)) {
                if (symeIsImport(syme1) && symeIsImport(syme2))
                        result = symeOriginEqual0(mods, syme1, syme2);
                else
                        result = true;
        }

        else {
                sstMarkSyme(syme1);
                result = (symeOriginEqual0(mods, syme1, syme2) &&
                          sefoListEqual0(mods, symeCondition(syme1),
                                         symeCondition(syme2)) &&
                          symeTypeEqual0(mods, syme1, syme2));
        }

        sefoEqualDEBUG({
                fprintf(dbOut, "<- symeEqual[%d] = %s",
                        (int) serial, boolToString(result));
                fnewline(dbOut);
        });

        return result;
}


local bool tformStructSimilar(TForm tf1, TForm tf2);

local bool
tformEqual0(List<Syme> mods, TForm tf1, TForm tf2)
{
        bool    result = true;
        Length  serial = 0;
#if CheckSimilar
        bool    similar = true;
#endif
        tf1 = tfFollowOnly(tf1);
        tf2 = tfFollowOnly(tf2);


        if (tf1 == tf2) return true;

        /* Only use tfFollow if exactly one of the types is lazy. */
        if (tfIsSubst(tf1) != tfIsSubst(tf2)) {
                if (!tformStructSimilar(tf1, tf2)) {
#if CheckSimilar
                        fprintf(dbOut, "(Similar test failed:\n");
                        tfPrintDb(tf1);
                        tfPrintDb(tf2);
                        fprintf(dbOut, ")\n");
                        similar = false;
#else
                        return false;
#endif
                }
                tfFollow(tf1);
                tfFollow(tf2);
        }

        /* If we can determine equality w/o using tfFollow, do so. */
        if (tfIsSubst(tf1) || tfIsSubst(tf2)) {
                assert(tfIsSubst(tf1) && tfIsSubst(tf2));
                if (tfSubstSigma(tf1) == tfSubstSigma(tf2) &&
                    tformEqual0(mods, tfSubstArg(tf1), tfSubstArg(tf2)))
                        return true;
                tfFollow(tf1);
                tfFollow(tf2);
        }

        /* Only use tfDefineeType if both types are fully analyzed. */
        if (!tfIsSyntax(tf1) && !tfIsSyntax(tf2)) {
                tf1 = tfDefineeType(tf1);
                tf2 = tfDefineeType(tf2);
        }

        /* The tfDefineeType of a type might also be a syntax type. */
        if (tfIsSyntax(tf1) || tfIsSyntax(tf2))
                return abEqualModDeclares(tfExpr(tf1), tfExpr(tf2));

        sstSerialDebug += 1;
        serial = sstSerialDebug;

        sefoEqualDEBUG({
                fprintf(dbOut, "-> tformEqual[%d]:", (int) serial);
                fnewline(dbOut);
                tformPrintDb(tf1);
                tformPrintDb(tf2);
        });

        if (tf1 == tf2)
                result = true;

        else if ((tfIsLibrary(tf1) || tfIsArchive(tf1)) &&
                 (tfIsLibrary(tf2) || tfIsArchive(tf2)))
                result = true;

        else if ((tfTag(tf1) != tfTag(tf2)) || (tfArgc(tf1) != tfArgc(tf2)))
                result = false;

        else if (tfIsSym(tf1))
                result = true;

        else if (tfTag(tf1) == TF_General)
                result = sefoEqual0(mods, tfGetExpr(tf1), tfGetExpr(tf2));

        else if (tfIsRecord(tf1) || tfIsRawRecord(tf1) ||
                 tfIsUnion(tf1) || tfIsEnum(tf1)) {
                Length  i;
                for (i = 0; result && i < tfArgc(tf1); i += 1) {
                        Symbol  sym1 = tfCompoundId(tf1, i);
                        Symbol  sym2 = tfCompoundId(tf2, i);
                        if (!sym1 || !sym2 || sym1 == sym2)
                                result = tformEqual0(mods, tfArgv(tf1)[i],
                                                     tfArgv(tf2)[i]);
                        else
                                result = false;
                }
        }
        /* ??? Do we need to special-case TrailingArray */
        else if (tfIsNode(tf1)) {
                Length  i;
                for (i = 0; result && i < tfArgc(tf1); i += 1)
                        result = tformEqual0(mods, tfArgv(tf1)[i],
                                             tfArgv(tf2)[i]);
        }
        else
                bugBadCase(tfTag(tf1));

        if (result && tfIsWith(tf1) &&
            (tfUseCatExports(tf1) || tfUseCatExports(tf2)))
                result = symeListEqual0(mods, tfParents(tf1),
                                        tfParents(tf2));

        if (result && tfIsThird(tf1) &&
            (tfUseThdExports(tf1) || tfUseThdExports(tf2)))
                result = symeListEqual0(mods, tfGetThdExports(tf1),
                                        tfGetThdExports(tf2));

        if (result && tformEqualCheckSymes(tf1) && tformEqualCheckSymes(tf2))
                result = symeListEqual0(mods, tfSymes(tf1), tfSymes(tf2));

        if (!result) {
                if (tfIsDefinedType(tf1))
                        result = tformEqual0(mods, tfDefinedVal(tf1), tf2);

                else if (tfIsDefinedType(tf2))
                        result = tformEqual0(mods, tf1, tfDefinedVal(tf2));
        }

        sefoEqualDEBUG({
                fprintf(dbOut, "<- tformEqual[%d] = %s",
                        (int) serial, boolToString(result));
                fnewline(dbOut);
        });
#if CheckSimilar
        if (result && !similar)
                bug("tformEqual, dissimilar, but the same!");
#endif
        return result;
}

local bool
tformStructSimilar(TForm tf1, TForm tf2)
{
        int i;
        /* We use the fact that only absyn can be replaced
         * by substitution
         */
        tf1 = tfFollowSubst(tf1);
        tf2 = tfFollowSubst(tf2);

        if (tfIsDeclare(tf1))
                tf1 = tfDeclareType(tf1);
        if (tfIsDeclare(tf2))
                tf2 = tfDeclareType(tf2);

        /* These may change into something real */
        if (tfIsGeneral(tf1) || tfIsGeneral(tf2))
                return true;

        if (tfIsSyntax(tf1) || tfIsSyntax(tf2))
                return true;
        /* TF_Define is downright bizzare, and can be whatever it
         * feels like.
         */
        if (tfIsDefine(tf1) || tfIsDefine(tf2))
                return true;

        if (tfTag(tf1) != tfTag(tf2))
                return false;
        if (tfArgc(tf1) != tfArgc(tf2))
                return false;

        for (i=0; i<tfArgc(tf1); i++)
                if (!tformStructSimilar(tfArgv(tf1)[i], tfArgv(tf2)[i]))
                    return false;

        return true;
}

local bool
sefoListEqual0(List<Syme> mods, List<Sefo> sefos1, List<Sefo> sefos2)
{
        /* Quick check for empty and identical lists. */
        if (sefos1 == sefos2)
                return true;

        if (listLength<Sefo>(sefos1) != listLength<Sefo>(sefos2))
                return false;

        for (; sefos1 && sefos2; sefos1 = cdr(sefos1), sefos2 = cdr(sefos2))
                if (!sefoEqual0(mods, car(sefos1), car(sefos2)))
                        return false;

        return true;
}

local bool
symeListEqual0(List<Syme> mods, List<Syme> symes1, List<Syme> symes2)
{
        /* Quick check for empty and identical lists. */
        if (symes1 == symes2)
                return true;

        if (listLength<Syme>(symes1) != listLength<Syme>(symes2))
                return false;

        for (; symes1 && symes2; symes1 = cdr(symes1), symes2 = cdr(symes2))
                if (!symeEqual0(mods, car(symes1), car(symes2)))
                        return false;

        return true;
}

local bool
tformListEqual0(List<Syme> mods, List<TForm> tl1, List<TForm> tl2)
{
        /* Quick check for empty and identical lists. */
        if (tl1 == tl2)
                return true;

        if (listLength<TForm>(tl1) != listLength<TForm>(tl2))
                return false;

        for (; tl1 && tl2; tl1 = cdr(tl1), tl2 = cdr(tl2))
                if (!tformEqual0(mods, car(tl1), car(tl2)))
                        return false;

        return true;
}

local bool
symeTypeEqual0(List<Syme> mods, Syme syme1, Syme syme2)
{
        if ((symeIsLazy(syme1) && symeIsImport(syme1)) ||
            (symeIsLazy(syme2) && symeIsImport(syme2)))
                return symeTypeCode(syme1) == symeTypeCode(syme2);
        else if (tfIsTrigger(syme1->type) || tfIsTrigger(syme2->type))
                return symeTypeCode(syme1) == symeTypeCode(syme2);
        else
                return tformEqual0(mods, symeType(syme1), symeType(syme2));
}

local bool
symeExtendEqual0(List<Syme> mods, Syme syme1, Syme syme2)
{
        Syme            syme, ext, e1, e2;
        List<Syme>        symes;

        /* Check the extension, if any. */

        e1 = symeExtension(syme1);
        e2 = symeExtension(syme2);

        if (e1 || e2) {
                bool    result;
                Syme    s1, s2;

                symeSetExtension(syme1, NULL);
                symeSetExtension(syme2, NULL);

                s1 = e1 ? e1 : syme1;
                s2 = e2 ? e2 : syme2;
                result = symeEqual0(mods, s1, s2);

                symeSetExtension(syme1, e1);
                symeSetExtension(syme2, e2);

                return result;
        }

        /* Check the extendees, if any. */

        if (symeIsImport(syme1) && symeIsImport(syme2))
                return false;

        if (symeIsImportOfExtend(syme1)) syme1 = symeOriginal(syme1);
        if (symeIsImportOfExtend(syme2)) syme2 = symeOriginal(syme2);

        if (symeIsExtend(syme1)) {
                ext = syme1;
                syme = syme2;
        }
        else if (symeIsExtend(syme2)) {
                ext = syme2;
                syme = syme1;
        }
        else
                return false;

        for (symes = symeExtendee(ext); symes; symes = cdr(symes))
                if (symeEqual0(mods, car(symes), syme))
                        return true;

        return false;
}

local bool
symeOriginEqual0(List<Syme> mods, Syme syme1, Syme syme2)
{
        bool    result = true;
        Length  serial = 0;

        sstSerialDebug += 1;
        serial = sstSerialDebug;

        sefoEqualDEBUG({
                fprintf(dbOut, "-> symeOriginEqual[%d]:", (int) serial);
                fnewline(dbOut);
                symePrintDb(syme1);
                symePrintDb(syme2);
        });

        if (symeKind(syme1) != symeKind(syme2))
                result = false;

        else if (symeOrigin(syme1) == symeOrigin(syme2))
                result = true;

        else switch (symeKind(syme1)) {
        case SYME_Builtin:
                result = (symeBuiltin(syme1) == symeBuiltin(syme2));
                break;
        case SYME_Foreign:
                result = forgEqual(symeForeign(syme1), symeForeign(syme2));
                break;
        case SYME_Import:
                result = tformEqual0(mods, symeExporter(syme1),
                                     symeExporter(syme2));
                break;
        case SYME_Library:
                result = libEqual(symeLibrary(syme1), symeLibrary(syme2));
                break;
        case SYME_Archive:
                result = arEqual(symeArchive(syme1), symeArchive(syme2));
                break;
        case SYME_Export:
        case SYME_Extend:
                result = true;
                break;
        default:
                bugBadCase(symeKind(syme1));
        }

        sefoEqualDEBUG({
                fprintf(dbOut, "<- symeOriginEqual[%d] = %s",
                        (int) serial, boolToString(result));
                fnewline(dbOut);
        });

        return result;
}

local bool
tformEqualCheckSymes(TForm tf)
{
        switch (tfTag(tf)) {
        case TF_Declare:
        case TF_Cross:
        case TF_Map:
        case TF_PackedMap:
        case TF_Multiple:
        case TF_Add:
        case TF_Third:
                return true;
        default:
                if (tfIsLibrary(tf) || tfIsArchive(tf))
                        return true;
                return false;
        }
}

/*
 * Equality preserving functions for sstEqual.
 */

local Sefo
sefoEqualMods(Sefo sefo)
{
        bool    changed = (sefo != NULL);

        while (changed)
                switch (abTag(sefo)) {
                case AB_PretendTo:
                        sefo = sefo->abPretendTo.expr;
                        break;
                case AB_RestrictTo:
                        sefo = sefo->abRestrictTo.expr;
                        break;
                case AB_Qualify:
                        sefo = sefo->abQualify.what;
                        break;
                case AB_Declare:
                        sefo = sefo->abDeclare.type;
                        break;
                default:
                        changed = false;
                        break;
                }

        return sefo;
}

/*****************************************************************************
 *
 * :: sefoCopy
 *
 ****************************************************************************/

Sefo
sefoCopy(Sefo sefo)
{
        Sefo    result = abCopy(sefo);
        abTransferSemantics(sefo, result);
        return result;
}

/*****************************************************************************
 *
 * :: sefoAudit
 *
 ****************************************************************************/

bool
sefoAudit(bool verbose, Sefo sefo)
{
        int     i;
        bool    ok = true;

        if (abIsLeaf(sefo) && !abSyme(sefo)) {
                if (verbose) {
                        MutString expr = abPretty(sefo);
                        fprintf(dbOut, "No symbol meaning for leaf: %s", expr);
                        fnewline(dbOut);
                        strFree(expr);
                }
                ok = false;
        }
        if (!abIsLeaf(sefo))
                for (i = 0; i < abArgc(sefo); i++)
                        ok &= sefoAudit(verbose, abArgv(sefo)[i]);
        return ok;
}

/******************************************************************************
 *
 * :: sstFreeVars
 *
 *****************************************************************************/

/*
 * sstFreeVars entry points.
 */

void
sefoFreeVars(Sefo sefo)
{
        if (abIsLeaf(sefo)) {
                if (abSyme(sefo))
                        symeFreeVars(abSyme(sefo));
        }
        else {
                Length  i;
                for (i = 0; i < abArgc(sefo); i += 1)
                        sefoFreeVars(abArgv(sefo)[i]);

                if (abState(sefo) == AB_State_HasUnique)
                        tformFreeVars(abTUnique(sefo));
        }
}

void
symeFreeVars(Syme syme)
{
        if (symeIsImport(syme))
                tformFreeVars(symeExporter(syme));
        else if (!symeIsSelf(syme))
                tformFreeVars(symeType(syme));

        if (symeCondition(syme))
                sefoListFreeVars(symeCondition(syme));
}

void
tformFreeVars(TForm tf)
{
        TForm           ancestor;
        FreeVar         fv;

        tf = tfFollowOnly(tf);
        if (tfIsPending(tf) && tfIsSyntax(tf))
                return;

        if (tfFVars(tf))
                return;

        sfvInitTable();
        sfvPushTable();
        ancestor = tf;

        sstNext("tformFreeVars", &ancestor, tf);
        tformFreeVars0(&ancestor, NULL, tf);
        sstDoneTForm(tf);

        fv = sfvPopTable(true);
        sfvFiniTable();

        tfSetFVars(tf, fv);
}

void
tqualFreeVars(TQual tqual)
{
        tformFreeVars(tqBase(tqual));
        tformListFreeVars(tqQual(tqual));
}

void
abSubFreeVars(AbSub sigma)
{
        TForm           ancestor = tfNone();
        FreeVar         fv;

        if (absFVars(sigma))
                return;

        sfvInitTable();
        sfvPushTable();

        sstNext("abSubFreeVars", &ancestor, sigma);
        abSubFreeVars0(&ancestor, ancestor, sigma);
        sstDoneAbSub(sigma);

        fv = sfvPopTable(true);
        sfvFiniTable();

        absSetFVars(sigma, fv);
}

void
sefoListFreeVars(List<Sefo> sefos)
{
        for (; sefos; sefos = cdr(sefos))
                sefoFreeVars(car(sefos));
}

void
symeListFreeVars(List<Syme> symes)
{
        for (; symes; symes = cdr(symes))
                symeFreeVars(car(symes));
}

void
tformListFreeVars(List<TForm> tforms)
{
        for (; tforms; tforms = cdr(tforms))
                tformFreeVars(car(tforms));
}

void
tqualListFreeVars(List<TQual> tquals)
{
        for (; tquals; tquals = cdr(tquals))
                tqualFreeVars(car(tquals));
}

/*
 * sstFreeVars table stack functions.
 */

static List<List<Syme>>     sfvTableList            = listNil<List<Syme>>;
static List<AInt>         sfvBase                 = listNil<AInt>;
static AInt             sfvDepth                = 0;

#define         sfvHasDepth(l,n)        ((l) != listNil<AInt> && car(l) == (n))
#define         sfvGetDepths(sy)        symeDepths(sy)
#define         sfvSetDepths(sy,l)      symeSetDepths(sy,l)
#define         sfvIsBase(n)            ((n) == car(sfvBase))

local void
sfvInitTable(void)
{
        sfvBase = listCons<AInt>(sfvDepth, sfvBase);
}

local void
sfvFiniTable(void)
{
        sfvBase = listFreeCons<AInt>(sfvBase);
}

local void
sfvPushTable(void)
{
        List<Syme>        tbl = listNil<Syme>;
        sfvTableList = listCons<List<Syme>>(tbl, sfvTableList);
        sfvDepth += 1;
}

local void
sfvConsTable(Syme syme)
{
        List<Syme>        tbl = car(sfvTableList);
        listPush(Syme, syme, tbl);
        setcar(sfvTableList, tbl);
}

local FreeVar
sfvPopTable(bool save)
{
        List<Syme>        tbl = car(sfvTableList);
        List<Syme>        symes, fsymes;
        int             odepth = sfvDepth;

        sfvTableList = listFreeCons<List<Syme>>(sfvTableList);
        sfvDepth -= 1;

        fsymes = listNil<Syme>;
        for (symes = tbl; symes; symes = cdr(symes)) {
                Syme            syme = car(symes);
                List<AInt>      elt = sfvGetDepths(syme);

                if (!sfvHasDepth(elt, odepth)) continue;

                if (sfvIsBase(sfvDepth)) {
                        elt = listFreeCons<AInt>(elt);
                        sfvSetDepths(syme, elt);
                }
                else if (sfvHasDepth(cdr(elt), sfvDepth)) {
                        elt = listFreeCons<AInt>(elt);
                        sfvSetDepths(syme, elt);
                }
                else {
                        setcar(elt, sfvDepth);
                        sfvConsTable(syme);
                }

                if (save) fsymes = listCons<Syme>(syme, fsymes);
        }

        listFree<Syme>(tbl);

        return save ? FreeVar::fromSymes(fsymes) : FreeVar::empty();
}

local void
sfvAddSyme(Syme syme)
{
        List<AInt>      elt = sfvGetDepths(syme);
        if (!sfvHasDepth(elt, sfvDepth)) {
                elt = listCons<AInt>(sfvDepth, elt);
                sfvSetDepths(syme, elt);
                sfvConsTable(syme);
        }
}

local void
sfvAddSymes(List<Syme> symes)
{
        for (; symes; symes = cdr(symes))
                if (symeIsSubstable(car(symes)))
                        sfvAddSyme(car(symes));
}

local void
sfvDelSyme(Syme syme)
{
        List<AInt>      elt = sfvGetDepths(syme);
        if (sfvHasDepth(elt, sfvDepth)) {
                elt = listFreeCons<AInt>(elt);
                sfvSetDepths(syme, elt);
        }
}

local void
sfvDelSymes(List<Syme> symes)
{
        for (; symes; symes = cdr(symes))
                if (symeIsSubstable(car(symes)))
                        sfvDelSyme(car(symes));
}

local void
sfvDelStab(Stab stab)
{
        sfvDelSymes(stabGetBoundSymes(stab));
}

local bool
sfvIsAncestor(TForm ancestor, TForm tf)
{
        while (tf) {
                if (tf == ancestor) return true;
                tf = tf->parent;
        }
        return false;
}

local TForm
sfvCommonAncestor(TForm tf1, TForm tf2)
{
        while (tf1) {
                if (sfvIsAncestor(tf1, tf2)) return tf1;
                tf1 = tf1->parent;
        }
        assert(false);
        return NULL;
}

/*
 * sstFreeVars local functions.
 */

local void
sefoFreeVars0(TForm *pa, TForm parent, Sefo sefo)
{
        Length  serial = 0;

        sstSerialDebug += 1;
        serial = sstSerialDebug;

        sefoFreeDEBUG({
                fprintf(dbOut, "-> sefoFree[%d]:\n", (int) serial);
                sefoPrintDb(sefo);
        });

        if (abIsLeaf(sefo)) {
                Syme    syme = abSyme(sefo);

                /*!! assert(syme); */
                if (!syme) return;

                if (symeIsSubstable(syme))
                        sfvAddSyme(syme);
                symeUnboundVars0(pa, parent, syme);
        }
        else {
                Length  i;
                for (i = 0; i < abArgc(sefo); i += 1)
                        sefoUnboundVars0(pa, parent, abArgv(sefo)[i]);

                if (abState(sefo) == AB_State_HasUnique)
                        tformUnboundVars0(pa, parent, abTUnique(sefo));
        }

        sefoFreeDEBUG({
                fprintf(dbOut, "<- sefoFree[%d]: %p\n", (int) serial, *pa);
        });
}

local void
symeFreeVars0(TForm *pa, TForm parent, Syme syme)
{
        Length  serial = 0;

        sstSerialDebug += 1;
        serial = sstSerialDebug;

        sefoFreeDEBUG({
                fprintf(dbOut, "-> symeFree[%d]:\n", (int) serial);
                symePrintDb(syme);
        });

        if (symeIsImport(syme))
                tformUnboundVars0(pa, parent, symeExporter(syme));
        else if (!symeIsSelf(syme) &&
                 symeDefLevelIsSubstable(syme))
                tformUnboundVars0(pa, parent, symeType(syme));

        if (symeCondition(syme))
                sefoListUnboundVars0(pa, parent, symeCondition(syme));

        sefoFreeDEBUG({
                fprintf(dbOut, "<- symeFree[%d]: %p\n", (int) serial, *pa);
        });
}

local void
tformFreeVars0(TForm *pa, TForm parent, TForm tf)
{
        TForm           ancestor;
        Length          serial = 0;
        bool            cascade = false;
        List<Syme>        symes;

        assert(tf);
        tf = tfFollowOnly(tf);
        if (tfIsDefine(tf) && tfIsWith(tfDefineVal(tf)))
                tf = tfDefineeType(tf);

        if (tfIsSubst(tf) && tfFVars(tf).isNull()) {
                TForm   arg = tfSubstArg(tf);
                if (sstTFormIsMarked(arg)) {
                        *pa = sfvCommonAncestor(*pa, arg);
                        return;
                }
                tformFreeVars(arg);
                tfSetFVars(tf, freeVarSubst0(tfSubstSigma(tf), tfFVars(arg)));
        }

        if (tfFVars(tf)) {
                sfvAddSymes(tfFVars(tf).symes());
                return;
        }

        if (sstTFormIsMarked(tf)) {
                *pa = sfvCommonAncestor(*pa, tf);
                return;
        }
        sstMarkTForm(tf);

        sstSerialDebug += 1;
        serial = sstSerialDebug;

        sefoFreeDEBUG({
                fprintf(dbOut, "-> tformFree[%d]:\n", (int) serial);
                tformPrintDb(tf);
        });

        sfvPushTable();

        tf->parent = parent;
        ancestor = parent = tf;

        if (tfIsSym(tf) || tfIsSyntax(tf))
                ;
        else if (tfIsLibrary(tf) || tfIsArchive(tf))
                ;
        else if (tfIsAbSyn(tf))
                sefoUnboundVars0(&ancestor, parent, tfGetExpr(tf));
        else if (tfIsWith(tf) && (symes = tfGetCatParents(tf, true)) != NULL) {
                tfGetCatSelf(tf);
                symeListUnboundVars0(&ancestor, parent, symes);
                tqualListFreeVars0(&ancestor, parent, tfGetCatCascades(tf));
                cascade = true;
        }
        else if (tfIsThird(tf) && (symes = tfGetThdParents(tf)) != NULL) {
                tfGetThdSelf(tf);
                symeListUnboundVars0(&ancestor, parent, symes);
                tqualListFreeVars0(&ancestor, parent, tfGetThdCascades(tf));
                cascade = true;
        }
        else if (tfIsNode(tf)) {
                Length  i;
                for (i = 0; i < tfArgc(tf); i += 1)
                        tformUnboundVars0(&ancestor, parent, tfArgv(tf)[i]);
        }
        else
                bugBadCase(tfTag(tf));

        if (tfSymes(tf)) {
                if (!(tfIsLibrary(tf) || tfIsArchive(tf)))
                        symeListUnboundVars0(&ancestor, parent, tfSymes(tf));
                if (tfIsDeclare(tf))
                        sfvAddSymes(tfSymes(tf));
                else
                        sfvDelSymes(tfSymes(tf));
        }
        if (tfQueries(tf))
                tformListUnboundVars0(&ancestor, parent, tfQueries(tf));
        if (!cascade && tfCascades(tf))
                tqualListUnboundVars0(&ancestor, parent, tfCascades(tf));

        if (tfStab(tf))
                sfvDelStab(tfStab(tf));
        if (tfSelf(tf))
                sfvDelSymes(tfSelf(tf));
        if (tfSelfSelf(tf))
                sfvDelSymes(tfSelfSelf(tf));

        if (ancestor == tf && tfIsMeaning(tf))
                tfSetFVars(tf, sfvPopTable(true));
        else {
                sfvPopTable(false);
                *pa = ancestor;
        }

        sefoFreeDEBUG({
                fprintf(dbOut, "<- tformFree[%d]: %p\n", (int) serial, *pa);
        });
}

local void
tqualFreeVars0(TForm *pa, TForm parent, TQual tq)
{
        tformUnboundVars0(pa, parent, tqBase(tq));
        if (tqIsQualified(tq))
                tformListUnboundVars0(pa, parent, tqQual(tq));
}

local void
abSubFreeVars0(TForm *pa, TForm parent, AbSub sigma)
{
        List<AbBind>      l0;
        for (l0 = sigma->l; l0; l0 = cdr(l0))
                sefoUnboundVars0(pa, parent, car(l0)->val);
}

local void
sefoListFreeVars0(TForm *pa, TForm parent, List<Sefo> sefos)
{
        for (; sefos; sefos = cdr(sefos))
                sefoUnboundVars0(pa, parent, car(sefos));
}

local void
symeListFreeVars0(TForm *pa, TForm parent, List<Syme> symes)
{
        for (; symes; symes = cdr(symes))
                symeUnboundVars0(pa, parent, car(symes));
}

local void
tformListFreeVars0(TForm *pa, TForm parent, List<TForm> tforms)
{
        for (; tforms; tforms = cdr(tforms))
                tformUnboundVars0(pa, parent, car(tforms));
}

local void
tqualListFreeVars0(TForm *pa, TForm parent, List<TQual> tquals)
{
        for (; tquals; tquals = cdr(tquals))
                tqualUnboundVars0(pa, parent, car(tquals));
}

local void
sefoUnboundVars0(TForm *pa, TForm parent, Sefo sefo)
{
        sefoFreeVars0(pa, parent, sefo);
        if (abStab(sefo))
                sfvDelStab(abStab(sefo));
}

local void
symeUnboundVars0(TForm *pa, TForm parent, Syme syme)
{
        symeFreeVars0(pa, parent, syme);
}

local void
tformUnboundVars0(TForm *pa, TForm parent, TForm tf)
{
        tfGetStab(tf);
        tformFreeVars0(pa, parent, tf);
}

local void
tqualUnboundVars0(TForm *pa, TForm parent, TQual tq)
{
        tqualFreeVars0(pa, parent, tq);
}

local void
sefoListUnboundVars0(TForm *pa, TForm parent, List<Sefo> sefos)
{
        sefoListFreeVars0(pa, parent, sefos);
}

local void
symeListUnboundVars0(TForm *pa, TForm parent, List<Syme> symes)
{
        symeListFreeVars0(pa, parent, symes);
}

local void
tformListUnboundVars0(TForm *pa, TForm parent, List<TForm> tforms)
{
        tformListFreeVars0(pa, parent, tforms);
}

local void
tqualListUnboundVars0(TForm *pa, TForm parent, List<TQual> tquals)
{
        tqualListFreeVars0(pa, parent, tquals);
}

/*****************************************************************************
 *
 * :: sstSubst
 *
 ****************************************************************************/

/*
 * symeListSubst external entry points.
 */

AbSub
absFrSymes(Stab stab, List<Syme> symes, Sefo sefo)
{
        AbSub           sigma = absNew(stab);

        if (symes && symeIsSelf(car(symes))) absSetSelf(sigma);
        for (; symes; symes = cdr(symes))
                sigma = absExtend(car(symes), sefo, sigma);

        return sigma;
}

List<Syme>
symeListSubstSelf(Stab stab, TForm tf, List<Syme> osymes)
{
        AbSub           sigma = absFrSymes(stab, tfGetDomSelf(tf), tfExpr(tf));
        List<Syme>        nsymes, symes;

        nsymes = symeListSubstSigma(sigma, osymes);

        for (symes = osymes; symes; symes = cdr(symes)) {
                Syme    syme = car(symes);
                Syme    nsyme = absGetSyme(sigma, syme);
                symeAddTwin(nsyme, syme);

                /* Now mutate the syme kind. */
                symeSetKind(nsyme, SYME_Import);
                symeSetExporter(nsyme, tf);

                if (tfIsLazyExporter(tf)) {
                        symeSetLib(nsyme, symeLib(syme));
                        symeSetHash(nsyme, symeHash(syme));
                }
                else if (!symeIsLazy(nsyme))
                        symeSetHash(nsyme, (Hash) 0);
        }

        /* Modifies nsymes indirectly via sigma ... */
        symeListSubstUseConstants(sigma, tfGetDomSelf(tf), osymes,
                                  tfGetDomConstants(tf));
        absFree(sigma);

        if (tfCascades(tf)) {
                /* Use a fresh sigma to avoid getting stomped-on symes. */
                sigma = absFrSymes(stab, tfGetDomSelf(tf), tfExpr(tf));
                tfCascades(tf) = tqualListSubst(sigma, tfCascades(tf));
                absFree(sigma);
        }

        return nsymes;
}

List<Syme>
symeListSubstCat(Stab stab, List<Syme> mods, TForm cat, List<Syme> osymes)
{
        AbSub           sigma;
        List<Syme>        nsymes, symes;

        if (!mods) return listCopy<Syme>(osymes);

        sigma = absFrSymes(stab, cdr(mods), abFrSyme(car(mods)));

        nsymes = symeListSubstSigma(sigma, osymes);

        for (symes = osymes; symes; symes = cdr(symes)) {
                Syme    syme = car(symes);
                Syme    nsyme = absGetSyme(sigma, syme);
                symeAddTwin(nsyme, syme);
        }

        symeListSubstUseConstants(sigma, mods, osymes, tfGetCatConstants(cat));

        absFree(sigma);
        return nsymes;
}

List<Syme>
symeListSubstSigma(AbSub sigma, List<Syme> osymes)
{
        List<Syme>        symes;

        if (absIsEmpty(sigma)) {
                for (symes = listNil<Syme>; osymes; osymes = cdr(osymes)) {
                        Syme    syme = car(osymes);
                        Syme    nsyme = symeCopy(syme);
                        symes = listCons<Syme>(nsyme, symes);
                        absSetSyme(sigma, syme, nsyme);
                        absSetStab(sigma, nsyme);
                }
                return listNReverse<Syme>(symes);
        }

        symeListFreeVars(osymes);
        abSubFreeVars(sigma);

        /* Mark the symes so tformWillSubst can find them. */
        /* Mark the twins so that after the subst they become ==. */
        for (symes = osymes; symes; symes = cdr(symes)) {
                Syme    syme = car(symes);
                absMarkSyme(sigma, syme);
                symeListMarkTwins(sigma, syme, syme);
        }

        /* Copy the symes that symeSubst doesn't need to. */
        for (symes = osymes; symes; symes = cdr(symes)) {
                Syme    syme = car(symes), nsyme;

                if (!symeWillSubst(sigma, syme)) {
                        nsyme = symeCopy(syme);
                        absSetSyme(sigma, syme, nsyme);
                        absSetStab(sigma, nsyme);
                }
        }

        /* Now symeSubst0 will use the symes we copied, and copy the rest. */
        return symeListSubst0(sigma, osymes);
}

TForm
tformSubstSigma(AbSub sigma, TForm tf)
{
        List<AbBind>      l0;

        if (absIsEmpty(sigma)) return tf;

        tformFreeVars(tf);
        abSubFreeVars(sigma);

        /* Mark the symes in sigma which have symes as substituands. */
        for (l0 = sigma->l; l0; l0 = cdr(l0)) {
                Syme    syme = car(l0)->key;
                AbSyn   sefo = car(l0)->val;
                if (abSyme(sefo))
                        absSetSyme(sigma, syme, abSyme(sefo));
        }

        /* Now tformSubst0 will use the symes we marked. */
        return tformSubst0(sigma, tf);
}

local void
symeListMarkTwins(AbSub sigma, Syme final, Syme twin)
{
        List<Syme>        symes;

        for (symes = symeTwins(twin); symes; symes = cdr(symes)) {
                Syme    syme = car(symes);
                absSetSyme(sigma, syme, final);
                symeListMarkTwins(sigma, final, syme);
        }
}

local bool
sefoDependsOnRuntimeValue(AbSub sigma, Sefo sefo)
{
        if (abIsLeaf(sefo)) {
                Syme    syme = abSyme(sefo);

                if (syme) {
                        Sefo replacement = absLookup(syme, NULL, sigma);

                        if (replacement &&
                            (!abIsLeaf(replacement) ||
                             abSyme(replacement) != syme))
                                return sefoDependsOnRuntimeValue(sigma,
                                                                 replacement);
                }

                return syme &&
                        (symeIsParam(syme) || symeIsLexVar(syme) ||
                         symeIsSelf(syme));
        }

        for (Length i = 0; i < abArgc(sefo); i += 1)
                if (sefoDependsOnRuntimeValue(sigma, abArgv(sefo)[i]))
                        return true;

        return false;
}

local Syme
sefoHeadSyme(AbSub sigma, Sefo sefo)
{
        if (abIsApply(sefo))
                return sefoHeadSyme(sigma, abApplyOp(sefo));

        if (abIsLeaf(sefo)) {
                Syme    syme = abSyme(sefo);
                Sefo    replacement;

                if (!syme)
                        return NULL;

                replacement = absLookup(syme, NULL, sigma);
                if (replacement &&
                    (!abIsLeaf(replacement) || abSyme(replacement) != syme))
                        return sefoHeadSyme(sigma, replacement);

                return syme;
        }

        return NULL;
}

local bool
symeImplOwnedByExporter(AbSub sigma, Syme nsyme, Syme rsyme)
{
        TForm   exporter = symeExporter(nsyme);
        Syme    owner;
        Lib     ownerLib, implLib;

        if (!tfIsGeneral(exporter))
                return false;

        owner = sefoHeadSyme(sigma, tfExpr(exporter));
        if (!owner || symeIsParam(owner) || symeIsLexVar(owner) ||
            symeIsSelf(owner))
                return false;

        ownerLib = symeConstLib(owner);
        implLib  = symeConstLib(rsyme);

        return ownerLib && implLib && libEqual(ownerLib, implLib);
}

local void
symeListSubstUseConstants(AbSub sigma, List<Syme> mods, List<Syme> osymes,
                          List<Syme> rsymes)
{
        for (; rsymes && osymes; osymes = cdr(osymes)) {
                Syme            osyme = car(osymes);
                Syme            nsyme = absGetSyme(sigma, osyme);
                List<Syme>        symes;
                bool            runtimeExporter = false;

                /*
                 * A copied category export may retain implementation
                 * metadata from one concrete domain even though its
                 * declaration ancestry is shared by every implementation of
                 * that category.  Such metadata is not valid for an import
                 * through a domain whose value is only known at run time.
                 * At an actual non-default match below, reject foreign
                 * implementation metadata unless it is owned by the exporter
                 * head constructor.  Category defaults remain transferable.
                 */
                if (!genIsRuntime() && symeIsImport(nsyme)) {
                        TForm exporter = symeExporter(nsyme);

                        runtimeExporter =
                                tfIsGeneral(exporter) &&
                                sefoDependsOnRuntimeValue(sigma, tfExpr(exporter));
                }

                for (symes = rsymes; symes; symes = cdr(symes)) {
                        Syme    rsyme = car(symes);
                        if (symeId(osyme) == symeId(rsyme) &&
                            tformEqualMod(mods, symeType(osyme),
                                          symeType(rsyme))) {
                                if (runtimeExporter &&
                                    !symeHasDefault(rsyme) &&
                                    !symeImplOwnedByExporter(sigma, nsyme,
                                                             rsyme)) {
                                        symeClearImplInfo(nsyme);
                                        continue;
                                }

                                symeTransferImplInfo(nsyme, rsyme);
                                if (symeIsImport(nsyme))
                                        symeClrDefault(nsyme);
                        }
                }
        }
}

/*
 * sstSubst external entry points.
 */

Sefo
sefoSubst(AbSub sigma, Sefo sefo)
{
        if (absIsEmpty(sigma)) return sefo;
        sefoFreeVars(sefo);
        abSubFreeVars(sigma);

        return sefoSubst0(sigma, sefo);
}

Syme
symeSubst(AbSub sigma, Syme syme)
{
        if (absIsEmpty(sigma)) return syme;
        symeFreeVars(syme);
        abSubFreeVars(sigma);

        return symeSubst0(sigma, syme);
}

TForm
tformSubst(AbSub sigma, TForm tf)
{
        if (absIsEmpty(sigma)) return tf;
        if (!tfIsPending(tf)) tformFreeVars(tf);
        abSubFreeVars(sigma);

        return tformSubst0(sigma, tf);
}

List<Syme>
symeListSubst(AbSub sigma, List<Syme> symes)
{
        if (absIsEmpty(sigma)) return symes;
        symeListFreeVars(symes);
        abSubFreeVars(sigma);

        return symeListSubst0(sigma, symes);
}

local List<TQual>
tqualListSubst(AbSub sigma, List<TQual> tquals)
{
        if (absIsEmpty(sigma)) return tquals;
        tqualListFreeVars(tquals);
        abSubFreeVars(sigma);

        return tqualListSubst0(sigma, tquals);
}

/*
 * The result from sefoSubst0 does not share Sefo nodes with sefo.
 */
local Sefo
sefoSubst0(AbSub sigma, Sefo sefo)
{
        Sefo    final;
        extern  bool stabDumbImport(void);

        assert(sefo);

        sefoSubstDEBUG({
                fprintf(dbOut, "-> sefoSubst[%ld]:\n", absSerial(sigma));
                sefoPrintDb(sefo);
        });

        if (abIsLeaf(sefo)) {
                Syme    syme = abSyme(sefo);

                /*!! assert(syme); */
                if (!syme)
                        return sefoCopy(sefo);

                final = absLookup(syme, sefo, sigma);
                /* If absLookup fails, walk the syme. */
                if (final == sefo) {
                        final = sefoCopy(final);
                        abSetSyme(final, symeSubst0(sigma, syme));
                }
                else
                        final = sefoCopy(final);
        }
        else {
                Length  i;
                final = abNewEmpty(abTag(sefo), abArgc(sefo));
                for (i = 0; i < abArgc(sefo); i++)
                        abArgv(final)[i] = sefoSubst0(sigma, abArgv(sefo)[i]);

#ifdef SefoSubstTUnique
                /*!! This may have weaker semantics than we need.
                 *!! Instead of creating the semantics on the resulting form,
                 *!! which may yield more information, we are just
                 *!! using the semantics on the unsubstituted form.
                 */
                if (abState(sefo) == AB_State_HasUnique) {
                        abSetState(final, abState(sefo));
                        abTUnique(final) = tformSubst0(sigma,abTUnique(sefo));
                }


                /* Copy semantics if they have some and we don't */
                if (abTForm(sefo) && !abTForm(final) && !stabDumbImport()) {
                        /*
                         * Really not sure about this: without it we fail
                         * to process conditional imports correctly after
                         * calling tfDefineeBaseType() in stabImportFrom().
                         * One might expect that tformSubst0() ought to be
                         * used but this induces bad mutual recursion with
                         * this function. At least this way seems to work.
                         */
                        abSetTForm(final, abTForm(sefo));
                }
#else
                if (abState(sefo) == AB_State_HasUnique)
                        abSetState(final, AB_State_AbSyn);
#endif
        }

        sefoSubstDEBUG({
                fprintf(dbOut, "<- sefoSubst[%ld]:\n", absSerial(sigma));
                sefoPrintDb(final);
        });

        return final;
}

local bool
tformSubstSkipCategory(TForm tf)
{
        if (tfIsAnyMap(tf)) tf = tfMapRet(tf);
        tf = tfDefineeTypeSubst(tf);

        /*!! The first if clause is really not quite right. */
        if (tfIsType(tf) || tfIsTypeSyntax(tf) || tfIsSyntax(tf))
                return true;

        if (tfIsCategory(tf) || tfIsCategorySyntax(tf) || tfIsThird(tf))
                return true;

        return false;
}

local bool
tformSubstSkipDomain(TForm tf)
{
        if (tfIsAnyMap(tf)) tf = tfMapRet(tf);
        tf = tfDefineeTypeSubst(tf);

        /*!! The first if clause is really not quite right. */
        if (tfIsType(tf) || tfIsTypeSyntax(tf) || tfIsSyntax(tf))
                return true;

        if (tfIsWith(tf) || tfIsWithSyntax(tf) || tfIsIf(tf) ||
            tfIsJoin(tf) || tfIsMeet(tf))
                return true;

        if (tfIsCategory(tf) || tfIsCategorySyntax(tf) || tfIsThird(tf))
                return true;

        if (tfIsId(tf) && tfIdSyme(tf) &&
            tformSubstSkipCategory(symeType(tfIdSyme(tf))))
                return true;

        return false;
}

local Syme
symeSubst0(AbSub sigma, Syme syme)
{
        Syme    final;
        bool    marked;

        assert(syme);

        final = absGetSyme(sigma, syme);
        marked = absSymeIsMarked(sigma, syme);
        if (final && !marked) {
                if (symeIsTwin(final, syme))
                        return absSetSyme(sigma, syme,
                                          symeSubst0(sigma, final));
                return final;
        }

        if (!symeWillSubst(sigma, syme))
                return absSetSyme(sigma, syme, syme);

        if (symeWillPush(syme)) symeType(syme);
        final = absSetSyme(sigma, syme, symeCopy(syme));
        absSetStab(sigma, final);

        sefoSubstDEBUG({
                fprintf(dbOut, "-> symeSubst[%ld]:\n", absSerial(sigma));
                symePrintDb(syme);
        });

        if (symeIsImport(syme)) {
                symeSetExporter(final, tformSubst0(sigma, symeExporter(syme)));
#define SefoSubstImportType
#ifdef SefoSubstImportType
                if (tformWillSubst(sigma, symeType(syme)))
                        symeSetType(final, tformSubst0(sigma, symeType(syme)));
#endif
        }

        else if (absSelf(sigma) && !symeIsSelf(syme) && !marked &&
                 tformSubstSkipDomain(symeType(syme)))
                ;

        else if (!symeIsSelf(syme) && symeDefLevelIsSubstable(syme))
                symeSetType(final, tformSubst0(sigma, symeType(syme)));

        if (symeCondition(syme))
                symeSetCondition(final,
                                 condListSubst0(sigma, symeCondition(syme)));

        sefoSubstDEBUG({
                fprintf(dbOut, "<- symeSubst[%ld]: ", absSerial(sigma));
                symePrintDb(final);
        });

        return final;
}

local TForm
tformSubst0(AbSub sigma, TForm tf)
{
        TForm           final;
        bool            lazy = absLazy(sigma);
        bool            fresh = true;
        bool            cascade = false;
        AbSyn           abnew;
        Length          i;
        List<Syme>        symes;

        assert(tf);
        absSetLazy(sigma);

        tf = tfFollowOnly(tf);
        if (tfIsDefine(tf) && tfIsWith(tfDefineVal(tf)))
                tf = tfDefineeType(tf);

        final = absGetTForm(sigma, tf);
        if (final && (!tfIsSubstOf(tf, final) || lazy)) return final;

        sefoSubstDEBUG({
                fprintf(dbOut, "-> tformSubst[%ld]:\n", absSerial(sigma));
                tformPrintDb(tf);
        });

        if (!lazy) tfFollow(tf);

        if (tfFVars(tf) && !tfFVars(tf).hasAbSub(sigma)) {
                final = absSetTForm(sigma, tf, tf);
                fresh = false;
        }
        else if (tfIsSym(tf)) {
                final = absSetTForm(sigma, tf, tf);
                fresh = false;
        }
        else if (tfIsPending(tf)) {
                /*!! May need to print an error message here. */
                final = absSetTForm(sigma, tf, tf);
                fresh = false;
        }
        else if (tfIsLibrary(tf) || tfIsArchive(tf)) {
                final = absSetTForm(sigma, tf, tf);
                fresh = false;
        }
        else if (lazy) {
                final = tfSubst(sigma, tf);
                fresh = false;
        }
        else if (tfIsAbSyn(tf)) {
                TForm   tfnew;

                final = absSetTForm(sigma, tf, tfNewEmpty(TF_Forward, 1));
                final->argv[0] = tf;

                abnew = sefoSubst0(sigma, tfGetExpr(tf));
#if SefoSubstShare
                tfnew = tfFullFrAbSyn(absStab(sigma), abnew);
#else
                tfnew = tfMeaning(absStab(sigma), abnew,
                                  tfPending(absStab(sigma), abnew));
#endif
                fresh = (tfGetExpr(tfnew) == abnew);

                if (fresh) tfOwnExpr(tfnew);

                final->argv[0] = tfnew;
                final = tfFollowOnly(final);
        }
        else if (tfIsWith(tf)) {
                final = tfNewEmpty(tfTag(tf), tfArgc(tf));
                final = absSetTForm(sigma, tf, final);
                tfArgv(final)[0] = tfNone();
                tfArgv(final)[1] = tfNone();
                symes = tfGetCatParents(tf, true);
                if (symes) {
                        tfGetCatSelf(tf);
                        tfParents(final) = parentListSubst0(sigma, symes);
                        if (tfCatExports(tf))
                                tfCatExports(final) =
                                        listCopy<Syme>(tfParents(final));
                        tfCascades(final) =
                                tqualListSubst0(sigma, tfGetCatCascades(tf));
                        tfHasCascades(final) = true;
                        cascade = true;
                        tfAuditExportList(tfParents(final));
                }
                else {
                        for (i = 0; i < tfArgc(tf); i += 1)
                                tfArgv(final)[i] =
                                        tformSubst0(sigma, tfArgv(tf)[i]);
                }
                tfSetMeaning(final);
        }
        else if (tfIsThird(tf)) {
                final = tfNewEmpty(tfTag(tf), tfArgc(tf));
                final = absSetTForm(sigma, tf, final);
                tfArgv(final)[0] = tfNone();
                symes = tfGetThdParents(tf);
                if (symes) {
                        tfGetThdSelf(tf);
                        tfParents(final) = parentListSubst0(sigma, symes);
                        tfCascades(final) =
                                tqualListSubst0(sigma, tfGetCatCascades(tf));
                        tfHasCascades(final) = true;
                        cascade = true;
                        tfAuditExportList(tfParents(final));
                }
                else {
                        for (i = 0; i < tfArgc(tf); i += 1)
                                tfArgv(final)[i] =
                                        tformSubst0(sigma, tfArgv(tf)[i]);
                }
                tfSetMeaning(final);
        }
        else if (tfIsRecord(tf) && tfRecordArgc(tf) == 1) {
                TForm   tf0, tfnew;
                Length  argc;

                final = absSetTForm(sigma, tf, tfNewEmpty(TF_Forward, 1));
                final->argv[0] = tf;

                tf0 = tformSubst0(sigma, tfArgv(tf)[0]);
                argc = tfAsMultiArgc(tf0);
                tfnew = tfNewEmpty(tfTag(tf), argc);
                for (i = 0; i < argc; i += 1)
                        tfArgv(tfnew)[i] = tfAsMultiArgN(tf0, argc, i);
                tfGetSymes(absStab(sigma), tfnew, tfExpr(tfnew));
                tfSetMeaning(tfnew);

                final->argv[0] = tfnew;
                final = tfFollowOnly(final);
        }
        else if (tfIsRawRecord(tf) && tfRawRecordArgc(tf) == 1) {
                /*
                 * Don't know if we need this or not.
                 * Don't know if having it will break things.
                 */
                TForm   tf0, tfnew;
                Length  argc;

                final = absSetTForm(sigma, tf, tfNewEmpty(TF_Forward, 1));
                final->argv[0] = tf;

                tf0 = tformSubst0(sigma, tfArgv(tf)[0]);
                argc = tfAsMultiArgc(tf0);
                tfnew = tfNewEmpty(tfTag(tf), argc);
                for (i = 0; i < argc; i += 1)
                        tfArgv(tfnew)[i] = tfAsMultiArgN(tf0, argc, i);
                tfGetSymes(absStab(sigma), tfnew, tfExpr(tfnew));
                tfSetMeaning(tfnew);

                final->argv[0] = tfnew;
                final = tfFollowOnly(final);
        }
        else if (tfIsNode(tf)) {
                final = tfNewEmpty(tfTag(tf), tfArgc(tf));
                final = absSetTForm(sigma, tf, final);
                for (i = 0; i < tfArgc(tf); i += 1)
                        tfArgv(final)[i] =
                                tformSubst0(sigma, tfArgv(tf)[i]);
                tfSetMeaning(final);
        }
        else {
                bugBadCase(tfTag(tf));
        }

        if (!fresh) {
                sefoSubstDEBUG({
                        fprintf(dbOut, "<- tformSubst[%ld]: ",
                                absSerial(sigma));
                        tformPrintDb(final);
                });
                return final;
        }

        if (tfSymes(tf) && !tfSymes(final))
                tfSetSymes(final, symeListSubst0(sigma, tfSymes(tf)));

        if (tfQueries(tf))
                tfSetQueries(final,
                             tformListSubst0(sigma, tfQueries(tf)));
        if (!cascade && tfCascades(tf)) {
                tfSetCascades(final,
                              tqualListSubst0(sigma, tfCascades(tf)));
                tfHasCascades(final) = true;
        }

        if (tfGetStab(tf))
                tfSetStab(final, tfStab(tf));

        if (tfSelf(tf)) {
                List<Syme> ssymes;
                ssymes = listCopy<Syme>(tfSelf(tf));
                tfSetSelf(final, ssymes);
                tfHasSelf(final) = tfHasSelf(tf);
        }
        if (tfFVars(tf))
                tfSetFVars(final, freeVarSubst0(sigma, tfFVars(tf)));
        if (tfHasExpr(final)) tfSetNeedsSefo(final);

        sefoSubstDEBUG({
                fprintf(dbOut, "<- tformSubst[%ld]: ", absSerial(sigma));
                tformPrintDb(final);
        });

        return final;
}

local TQual
tqualSubst0(AbSub sigma, TQual tq)
{
        return tqNewFrList(tformSubst0(sigma, tqBase(tq)),
                           tformListSubst0(sigma, tqQual(tq)));
}

local FreeVar
freeVarSubst0(AbSub sigma, FreeVar fv)
{
        FreeVar         fv0 = absFVars(sigma);
        List<Syme>        symes, final;

        if (fv.symes() == listNil<Syme>)
                return fv0;

        final = listReverse<Syme>(fv0.symes());
        for (symes = fv.symes(); symes; symes = cdr(symes)) {
                Syme    syme = car(symes);
                if (absLookup(syme, NULL, sigma))
                        continue;
                syme = symeSubst0(sigma, syme);
                if (!fv0.hasSyme(syme))
                        final = listCons<Syme>(syme, final);
        }
        final = listNReverse<Syme>(final);
        return FreeVar::fromSymes(final);
}

/* Make sure that the symes are not going to be stomped on. */
local List<Syme>
parentListSubst0(AbSub sigma, List<Syme> symes)
{
        if (absSelf(sigma))
                return listCopy<Syme>(symes);
        else
                return symeListSubst0(sigma, symes);
}

local List<Sefo>
condListSubst0(AbSub sigma, List<Sefo> conds)
{
        bool            self  = absSelf(sigma);
        List<Sefo>        final = listNil<Sefo>;

        for (; conds; conds = cdr(conds)) {
                Sefo    cond = car(conds);
                Sefo    expr, prop;

                if (abHasTag(cond, AB_Has)) {
                        expr = car(conds)->abHas.expr;
                        prop = car(conds)->abHas.property;

                        expr = sefoSubst0(sigma, expr);
                        prop = self ? sefoCopy(prop) : sefoSubst(sigma, prop);

                        cond = abNewHas(SrcPos::none, expr, prop);
                }
                else
                        cond = sefoSubst0(sigma, cond);

                final = listCons<Sefo>(cond, final);
        }

        return listNReverse<Sefo>(final);
}

local List<Syme>
symeListSubst0(AbSub sigma, List<Syme> symes)
{
        List<Syme>        final = listNil<Syme>;

        for (; symes; symes = cdr(symes))
                final = listCons<Syme>(symeSubst0(sigma, car(symes)),
                                        final);

        return listNReverse<Syme>(final);
}

local List<TForm>
tformListSubst0(AbSub sigma, List<TForm> tforms)
{
        List<TForm>       final = listNil<TForm>;

        for (; tforms; tforms = cdr(tforms))
                final = listCons<TForm>(tformSubst0(sigma, car(tforms)),
                                         final);

        return listNReverse<TForm>(final);
}

local List<TQual>
tqualListSubst0(AbSub sigma, List<TQual> tquals)
{
        List<TQual>       final = listNil<TQual>;

        for (; tquals; tquals = cdr(tquals))
                final = listCons<TQual>(tqualSubst0(sigma, car(tquals)),
                                         final);

        return listNReverse<TQual>(final);
}

/* Return true if symeSubst0 must explicitly push a lazy syme. */
local bool
symeWillPush(Syme syme)
{
        bool    result = false;

        if (symeIsLazy(syme)) {
                switch (symeKind(syme)) {
                case SYME_Export:
                case SYME_Param:
                        if (symeDefLevelIsSubstable(syme))
                                result = true;
                        break;
                case SYME_Import: {
                        TForm   exporter = symeExporter(syme);
                        if (!tfIsLazyExporter(exporter))
                                result = true;
                        break;
                }
                default:
                        break;
                }
        }

        return result;
}

/* Return true if symeSubst0 will create a new syme. */
local bool
symeWillSubst(AbSub sigma, Syme syme)
{
        assert(syme);

        if (symeCondition(syme))
                return true;
        else if (symeIsImport(syme) &&
                 tformWillSubst(sigma, symeExporter(syme)))
                return true;
        else if (!symeIsImport(syme) &&
                 absSelf(sigma) && !symeIsSelf(syme) &&
                 !absSymeIsMarked(sigma, syme) &&
                 tformSubstSkipDomain(symeType(syme)))
                return false;
        else if (!symeIsImport(syme) &&
                 !symeIsSelf(syme) &&
                 symeDefLevelIsSubstable(syme) &&
                 tformWillSubst(sigma, symeType(syme)))
                return true;
        else
                return false;
}

/* Return true if tformSubst0 will create a new tform. */
local bool
tformWillSubst(AbSub sigma, TForm tf)
{
        TForm   final;

        assert(tf);

        tf = tfFollowOnly(tf);

        final = absGetTForm(sigma, tf);
        if (final && (!tfIsSubstOf(tf, final) || absLazy(sigma)))
                return tf != final;

        if (tfFVars(tf) && !tfFVars(tf).hasAbSub(sigma))
                return false;

        else if (tfIsSym(tf))
                return false;

        else if (tfIsLibrary(tf) || tfIsArchive(tf))
                return false;

        else if (tfIsPending(tf))
                return false;

        else
                return true;
}

/*****************************************************************************
 *
 * :: symeListClosure
 *
 ****************************************************************************/

local void      sefoClosureReleaseType  (Sefo);
/*
 * External entry points.
 */

bool
symeCloseOverDetails(Syme syme)
{
        if (symeIsLibrary(syme) || symeIsArchive(syme))
                return false;

        if (symeLib(syme) == NULL)
                return true;

        if (symeConstLib(syme) == NULL)
                return true;

        if (symeIsExport(syme)
            && symeConstLib(symeOriginal(syme)) != symeConstLib(syme)
            && symeConstNum(syme) != SYME_NUMBER_UNASSIGNED)
                return true;

        if (symeIsExtend(syme) && syme->lib != NULL)
                return false;

        if (symeIsExport(syme) || symeIsParam(syme) || symeIsSelf(syme))
                return false;

        if (symeIsImport(syme)) {
                TForm   exporter = symeExporter(syme);
                if (tfIsLazyExporter(exporter))
                        return false;
        }

        return true;
}

void
symeListClosure(Lib lib, List<Syme> symes)
{
        List<Syme>        sl0, sl;

        sefoCloseDEBUG({
                printf("symeListClosure: %d symes", (int) listLength<Syme>(symes));
                fnewline(dbOut);
                symeListPrintDb(symes);
        });

        lib->topc  = 0;
        lib->symec = 0;
        lib->symes = 0;
        lib->symev = 0;

        lib->typec = 0;
        lib->types = 0;
        lib->typev = 0;

        /* Collect the symes and types, putting the original list first. */
        sstNext("symeListClosure", lib, symes);
        for (sl = symes; sl; sl = cdr(sl))
                slcAddSyme(lib, car(sl));
        sl0 = lib->symes;
        for (sl = sl0; sl; sl = cdr(sl))
                symeClosure1(lib, car(sl));
        sstDoneSymeList(sl0);

        /* Cache the symes/types in the lib. */
        lib->symes = listNReverse<Syme>(lib->symes);
        lib->types = listNReverse<TForm>(lib->types);

        sefoCloseDEBUG({
                printf(" -> %d symes\n", (int) listLength<Syme>(lib->symes));
        });
}

/*
 * Local functions.
 */

local void
slcAddSyme(Lib lib, Syme syme)
{
        if (sstSymeIsMarked(syme))
                return;

        sstMarkSyme(syme);
        lib->symes = listCons<Syme>(syme, lib->symes);
        lib->symec += 1;
        if (libSymeIsTop(lib, syme)) lib->topc += 1;

        sefoCloseDEBUG({
                fprintf(dbOut, "+syme:");
                fnewline(dbOut);
                symePrintDb(syme);
        });
}

local void
slcAddType(Lib lib, TForm tf)
{
        lib->types = listCons<TForm>(tf, lib->types);
        lib->typec += 1;

        sefoCloseDEBUG({
                fprintf(dbOut, "+type:");
                fnewline(dbOut);
                tformPrintDb(tf);
        });
}

local void
sefoClosure0(Lib lib, Sefo sefo)
{
        Length  serial = 0;

        assert(sefo);

        sstSerialDebug += 1;
        serial = sstSerialDebug;

        sefoCloseDEBUG({
                fprintf(dbOut, "(S ");
                sefoPrintDb(sefo);
        });

        if (abIsLeaf(sefo)) {
                if (abSyme(sefo))
                        symeClosure0(lib, abSyme(sefo));
                if (abImplicit(sefo))
                        sefoClosure0(lib, abImplicit(sefo));
        }
        else {
                Length  i;
                for (i = 0; i < abArgc(sefo); i += 1)
                        sefoClosure0(lib, abArgv(sefo)[i]);
                if (abState(sefo) == AB_State_HasUnique) {
#ifdef Bug1075
                        abSetState(sefo, AB_State_AbSyn);
#endif
                        sefoClosureReleaseType(sefo);
                }
        }

        sefoCloseDEBUG({
                fprintf(dbOut, " S)");
                sefoPrintDb(sefo);
        });
}

local void
sefoClosureReleaseType(Sefo sefo)
{
        abTUnique(sefo) = tfUnknown;
}

/*
 * Close using the given syme as a root, adding it to the lib.
 */
local void
symeClosure0(Lib lib, Syme syme)
{
        if (sstSymeIsMarked(syme))
                return;

        slcAddSyme(lib, syme);
        symeClosure1(lib, syme);
}

/*
 * Close using the given syme as a root, even though it is marked already.
 */
local void
symeClosure1(Lib lib, Syme syme)
{
        Length          serial = 0;
        Syme            extension = NULL;
        List<Syme>        extendee = NULL, symes;
        bool            details = symeCloseOverDetails(syme);

        sstSerialDebug += 1;
        serial = sstSerialDebug;

        sefoCloseDEBUG({
                fprintf(dbOut, "([%d]:\t", (int) serial);
                symePrintDb(syme);
                if (sstSerialDebug % 25 == 0) fprintf(dbOut, "\n");
        });

        assert(syme);
        if (symeIsExtend(syme)) {
                if (symeExtension(syme))
                        symeClosure0(lib, symeExtension(syme));
                extension = syme;
                extendee = symeExtendee(extension);
        }
        else if (symeExtension(syme)) {
                extension = symeExtension(syme);
                if (symeIsExtend(extension))
                        extendee = symeExtendee(extension);
        }
        for (symes = extendee; details && symes; symes = cdr(symes))
                slcAddSyme(lib, car(symes));
        symeListSetExtension(extendee, NULL);
        symeSetExtension(syme, NULL);

        if (symeLib(syme))
                symeClosure0(lib, libLibrarySyme(symeLib(syme)));
        if (symeConstLib(syme))
                symeClosure0(lib, libLibrarySyme(symeConstLib(syme)));

        if (symeIsImport(syme))
                tformClosure0(lib, symeExporter(syme));
        else if (details == false)
                ;
        else if (symeIsSelf(syme))
                tformClosure0(lib, tfType);
        else {
                TForm   tf = symeType(syme);
                tformClosure0(lib, tf);
                if (symeTop(syme) && !tfIsAnyMap(tf) &&
                    tfFVars(tf).count() > 1)
                        tfFVars(tf).setSkipParam();
        }

        if (details) {
                sefoListClosure0(lib, symeCondition(syme));
                symeListClosure0(lib, symeTwins(syme));

                if (!symeInlined(syme))
                        symeSetInlined(syme, genGetSymeInlined(syme));

                if (symeInlined(syme))
                        symeListClosure0(lib, symeInlined(syme));

                if (extension)
                        symeClosure0(lib, extension);
                for (symes = extendee; symes; symes = cdr(symes))
                        symeClosure1(lib, car(symes));
        }

        symeListSetExtension(extendee, extension);

        sefoCloseDEBUG({
                fprintf(dbOut, " [%d])", (int) serial);
                symePrintDb(syme);
        });

        return;
}

local void
tformClosure0(Lib lib, TForm tf)
{
        Length  serial = 0;
        bool    cascade = false;

        assert(tf);

        tfFollow(tf);

        if (sstTFormIsMarked(tf))
                return;
        sstMarkTForm(tf);

        sstSerialDebug += 1;
        serial = sstSerialDebug;

        sefoCloseDEBUG({
                fprintf(dbOut, "(T ");
                tformPrintDb(tf);
        });

        if (tfFVars(tf).isNull())
                tformFreeVars(tf);

        if (tfSatDom(tf)) {
                tfGetCatSelf(tf);
                tfGetCatSelfSelf(tf);
        }

        if (tfIsWith(tf) && tfUseCatExports(tf)) {
                symeListClosure0(lib, tfGetCatExports(tf));
                tqualListClosure0(lib, tfCascades(tf));
                cascade = true;
        }
        if (tfIsThird(tf) && tfUseThdExports(tf)) {
                symeListClosure0(lib, tfGetThdExports(tf));
                tqualListClosure0(lib, tfCascades(tf));
                cascade = true;
        }

        if (tfIsSym(tf))
                ;
        else if (tfIsAbSyn(tf)) {
                sefoClosure0(lib, tfGetExpr(tf));
                tfSetNeedsSefo(tf);
        }
        else if (tfIsNode(tf)) {
                Length  i;
                for (i = 0; i < tfArgc(tf); i += 1) {
                        tfFollow(tfArgv(tf)[i]);
                        tformClosure0(lib, tfArgv(tf)[i]);
                }
        }
        else
                bugBadCase(tfTag(tf));

        slcAddType(lib, tf);

        if (tfSymes(tf))
                symeListClosure0(lib, tfSymes(tf));
        if (tfSelf(tf))
                symeListClosure0(lib, tfSelf(tf));
        if (tfSelfSelf(tf))
                symeListClosure0(lib, tfSelfSelf(tf));
        if (tfQueries(tf))
                tformListClosure0(lib, tfQueries(tf));
        if (!cascade && tfCascades(tf))
                tqualListClosure0(lib, tfCascades(tf));

        sefoCloseDEBUG({
                fprintf(dbOut, " T)");
                tformPrintDb(tf);
        });
}

local void
tqualClosure0(Lib lib, TQual tqual)
{
        tfFollow(tqBase(tqual));
        tformClosure0(lib, tqBase(tqual));
        if (tqIsQualified(tqual))
                tformListClosure0(lib, tqQual(tqual));
}

local void
sefoListClosure0(Lib lib, List<Sefo> sefos)
{
        for ( ; sefos; sefos = cdr(sefos))
                sefoClosure0(lib, car(sefos));
}

local void
symeListClosure0(Lib lib, List<Syme> symes)
{
        for ( ; symes; symes = cdr(symes))
                symeClosure0(lib, car(symes));
}

local void
tformListClosure0(Lib lib, List<TForm> tforms)
{
        for ( ; tforms; tforms = cdr(tforms)) {
                tfFollow(car(tforms));
                tformClosure0(lib, car(tforms));
        }
}

local void
tqualListClosure0(Lib lib, List<TQual> tquals)
{
        for ( ; tquals; tquals = cdr(tquals))
                tqualClosure0(lib, car(tquals));
}

/*****************************************************************************
 *
 * :: symeList set operations
 *
 ****************************************************************************/

bool
symeListMember(Syme syme, List<Syme> symes, SymeEqFun eq)
{
        for (; symes; symes = cdr(symes)) {
                /* The extra '==' check saves a lot of funcalls on symeEq. */
                if (syme == car(symes))
                        return true;
                if (eq != symeEq && eq(syme, car(symes)))
                        return true;
        }
        return false;
}

/* Return true if each syme in symes1 is a member of symes2. */
bool
symeListSubset(List<Syme> symes1, List<Syme> symes2, SymeEqFun eq)
{
        for (; symes1; symes1 = cdr(symes1))
                if (!symeListMember(car(symes1), symes2, eq))
                        return false;
        return true;
}

List<Syme>
symeListUnion(List<Syme> symes1, List<Syme> symes2, SymeEqFun eq)
{
        List<Syme>        result = listNil<Syme>;

        sefoUnionDEBUG({
                fprintf(dbOut, "symeListUnion: \n");
                fprintf(dbOut, "    symes1: ");
                symeListPrintDb(symes1);
                fprintf(dbOut, "    symes2: ");
                symeListPrintDb(symes2);
        });

        for (; symes1; symes1 = cdr(symes1))
                if (!symeListMember(car(symes1), symes2, eq))
                        result = listCons<Syme>(car(symes1), result);

        result = listNConcat<Syme>(listNReverse<Syme>(result),
                                   listCopy<Syme>(symes2));

        sefoUnionDEBUG({
                fprintf(dbOut, "symeListUnion result: ");
                symeListPrintDb(result);
        });

        return result;
}

List<Syme>
symeListIntersect(List<Syme> symes1, List<Syme> symes2, SymeEqFun eq)
{
        List<Syme>        result = listNil<Syme>;

        sefoInterDEBUG({
                fprintf(dbOut, "symeListIntersect: \n");
                fprintf(dbOut, "    symes1: ");
                symeListPrintDb(symes1);
                fprintf(dbOut, "    symes2: ");
                symeListPrintDb(symes2);
        });

        for (; symes1; symes1 = cdr(symes1))
                if (symeListMember(car(symes1), symes2, eq))
                        result = listCons<Syme>(car(symes1), result);

        result = listNReverse<Syme>(result);

        sefoInterDEBUG({
                fprintf(dbOut, "symeListIntersect result: ");
                symeListPrintDb(result);
        });

        return result;
}

/*****************************************************************************
 *
 * :: sstToBuffer
 *
 ****************************************************************************/

int
sefoToBuffer(Lib lib, Buffer buf, Sefo sefo)
{
        Length          start = buf.position();
        AbSynTag        tag = abTag(sefo);
        UShort          i, argc;

        BUF_PUT_BYTE(buf, tag);

        switch (tag) {
        case AB_Nothing:
        case AB_Blank:
                break;

        case AB_IdSy:
                bugUnimpl("sefoToBuffer:  abIdSy");
                break;

        case AB_Id:
                assert(abSyme(sefo));
                symeToBuffer(lib, buf, abSyme(sefo));
                break;

        case AB_LitInteger:
        case AB_LitString:
        case AB_LitFloat:
                assert(abSyme(sefo));
                symeToBuffer(lib, buf, abSyme(sefo));
                buf.writeString(abLeafStr(sefo));
                break;

        case AB_Lambda:  /* Lies, damn lies, and leaving out the body */
                sefoToBuffer(lib, buf, sefo->abLambda.param);
                sefoToBuffer(lib, buf, sefo->abLambda.rtype);
                break;
        default:
                argc = abArgc(sefo);
                BUF_PUT_HINT(buf, argc);
                for (i = 0; i < argc; i += 1)
                        sefoToBuffer(lib, buf, abArgv(sefo)[i]);
        }

        return buf.position() - start;
}

int
symeToBuffer(Lib lib, Buffer buf, Syme syme)
{
        UShort  n = symeLibNum(syme);

        assert(libCheckSymeNumber(lib, syme, n));
        BUF_PUT_HINT(buf, n);

        return HINT_BYTES;
}

local int
tformToBuffer1(Lib lib, Buffer buf, TForm tf)
{
        ULong   n = tf->libNum;

        assert(libCheckTypeNumber(lib, tf, n));
        BUF_PUT_SINT(buf, n);

        return SINT_BYTES;
}

int
tformToBuffer(Lib lib, Buffer buf, TForm tf)
{
        Length          start = buf.position();
        TFormTag        tag = tfTag(tf);
        UByte           wireTag = (UByte) tag;
        UShort          i, argc;

        if (tfHasSelf(tf))      wireTag |= 0x80;
        if (tfHasCascades(tf))  wireTag |= 0x40;
        BUF_PUT_BYTE(buf, wireTag);
        tag = (TFormTag) (wireTag & 0x3f);

        if (tfIsSymTag(tag))
                ;

        else if (tfIsAbSynTag(tag))
                sefoToBuffer(lib, buf, tfGetExpr(tf));

        else if (tfIsNodeTag(tag)) {
                argc = tfArgc(tf);
                BUF_PUT_HINT(buf, argc);
                for (i = 0; i < argc; i += 1)
                        tformToBuffer1(lib, buf, tfArgv(tf)[i]);
        }

        else
                bugBadCase(tag);

        symeListToBuffer (lib, buf, tfSelf(tf));
        symeListToBuffer (lib, buf, tfSelfSelf(tf));
        tformListToBuffer(lib, buf, tfQueries(tf));
        tqualListToBuffer(lib, buf, tfCascades(tf));

        if (tfTagHasSymes(tfTag(tf)))
                symeListToBuffer(lib, buf, tfSymes(tf));

        if (tfIsWith(tf)) {
                if (tfUseCatExports(tf))
                        symeListToBuffer(lib, buf, tfGetCatExports(tf));
                else
                        symeListToBuffer(lib, buf, listNil<Syme>);
        }

        if (tfIsThird(tf)) {
                if (tfUseThdExports(tf))
                        symeListToBuffer(lib, buf, tfGetThdExports(tf));
                else
                        symeListToBuffer(lib, buf, listNil<Syme>);
        }

        if (tfFVars(tf).isNull())
                tformFreeVars(tf);

        freeVarToBuffer(lib, buf, tfFVars(tf));

        return buf.position() - start;
}

int
tqualToBuffer(Lib lib, Buffer buf, TQual tq)
{
        Length          start = buf.position();

        tformToBuffer1(lib, buf, tqBase(tq));
        tformListToBuffer(lib, buf, tqQual(tq));

        return buf.position() - start;
}

int
sefoListToBuffer(Lib lib, Buffer buf, List<Sefo> sefos)
{
        Length          start = buf.position();
        UShort          sefoc = listLength<Sefo>(sefos);

        BUF_PUT_HINT(buf, sefoc);

        for (; sefos; sefos = cdr(sefos))
                sefoToBuffer(lib, buf, car(sefos));

        return buf.position() - start;
}

int
symeListToBuffer(Lib lib, Buffer buf, List<Syme> symes)
{
        Length          start = buf.position();
        UShort          symec = listLength<Syme>(symes);

        BUF_PUT_HINT(buf, symec);

        for (; symes; symes = cdr(symes))
                symeToBuffer(lib, buf, car(symes));

        return buf.position() - start;
}

int
tformListToBuffer(Lib lib, Buffer buf, List<TForm> tforms)
{
        Length          start = buf.position();
        UShort          tformc = listLength<TForm>(tforms);

        BUF_PUT_HINT(buf, tformc);

        for (; tforms; tforms = cdr(tforms))
                tformToBuffer1(lib, buf, car(tforms));

        return buf.position() - start;
}

int
tqualListToBuffer(Lib lib, Buffer buf, List<TQual> tquals)
{
        Length          start = buf.position();
        UShort          tqualc = listLength<TQual>(tquals);

        BUF_PUT_HINT(buf, tqualc);
        for (; tquals; tquals = cdr(tquals))
                tqualToBuffer(lib, buf, car(tquals));

        return buf.position() - start;
}

local int
freeVarToBuffer(Lib lib, Buffer buf, FreeVar fv)
{
        Length          start = buf.position();
        UShort          symec;
        List<Syme>        symes;
        bool            skipParams = fv.skipParam();

        /* Count the number of symes to put. */
        for (symec = 0, symes = fv.symes(); symes; symes = cdr(symes)) {
                if (skipParams && symeIsParam(car(symes))) continue;
                if (libHasSyme(lib, car(symes)))
                        symec += 1;
        }

        BUF_PUT_HINT(buf, symec);

        for (symes = fv.symes(); symes; symes = cdr(symes)) {
                if (skipParams && symeIsParam(car(symes))) continue;
                if (libHasSyme(lib, car(symes)))
                        symeToBuffer(lib, buf, car(symes));
        }

        return buf.position() - start;
}

/*****************************************************************************
 *
 * :: sstFrBuffer
 *
 ****************************************************************************/

Sefo
sefoFrBuffer(Lib lib, Buffer buf)
{
        Sefo            sefo, sefo1;
        Syme            syme;
        AbSynTag        tag;
        UShort          i, argc;

        BUF_GET_BYTE(buf, tag);

        switch (tag) {
        case AB_Nothing:
                sefo = abNewNothing(SrcPos::none);
                break;
        case AB_Blank:
                sefo = abNewBlank(SrcPos::none, ssymVariable);
                break;
        case AB_IdSy:
                bugUnimpl("sefoFrBuffer:  abIdSy");
                NotReached(sefo = NULL);
                break;
        case AB_Id:
                syme = symeFrBuffer(lib, buf);
                sefo = abNewId(SrcPos::none, symeId(syme));
                abSetSyme(sefo, syme);
                break;
        case AB_LitInteger:
                syme = symeFrBuffer(lib, buf);
                sefo = abNewLitInteger(SrcPos::none, buf.readString());
                abSetSyme(sefo, syme);
                break;
        case AB_LitString:
                syme = symeFrBuffer(lib, buf);
                sefo = abNewLitString(SrcPos::none, buf.readString());
                abSetSyme(sefo, syme);
                break;
        case AB_LitFloat:
                syme = symeFrBuffer(lib, buf);
                sefo = abNewLitFloat(SrcPos::none, buf.readString());
                abSetSyme(sefo, syme);
                break;
        case AB_Lambda: {
                Sefo body;
                sefo1 = sefoFrBuffer(lib, buf);
                sefo  = sefoFrBuffer(lib, buf);
                body = abNewNever(SrcPos::none);
                abUse(body) = AB_Use_Elided;
                sefo  = abNewLambda(SrcPos::none, sefo1, sefo, body);
                }
                break;
        default:
                BUF_GET_HINT(buf, argc);
                sefo = abNewEmpty(tag, argc);
                for (i = 0; i < argc; i += 1)
                        abArgv(sefo)[i] = sefoFrBuffer(lib, buf);
        }

        return sefo;
}

Syme
symeFrBuffer(Lib lib, Buffer buf)
{
        UShort  n;

        BUF_GET_HINT(buf, n);
        assert(libCheckSymeNumber(lib, NULL, n));

        return lib->symev[n];
}

local TForm
tformFrBuffer1(Lib lib, Buffer buf)
{
        ULong   n;

        BUF_GET_SINT(buf, n);
        assert(libCheckTypeNumber(lib, NULL, n));

        /* The type forms are filled in later. */
        return (TForm) n;
}

TForm
tformFrBuffer(Lib lib, Buffer buf)
{
        TForm           tf;
        TFormTag        tag;
        UShort          i, argc;
        bool            hasSelf = false, hasCascades = false;

        {
                UByte wireTag;
                BUF_GET_BYTE(buf, wireTag);
                if (wireTag & 0x80) hasSelf = true;
                if (wireTag & 0x40) hasCascades = true;
                tag = (TFormTag) (wireTag & 0x3f);
        }

        if (tfIsSymTag(tag))
                switch (tag) {
                case TF_Unknown:
                        tf = tfUnknown;
                        break;
                case TF_Exit:
                        tf = tfExit;
                        break;
                case TF_Test:
                        tf = tfTest;
                        break;
                case TF_Literal:
                        tf = tfLiteral;
                        break;
                case TF_Type:
                        tf = tfType;
                        /* TTT */
                        return tf;
                        break;
                case TF_Category:
                        tf = tfCategory;
                        break;
                default:
                        bugBadCase(tag);
                        NotReached(tf = NULL);
                }

        else if (tfIsAbSynTag(tag)) {
                Sefo    sefo = sefoFrBuffer(lib, buf);
                Syme    syme = abSyme(sefo);

                if (syme && syme->kind == SYME_Library)
                        tf = tfGetLibrary(syme);
                else if (syme && syme->kind == SYME_Archive)
                        tf = tfGetArchive(syme);
                else {
                        tf = tfNewAbSyn(tag, sefo);
                        tf->intStepNo = 0;
                        tfOwnExpr(tf);
                }
        }

        else if (tfIsNodeTag(tag)) {
                BUF_GET_HINT(buf, argc);
                tf = tfNewEmpty(tag, argc);
                tf->intStepNo = 0;
                for (i = 0; i < argc; i += 1)
                        tfArgv(tf)[i] = tformFrBuffer1(lib, buf);
        }

        else {
                bugBadCase(tag);
                NotReached(tf = NULL);
        }

        tfSetSelf(tf, symeListFrBuffer(lib, buf));
        tfSetSelfSelf(tf, symeListFrBuffer(lib, buf));
        tfSetQueries(tf, tformListFrBuffer(lib, buf));
        tfSetCascades(tf, tqualListFrBuffer(lib, buf));

        tfHasSelf(tf) = hasSelf;
        tfHasCascades(tf) = hasCascades;

        if (tfTagHasSymes(tfTag(tf)))
                tfSetSymes(tf, symeListFrBuffer(lib, buf));

        if (tfIsWith(tf))
                tfCatExports(tf) = symeListFrBuffer(lib, buf);
        if (tfIsThird(tf))
                tfThdExports(tf) = symeListFrBuffer(lib, buf);

        tfSetFVars(tf, FreeVar::fromSymes(symeListFrBuffer(lib, buf)));

        return tf;
}

TQual
tqualFrBuffer(Lib lib, Buffer buf)
{
        TForm           base = tformFrBuffer1(lib, buf);
        List<TForm>       qual = tformListFrBuffer(lib, buf);

        return tqNewFrList(base, qual);
}

List<Sefo>
sefoListFrBuffer(Lib lib, Buffer buf)
{
        List<Sefo>        sefos = listNil<Sefo>;
        UShort          i, sefoc;

        BUF_GET_HINT(buf, sefoc);

        for (i = 0; i < sefoc; i += 1)
                sefos = listCons<Sefo>(sefoFrBuffer(lib, buf), sefos);

        return listNReverse<Sefo>(sefos);
}

List<Syme>
symeListFrBuffer(Lib lib, Buffer buf)
{
        List<Syme>        symes = listNil<Syme>;
        UShort          i, symec;

        BUF_GET_HINT(buf, symec);

        for (i = 0; i < symec; i += 1)
                symes = listCons<Syme>(symeFrBuffer(lib, buf), symes);

        return listNReverse<Syme>(symes);
}

List<TForm>
tformListFrBuffer(Lib lib, Buffer buf)
{
        List<TForm>       tforms = listNil<TForm>;
        UShort          i, tformc;

        BUF_GET_HINT(buf, tformc);

        for (i = 0; i < tformc; i += 1)
                tforms = listCons<TForm>(tformFrBuffer1(lib, buf), tforms);

        return listNReverse<TForm>(tforms);

}

List<TQual>
tqualListFrBuffer(Lib lib, Buffer buf)
{
        List<TQual>       tquals = listNil<TQual>;
        UShort          i, tqualc;

        BUF_GET_HINT(buf, tqualc);

        for (i = 0; i < tqualc; i += 1)
                tquals = listCons<TQual>(tqualFrBuffer(lib, buf), tquals);

        return listNReverse<TQual>(tquals);

}

/*****************************************************************************
 *
 * :: sstFrBuffer0
 *
 ****************************************************************************/

/*
 * Read the number of types from the type form section in a buffer.
 */
int
tformTypecFrBuffer(Buffer buf)
{
        ULong   argc;

        BUF_START(buf);
        BUF_GET_SINT(buf, argc);

        return argc;
}

/*
 * Compute the positions of the types from the type form section in a buffer.
 */
void
tformTypepFrBuffer(Buffer buf, int posc, int *posv)
{
        int     i;

        for (i = 0; i < posc; i += 1)
                posv[i] = 0;

        /* Skip over the number of type forms. */
        tformTypecFrBuffer(buf);

        for (i = 0; i < posc; i += 1) {
                posv[i] = buf.position();
                tformFrBuffer0(buf);
        }
}

local void
sefoFrBuffer0(Buffer buf)
{
        AbSynTag        tag;
        UShort          i, argc;
        ULong           cc;

        BUF_GET_BYTE(buf, tag);

        switch (tag) {
        case AB_Nothing:
        case AB_Blank:
        case AB_IdSy:
                break;

        case AB_Id:
                symeFrBuffer0(buf);
                break;

        case AB_LitInteger:
        case AB_LitString:
        case AB_LitFloat:
                symeFrBuffer0(buf);
                BUF_GET_SINT(buf, cc);
                buf.skip(cc);
                break;

        case AB_Lambda:
                sefoFrBuffer0(buf);
                sefoFrBuffer0(buf);
                break;

        default:
                BUF_GET_HINT(buf, argc);
                for (i = 0; i < argc; i += 1)
                        sefoFrBuffer0(buf);
                break;
        }
}

local void
symeFrBuffer0(Buffer buf)
{
        buf.skip(HINT_BYTES);
}

local void
tformFrBuffer0(Buffer buf)
{
        TFormTag        tag;

        {
                UByte wireTag;
                BUF_GET_BYTE(buf, wireTag);
                tag = (TFormTag) (wireTag & 0x3f);
        }

        if (tfIsSymTag(tag))
                ;

        else if (tfIsAbSynTag(tag))
                sefoFrBuffer0(buf);

        else if (tfIsNodeTag(tag))
                tformListFrBuffer0(buf);

        symeListFrBuffer0 (buf);        /* tfSelf(tf) */
        symeListFrBuffer0 (buf);        /* tfSelfSelf(tf) */
        tformListFrBuffer0(buf);        /* tfQueries(tf) */
        tqualListFrBuffer0(buf);        /* tfCascades(tf) */

        if (tfTagHasSymes(tag))
                symeListFrBuffer0(buf); /* tfSymes(tf) */

        if (tag == TF_With)
                symeListFrBuffer0(buf); /* tfCatExports(tf) */
        if (tag == TF_Third)
                symeListFrBuffer0(buf); /* tfThdExports(tf) */

        symeListFrBuffer0(buf);         /* tfFVars(tf).symes() */
}

local void
tqualFrBuffer0(Buffer buf)
{
        buf.skip(SINT_BYTES);
        tformListFrBuffer0(buf);
}

void
symeListFrBuffer0(Buffer buf)
{
        UShort          symec;

        BUF_GET_HINT(buf, symec);
        buf.skip(symec * HINT_BYTES);
}

local void
tformListFrBuffer0(Buffer buf)
{
        UShort          tformc;

        BUF_GET_HINT(buf, tformc);
        buf.skip(tformc * SINT_BYTES);
}

local void
tqualListFrBuffer0(Buffer buf)
{
        UShort          i, tqualc;

        BUF_GET_HINT(buf, tqualc);

        for (i = 0; i < tqualc; i += 1)
                tqualFrBuffer0(buf);
}
