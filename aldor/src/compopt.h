/*****************************************************************************
 *
 * compopt.h: optimisations for various platforms (declarations)
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

#ifndef _COMPOPT_H_
#define _COMPOPT_H_

#include "cport.h"

ALDOR_C_BEGIN

/*
 * Internal (both to RTS and compiler) optimizations.
 */

/*
 * compopt_linkage should be defined as "extern" before including
 * this file in order to give linkable versions.
 */
#ifndef compopt_linkage
#  define compopt_linkage __inline
#endif

 
#if defined(OPT_Linux_i386)

/* asms are: <string> : <output> : <input>: <side-effects> */

compopt_linkage void
xxTimesDouble(ULong *hi, ULong *lo, ULong a, ULong b) {
        __asm__( "mull %3" : "=a" (*lo), "=d" (*hi) : "a" (a), "b" (b) : "cc");
}

compopt_linkage void
xxDivideDouble(ULong* pqhi,ULong* pqlo,ULong* prem, ULong hi, ULong lo, ULong d)
{
    ULong qhi;
    __asm__("divl %4"
           : "=a" (qhi), "=d" (hi) : "d" (0), "a" (hi), "r" (d) : "cc");
    *pqhi = qhi;
    __asm__("divl %4"
           : "=a" (*pqlo), "=d" (*prem) : "d" (hi), "a" (lo), "r" (d) : "cc");
}

compopt_linkage ULong
xxModDouble(ULong hi, ULong lo, ULong d) {
    ULong r1;

    __asm__("divl %3" : "=d" (hi) : "d" (0),  "a" (hi), "r" (d) : "cc");
    __asm__("divl %3" : "=d" (r1) : "d" (hi), "a" (lo), "r" (d) : "cc" );

    return r1;
}

#elif defined(OPT_Sparc_v8)

compopt_linkage void
xxTimesDouble(ULong *hi, ULong *lo, ULong a, ULong b) {
    __asm__ ("umul %2,%3,%1;rd %%y,%0" : "=r" (*hi), "=r" (*lo)
            : "r" (a), "r" (b));
}

compopt_linkage void
xxDivideDouble(ULong *pqhi,ULong *pqlo,ULong *prem, ULong hi, ULong lo, ULong d)
{
    ULong qhi;
    /* It seems you have to write to %y in any case... */
    __asm__ ("mov %2,%%y;nop;nop;nop;udiv %3,%4,%0;umul %0,%4,%1;sub %3,%1,%1"
            : "=&r" (qhi), "=&r" (hi)
            : "r" (0), "r" (hi), "r" (d));
    *pqhi = qhi;
    __asm__ ("mov %2,%%y;nop;nop;nop;udiv %3,%4,%0;umul %0,%4,%1;sub %3,%1,%1"
            : "=&r" (*pqlo), "=&r" (*prem)
            : "r" (hi), "r" (lo), "r" (d));
}

compopt_linkage ULong
xxModDouble(ULong hi, ULong lo, ULong d) {
    ULong r1, foo;
    /* It seems you have to write to %y in any case... */
    __asm__ ("mov %2,%%y;nop;nop;nop;udiv %3,%4,%0;umul %0,%4,%1;sub %3,%1,%1"
            : "=&r" (r1), "=&r" (hi)
            : "r" (0), "r" (hi), "r" (d));

    __asm__ ("mov %1,%%y;nop;nop;nop;udiv %2,%3,%0;umul %0,%4,%1;sub %3,%1,%1"
            : "=&r" (foo),"=&r" (r1)
            : "r" (hi), "r" (lo), "r" (d));
    return r1;
}

#else /* No optimizations */

extern void  xxTimesDouble (ULong *, ULong *, ULong, ULong);
extern void  xxDivideDouble(ULong *, ULong *, ULong *, ULong, ULong, ULong);
extern ULong xxModDouble(ULong, ULong, ULong);

#endif

/*
 * These don't have asm-optimized versions in any case.
 */
extern void  xxTestGtDouble(int *, ULong, ULong, ULong, ULong);
extern void  xxPlusStep    (ULong *, ULong *, ULong, ULong, ULong);
extern void  xxTimesStep   (ULong *, ULong *, ULong, ULong, ULong, ULong);

ALDOR_C_END

#endif /*!_COMPOPT_H_*/
