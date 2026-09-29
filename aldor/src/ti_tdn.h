/*****************************************************************************
 *
 * ti_tdn.h: Type inference -- top down pass.
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

#ifndef _TI_TDN_H_
#define _TI_TDN_H_

#include "axlobs.h"

extern void             tiTopDown               (Stab, AbSyn, TForm);
extern bool             titdn                   (Stab, AbSyn, TForm);

#endif /* !_TI_TDN_H_ */
