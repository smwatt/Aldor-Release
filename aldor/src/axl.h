/*****************************************************************************
 *
 * axl.h: Aldor compiler top-level entry point
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

#ifndef _AXL_H_
#define _AXL_H_

# include "axlport.h"

extern int              compCmd(int argc, MutString *argv);
                        /*
                         * Command line interface.
                         */

#endif /* !_AXL_H_ */
