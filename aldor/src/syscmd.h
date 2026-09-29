/*****************************************************************************
 *
 * syscmd.h: System command processing.
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

#ifndef _SYSCMD_H_
#define _SYSCMD_H_

# include "axlobs.h"

/*
 * Top-level dispatchers.
 */
extern  TokenList       scmdProcessList (TokenList);
extern  TokenList       scmdProcessToken(Token);
extern  void            scmdCheck       (SrcPos, MutString cmd);
extern  TokenList       scmdProcess     (SrcPos, MutString cmd);

/*
 * Each of these returns the remainder of the line on success and 0 on failure.
 */
extern  MutString  scmdIsDirective         (MutString ln, String kword);
extern  MutString  scmdIsAbbrev            (MutString ln, String kwabbrev);

extern  MutString  scmdScanInteger         (MutString ln, int    *pno);
extern  MutString  scmdScanId              (MutString ln, MutString *pid);
extern  MutString  scmdScanFName           (MutString ln, MutString *pfn);
extern  MutString  scmdScanLibraryOption   (MutString ln, MutString *pid, MutString *pkey);

/*
 * Handlers.
 */
extern  int     scmdHandleLibraryDir    (MutString dname);
extern  int     scmdHandleIncludeDir    (MutString dname);
extern  int     scmdHandleLibrary       (MutString id, MutString key);
extern  int     scmdHandleMacro         (SrcPos pos, MutString s);

#endif  /* !_SYSCMD_H_ */
