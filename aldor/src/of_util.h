/*****************************************************************************
 *
 * of_util.h: Foam-to-foam optimizaion utililities.
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

#ifndef _OF_UTIL_H_
#define _OF_UTIL_H_

#include "axlobs.h"

typedef enum {
        INL_NotInlined,
        INL_BeingInlined,
        INL_Inlined
} InlineState;

typedef enum {
        DV_NotChecked,
        DV_Checked
} DeadVarState;

typedef enum usageState {
        DV_Unused,
        DV_DefinedNoSdEfx, 
        DV_DefinedSdEfx,
        DV_Keep,
        DV_Used
} UsageState;

typedef struct dvUsageStruct {
        UsageState      used;
        int             newIndex;
} *DvUsage;

struct foamBox {
        FoamTag         tag;
        FoamList        l;
        Length          argc;
        Foam            initial;
};


typedef struct varPool {
        FoamBox         fbox;
        AIntList        vars[FOAM_LIMIT];
} *VarPool;

struct optInfo {
        InlineState     inlState;
        DeadVarState    dvState; 
        Stab            stab;
        Syme            syme;
        int             constNum;
        bool            isGener;        /* Is generator? */
        Foam            prog;
        Foam            seq;
        FoamList        seqBody;
        VarPool         locals;
        int             numLabels;
        int             newLabel;
        Foam            denv;
        bool            changed;
        DvUsage         localUsage;

        FlowGraph       flog;
        PriQ            priq;           /* Priority queue for this prog */
        bool            converged;      /* Usedef info available        */
        UShort          numRefs;        /* how many progs call this     */
        unsigned        originalSize;   /* size before inlining         */
        unsigned        size;           /* size during inlining         */

        UShort          optMask;        /* Pending optimizations  */

};


# define optInfoRefs(opt)       (opt)->numRefs


/* Functions for Foam boxes. */
extern  FoamBox         fboxNewEmpty    (FoamTag);
extern  FoamBox         fboxNew         (Foam);
extern  void            fboxFree        (FoamBox);
extern  int             fboxAdd         (FoamBox, Foam);
extern  Foam            fboxMake        (FoamBox);
extern  Foam            fboxNth         (FoamBox, int);

/* Functions for temporary variable pools. */
extern  VarPool         vpNew           (FoamBox);
extern  void            vpFree          (VarPool);
extern  int             vpNewVarDecl    (VarPool, Foam);
extern  int             vpNewVar0       (VarPool, FoamTag, int);
extern  void            vpFreeVar       (VarPool, int);

# define vpNewVar(pool, type)  vpNewVar0((pool), (type), (emptyFormatSlot))
# define fboxSize(fbox)         ((fbox)->argc)

/*****************************************************************************
 *
 * :: Flags for -W runtime.
 *
 ****************************************************************************/

extern bool             gen0IsRuntime;

#define                 genIsRuntime()                  (gen0IsRuntime)
#define                 genSetRuntime()                 (gen0IsRuntime = true)

extern bool             inl0AfterInline;

#define                 inlAfterInline()                (inl0AfterInline)
#define                 inlSetAfterInline()             (inl0AfterInline=true)

/*****************************************************************************
 *
 * :: Flags for -W runtime-hashcheck
 *
 ****************************************************************************/

#if EDIT_1_0_n1_06
extern bool             gen0Hashcheck;

#define                 genHashcheck()                  (gen0Hashcheck)
#define                 genSetHashcheck()               (gen0Hashcheck = true)
#endif

/*****************************************************************************
 *
 * :: Foam Patching
 *
 ****************************************************************************/

void    fpPatchUnit             (Foam unit);

/*****************************************************************************
 *
 * :: Make Flat Sequences
 *
 ****************************************************************************/

Foam            utilMakeFlatSeq         (Foam);

/*****************************************************************************
 *
 * :: General Utility
 *
 ****************************************************************************/


#endif /* !_OF_UTIL_H_ */
