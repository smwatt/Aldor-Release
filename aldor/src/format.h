/*****************************************************************************
 *
 * format.h: Code for prettyprinting.
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

#ifndef _FORMAT_H_
#define _FORMAT_H_

# include "axlport.h"

ALDOR_C_BEGIN

extern int  findent;
extern int  fnewline    (FILE *f);

/*
 * xprintf takes a function to put the characters.
 *
 * The XPutFun consumes at most n characters of s and returns the count.
 * If n == -1 no limit is imposed.
 * A null function pointer computes the count without doing output.
 */
typedef int (*XPutFun)  (const char *s, int n);

extern  int  xprintf    (XPutFun f, const char *fmt, ...);
extern  int  vxprintf   (XPutFun f, const char *fmt, va_list argp);
ALDOR_C_END

#endif
