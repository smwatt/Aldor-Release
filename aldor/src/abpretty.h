/*****************************************************************************
 *
 * abpretty.h: Pretty print abstract syntax to produce inputable Aldor code
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

#ifndef _ABPRETTY_H_
#define _ABPRETTY_H_

# include "axlobs.h"

#define ABPP_UNCLIPPED  (200000L)
#define ABPP_NOINDENT   (-1)

extern MutString abPretty                  (AbSyn);
extern MutString abPrettyClippedIn         (AbSyn, long clip, int indent);
extern int    abPrettyPrint             (FILE *, AbSyn);
extern int    abPrettyPrintClippedIn    (FILE *, AbSyn, long clip, int indent);

extern MutString tfPretty                  (TForm);
extern MutString tfPrettyClippedIn         (TForm, long clip, int indent);
extern int    tfPrettyPrint             (FILE *, TForm);
extern int    tfPrettyPrintClippedIn    (FILE *, TForm, long clip, int indent);

extern MutString symePretty                (Syme);
extern MutString symePrettyClippedIn       (Syme, long clip, int indent);

#endif
