/*****************************************************************************
 *
 * gf_rtime.h: Foam generation for runtime objects
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

#ifndef _GF_RTIME_H_
#define _GF_RTIME_H_

/*
 * Info on builtin runtime calls
 */
typedef struct RuntimeCallInfoStruct {
        String name;
        bool    hasSideFx;
        int     argc;
        int     flags;
} *RuntimeCallInfo;

#define RT_FORCER       (1 << 0)
#define RT_DOM_RTHASH   (1 << 1)

#define rtCallIsForce(info)     (info && ((info->flags) & RT_FORCER))
#define rtCallIsDomHash(info)   (info && ((info->flags) & RT_DOM_RTHASH))

extern RuntimeCallInfo  gen0GetRuntimeCallInfo  (Foam decl);

#define gen0StdLazyNamePrefix() "stdGet"


/*
 * Runtime cache-size tweaking.
 */
extern void     gen0SetRtCacheSize(AInt);
extern AInt     gen0GetRtCacheSize(void);


/*
 * Tuples
 */
extern AInt             gen0TupleFormat;

#define         gen0NewTupleSizeRef(val)        \
        foamNewRElt(gen0MakeTupleFormat(), foamCopy(val), int0)
#define         gen0NewTupleValsRef(val)        \
        foamNewRElt(gen0MakeTupleFormat(), foamCopy(val), 1)

extern void     gen0MakeEmptyTuple      (Foam, Foam *, AbSyn);
extern Foam     gen0MakeTuple           (Length argc, AbSyn *argv,
                                         AbSyn absyn);
extern Foam     gen0MakeTupleFromFoam   (int, Foam *);
extern AInt     gen0MakeTupleFormat     (void);
extern Foam     gen0TupleElt            (Foam tuple, int index);

extern Foam     gen0MakeCross           (Length argc, AbSyn *argv,
                                         AbSyn absyn);
/*
 * Unions
 */
extern int      gen0MakeUnionFormat       (void);
extern int      gen0UnionFormat;

#define gen0NewUnionTagRef(val)    foamNewRElt(gen0MakeUnionFormat(), val, 0)
#define gen0NewUnionValRef(val)    foamNewRElt(gen0MakeUnionFormat(), val, 1)

/*
 * Arrays
 */

extern int              gen0ArrayFormat;
extern Foam             gen0MakeArray           (Foam, Foam, AbSyn);
extern int              gen0MakeArrayFormat     (void);

/*
 * Doubles
 */
extern AInt             gen0MakeDoubleFormat    (void);
extern Foam             gen0MakeDouble          (Foam);
extern Foam             gen0MakeDoubleCode      (Foam, FoamList *);
extern Foam             gen0DoubleValue         (Foam);
extern Foam             gen0WriteDoubleValue    (Foam, Foam);


/*
 * Characters
 */
extern AInt gen0MakeCharFormat(void);
extern Foam gen0MakeCharRecValue(Foam, FoamList *);
extern Foam gen0ReadCharRecValue(Foam);
extern Foam gen0WriteCharRecValue(Foam, Foam);


/*
 * Integers
 */
extern AInt gen0MakeIntFormat(void);
extern Foam gen0MakeIntRecValue(Foam, FoamList *);
extern Foam gen0ReadIntRecValue(Foam);
extern Foam gen0WriteIntRecValue(Foam, Foam);


/*
 * Floats
 */
extern AInt gen0MakeFloatFormat(void);
extern Foam gen0MakeFloatRecValue(Foam, FoamList *);
extern Foam gen0ReadFloatRecValue(Foam);
extern Foam gen0WriteFloatRecValue(Foam, Foam);


/*
 * Complex numbers (Aldor and two flavours of Fortran)
 */
extern AInt gen0AldorCpxFormat(void);
extern AInt gen0SingleCpxFormat(void);
extern AInt gen0DoubleCpxFormat(void);
extern void gen0CopyComplex(Foam, Foam, AInt, AInt, FoamList *);


/*
 * Caches
 */

typedef struct RTCacheInfoStruct {
        Foam    tuple;          /* tuple holding parameters */
        Foam    value;          /* Return variable      */
        Foam    cache;          /* Lexical var. holding cache */
        Foam    init;           /* Initialisation statement */
        int     exitLabel;
        AbSyn   ab;             /* used for stmt. position */
} *RTCacheInfo;

extern int              gen0CCheckFormat;

extern RTCacheInfo      gen0CacheMakeEmpty      (AbSyn);
extern void             gen0CacheCheck          (RTCacheInfo, int, Foam *);
extern Foam             gen0CacheReturn         (RTCacheInfo, Foam);
extern void             gen0CacheKill           (RTCacheInfo);


/*
 * Debugging (old style via -Wdebug)
 */

typedef enum {
        GenDebugAssign,
        GenDebugFnEntry,
        GenDebugFnExit,
        GenDebugDomInit,
        GenDebugThrow,
        GenDebugCatch,
        GenDebugDeclare,
        GenDebugStep
} GenDebugKind;

void    gen0DebugIssueStmt(GenDebugKind, String, int, Foam, Foam, Foam);


/*
 * Debugging (new style via -Wdebugger)
 */
extern Foam gen0DbgGetPause     (void);
extern Foam gen0DbgSetPause     (Foam);
extern Foam gen0DbgEnter        (String, AInt, Foam, String, Foam, AInt);
extern Foam gen0DbgInit         (String, AInt, Foam, String, Foam, AInt);
extern Foam gen0DbgInside       (Foam);
extern Foam gen0DbgReturn       (MutString, AInt, Foam, Foam, Foam);
extern Foam gen0DbgExit         (MutString, AInt, Foam);
extern Foam gen0DbgAssign
                        (String, AInt, Foam, String, Foam, Foam, AInt, AInt);
extern Foam gen0DbgThrow        (MutString, AInt, Foam, Foam);
extern Foam gen0DbgCatch        (MutString, AInt, Foam, Foam);
extern Foam gen0DbgCall         (MutString, AInt, Foam, MutString, Foam, Foam, AInt);
extern Foam gen0DbgStep         (MutString, AInt, Foam);

typedef enum {
        DbgDepthLocal  = -1,    /* (Loc ...) */
        DbgDepthParam  = -2,    /* (Par ...) */
        DbgDepthFake   = -3,    /* Argument to next call */
        DbgDepthGlobal = -4,    /* (Glo ...) */
        DbgDepthFluid  = -5     /* Fluid variable */
} DbgDepthTag;

#endif /*!_GF_RTIME_H_*/

