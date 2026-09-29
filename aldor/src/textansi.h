/*****************************************************************************
 *
 * textansi.h: text utility functions for ANSI/Tektronix terminals
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

#ifndef _TEXTANSI_H_
#define _TEXTANSI_H_

#include "axlobs.h"

/* ANSI terminal colours */
enum colourName_enum {
        NormalColour = -1, /* Our special value */
        Black = 0,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White
};
typedef enum colourName_enum ColourANSI;

/* Expanded AF/AB escapes must not exceed ANSI_ESCAPE_MAXLEN in length */
#define ANSI_ESCAPE_MAXLEN 512

/* Exported operations */
extern String txtBoldANSI(void);
  /*
   * txtBoldANSI() returns a string of escape codes for enabling the
   * bold terminal text attribute (extra bright). The only way to remove
   * this attribute is to write txtNormalANSI() onto the stream and
   * then set the foreground and background colours again. Note that we
   * assume term_type() returns a valid ANSI terminal (not UnknownTerm).
   */

extern String txtNormalANSI(void);
  /*
   * txtNormalANSI() returns a string of escape codes for disabling all
   * terminal text attributes currently in force. Note that we assume
   * term_type() returns a valid ANSI terminal (not UnknownTerm).
   */

extern String txtForegroundANSI(ColourANSI);
  /*
   * txtForegroundANSI(col) returns a string of escape codes for setting
   * the terminal text foreground colour to "col". Note that we assume
   * term_type() returns a valid ANSI terminal (not UnknownTerm).
   */

extern String txtBackgroundANSI(ColourANSI);
  /*
   * txtBackgroundANSI(col) returns a string of escape codes for setting
   * the terminal text background colour to "col". Note that we assume
   * term_type() returns a valid ANSI terminal (not UnknownTerm).
   */

extern String txtColourANSI(ColourANSI, ColourANSI);
  /*
   * txtColourANSI(fg,bg) returns a string of escape codes for setting
   * the terminal foreground and background colours to "fg" and "bg"
   * respectively. Use NormalColour for "fg" or "bg" values that are to
   * remain unchanged (e.g. to allow just the background colour to be set).
   */
#endif /* _TEXTANSI_H_ */
