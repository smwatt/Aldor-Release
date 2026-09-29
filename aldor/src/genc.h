/*****************************************************************************
 *
 * genc.h: Foam-to-C translation.
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

#ifndef _GENC_H_
#define _GENC_H_

# include "axlobs.h"

extern CCodeList        genC                    (Foam, MutString);
extern CCode            genAXLmainC             (MutString);

extern void             genCSetSMax             (int);
extern void             genCSetIdLen            (int);
extern void             genCSetIdHash           (bool);


/* Tracking Fortran functional parameter passing */
typedef struct
{
        MutString  base;   /* Name of function of which this is a parameter */
        AInt    num;    /* Functional parameter postion */
        CCode   fun;    /* Name of the parameter (wrapper function) */
        CCode   clos;   /* Name of the closure for Aldor called via wrapper */
        CCode   class_;  /* Storage class of parameter and closure */
} *FtnFunParam;


/* Accessers for FtnFunParam */
#define gc0FtnFunBase(p)        ((p)->base)
#define gc0FtnFunNumber(p)      ((p)->num)
#define gc0FtnFunName(p)        ((p)->fun)
#define gc0FtnFunClosure(p)     ((p)->clos)
#define gc0FtnFunClass(p)       ((p)->class_)

#endif /* !_GENC_H_ */
