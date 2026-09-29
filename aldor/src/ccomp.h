/*****************************************************************************
 *
 * ccomp.h: C compiler interface
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

#ifndef _CCOMP_H_
#define _CCOMP_H_

#include "axlobs.h"

/*
 * Option handling
 */
extern int      ccOption                (MutString);

extern void     ccSetVerbose            (bool);
extern void     ccSetDebug              (bool);
extern void     ccSetProfile            (bool);
extern void     ccSetOptimize           (bool,bool);
extern void     ccSetOutputFile         (MutString);
extern void     ccSetLineNos            (bool);
extern bool     ccLineNos               (void);
extern bool     ccDoStandardC           (void);

/*
 * Actually do stuff
 */
extern void     ccGetReady              (void);
extern void     ccCompileFile           (MutString newwd, FileName);
extern void     ccLinkProgram           (MutString newwd, FileName *, int);
extern int      ccGoProgram             (FileName, int argc1, MutString *argv1);

#endif /* _CCOMP_H_ */
