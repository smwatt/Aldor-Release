/*****************************************************************************
 *
 * gf_fortran.h: Foam code generation for the Aldor/Fortran interface.
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

#ifndef _GF_FORTRAN_H_
#define _GF_FORTRAN_H_

# include "axlobs.h"

/* Aldor-calls-Fortran */
extern Foam     gen0ModifyFortranCall   (Syme, Foam, AbSyn, bool);
extern Foam     gen0MakePointerTo       (FoamTag, Foam, FoamList *);
extern Foam     gen0ReadPointerTo       (FoamTag, Foam);
extern Foam     gen0WritePointerTo      (FoamTag, Foam, Foam);


/* Fortran-calls-Aldor */
extern void     gen0ExportToFortran     (AbSyn fun);
extern Foam     gen0FortranExportFn     (TForm, FoamTag, Foam, String, AbSyn);

#endif /* !_GF_FORTRAN_H_ */
