/*****************************************************************************
 *
 * absub.h: Semantic substitution.
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

#ifndef _ABSUB_H_
#define _ABSUB_H_

#include "axlobs.h"

/******************************************************************************
 *
 * :: Substitution bindings.
 *
 *****************************************************************************/

struct abBind {
        Syme            key;            /* Symbol meaning to be replaced. */
        Sefo            val;            /* Fully type-analysed replacement. */
};

/******************************************************************************
 *
 * :: Substitution lists.
 *
 *****************************************************************************/

struct abSub {
        BPack(bool)     self;           /* Are we substituting for self? */
        BPack(bool)     lazy;           /* Are we allowing lazy subst? */
        ULong           serialNo;       /* Unique serial number. */
        ULong           refc;           /* Reference count. */
        AbBindList      l;              /* Association list. */

        Stab            stab;           /* Symbol table. */
        Table<void *, void *> results;        /* Table of results. */
        FreeVar         fv;             /* Free variables in the values. */
};

#define                 absStab(sigma)          ((sigma)->stab)
#define                 absSelf(sigma)          ((sigma)->self)
#define                 absLazy(sigma)          ((sigma)->lazy)
#define                 absSerial(sigma)        ((sigma)->serialNo)
#define                 absFVars(sigma)         ((sigma)->fv)

#define                 absSetSelf(sigma)       ((sigma)->self = true)
#define                 absSetLazy(sigma)       ((sigma)->lazy = true)
#define                 absClrLazy(sigma)       ((sigma)->lazy = false)
#define                 absSetFVars(sigma,v)    ((sigma)->fv = (v))

/*
 * Basic operations.
 */

extern AbSub            absNew                  (Stab);
extern AbSub            absFail                 (void);
extern int              absPrint                (FILE *, AbSub);
extern int              absPrintDb              (AbSub);
extern AbSub            absRefer                (AbSub);
extern void             absFree                 (AbSub);
extern void             absFreeDeeply           (AbSub);

extern bool             absIsEmpty              (AbSub);
extern bool             absHasSymes             (AbSub, SymeList);
extern AbSub            absExtend               (Syme, Sefo, AbSub);
extern Sefo             absLookup               (Syme, Sefo, AbSub);

extern void             absSetStab              (AbSub, Syme);
extern Syme             absSetSyme              (AbSub, Syme,  Syme);
extern TForm            absSetTForm             (AbSub, TForm, TForm);

extern Syme             absGetSyme              (AbSub, Syme);
extern TForm            absGetTForm             (AbSub, TForm);

extern Syme             absMarkSyme             (AbSub, Syme);
extern bool             absSymeIsMarked         (AbSub, Syme);

#endif /* !_ABSUB_H_ */
