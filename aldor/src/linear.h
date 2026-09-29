/*****************************************************************************
 *
 * linear.h: Bracketing of piles.
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
 * Convert a two dimensional list of lines of tokens to a flat list.
 * The layout information is encodes as SETTAB, BACKSET and BACKTAB tokens.
 *
 * The lists of tokens in the original lines get modified.
 */

#ifndef _LINEAR_H_
#define _LINEAR_H_

# include "axlobs.h"

extern TokenList        linearize       (TokenList tl);

extern Token            linDoPileStart  (SrcPos);
extern Token            linDoPileEnd    (SrcPos);
extern TokenList        linDoPileWrap   (TokenList);


extern  bool            linIsPileOn     (void);
extern  void            linPilePush     (bool);
extern  void            linPilePop      (void);
extern  void            linResetPileStack(void);


#endif /* !_LINEAR_H_ */
