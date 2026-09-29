/*****************************************************************************
 *
 * include.h: The source file includer.
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

#ifndef _INCLUDE_H_
#define _INCLUDE_H_

# include "axlobs.h"

typedef bool            (*InclIsContinuedFun)(MutString line);
                        /*
                         * Called initially with *pflags = 0.
                         */

extern void             inclGlobalAssert  (MutString property);
extern void             inclGlobalUnassert(MutString property);

extern SrcLineList      include           (FileName, FILE *fin, int *pline,
                                           InclIsContinuedFun);
                        /*
                         * Calls either include "includeFile" or "includeLine".
                         * If fin is non-null include one line.
                         * Otherwise include the whole file.
                         */

extern SrcLineList      includeFile       (FileName);
extern SrcLineList      includeLine       (FileName, FILE *, int *,
                                           InclIsContinuedFun);

extern long             inclTotalLineCount(void);
extern long             inclFileLineCount (void);
                        /*
                         * These tell what happened during the last call to
                         * include, includeFile or includeLine.
                         */

extern int              inclWrite         (FILE *, SrcLineList);
extern void             inclFree          (SrcLineList);

#endif /* !_INCLUDE_H_ */
