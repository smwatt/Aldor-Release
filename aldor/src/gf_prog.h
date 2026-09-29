/*****************************************************************************
 *
 * gf_prog.h: Common declarations and macros for foam prog generation.
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

#ifndef _GF_PROG_H_
#define _GF_PROG_H_

# include "axlobs.h"

/*****************************************************************************
 *
 * :: Foam prog construction operations.
 *
 ****************************************************************************/

#define                 gen0NLabels()           (gen0State->labelNo)

extern Foam             gen0BuildFunction       (ProgType, MutString, AbSyn);

extern void             gen0ProgPushState       (Stab, GenFoamTag);
extern void             gen0ProgPopState        (void);
extern GenFoamState     gen0ProgSaveState       (ProgType);
extern void             gen0ProgUseBaseState    (void);
extern void             gen0ProgUseUpperState   (void);
extern void             gen0ProgRestoreState    (GenFoamState);
extern void             gen0ProgPushFormat      (AInt);
extern Foam             gen0ProgInitEmpty       (String, AbSyn);
extern void             gen0ProgFiniEmpty       (Foam, AInt, AInt);
extern Foam             gen0ProgClosEmpty       (void);

extern void             gen0ProgAddParams       (Length, String *);
extern void             gen0ProgAddFormat       (AInt);

extern AbSyn            gen0ProgGetExporter     (void);
extern AbSyn            gen0ProgPushExporter    (AbSyn);
extern void             gen0ProgPopExporter     (AbSyn);
extern void             gen0ProgAddExporterArgs (AbSyn);
extern void             gen0ProgPopExporterArgs (void);
extern bool             gen0ProgHasReturn       (void);

extern void             gen0AddInit             (Foam);
extern void             gen0AddStmt             (Foam, AbSyn);
extern void             gen0AddStmtNth          (Foam, AInt);
extern Foam             gen0SeqAdd              (Foam, Foam);
extern AInt             gen0AddParam            (Foam);
extern AInt             gen0AddLocal            (Foam);
extern AInt             gen0AddLex              (Foam);
extern AInt             gen0AddLexNth           (Foam, AInt, AInt);

#endif /* !_GF_PROG_H_ */
