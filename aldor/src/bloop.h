/*****************************************************************************
 *
 * bloop.h: Interactive break loop.
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

#ifndef _BLOOP_H_
#define _BLOOP_H_

# include "axlobs.h"

extern AbSyn    breakSetRoot    (AbSyn);
extern void     breakLoop       (bool forHuman, int msgc, CoMsg* msgv);
extern void     breakInterrupt  (void);

extern int      bloopMsgFPrintf (FILE *fout, Msg msg, ...);
extern int      bloopMsgVFPrintf(FILE *fout, Msg msg, va_list);

#endif /* !_BLOOP_H_ */
