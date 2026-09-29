/*****************************************************************************
 *
 * bigint.h: Big integer arithmetic.
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

#ifndef _BIGINT_H_
#define _BIGINT_H_

# include "axlport.h"

ALDOR_C_BEGIN

typedef UNotAsLong      BIntS;  /* Unsigned integer type for BInt digits. */
                                /* s/b the second largest integer type. */

typedef struct bint {
        CBool    isNeg;          /* Sign of number. Is it negative?   */
        Length  placea;         /* No. of slots allocated for placev */
        Length  placec;         /* No. of slots used in placev       */
        BIntS   placev[NARY];   /* Digits in radix representation    */
} *BInt;

/*
 * Error handling
 */
typedef BInt       (*BIntHandler)(int, ...);
extern BIntHandler bintSetHandler(BIntHandler);

/*
 * Related machine integer operations
 */
extern Length   uintLength      (unsigned long);                /*  no. bits   */
extern Length   intLength       (long);                 /*  no. bits   */

extern CBool     uintBit         (unsigned long, Length);   /*  coef 2**ix */
extern CBool     intBit          (long         , Length);   /*  coef 2**ix */

/*
 * Input/output
 */
extern int      bintPrint       (FILE *, BInt);
extern int      bintPrint2      (FILE *, BInt);
extern int      bintPrint16     (FILE *, BInt);

extern int      bintStringSize  (BInt);         /* Size for bintIntoString. */
extern MutString   bintIntoString  (MutString, BInt);

extern MutString   bintToString            (BInt);
extern BInt     bintFrString            (String);
extern BInt     bintScanFrString        (MutString, MutString *);
extern ULong    bintScanIIntFrString    (MutString, Length, ULong);
extern BInt     bintRadixScanFrString   (String, String *);

extern void     bintToPlacevS           (BInt, int *, U16 **);
extern void     bintReleasePlacevS      (U16 *);

/*
 * Basic allocation
 */
extern BInt     bintAllocPlaces (Length);
extern BInt     bintAlloc       (Length);
extern void     bintFree        (BInt);

extern BInt     bintNew         (long);                 /* may be immeditate */
extern BInt     bintCopy        (BInt);
extern BInt     bintFrPlacev    (CBool, Length, BIntS *);
extern BInt     bintFrPlacevS   (CBool, Length, U16 *);

/*
 * Small integers
 */
extern CBool     bintIsSmall     (BInt);                 /* can be an int?  */
extern long     bintSmall       (BInt);                 /* convert to int. */

/*
 * General arithmetic
 */
extern BInt     bint0;
extern BInt     bint1;

extern CBool     bintIsNeg       (BInt a);               /*  a <  0 ?    */
extern CBool     bintIsZero      (BInt a);               /*  a == 0 ?    */
extern CBool     bintIsPos       (BInt a);               /*  a >  0 ?    */

extern CBool     bintEQ          (BInt a, BInt b);       /*  a == b ?    */
extern CBool     bintLT          (BInt a, BInt b);       /*  a <  b ?    */
extern CBool     bintGT          (BInt a, BInt b);       /*  a >  b ?    */
#define         bintNE(a,b)     (!bintEQ(a,b))          /*  a != b ?    */
#define         bintLE(a,b)     (!bintGT(a,b))          /*  a <= b ?    */
#define         bintGE(a,b)     (!bintLT(a,b))          /*  a >= b ?    */

extern BInt     bintAbs         (BInt a);               /*  |a|         */
extern BInt     bintNegate      (BInt a);               /*  -a          */
extern BInt     bintPlus        (BInt a, BInt b);       /*  a + b       */
extern BInt     bintMinus       (BInt a, BInt b);       /*  a - b       */
extern BInt     bintTimes       (BInt a, BInt b);       /*  a * b       */
extern BInt     bintDivide      (BInt *r,BInt a,BInt b);/*  a/b (r=a%b) */
extern BInt     bintGcd         (BInt a, BInt b);       /*  gcd(a,b)    */
extern BInt     bintMod         (BInt a, BInt b);       /*  mod(a,b)    */

extern Length   bintLength      (BInt a);               /*  no. bits    */
extern CBool     bintBit         (BInt a, Length ix);    /*  coef 2^ix   */
extern BInt     bintShift       (BInt a, int n);        /*  a * 2^n     */
extern BInt     bintShiftRem    (BInt, int);

/* These free their operands */
extern BInt     xintNegate      (BInt a);               /*  -a          */
extern BInt     xintPlus        (BInt a, BInt b);       /*  a + b       */
extern BInt     xintMinus       (BInt a, BInt b);       /*  a - b       */
extern BInt     xintTimes       (BInt a, BInt b);       /*  a * b       */
extern BInt     xintDivide      (BInt *r,BInt a,BInt b);/*  a/b (r=a%b) */
extern BInt     xintGcd         (BInt a, BInt b);       /*  gcd(a,b)    */
extern BInt     xintShift       (BInt a, int  n);       /*  a * 2^n     */

/*
 * Low-level, fine control allocation
 */
extern BInt     xintStoreI      (long  n);              /* convert to alloc */
extern BInt     xintStore       (BInt a);               /* convert to alloc */
extern BInt     xintImmedIfCan  (BInt a);               /* convert to immed */
extern BInt     xintCopyInI     (BInt a, long n);       /* Make value = n.  */
extern BInt     xintNeeds       (BInt a, Length bitc);  /* Make big enough. */

/*
 * Low-level, non-allocating operations.
 */
extern void     iintAbs       (BInt r, BInt a);                 /* r = |a|   */
extern void     iintNegate    (BInt r, BInt a);                 /* r = -a    */
extern void     iintPlus      (BInt r, BInt a, BInt b);         /* r = a + b */
extern void     iintMinus     (BInt r, BInt a, BInt b);         /* r = a - b */
extern void     iintTimes     (BInt r, BInt a, BInt b);         /* r = a * b */
extern void     iintDivide    (BInt q, BInt r, BInt a,BInt b);  /* qr= a/%b  */
extern void     iintShift     (BInt r, BInt b, int  n);         /* r = b*2^n */

extern void     iintTimesS    (BInt r, BInt a, BIntS b);        /* r = a*b   */
extern void     iintTimesPlusS(BInt r, BInt a, BIntS b,BIntS c);/* r = a*b+c */
extern void     iintDivideS   (BInt q, BIntS*r,BInt  a,BIntS b);/* qr= a/%b  */

extern void     xxTestGtDouble(int *, ULong, ULong, ULong, ULong);
extern void     xxPlusStep    (ULong *, ULong *, ULong, ULong, ULong);
extern void     xxTimesStep   (ULong *, ULong *, ULong, ULong, ULong, ULong);

#ifndef OPT_NoDoubleOps
extern void     xxTimesDouble (ULong *, ULong *, ULong, ULong);
extern ULong    xxModDouble   (ULong, ULong, ULong);
extern void     xxDivideDouble(ULong *, ULong *, ULong *,
                               ULong, ULong, ULong);
#endif

ALDOR_C_END

#endif  /* !_BIGINT_H_ */
