/*****************************************************************************
 *
 * macex.h: Macro expansion of parse trees.
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

#ifndef _MACEX_H_
#define _MACEX_H_

# include "axlobs.h"

extern void     macexInitFile   (void);
extern void     macexFiniFile   (void);

extern  AbSyn   macroExpand     (AbSyn);

extern  void    macexUseGlobalMacros    (bool);
extern  AbSyn   macexGetMacros(void);
extern  void    macexAddMacro(AbSyn, bool);
#endif /* !_MACEX_H_ */
