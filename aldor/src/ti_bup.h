/*****************************************************************************
 *
 * ti_bup.h: Type inference -- bottom up pass.
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

#ifndef _TI_BUP_H_
#define _TI_BUP_H_

#include "axlobs.h"

extern void             tiBottomUp              (Stab, AbSyn, TForm);
extern void             tibup                   (Stab, AbSyn, TForm);

#endif /* !_TI_BUP_H_ */
