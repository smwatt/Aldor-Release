/*****************************************************************************
 *
 * util.h: General low-level utility functions.
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

#ifndef _UTIL_H_
#define _UTIL_H_

# include "axlport.h"

ALDOR_C_BEGIN

/******************************************************************************
 *
 * :: Exits
 *
 *****************************************************************************/

extern void     exitSuccess     (void);
extern void     exitFailure     (void);
        /*
         * Exit from program with an appropriate return code.
         */

typedef void    (*ExitFun)      (int status);
extern ExitFun  exitSetHandler  (ExitFun);
        /*
         * exitSetHandler is evaluated by exitSuccess and exitFailure.
         * A value of 0 restores the default.
         */

/******************************************************************************
 *
 * :: Bugs
 *
 *****************************************************************************/
extern void     bugWarning      (String fmt, ...);

extern void     bug             (String fmt, ...);

#define         bugUnimpl(m)    _bug("Unimplemented %s (line %d in file %s).",m)
#define         bugBadCase(no)  _bug("Bad case %d (line %d in file %s).", no)
#define         _bug(fmt,a)     bug(fmt, a, __LINE__, __FILE__)

/******************************************************************************
 *
 * :: Numerics
 *
 *****************************************************************************/

# define QUO_ROUND_UP(n,d)      ((n) % (d) ? (n)/(d) + 1 : (n)/(d))
        /*
         * QUO_ROUND_UP(n,d) is the smallest integer q such that q*d <= n.
         * Capitalized to remind that the arguments are multiply evaluated.
         */

# define ROUND_UP_INCR(n,d)     ((n) % (d) ? (d) - (n) % (d) : 0)
# define ROUND_UP(n,d)          ((n) + ROUND_UP_INCR(n,d))
        /*
         * ROUND_UP(n,d) is the smallest multiple of d >= n.
         * Capitalized to remind that the arguments are multiply evaluated.
         * ROUND_UP_INCR(n,d) is the amount to add to n to round up.
         */

# undef MAX
# undef MIN

# define MAX(a,b)               ((a) > (b) ? (a) : (b))
# define MIN(a,b)               ((a) < (b) ? (a) : (b))
        /*
         * maximum and minimum of arguments.
         * Capitalized to remind that the arguments are multiply evaluated.
         */

extern ULong    binPrime        (ULong);
        /*
         * p = binPrime(i) is the largest prime p such that p <= 2**i
         */

extern int      cielLg          (ULong);
        /*
         * i = cielLg(n) returns the smallest integer i such that n <= 2**i.
         */

/******************************************************************************
 *
 * :: Strings
 *
 *****************************************************************************/

extern MutString   bite            (MutString buf, MutString str, int sep);
        /*
         * Using 'sep' as the separator character, the first word of 'str'
         * into 'buf' and return the remainder of 'str'.
         */

/******************************************************************************
 *
 * :: Sorting
 *
 *****************************************************************************/

extern void     lisort          (void * base, Length n, Length sz,
                                 int (*cmpfn)(ConstPointer, ConstPointer));
        /*
         * A stable sort.
         */

/******************************************************************************
 *
 * :: Memory operations
 *
 *****************************************************************************/

extern void     memswap         (void * p, void * q, Length);
extern void *  memlset         (void * p, int c,     ULong l);
extern CBool     memltest        (void * p, int c,     ULong l);
        /*
         *      memlswap swaps the data in the regions pointed to by p and q.
         *      memlset  sets the first l characters pointed to by p to be c.
         *      memltest tests whether the first l characters pointed to by p
         *               are all equal to c.
         */

/******************************************************************************
 *
 * :: Bit-fiddling
 *
 *****************************************************************************/

extern void     bfShiftUp(int nb,UByte *t,int nsh,UByte *s,int bF);
extern void     bfShiftDn(int nb,UByte *t,int nsh,UByte *s,int b0,int b1);
extern int      bfFirst1 (int nb,UByte *t);
        /*
         * bfShiftUp
         *       shifts bits up (toward lower address) by 'nsh' bit positions.
         *      's' is a pointer to 'nb' bytes treated as a bit string.
         *      't' is a pointer to 'nb' bytes to hold the result.
         *      Bits shifted out are lost.  'bF' is the bit to shift in.
         *
         * bfShiftDn
         *      shifts bits down (toward higher address) by nsh bit positions.
         *      's' is a pointer to 'nb' bytes treated as a bit string.
         *      't' is a pointer to 'nb' bytest to hold the result.
         *      Bits shifted out are lost.
         *      'b1' is the first bit to shift in the top.
         *      'b0' is shifted in for the subsequent bits.
         *
         * bfFirst1
         *      finds the bit index of the lowest address 1-bit in 't'.
         *      The most significant bit of t[0] is bit 0 and the least
         *      significant bit of t[nb-1] is bit nb * CHAR_BIT - 1.
         */


/*****************************************************************************
 *
 * :: Input-Output
 *
 ****************************************************************************/

extern void     prompt          (FILE *fin, FILE *fout, String fmt, ...);
        /*
         * If fin is interactive print the prompt on fout using
         * printf-style formatting.
         */

extern int      fputcTimes      (int c, int n, FILE *f);
        /*
         * Output character c n times, returning n.
         */

extern int      fputsUntab      (char *s, int tabstop, FILE *f);
        /*
         * Output the string, expanding tab characters to spaces.
         * The the return value is the number of characters output.
         */

#define MAX_FLOAT_SIZE  512
        /*
         * The maximum size of a string representation of a double.
         */
extern  MutString  DFloatSprint(MutString, DFloat);
        /*
         * Print a double float value into a string.
         */
extern  DFloat  DFloatScan(MutString);
        /*
         * Scan a double float value from a buffer and return the value.
         */

#define ubPrintBits(a)  ubPrintBits0(sizeof(a), (UByte *)&(a))

extern  void    ubPrintBits0    (int, UByte *);


ALDOR_C_END

#endif /* !_UTIL_H_ */
