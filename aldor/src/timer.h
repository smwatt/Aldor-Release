/******************************************************************************
 *
 * :: Timers
 *
 *****************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_
 
#include "foam_c.h"
#include "axlgen.h"

ALDOR_C_BEGIN

typedef struct {
        FiWord time;
        FiWord start;
        FiWord live;
} *TmTimer, _TmTimer;

extern TmTimer tmAlloc(void);
extern void tmFree(TmTimer tm);
extern FiSInt tmRead(TmTimer tm);
extern void tmStart(TmTimer tm);
extern void tmStop(TmTimer tm);

ALDOR_C_END

#endif /*_TIMER_H_*/

