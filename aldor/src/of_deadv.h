/*****************************************************************************
 *
 * of_deadv.h: Dead variable elimination.
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

#ifndef _OF_DEADV_H_
#define _OF_DEADV_H_

# include "axlobs.h"

extern void             dvInit                  (void);
extern void             dvElim                  (Foam);

#endif /* !_OF_DEADV_H_ */
