/*****************************************************************************
 *
 * of_jflow.h: Test and jump optimization.
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

#ifndef _OF_JFLOW_H_
#define _OF_JFLOW_H_

# include "axlobs.h"

extern void             jflowUnit       (Foam, int);
extern void             jflowProg       (Foam);
extern void             jflowSetNegate  (bool);
extern bool             jflowCanNegate  (void);

#endif /* !_OF_JFLOW_H_ */
