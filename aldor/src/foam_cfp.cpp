/* Floating-point environment support for the supported Aldor targets. */

#include "axlgen.h"
#include "foam_c.h"
#include "foam_cfp.h"
#include "fenvport.h"

ALDOR_C_BEGIN

/*
 * EncodeExceptsOf:  OR-ed B_INV, B_OFL etc     -> OR-ed implementation bits
 * DecodeExceptsOf:  OR-ed implementation bits  -> OR-ed B_INV, B_OFL, etc
 *
 * EncodeMode:       single fiRoundXxxx         -> single implementation mode
 * DecodeMode:       single implementation mode -> single fiRoundXxxx
 */

#define B_INV   0x10
#define B_OFL   0x08
#define B_UFL   0x04
#define B_DBZ   0x02
#define B_INX   0x01

/* In C++ these could be generics. */
#define EncodeExceptsOf(arg, _invalid,_oflow,_uflow,_div0,_inexact) ( \
    (((arg) & B_INV) ? (_invalid) : 0) | \
    (((arg) & B_OFL) ? (_oflow)   : 0) | \
    (((arg) & B_UFL) ? (_uflow)   : 0) | \
    (((arg) & B_DBZ) ? (_div0)    : 0) | \
    (((arg) & B_INX) ? (_inexact) : 0)   \
)
#define EncodeExcepts(arg) \
    EncodeExceptsOf(arg, \
        FE_INVALID, FE_OVERFLOW, FE_UNDERFLOW, FE_DIVBYZERO, FE_INEXACT)

#define DecodeExceptsOf(arg, _invalid,_oflow,_uflow,_div0,_inexact) ( \
    (((arg) & (_invalid)) ? B_INV : 0) | \
    (((arg) & (_oflow))   ? B_OFL : 0) | \
    (((arg) & (_uflow))   ? B_UFL : 0) | \
    (((arg) & (_div0))    ? B_DBZ : 0) | \
    (((arg) & (_inexact)) ? B_INX : 0)   \
)
#define DecodeExcepts(arg) \
    DecodeExceptsOf(arg, \
        FE_INVALID, FE_OVERFLOW, FE_UNDERFLOW, FE_DIVBYZERO, FE_INEXACT)


#define DecodeMode(mode) ( \
    (mode) == FE_TONEAREST     ? fiRoundNearest() : \
    (mode) == FE_UPWARD        ? fiRoundUp()      : \
    (mode) == FE_DOWNWARD      ? fiRoundDown()    : \
    (mode) == FE_TOWARDZERO    ? fiRoundZero()    : fiRoundNearest() \
)

#define EncodeMode(mode) ( \
    (mode) == fiRoundNearest() ? FE_TONEAREST     : \
    (mode) == fiRoundUp()      ? FE_UPWARD        : \
    (mode) == fiRoundDown()    ? FE_DOWNWARD      : \
    (mode) == fiRoundZero()    ? FE_TOWARDZERO    : FE_TONEAREST \
)


static char double_hex_print_string[80];


/*****************************************************************************
 *****************************************************************************
 ***
 *** ::: Standard C <fenv.h>
 ***
 *****************************************************************************
 *****************************************************************************/

FiWord 
fiDoubleHexPrintToString(FiWord f) 
{
    /* TODO: Clean up in case 64-bit int. */
    sprintf(double_hex_print_string,"%.8x %.8x",*(int *)f,*((int *)f+1));
    return (FiWord) double_hex_print_string;
}

void fiInitialiseFpu() { }

FiWord
fiIeeeGetRoundingMode()
{
    unsigned tmp = fegetround();
    return DecodeMode(tmp);
}

FiWord
fiIeeeSetRoundingMode(FiWord s)
{
    int      mode = EncodeMode(s);
    unsigned tmp  = fesetround(mode);
    return DecodeMode(tmp);
}

FiWord
fiIeeeGetExceptionStatus()
{
    fexcept_t fm;

    fegetexceptflag(&fm, FE_ALL_EXCEPT);
    return DecodeExcepts(fm);
}

FiWord
fiIeeeSetExceptionStatus(FiWord s)
{
    FiWord    result = fiIeeeGetExceptionStatus();
    fexcept_t set    = EncodeExcepts(s); 

    fesetexceptflag(&set,FE_ALL_EXCEPT);
    return result;
}

/*
 * ISO C does not standardize per-exception trap enable/query operations.
 * fenvport.h provides cportFenv* insulation for that deficiency.
 */
FiWord
fiIeeeGetEnabledExceptions()
{
    return DecodeExcepts(cportFenvGetEnabledExcepts());
}

FiWord
fiIeeeSetEnabledExceptions(FiWord s)
{
    int old = cportFenvSetEnabledExcepts(EncodeExcepts(s));
    return DecodeExcepts(old);
}


ALDOR_C_END
