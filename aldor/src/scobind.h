/*****************************************************************************
 *
 * scobind.h: Deduce the scopes of identifiers.
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

#ifndef _SCOBIND_H_
#define _SCOBIND_H_

# include "axlobs.h"

extern void     scopeBind       (Stab, AbSyn);

extern void     scobindInitGlobal       (void);
extern void     scobindFiniGlobal       (void);
extern void     scobindInitFile         (void);
extern void     scobindFiniFile         (void);
extern void     scoSetUndoState         (void);
extern int      scobindMaxDef           (void);
#endif /* !_SCOBIND_H_ */
