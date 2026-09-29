/*****************************************************************************
 *
 * terror.h: Type errors.
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

#ifndef _TERROR_H_
#define _TERROR_H_

#include "axlobs.h"

extern bool terror (Stab stab, AbSyn absyn, TForm type);
extern void terrorNoTypes           (Msg, AbSyn, TForm, TPoss);
extern void terrorNotUniqueType     (Msg, AbSyn, TForm, TPoss);
extern void terrorNotUniqueMeaning  (Msg, AbSyn, SymeList, SymeList, String,
                                     TForm);

extern void terrorNotEnoughExports  (AbSyn, TPoss, bool);
extern void terrorAssign            (AbSyn, TForm, TPoss);
extern void terrorSetBang           (Stab, AbSyn, Length, AbSynGetter);
extern void terrorTypeConstFailed   (TConst);
extern void terrorApplyFType        (AbSyn, TForm, TPoss, AbSyn op, Stab,
                                     Length argc, AbSynGetter argf);
extern void terrorIdCondition       (TForm, AbSyn, AbLogic, AbLogic);
extern void terrorApplyCondition    (AbSyn, TForm, AbSyn, AbLogic, AbLogic);
extern void terrorApplyNotAnalyzed  (AbSyn, AbSyn, TForm);
extern void terrorMeaningsOutOfScope(Stab, AbSyn, AbSyn, TForm,
                                     Length argc, AbSynGetter argf);
extern void terrorNoMeaningForId(AbSyn,String);


extern bool terrorAuditPoss         (bool verbose, AbSyn absyn);
extern bool terrorAuditBottomUp     (bool verbose, AbSyn absyn);
extern bool terrorAuditTopDown      (bool verbose, AbSyn absyn);

#define TUNI_INAPPROPRIATE_TPOSS_CODE   12UL
#define TUNI_NOVALUE_TPOSS_CODE          4UL
#define TUNI_UNKNOWN_TPOSS_CODE          8UL
#define TUNI_ERROR_TPOSS_CODE           14UL

#define tuniInappropriateTPoss          ((TPoss) TUNI_INAPPROPRIATE_TPOSS_CODE)
#define tuniNoValueTPoss                ((TPoss) TUNI_NOVALUE_TPOSS_CODE)
#define tuniUnknownTPoss                ((TPoss) TUNI_UNKNOWN_TPOSS_CODE)
#define tuniErrorTPoss                  ((TPoss) TUNI_ERROR_TPOSS_CODE)

#define tuniIsInappropriate(tposs)      ((tposs) == tuniInappropriateTPoss)
#define tuniIsNoValue(tposs)            ((tposs) == tuniNoValueTPoss)
#define tuniIsUnknown(tposs)            ((tposs) == tuniUnknownTPoss)
#define tuniIsError(tposs)              ((tposs) == tuniErrorTPoss)

/* NOTE: don't change the number; sorting procedures in terror.c use it */
#define TR_BadFnType                    1
#define TR_BadArgType                   2
#define TR_ArgMissing                   3
#define TR_EmbedFail                    4
#endif /* !_TERROR_H_ */



