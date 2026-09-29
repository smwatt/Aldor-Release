/*****************************************************************************
 *
 * of_cfold.h: Foam constant folding.
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

#ifndef _OF_CFOLD_H_
#define _OF_CFOLD_H_

# include "axlobs.h"
# include "foam_c.h" /* For fiType*() calls */

extern bool             cfoldUnit               (Foam, bool, bool);

#endif /* !_OF_CFOLD_H_ */
