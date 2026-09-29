/*****************************************************************************
 *
 * store.h: Storage management.
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

#ifndef _STORE_H_
#define _STORE_H_


# include "axlport.h"

ALDOR_C_BEGIN

# define STO_CODE_LIMIT 32

extern ULong            stoBytesOwn;
extern ULong            stoBytesAlloc;
extern ULong            stoBytesFree;
extern ULong            stoBytesGc;
extern ULong            stoPiecesGc[STO_CODE_LIMIT];


#ifndef STO_DEBUG_ALLOCATOR
#  define  stoAlloc        __stoAlloc
#  define  stoFree         __stoFree
#  define  stoSize         __stoSize
#  define  stoResize       __stoResize
#else
#  define  __stoSrcLine(e)  (__stoLine = __LINE__, __stoFile = __FILE__, (e))

#  define  stoAlloc(c,n)   __stoSrcLine(__stoAllocDebug(c,n))
#  define  stoFree(p)      __stoSrcLine(__stoFreeDebug(p))
#  define  stoSize(p)      __stoSrcLine(__stoSizeDebug(p))
#  define  stoResize(p,n)  __stoSrcLine(__stoResizeDebug(p,n))
#endif

/* Alternative entry points for use by debug support . */
extern MostAlignedType* __stoAlloc      (unsigned code, ULong size);
extern MostAlignedType* __stoResize     (void * p, ULong size);
extern ULong            __stoSize       (void * p);
extern void             __stoFree       (void * p);

extern int              __stoLine;
extern String     __stoFile;

extern MostAlignedType* __stoAllocDebug (unsigned code, ULong size);
extern MostAlignedType* __stoResizeDebug(void * p, ULong size);
extern ULong            __stoSizeDebug  (void * p);
extern void             __stoFreeDebug  (void * p);

extern CBool             stoIsPointer    (void * p);
extern unsigned         stoCode         (void * p);
extern MostAlignedType* stoRecode       (void * p, unsigned code);

/*
 * A storage manager may support garbage collection.
 * If it doesn't, make gc and stoXxxxRegion no-ops.
 *
 * 'stoRootRegion' declares pointers in the region keep objects alive on gc.
 *
 * 'stoWeakRegion' declares any pointers in a region are weak.
 *    That is, any pointer from this region does not keep an object alive on gc.
 *    If an object is pointed to only by weak pointers, then it is collected and
 *    'collectOneAction' is called on the (address of) the (potential) pointer.
 *    After the registered 'collectOneAction' has been called on all (potential)
 *    weak pointer locations, 'collectDoneAction' is called.
 *
 *    LIMITATION: Can be applied only to non-heap regions.
 */

extern void             stoGc           (void);
extern void             stoRootRegion   (void * base, void * limit);
extern void             stoWeakRegion   (void * base, void * limit,
                                         void (*collectOneAction) (void * *),
                                         void (*collectDoneAction)(void));

extern void             stoTune         (void);
extern void             stoAudit        (void);
extern void             stoShow         (void);
extern void             stoShowDetail   (int);

extern void             stoRegister     (int code, CBool hasPtrs);
extern int              stoShowArgs     (const char *);

/*
 * User programs can make limited checks on pointers to ensure
 * that they do not attempt to write to areas of memory that
 * are writable. Although we could give the user a much more
 * precise result (such as this object is a free object on the
 * heap) we choose not to.
 */
extern int              stoWritablePointer      (void *);
#  define POINTER_IS_INVALID              (-1)
#  define POINTER_IS_UNKNOWN              ( 0)
#  define POINTER_IS_VALID                ( 1)

extern void             stoSetAldorTracer       (int, void *);
extern void             stoSetTracer            (int, void *);
extern int              stoMarkObject           (void *);

/*
 * Control storage management behaviour.
 */
extern int              stoCtl          (int cmd, ...);
                        /*
                         * 0 => success, -1 => failure.
                         */

# define StoCtl_GcLevel 1
                        /* Control GC activity (when available).
                         * Arg 1 int: See values above.
                         *
                         * Once StoCtl_GcLevel_Never has been called, the
                         * demand and automatic gc support is forever disabled.
                         */
# define StoCtl_GcLevel_Never     0
# define StoCtl_GcLevel_Demand    1
# define StoCtl_GcLevel_Automatic 2

# define StoCtl_GcFile  2
                        /* Place for gc messages.
                         * Arg 1 FILE *: NULL => quiet.
                         */

# define StoCtl_Wash    3
                        /* Control filling of new+freed pieces for debugging.
                         * Arg 1 Bool: true/false => do/don't fill.
                         */

/*
 * Install handler for error situations.
 */
typedef MostAlignedType*        (*StoErrorFun)  (int errnum);

extern  StoErrorFun             stoSetHandler   (StoErrorFun);


# define StoErr_OutOfMemory     1  /* Cannot get any more storage. */
# define StoErr_UsedNonalloc    2  /* Given pointer to non-allocated store. */
# define StoErr_CantBuild       3  /* Can't build internal structure. */
# define StoErr_FreeBad         4  /* asked to free something not allocated */

/*
 * Martin's experimental stuff
 */
typedef int (*StoTraceFun)(void * *, void * *);

ALDOR_C_END

#endif /* !_STORE_H_ */
