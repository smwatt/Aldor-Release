/*****************************************************************************
 *
 * gf_imps.h: Routines dealing with retrieving lazy imports
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

#ifndef _GF_IMPS_H_
#define _GF_IMPS_H_

extern void     gen0IssueLazyFunctions  (void);
extern void     gen0InitImport          (Syme);
extern Foam     gen0GetDomainLex        (TForm);
extern void     gen0GetLazyImport       (void);
extern Foam     gen0GetDomImport        (Syme, Foam);
extern Foam     gen0LazyValue           (Foam, Syme);
extern Foam     gen0GetLazyBuiltin      (MutString, AInt, Length, Length);

extern void     gen0InitGVectTable      (void);
extern void     gen0IssueGVectFns       (void);
extern void     gen0FiniGVectTable      (void);

#define         gen0IsLazyConst(tf)     !(tfIsAnyMap(tf) || tfSatType(tf))

typedef struct _GenSaveState {
        GenFoamState state;
        FoamList     savedLines;
        FoamList     *savedPlace;
        FoamList     *wherePlace;
        bool         deep;
        int          idx;
} GenSaveState;

extern int      gen0MoveToImportPlace           (GenSaveState *, AInt);
extern void     gen0RestoreFromImportPlace      (GenSaveState *);

void    gen0StdLazyGetsCreate                   (void);

#endif
