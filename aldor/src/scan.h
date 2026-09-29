/*****************************************************************************
 *
 * scan.h: The lexical analyzer.
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

#ifndef _SCAN_H_
#define _SCAN_H_

# include "axlobs.h"

extern TokenList scan             (SrcLineList);
extern bool      scanIsContinued  (MutString line);

#endif /* !_SCAN_H_ */
