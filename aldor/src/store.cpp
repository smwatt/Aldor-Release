///////////////////////////////////////////////////////////////////////////////
//
// store.cpp: Storage management.
//
///////////////////////////////////////////////////////////////////////////////

// This file is part of Aldor.
//
// Aldor is licensed under the Apache License, Version 2.0.
//
//
// See legal/LICENSE in the Aldor distribution for details.
//
// Copyright (C) 1990-2026 Stephen M. Watt.

#include "store.h"
#include "storeconcept.hpp"

/*
 *   Set STO_USE to be one of:
 *      STO_USE_BTREE   B-Tree based quick fit.
 *      STO_USE_ONCE    Don't look back.
 *      STO_USE_MALLOC  Based on malloc/free.
 */

#define STO_USE_BTREE       1
#define STO_USE_ONCE        2
#define STO_USE_MALLOC      3
#define STO_USE_BOEHM       4

#define STO_USE_MIN_LAYERED 2   /* Alloc >= this use layered implementation. */

/*
 * If allocator is not specified, give a default.
 */
#if !defined(STO_USE)
# define STO_USE STO_USE_BTREE
#endif

/* this will create a debug version of GC that should be safer */
#undef NDEBUG

int   __stoLine = 0;
String __stoFile = "--NONE--";

/* Preload implementation dependencies before entering C linkage. */
#include "axlgen0.h"
#include "btree.h"
#include "memclim.h"
#include "util.h"
#if defined(OS_WINDOWS)
#include <windows.h>
#endif
#if STO_USE == STO_USE_BOEHM
#include <gc.h>
#endif

/*
 * Include the implementation requested.  These implementation files are
 * historically C and their externally visible symbols retain C linkage.
 */
#if STO_USE == STO_USE_BTREE
ALDOR_C_BEGIN
#  include "sto_btree.c0"
ALDOR_C_END
#endif

#if STO_USE == STO_USE_ONCE
#  include "sto_once.c0"
#endif

#if STO_USE == STO_USE_MALLOC
#  include "sto_malloc.c0"
#endif

#if STO_USE == STO_USE_BOEHM
#  include "sto_boehm.c0"
#endif
