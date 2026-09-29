///////////////////////////////////////////////////////////////////////////////
//
// sto_debug.cpp: Storage manager debugging.
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

# include "sto_debug.h"

ALDOR_C_BEGIN

int stoDebugWhen =  STO_DEBUG_ON_EXIT;
int stoDebugWhat = ~STO_DEBUG_DO_ALLOC_COUNT;

int noisy = 0;
local CBool sdbStats = false;


#ifdef STO_DEBUG_ALLOCATOR

# include <stdint.h>
# include <stdlib.h>
# include "util.h"

FILE *sdbout;


/*
 * Globals for keeping track of storage debug info.
 */

local unsigned        sdbSerialNo = 0;

void sdbFatalError(const char *fmt, ...) {
    va_list argp;
    fprintf(sdbout, "Error in file \"%s\" line %d.\n", __stoFile, __stoLine);
    va_start(argp, fmt);
    vfprintf(sdbout, fmt, argp);
    va_end(argp);

    fprintf(sdbout, "Exiting...\n");
    exit(EXIT_FAILURE);
}


/******************************************************************************
 *
 * This wraps each allocated piece with a header and tail to
 * -- check for under-run/over-run corruption.
 * -- number allocations for debugging.
 * -- later (in C++) can do bounds checking.
 *
 * The layout is
 *    [Head] [gap1] [User data......] [gap2] [Tail]
 *
 * Gap1 and/or gap2 could have size zero, depending on alignment needs.
 * The size of gap1 is the same for all objects and is to align user data.
 * The size of gap2 is can vary and is to align the Tail.
 */

typedef struct {
    ULong    sentinel;
} Tail;

typedef struct {
    Tail     *ptail;
    unsigned serialNo;
    UByte    gap2len;
    ULong    sentinel;
} Head;

local const unsigned HeadOffset = MAX(cportAlignof(MostAlignedType), sizeof(Head));
#ifndef __ALLOK__
#define Gap1Len    (HeadOffset - sizeof(Head))
#else
local const unsigned Gap1Len    = HeadOffset - sizeof(Head);
#endif

local unsigned sentinelPatWhole = (unsigned) 
#ifndef __ALLOK__
0xAABBCCDDAABBCCDDull;
#else
0xAABBCCDDAABBCCDDul;
#endif
local UByte    sentinelPatByte  = 0xAA;
local UByte    newPatByte       = 'x';
local UByte    freePatByte      = '-';

local char *   fakeHeapStart    = 0;


/******************************************************************************
 *
 * Gross-up sizes.
 *
 */

local int
extraSizeForBody(ULong size) { return ROUND_UP_INCR(size, cportAlignof(Tail)); }

local ULong
grossSizeFor(ULong size) {
    /* Calculate sizes for extras. */
    int extraFront = HeadOffset;
    int extraBody  = extraSizeForBody(size);
    int extraTail  = sizeof(Tail);
    return extraFront + size + extraBody + extraTail;
}


/******************************************************************************
 *
 * Conversion between user and grossed-up objects.  Field access.
 *
 */

local Head  *getHead(UByte *puser) { return (Head *) (puser - HeadOffset); }

local UByte *getUser(Head *ph)   { return (UByte *) ph + HeadOffset; }
local UByte *getGap1(Head *ph)   { return (UByte*) ph + sizeof(Head); }
local UByte *getGap2(Head *ph)   { return (UByte*) ph->ptail - ph->gap2len; }

local ULong getUserLen(Head *ph) {
    return ((char *) ph->ptail - (char *) ph) - HeadOffset - ph->gap2len;
}
local ULong getGap1Len(Head *) { return Gap1Len; }
local ULong getGap2Len(Head *ph) { return ph->gap2len; }


/******************************************************************************
 *
 * Object tracking array.
 *
 */

#undef USER_POINTERS
#ifdef USER_POINTERS
/*
 * The array sdbObjects is stored as an array of pointers to the user areas
 * so it can be declared as an array of weak pointers for GC purposes.
 */

typedef void * Tracker;
#define getTracker(hd)     getUser(hd)
#define getTrackerHead(tr) getHead(tr)

#else
typedef Head *  Tracker;
#define getTracker(hd)     (hd)
#define getTrackerHead(tr) (tr)
#endif


/*
 * ToDo -- disconnect array index from serial number.
 */
#define SDB_TRACK_LIMIT    10000000

local Tracker sdbObjects[SDB_TRACK_LIMIT];

int sdbShowActionTitle = 1;
int sdbShowActionCount = 0;

