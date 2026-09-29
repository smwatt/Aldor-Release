/*****************************************************************************
 *
 * fint.h: foam interpreter.
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

#ifndef _FINT_H_
#define _FINT_H_

extern void     fintInit                (void);
extern void     fintFini                (void);
extern bool     fint                    (Foam);
extern void     fintFile                (FileName);
extern void     fintInitFile            (void);
extern void     fintPrintType           (FILE *, AbSyn);


extern AbSyn    fintWrap                (AbSyn, int);
extern void     fintPrintType           (FILE *, AbSyn);
extern void     fintParseOptions        (MutString);
extern bool     fintYesOrNo             (String);
extern bool     fintIsCompilerSyntax    (void);

extern void     fintGetInitCompTime     (void);
extern void     fintDisplayTimings      (void);

extern void     fintWhere               (int);
extern void     fintRaiseException(char *, void *);

//extern int      compGDebugger           (int, char **, FILE *, FILE *);
extern int      compCmd                 (int, char**);
extern void     compFini                (void);

extern int      fintMode;
#define                 FINT_DONT       0
#define                 FINT_RUN        1
#define                 FINT_LOOP       2
#define                 FINT_DEBUGGER   3

#define                 memory_alignment(n, d) (((n)+sizeof(double)) % (d) ? (n) + (d) - (n) % (d) : (n))
#define                 MAXLINE 256

extern bool     fintVerbose;
extern bool     fintHistory;
extern void     fintDebugProg(FileName fname);
extern UShort   intStepNo;

#endif    /* _FINT_H */


