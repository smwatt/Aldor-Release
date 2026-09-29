/*****************************************************************************
 *
 * parseby.h: Various support functions for the parser.
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
 * The main job is done by axl.y.
 */
#ifndef _PARSEBY_H_
#define _PARSEBY_H_

# include "axlobs.h"

extern AbSyn parse             (TokenList *prest);

extern AbSyn yypval;
extern int   yylex             (void);
extern int   yyerrorfn         (String);

#ifndef UNUSED_LABELS
# define UNUSED_LABELS         /* Needed for Bison output. */
#endif

/*
 * Node forming operations which can cause messages go here -- not in absyn.*.
 */
extern AbSyn parseMakeJuxtapose(AbSyn, AbSyn);
extern AbSyn parseDeprecated(TokenTag, AbSyn);

#endif /* !_PARSEBY_H_ */