void 
sdbCollectOneAction(void * *pp) {

    if (sdbShowActionTitle == 1) {
/*
        printf("sdbCollectOneActions [base = %p], sdbSerialNo = %d: ",
               sdbObjects, (int) sdbSerialNo);
*/
        sdbShowActionTitle = 0;
        sdbShowActionCount = 0;
    }
    if (ptrLT(pp, sdbObjects+sdbSerialNo) && *pp) {
        sdbShowActionCount++;
/*
        if (sdbShowActionCount % 6 == 0) printf("\n");
        printf("[%d]=%ld, ", (int)(pp- (Pointer *) sdbObjects),
                             (long) ((char *) *pp - fakeHeapStart));
*/
        *pp = 0;
    }
}

void
sdbCollectDoneAction(void) {
/*
    printf("\n");
    printf("Wrapped up %d weak pointers.\n", sdbShowActionCount);
*/
    sdbShowActionTitle = 1;
}

void
sdbTrackInit() {
    int i;
    for (i = 0; i < SDB_TRACK_LIMIT; i++) sdbObjects[i] = 0;
    stoWeakRegion((void *) sdbObjects,
                  (void *) (sdbObjects + SDB_TRACK_LIMIT),
                  sdbCollectOneAction,
                  sdbCollectDoneAction);
}

int sdbTrackAnnouncedOverflow = 0;

void
sdbTrackObject(Head *ob) {
    int serNo = ob->serialNo;
    if (serNo >= SDB_TRACK_LIMIT) {
        if (sdbStats &&
            (stoDebugWhat & STO_DEBUG_DO_OVERFLOW_CHECK) &&
            !sdbTrackAnnouncedOverflow)
        {
            fprintf(sdbout, "More than %d allocations.  Not tracking rest.\n",
                    (int) SDB_TRACK_LIMIT);
            sdbTrackAnnouncedOverflow = 1;
        }
        return;
    }

    if (sdbObjects[serNo])
        sdbFatalError("sdbTrackObject: %d is already tracked.\n", serNo);

    sdbObjects[serNo] = getTracker(ob);
}

void
sdbUntrackObject(Head *ob) {
    int serNo = ob->serialNo;
    if (serNo >= SDB_TRACK_LIMIT) return;
    sdbObjects[serNo] = 0;
}


int
sdbMapReduceTrackedObjects(int (*fnToMap)(Head*, int), int v0, int (*redFn)(int,int)) {
    int v = v0;
    unsigned i;
    unsigned n = sdbSerialNo < SDB_TRACK_LIMIT ? sdbSerialNo : SDB_TRACK_LIMIT;
    for (i = 0; i < n; i++)
        if (sdbObjects[i]) {
            int vi = fnToMap(getTrackerHead(sdbObjects[i]), (int) i);
            v = redFn(vi, v);
        }
    return v;
}


/******************************************************************************
 *
 * Initialization and checking of debug info.
 *
 */

void
sdbInitLayout(Head *ph, ULong size) {
    ph->gap2len = extraSizeForBody(size);
    ph->ptail   = (Tail *) (getUser(ph) + size + ph->gap2len);
}
void
sdbInitSentinels(Head *ph) {
    ph->serialNo        = sdbSerialNo++;
    ph->sentinel        = sentinelPatWhole;
    ph->ptail->sentinel = sentinelPatWhole;

    UByte *g = getGap1(ph);
    int    n = Gap1Len;
    int    i;
    for (i = 0; i < n; i++) g[i] = sentinelPatByte;

    g = getGap2(ph);
    n = ph->gap2len;
    for (i = 0; i < n; i++) g[i] = sentinelPatByte;
}
local void
sdbFillUserArea(Head *ph, UByte b) {
    UByte *u = getUser(ph);
    ULong  n = getUserLen(ph);
    ULong  i;
    for (i = 0; i < n; i++) u[i] = b;
}


/* Returns 1 if error, zero otherwise. */
int
checkBadFill(UByte *p, ULong len, UByte b) {
    ULong i;
    for (i = 0; i < len; i++) if (p[i] != b) return 1;
    return 0;
}
            
