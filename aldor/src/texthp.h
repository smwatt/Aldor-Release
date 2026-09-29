/*****************************************************************************
 *
 * texthp.h: text utility functions for HP terminals
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

#ifndef _TEXTHP_H_
#define _TEXTHP_H_

#include "axlobs.h"

/* Expanded AF/AB escapes must not exceed HP_ESCAPE_MAXLEN in length */
#define HP_ESCAPE_MAXLEN 512

/*
 * HP terminal colour pairs. The RGB settings of the eight colour-pairs
 * may vary from system to system ...
 */
enum colourPair_enum {
   DefaultOnDefault = 0,
   RedOnDefault,
   GreenOnDefault,
   YellowOnDefault,
   BlueOnDefault,
   MagentaOnDefault,
   CyanOnDefault,
   DefaultOnYellow
};
typedef enum colourPair_enum ColourHP;

/* Exported operations */

extern String txtBoldHP(void);
        /*
         * txtBoldHP() returns a string of escape codes for enabling the bold
         * terminal text attribute (extra bright). The only way to remove this
         * attribute is to write txtNormalHP() onto the stream. Note that we
         * assume term_type() returns a valid HP terminal (not UnknownTerm).
         */

extern String txtNormalHP(void);
        /*
         * txtNormalHP() returns a string of escape codes for disabling
         * all terminal text attributes currently in force. It also resets
         * the current colour pair to the default colour pair. Note that we
         * assume term_type() returns a valid HP terminal (not UnknownTerm).
         */

extern String txtColourHP(ColourHP);
        /*
         * txtColourHP(n) returns a string of escape codes for setting the
         * terminal foreground and background colours to colour-pair "n".
         * Colour-pair 0 corresponds to the default foreground/background.
         */
#endif /* _TEXTHP_H_ */
