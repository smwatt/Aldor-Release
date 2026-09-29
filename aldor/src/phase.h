/*****************************************************************************
 *
 * phase.h: Phase statististics and reporting.
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

#ifndef _PHASE_H_
#define _PHASE_H_

# include "axlobs.h"

enum phTag {
        PH_START,
                PH_Load = PH_START,
                PH_Include,
                PH_Scan,
                PH_SysCmd,
                PH_Linear,
                PH_Parse,
                PH_MacEx,
                PH_AbNorm,
                PH_AbCheck,
                PH_ScoBind,
                PH_TInfer,
                PH_GenFoam,
                PH_OptFoam,
                PH_PutIntermed,
                PH_PutLisp,
                PH_PutC,
                PH_PutObject,
                PH_Misc,                /* Time spent outside of the others. */
        PH_LIMIT
};

struct phInfo {
        enum phTag      phno;
        String     abbrev;
        String     name;
        UShort          flags;          /* Control display of results, etc. */
        Millisec        time;           /* Time in phase            */
        ULong           alloc;          /* Bytes alloc in phase     */
        ULong           free;           /* Bytes freed in phase     */
        ULong           gc;             /* Bytes collected in phase */
};

typedef enum phTag     PhTag;
typedef void            (*PhPrFun)(FILE *, void *);

extern int      phTraceOption(MutString flags);
                                /*
                                 * flags  contains combination of "ardt+phases"
                                 * phases contains ph1+ph2+phN  or "all"
                                 * 0 on success, -1 on error
                                 */


extern void     phStartAll(bool verboseFlag);
                                /*
                                 * Initialize data structures
                                 */

extern PhTag    phNumber(MutString);
                                /*
                                 * Find a phase tag given the name
                                 */

extern bool     phIsStarter(PhTag pht);
                                /*
                                 * Find whether given phase shd be started.
                                 */

extern void     phStart(PhTag pht);
                                /*
                                 * Do set up for named phase.
                                 */

extern void     phEnd(PhPrFun, PhPrFun, void *);
                                /*
                                 * Declare that the given phase has ended
                                 * with the given result.
                                 */

extern void     phLibStats(FileName);
                                /*
                                 * Record library statistics.
                                 */

extern void     phEndAll(void);
                                /*
                                 * Print a summary of what happened in the
                                 * phases of this phStartAll..phEndAll.
                                 */

extern void     phGrandTotals(bool verboseFlag);
                                /*
                                 * Print grand totals of what has happened.
                                 */

extern struct phInfo    phInfo[];
                                /*
                                 * E.g. phInfo[PH_Scan]
                                 */

struct phSymbolData {
        Symbol      sym;
        void *     data;
};

typedef struct phSymbolData *PhaseSymbolData;

DECLARE_LIST(PhaseSymbolData);

extern PhaseSymbolDataList  saveAndEmptyAllPhaseSymbolData(void);
                                /*
                                 * Save phase-specific symbol data and null
                                 * out that field. Return a list of the by
                                 * symbol.
                                 */

extern void                 restoreAllPhaseSymbolData(PhaseSymbolDataList);
                                /*
                                 * Restore phase-specific symbol data as given
                                 * in the list. Free the list.
                                 */


#endif /* !_PHASE_H_ */
