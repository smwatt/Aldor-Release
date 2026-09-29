/*****************************************************************************
 *
 * textcolour.h: text highlighting
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

#ifndef _TEXTCOLOUR_H_
#define _TEXTCOLOUR_H_

#include "axlobs.h"
#include "termtype.h"
#include "textansi.h"
#include "texthp.h"

/* Exported functions */
extern MutString tcolPrefix(CoMsgTag);
  /*
   * tcolPrefix(c) returns the text to be emitted before a message
   * of class c.
   */

extern MutString tcolPostfix(CoMsgTag);
  /*
   * tcolPostfix(c) returns the text to be emitted after a message
   * of class c.
   */

#endif /* _TEXT_COLOUR_H_ */
