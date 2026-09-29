/*****************************************************************************
 *
 * ablogic.h: Structures for inference about conditional exports.
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

#ifndef _ABLOGIC_H_
#define _ABLOGIC_H_

#include "axlobs.h"

/*
 * The abLogic structure is used to keep track of conditions known or required
 * in type inference for conditional exports.
 */

struct abLogic {
        double  fake;
};

extern AbLogic  abCondKnown;      /* Conditions with known value (tinfer) */
extern AbLogic  gfCondKnown;      /* Ditto (genfoam) */

extern void     ablogInit         (void);
extern void     ablogFini         (void);

extern AbLogic  ablogFrSefo       (Sefo);

extern AbLogic  ablogCopy         (AbLogic);
extern void     ablogFree         (AbLogic);
extern int      ablogPrint        (FILE *, AbLogic);
extern int      ablogPrintDb      (AbLogic);

extern AbLogic  ablogTrue         (void);
extern AbLogic  ablogFalse        (void);
extern AbLogic  ablogNot          (AbLogic);
extern AbLogic  ablogAnd          (AbLogic, AbLogic);
extern AbLogic  ablogOr           (AbLogic, AbLogic);

extern bool     ablogIsTrue       (AbLogic);
extern bool     ablogIsFalse      (AbLogic);

extern bool     ablogEqual        (AbLogic, AbLogic);
extern bool     ablogIsImplied    (AbLogic, Sefo cond);
extern bool     ablogImplies      (AbLogic, AbLogic);
extern bool     ablogIsListImplied(AbLogic, SefoList);
extern bool     ablogIsListKnown  (SefoList sefolist);
 
extern void     ablogAndPush      (AbLogic* glo,AbLogic* save,Sefo,bool sense);
extern void     ablogAndPop       (AbLogic* glo,AbLogic* save);

extern int      bputAblog         (Buffer, AbLogic);
#endif /* !_ABLOGIC_H_ */
