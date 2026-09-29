/*****************************************************************************
 *
 * axltop.h: All definitions for compiler.
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

/*
 * This includes all definitions needed anywhwere in the compiler
 * and should only be used by the top level files.
 * 
 * Each of these includes the ones before it and the most specific one
 * possible should be used.
 *
 *      cport.h         -- Portable C99/C++20 compilation environment
 *      axlport.h       -- Additional portability definitions
 *
 *      axlgen0.h       -- General functions on standard types
 *      axlgen.h                -- Additional general types
 *
 *      axlobs.h                -- Compiler structures
 *      axlphase.h      -- Compiler phases
 *
 *      axltop.h                -- Everything about the compiler (this file)
 */

#ifndef _AXLTOP_H_
#define _AXLTOP_H_

/* Pre-empt the include by axlphase.h to avoid too deep nesting. */
# include "axlgen.h"

# include "axlphase.h"

# include "cmdline.h"
# include "axlcomp.h"

#endif  /* !_AXLTOP_H_ */