/* Returns the number of errors. */
int
sdbCheck(Head *ph, int index) {
    int serNo = ph->serialNo;

    int nerrs = 0;
    if (stoDebugWhat & STO_DEBUG_DO_SENTINEL_CHECK) {
        if (ph->sentinel != sentinelPatWhole) {
            fprintf(sdbout, "Area BEFORE object %d = 0x%x @ %p  -> '%c' = 0x%x (%d from start) corrupted. ",
                serNo, serNo, ph, *getUser(ph), *getUser(ph), (int)((char *) ph - fakeHeapStart));
            nerrs++;
        }
        if (nerrs > 0) {
            /* Can't trust ph->ptail. */
            if (noisy) fprintf(sdbout, "No more checks for this object. ");
        }
        else {
            if (ph->ptail->sentinel != sentinelPatWhole) {
                fprintf(sdbout, "Area after object %d corrupted. ", serNo);
                nerrs++;
            }
            if (checkBadFill(getGap1(ph), getGap1Len(ph), sentinelPatByte)) {
                fprintf(sdbout, "Area just before object %d corrupted. ", serNo);
                nerrs++;
            }
            if (checkBadFill(getGap2(ph), getGap2Len(ph), sentinelPatByte)) {
                fprintf(sdbout, "Area just after object %d corrupted. ", serNo);
                nerrs++;
            }
        }
    }
    if (nerrs) {
        if (noisy)
            fprintf(sdbout, "A good place to put a breakpoint.\n");
        fprintf(sdbout, "Index %d.\n", index);
    }
    return nerrs;
}

local int intPlus(int a, int b) { return a + b; }

int
sdbCheckAll(void) {
    return sdbMapReduceTrackedObjects(sdbCheck, int0, intPlus);
}


/******************************************************************************
 *
 * Initialization / Finalization
 *
 */

local unsigned sdbInitialized = 0;

local void sdbFini(void) {
    if (stoDebugWhen & STO_DEBUG_ON_EXIT) {
        if (stoDebugWhat & STO_DEBUG_DO_ALLOC_COUNT)
           fprintf(sdbout, "Number of allocations is %d\n", sdbSerialNo);

        int nerrs = sdbCheckAll();
        if (nerrs != 0)
            fprintf(sdbout, "Memory check returned %d errors.\n", nerrs);
    }
}

local void sdbInit(void) {
    if (sdbInitialized) return;
    sdbout = stdout;
    sdbStats = osGetEnv("ALDOR_ALLOC_STATS") != 0;
    sdbTrackInit();
    atexit(sdbFini);
    sdbInitialized = 1;
}


/******************************************************************************
 *
 * Initialization and checking of debug info.
 *
 */

MostAlignedType*
__stoAllocDebug(unsigned code, ULong size) {
    sdbInit();

    /* Calculate gross size and allocate. */
    Head  *ph = (Head *) __stoAlloc(code, grossSizeFor(size));
    if (!fakeHeapStart) fakeHeapStart = (char *) ph;

    /* Initialize head and tail pointers and fill sentinels. */
    sdbInitLayout     (ph, size);
    sdbInitSentinels  (ph);
    sdbFillUserArea   (ph, newPatByte);
    sdbTrackObject    (ph);

    /* Perform check */
    if (stoDebugWhen & STO_DEBUG_ON_ALLOC) {
        int serNo = ph->serialNo;
        int nerrs = sdbCheckAll();
        if (nerrs)
            fprintf(sdbout, "=== Had %d errors.  Was allocating serialNo %d.\n",
                    nerrs, serNo);
    }

    //if (ph->serialNo >= 442 && ph->serialNo <= 450) {
    if (ph->serialNo == 1000) {
        /* TEMP */
        /* Here is a good place to breakpoint. */
        stoAudit();
    }
    /* Return result */
    return (MostAlignedType *) getUser(ph);
}

void
__stoFreeDebug(void * p) {
    Head *ph = getHead((UByte *) p);
    int  serNo = ph->serialNo;

    sdbFillUserArea(ph, freePatByte);
    sdbUntrackObject(ph);
    __stoFree(ph);

    /* Perform check */
    if (stoDebugWhen & STO_DEBUG_ON_FREE) {
        int nerrs = sdbCheckAll();
        if (nerrs)
            fprintf(sdbout, "=== Had %d errors. Was freeing serialNo %d.\n",
                    nerrs, serNo);
    }
    if (ph->serialNo == 1000) {
        /* TEMP */
        /* Here is a good place to breakpoint. */
        stoAudit();
    }

}

ULong __stoSizeDebug(void * p) { return getUserLen(getHead((UByte *) p)); }

/*
 * In principle, we could check for 
 * (1) existing extra space in object to grow, or 
 * (2) insufficient returned space in object to shrink,
 * but we prefer to keep sentinels tight to the user space.
 */
MostAlignedType*
__stoResizeDebug(void * oldp, ULong newsize) {
    MostAlignedType *newp    = __stoAllocDebug(stoCode(oldp), newsize);
    ULong            oldsize = __stoSizeDebug(oldp);

    memcpy(newp, oldp,  MIN(oldsize, newsize));

    __stoFreeDebug(oldp);
    return newp;
}
#endif /* STO_DEBUG_ALLOCATOR */


ThatsAll(2)

ALDOR_C_END
