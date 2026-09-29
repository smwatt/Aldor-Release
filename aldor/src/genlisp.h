/*****************************************************************************
 *
 * genlisp.h: Foam-to-Lisp translation.
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

#ifndef _GENLISP_H_
#define _GENLISP_H_

# include "axlobs.h"

extern int      genLispOption(MutString);

extern SExpr    genLisp(Foam);

extern ULong    glWriteMode;    /* used to write .l files */

#endif /* !_GENLISP_H_ */
