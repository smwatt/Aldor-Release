/*****************************************************************************
 *
 * genfoam.h: Foam code generation.
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

#ifndef _GENFOAM_H_
#define _GENFOAM_H_

# include "axlobs.h"

extern Foam     generateFoam    (Stab, AbSyn, MutString);

extern bool     genIsConst      (Syme);
extern bool     genIsLocalConst (Syme);
extern bool     genIsVar        (Foam);

extern void     genSetConstNum          (Syme, int, UShort, bool);
extern bool     genHasConstNum          (Syme);
extern UShort   genGetConstNum          (Syme);
extern void     genGetConstNums         (SymeList);
extern SymeList genGetSymeInlined       (Syme);
extern void     genKillOldSymeConstNums (int);

extern void     genSetAxiomAx           (bool);
extern void     genSetDebugWanted       (bool);
extern void     genSetDebuggerWanted    (bool);
extern void     genSetSmallHashCodes    (bool);

extern Foam     gen0ApplyReturn         (AbSyn, Syme, TForm, Foam);
extern Foam     gen1ApplyReturn         (AbSyn, Syme, TForm, Foam, Foam *);
extern Foam     gen0CCallFrFoam         (FoamTag, Foam, Length, Foam **);

extern bool     gen0IsFortranCall       (AbSyn);

extern AInt     gen0CSigFormatNumber(TForm);
extern AInt     gen0FortranSigExportNumber(TForm);

extern AInt     gen0CatchFormatNumber   (TForm, TForm);
extern AInt     gen0VoidCatchFormatNumber(TForm);


/* COND-DEF */
struct GF_COND
{
        Syme    syme;
        AbLogic condition;
};

typedef struct GF_COND _GfCond;
typedef _GfCond *GfCond;

#endif /* !_GENFOAM_H_ */
