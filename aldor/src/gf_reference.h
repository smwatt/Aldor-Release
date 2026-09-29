/*****************************************************************************
 *
 * gf_reference.h: Foam code generation for reference expressions
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

#ifndef _GF_REFERENCE_H_
#define _GF_REFERENCE_H_

 
extern Foam genReference(AbSyn);
extern Foam genReferenceFrFoam(Foam, TForm, AbSyn);
extern AInt gen0MakeRefFormat(void);

#endif /*!_GF_REFERENCE_H_*/
