/*****************************************************************************
 *
 * debug.h: Debugging code.
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

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "axlport.h"

#if defined(NDEBUG)

#   define DEBUG_MODE(flag)             Nothing
#   define DO_DEBUG(e)                  Nothing
#   define DEBUG_IF(v,e)                Nothing
#   define DEBUG_CONFIG                 ""
#   define DEBUG_DECL(s)                Nothing
#   define debugStream                  stdout

#else
    extern bool dbFlag;

#   define DEBUG_MODE(flag)             (dbFlag = (flag))
#   define DO_DEBUG(e)                  Statement(if (dbFlag) {e;})
#   define DEBUG_IF(v,e)                Statement(if (v) {e;})
#   define DEBUG_CONFIG                 "(debug version)"
#   define DEBUG_DECL(s)                s
#   define debugStream                  dbOut


#endif

extern  FILE    *dbIn, *dbOut;

extern  void    dbInit  (void);

#endif /* _DEBUG_H_ */
