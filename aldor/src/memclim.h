/*****************************************************************************
 *
 * memclim.h: Memory Climates.
 *
 * Copyright Aldor.org 1990-2007.
 *
 ****************************************************************************/

#ifndef _MEMCLIM_H_
#define _MEMCLIM_H_

#include "cport.h"
#include <stdio.h>

ALDOR_C_BEGIN

typedef int             MemoryClimate;

extern void             limitNumberOfMemoryClimates(int n);

extern void             registerMemoryClimate   (MemoryClimate, const char * mcName);
extern int              numberOfMemoryClimates  (void);

extern MemoryClimate    setMemoryClimate        (MemoryClimate);

extern MemoryClimate    getMemoryClimate        (void);
extern const char *      getMemoryClimateString  (MemoryClimate);
extern int              getMemoryClimateIndex   (MemoryClimate);
extern MemoryClimate    getMemoryClimateOfIndex (int index);

extern void initMemoryClimateHistogram         (void);
extern void incrMemoryClimateHistogram         (MemoryClimate clim, int, int);
extern void finiMemoryClimateHistogram         (void);
extern void showMemoryClimateHistogram         (FILE *outf);

ALDOR_C_END

#endif /* !_MEMCLIM_H_ */
