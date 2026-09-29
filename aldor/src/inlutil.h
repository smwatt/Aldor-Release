/*****************************************************************************
 *
 * inlutil.h: Utilities for the inliner.
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

#ifndef _INLUTIL_H_
#define  _INLUTIL_H_

#include "axlobs.h"

extern Foam     inuGetClosFrVar         (Foam);

extern Syme     inuGetSymeFrClosVar     (Foam);
extern Syme     inuGetSymeFrEnv         (Foam);

extern Foam     inuGetConstEnvFrClosVar (Foam);
extern Foam     inuGetConstEnvFrEnv     (Foam);

extern Foam     inuGetPushEnvFrClosVar  (Foam);
extern Foam     inuGetPushEnvFrVar      (Foam);
extern bool     inuIsLocalEnv           (Foam);

extern void     inuProgInit             (Foam);
extern void     inuProgUpdate           (Foam);
extern void     inuProgFini             (Foam);

extern void     inuUnitInit             (Foam);
extern void     inuUnitFini             (Foam);

extern Foam     inuPeepExpr             (Foam);

extern void     inuUnitInfoRefresh      (Foam);

#endif
