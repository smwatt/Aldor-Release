/*****************************************************************************
 *
 * dword.h: Double-Word arithmetic
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

#ifndef _DWORD_H_
#define _DWORD_H_

#include "cport.h"

ALDOR_C_BEGIN
 
extern void     xxTestGtDouble(int *, ULong, ULong, ULong, ULong);
extern void     xxPlusStep    (ULong *, ULong *, ULong, ULong, ULong);
extern void     xxTimesStep   (ULong *, ULong *, ULong, ULong, ULong, ULong);

#ifndef OPT_NoDoubleOps
extern void     xxTimesDouble (ULong *, ULong *, ULong, ULong);
extern ULong    xxModDouble   (ULong, ULong, ULong);
extern void     xxDivideDouble(ULong *, ULong *, ULong *,
                               ULong, ULong, ULong);
#endif

ALDOR_C_END

#endif /*!_DWORD_H_*/
