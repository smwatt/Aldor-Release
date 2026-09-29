/*****************************************************************************
 *
 * gf_gener.h: Foam code generation for generators.
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

#ifndef _GF_GENER_H_
#define _GF_GENER_H_

# include "axlobs.h"


extern Foam      genGenerate            (AbSyn);
extern Foam      genYield               (AbSyn);
extern Foam      gen0RetFormatDDecl     (void);
extern AInt      gen0MakeGenerRetFormat (void);
extern Foam      gen0GenLiftedGener     (AbSyn, AbSyn);

#endif /* !_GF_GENER_H_ */
