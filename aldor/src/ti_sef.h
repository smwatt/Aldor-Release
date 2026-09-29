/*****************************************************************************
 *
 * ti_sef.h: Type inference -- sefo pass.
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

#ifndef _TI_SEF_H_
#define _TI_SEF_H_

#include "axlobs.h"

extern void             tiSefo                  (Stab, Sefo);
extern void             tisef                   (Stab, Sefo);
extern bool             tiCanSefo               (Sefo);
extern Stab             stabFindLevel           (Stab, Syme);

#endif /* !_TI_SEF_H_ */
