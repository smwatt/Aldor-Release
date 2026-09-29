/*****************************************************************************
 *
 * sefo.h: Semantic forms
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

#ifndef _SEFO_H_
#define _SEFO_H_

# include "axlobs.h"

#define                 abIsSefo(ab)    (abState(ab) == AB_State_HasUnique)

/*
 * sstPrint
 */
extern int              sefoPrint               (FILE *, Sefo);
extern int              symePrint               (FILE *, Syme);
extern int              tformPrint              (FILE *, TForm);
extern int              sefoListPrint           (FILE *, SefoList);
extern int              symeListPrint           (FILE *, SymeList);
extern int              tformListPrint          (FILE *, TFormList);

/*
 * sstPrintDb
 */
extern int              sefoPrintDb             (Sefo);
extern int              symePrintDb             (Syme);
extern int              symePrintDb2            (Syme);
extern int              tformPrintDb            (TForm);
extern int              sefoListPrintDb         (SefoList);
extern int              symeListPrintDb         (SymeList);
extern int              tformListPrintDb        (TFormList);

/*
 * sstEqual
 */
extern bool             sefoEqual               (Sefo,  Sefo);
extern bool             symeEqual               (Syme,  Syme);
extern bool             tformEqual              (TForm, TForm);
extern bool             sefoListEqual           (SefoList,  SefoList);
extern bool             symeListEqual           (SymeList,  SymeList);
extern bool             tformListEqual          (TFormList, TFormList);

extern bool             symeIsTwin              (Syme, Syme);

/*
 * sstEqualMod
 */
extern bool             sefoEqualMod            (SymeList, Sefo,  Sefo);
extern bool             symeEqualMod            (SymeList, Syme,  Syme);
extern bool             tformEqualMod           (SymeList, TForm, TForm);
extern bool             sefoListEqualMod        (SymeList, SefoList, SefoList);
extern bool             symeListEqualMod        (SymeList, SymeList, SymeList);
extern bool             tformListEqualMod       (SymeList,TFormList,TFormList);

extern bool             symeEqualModConditions  (SymeList, Syme,  Syme);

/*
 * sefoCopy
 */
extern Sefo             sefoCopy                (Sefo);

/*
 * sefoAudit
 */
extern bool             sefoAudit               (bool, Sefo);

/*
 * sstFreeVars
 */
extern void             sefoFreeVars            (Sefo);
extern void             symeFreeVars            (Syme);
extern void             tformFreeVars           (TForm);
extern void             abSubFreeVars           (AbSub);
extern void             sefoListFreeVars        (SefoList);
extern void             symeListFreeVars        (SymeList);
extern void             tformListFreeVars       (TFormList);

/*
 * symeListSubst
 */
extern SymeList         symeListSubstSelf       (Stab, TForm, SymeList);
extern SymeList         symeListSubstCat        (Stab, SymeList, TForm,
                                                 SymeList);
extern SymeList         symeListSubstSigma      (AbSub, SymeList);
extern TForm            tformSubstSigma         (AbSub, TForm);
extern SymeList         symeListSubst           (AbSub, SymeList);

/*
 * sstSubst
 */
extern Sefo             sefoSubst               (AbSub, Sefo);
extern Syme             symeSubst               (AbSub, Syme);
extern TForm            tformSubst              (AbSub, TForm);

/*
 * symeList set operations.
 */
extern bool             symeListMember          (Syme,SymeList,SymeEqFun);
extern bool             symeListSubset          (SymeList,SymeList,SymeEqFun);
extern SymeList         symeListUnion           (SymeList,SymeList,SymeEqFun);
extern SymeList         symeListIntersect       (SymeList,SymeList,SymeEqFun);
extern bool             symeCloseOverDetails    (Syme);
extern void             symeListClosure         (Lib, SymeList);

/*
 * sstToBuffer
 */
extern int              sefoToBuffer            (Lib, Buffer, Sefo);
extern int              symeToBuffer            (Lib, Buffer, Syme);
extern int              tformToBuffer           (Lib, Buffer, TForm);
extern int              tqualToBuffer           (Lib, Buffer, TQual);
extern int              sefoListToBuffer        (Lib, Buffer, SefoList);
extern int              symeListToBuffer        (Lib, Buffer, SymeList);
extern int              tformListToBuffer       (Lib, Buffer, TFormList);
extern int              tqualListToBuffer       (Lib, Buffer, TQualList);

/*
 * sstFrBuffer
 */
extern Sefo             sefoFrBuffer            (Lib, Buffer);
extern Syme             symeFrBuffer            (Lib, Buffer);
extern TForm            tformFrBuffer           (Lib, Buffer);
extern TQual            tqualFrBuffer           (Lib, Buffer);
extern SefoList         sefoListFrBuffer        (Lib, Buffer);
extern SymeList         symeListFrBuffer        (Lib, Buffer);
extern TFormList        tformListFrBuffer       (Lib, Buffer);
extern TQualList        tqualListFrBuffer       (Lib, Buffer);

/*
 * tformType[cp]FrBuffer
 */
extern int              tformTypecFrBuffer      (Buffer);
extern void             tformTypepFrBuffer      (Buffer, int, int *);
extern void             symeListFrBuffer0       (Buffer);

/*
 * Debugging
 */
extern int sstGetMaxDepth(void);

#endif /* !_SEFO_H_ */
