/*****************************************************************************
 *
 * sto_debug.h: Storage manager debugging.
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

#ifndef _STO_DEBUG_H_
#define _STO_DEBUG_H_

# include "store.h"

ALDOR_C_BEGIN

/*
 * These bits may turned on or off independently.
 */

extern int stoDebugWhen;
#  define STO_DEBUG_ON_ALLOC           0x1
#  define STO_DEBUG_ON_FREE            0x2
#  define STO_DEBUG_ON_EXIT            0x4

extern int stoDebugWhat;
#  define STO_DEBUG_DO_ALLOC_COUNT     0x1
#  define STO_DEBUG_DO_FREE_CHECK      0x2
#  define STO_DEBUG_DO_SENTINEL_CHECK  0x4
#  define STO_DEBUG_DO_OVERFLOW_CHECK  0x8

extern int sdbCheckAll(void);

ALDOR_C_END

#endif /* !_STO_DEBUG_H_ */
