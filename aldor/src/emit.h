/*****************************************************************************
 *
 * emit.h: Compiler output.
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

#ifndef _EMIT_H_
#define _EMIT_H_

# include "axlobs.h"

/*
 * Controlling options.
 */
extern int    emitSelect        (MutString);    /* Parse and handle -F option.  */
extern void   emitAllDone       (void);
extern void   emitSetDone       (int);

extern void   emitSetEntryFile  (MutString);    /* Set entry file:   -e         */
extern int    emitSetOutputDir  (MutString);    /* Output directory: -R         */
extern int    emitSetOutputFile (FTypeNo,MutString);
                                             /* Select output:    -F<ft>=<fn>*/
extern void   emitSetCfgFile    (MutString);

extern void   emitSetCName      (String);    /* Prefix for C names.          */
extern void   emitSetDebug      (bool);      /* Want debug info:  -Zg        */
extern void   emitSetProfile    (bool);      /* Want profile info:-Zp        */
extern void   emitSetRun        (bool);      /* Run result:       -go        */
extern void   emitSetInterp     (bool);      /* Run result:       -g[fi]     */
extern void   emitSetStandardC  (bool);      /* -Cstandard vs -Coldc.        */

extern void   emitDoneOptions   (int argc, MutString *argv);

extern MutString emitGetEntryFile  (void);

/*
 * File information structure.
 */
typedef struct emitInfo {
        FileName *      fnameTempV;
        FileName        fname[FTYPENO_LIMIT];
        BPack(bool)     inUse[FTYPENO_LIMIT];
        FileNameList    flist;
        bool            isAXLmain; /* Is the generated file invoking "entry"? */
        bool            isStdIn;
} *EmitInfo;

extern EmitInfo emitInfoNew             (FileName srcfn);
extern EmitInfo emitInfoNewAXLmain      (void);
extern void     emitInfoFree            (EmitInfo finfo);

extern void     emitSetDependsWanted    (bool);
extern bool     emitDependsWanted       (void);

/*
 * File names to use.
 */

extern FileName emitSrcFile             (EmitInfo);
extern FileName emitFileName            (EmitInfo, FTypeNo);

extern bool     emitIsOutputNeeded      (EmitInfo, FTypeNo);
extern bool     emitIsOutputNeededOrWarn(EmitInfo, FTypeNo);
extern EmitInfo emitExecutableNameOrWarn(int, EmitInfo *);

extern bool     emitIsGeneratedFile     (FileName);     /* Examines contents */

extern MutString   emitGetFileIdName       (EmitInfo);
extern void     emitSetFileIdName       (MutString);
extern void     emitSetFileIdPrefix     (MutString);

/*
 * Syme list collectors.
 */

extern SymeList emitCollectIntermedSymes(Stab, Foam);

/*
 * Dumb emitters -- just do what they're told.
 */
extern void     emitTheIncluded          (EmitInfo, SrcLineList);
extern void     emitTheOldAbSyn          (EmitInfo, AbSyn);
extern void     emitTheAbSyn             (EmitInfo, AbSyn);
extern void     emitTheIntermed          (EmitInfo, SymeList, Foam, AbSyn);
extern void     emitTheDependencies      (EmitInfo);
extern void     emitTheSymbolExpr        (EmitInfo, SymeList, AbSyn);
extern void     emitTheSymbolExprExtended(EmitInfo, SymeList, AbSyn);
extern void     emitTheFoamExpr          (EmitInfo, Foam);
extern void     emitTheLisp              (EmitInfo, SExpr);
extern void     emitTheC                 (EmitInfo, CCodeList);
extern void     emitTheCpp               (void);
extern void     emitTheObject            (EmitInfo);

/*
 * Linkage, execution, and cleanup.
 */
extern void     emitLink                (int, EmitInfo *);
extern int      emitRun                 (int, MutString   *);
extern void     emitInterp              (int, MutString   *);
extern void     emitCleanup             (int, EmitInfo *);

#endif /* !_EMIT_H_ */
