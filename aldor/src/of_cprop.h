/*****************************************************************************
 *
 * of_cprop.h: Copy propagation
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

#ifndef OF_CPROP_H
#define OF_CPROP_H

# include "axlobs.h"

/****************************************************************************
 *
 * :: External entry points
 *
 ****************************************************************************/

extern void     cpropUnit       (Foam, bool);
extern bool     cpFlog          (FlowGraph);

#endif


