/*****************************************************************************
 *
 * fenvport.h: Insulation for floating-point environment deficiencies.
 *
 * ISO C provides exception status and rounding control, but not portable
 * per-exception trap enable/query operations.  The compiler/runtime code uses
 * the cportFenv* interface below; platform details live only here.
 *
 ****************************************************************************/

#ifndef _FENVPORT_H_
#define _FENVPORT_H_

#include "platform.h"
#include "cconfig.h"
#include <fenv.h>

/* glibc provides these useful extensions, but they need not be declared when
 * feature-test macros were fixed before <fenv.h> was included. */
#if defined(CC_FENV_GLIBC_TRAPS)
# ifdef __cplusplus
extern "C" {
# endif
extern int fegetexcept(void);
extern int feenableexcept(int);
extern int fedisableexcept(int);
# ifdef __cplusplus
}
# endif

static inline int
cportFenvGetEnabledExcepts(void)
{
        return fegetexcept();
}

static inline int
cportFenvSetEnabledExcepts(int excepts)
{
        int old = fegetexcept();
        if (old & ~excepts) fedisableexcept(old & ~excepts);
        if (excepts & ~old) feenableexcept(excepts & ~old);
        return old;
}

#elif defined(CC_FENV_DARWIN_X86)

static inline unsigned short
cportFenvGetX87ControlWord(void)
{
        unsigned short control;
        __asm__ volatile ("fnstcw %0" : "=m" (control));
        return control;
}

static inline void
cportFenvSetX87ControlWord(unsigned short control)
{
        __asm__ volatile ("fldcw %0" : : "m" (control));
}

static inline unsigned int
cportFenvGetMxcsr(void)
{
        unsigned int mxcsr;
        __asm__ volatile ("stmxcsr %0" : "=m" (mxcsr));
        return mxcsr;
}

static inline void
cportFenvSetMxcsr(unsigned int mxcsr)
{
        __asm__ volatile ("ldmxcsr %0" : : "m" (mxcsr));
}

static inline int
cportFenvGetEnabledExcepts(void)
{
        unsigned int mask = FE_ALL_EXCEPT;
        unsigned int x87Mask = cportFenvGetX87ControlWord();
        unsigned int sseMask = cportFenvGetMxcsr() >> 7;
        return (int) (mask & ~(x87Mask | sseMask));
}

static inline int
cportFenvSetEnabledExcepts(int excepts)
{
        int old = cportFenvGetEnabledExcepts();
        unsigned int mask = FE_ALL_EXCEPT;
        unsigned short x87 = cportFenvGetX87ControlWord();
        unsigned int mxcsr = cportFenvGetMxcsr();

        x87 = (unsigned short) ((x87 & ~mask) | (mask & ~excepts));
        mxcsr = (mxcsr & ~(mask << 7)) | ((mask & ~excepts) << 7);
        cportFenvSetX87ControlWord(x87);
        cportFenvSetMxcsr(mxcsr);
        return old;
}

#elif defined(CC_FENV_DARWIN_ARM)

static inline unsigned long
cportFenvGetFpcr(void)
{
        unsigned long fpcr;
        __asm__ volatile ("mrs %0, fpcr" : "=r" (fpcr));
        return fpcr;
}

static inline void
cportFenvSetFpcr(unsigned long fpcr)
{
        __asm__ volatile ("msr fpcr, %0" : : "r" (fpcr));
}

static inline int
cportFenvGetEnabledExcepts(void)
{
        return (int) ((cportFenvGetFpcr() >> 8) & FE_ALL_EXCEPT);
}

static inline int
cportFenvSetEnabledExcepts(int excepts)
{
        int old = cportFenvGetEnabledExcepts();
        unsigned long fpcr = cportFenvGetFpcr();
        unsigned long mask = ((unsigned long) FE_ALL_EXCEPT) << 8;
        fpcr = (fpcr & ~mask) | (((unsigned long) excepts) << 8);
        cportFenvSetFpcr(fpcr);
        return old;
}

#else

/* ISO C fallback: trap-enable state is not queryable.  Preserve the historical
 * all-held/default behavior while keeping that deficiency out of main code. */
static inline int
cportFenvGetEnabledExcepts(void)
{
        return 0;
}

static inline int
cportFenvSetEnabledExcepts(int excepts)
{
        if (!excepts) {
                fenv_t env;
                feholdexcept(&env);
        }
        else {
                int rmode = fegetround();
                fesetenv(FE_DFL_ENV);
                fesetround(rmode);
        }
        return 0;
}

#endif

#endif /* !_FENVPORT_H_ */
