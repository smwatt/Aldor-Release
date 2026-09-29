/*****************************************************************************
 *
 * strops.h: MutString manipulations which can allocate.
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

/*
 * This file is called "strops.h" since on some systems it is not possible
 * to have both "string.h" and <string.h>.
 */

#ifndef _STROPS_H_
#define _STROPS_H_

# include "axlport.h"

extern MutString           strOfChars      (char *);
#define                 strChars(s)     (s)

extern Length           strLength       (String);
extern Length           strUntabLength  (String, Length tabstop);

extern MutString           strCopy         (String);
extern MutString           strnCopy        (String s, Length n);
extern MutString           strCopyIf       (String);
                        /*
                         * strCopy   allocates store and copies its argument.
                         * strnCopy  same but copies at most n characters.
                         * strCopyIf same but returns 0 if its argument was 0.
                         */

extern MutString           strConcat       (String, String);
extern MutString           strlConcat      (String, ...);
                        /*
                         * These functions allocate a new string
                         * and concatenate the arguments into it.
                         * strlConcat's argument list is terminated by a 0.
                         *
                         * E.g., r = strlConcat(s1, s2, ..., sn, NULL)
                         */

extern MutString           strPrintf  (const char *, ...);
extern MutString           strVPrintf (const char *, va_list);
                        /*
                         * Allocate a new string and print into it.
                         */
extern MutString           strAlloc        (Length);
extern void             strFree         (MutString);
extern MutString           strResize       (MutString, Length);

extern bool             strEqual        (String, String); /* case-sensitive  */
extern bool             strAEqual       (String, String); /* case-insensitive*/

extern Hash             strHash         (String);         /* case-sensitive  */
extern Hash             strSmallHash    (String);         /* less filling  */
extern Hash             strAHash        (String);         /* case-insensitive*/

extern Length           strMatch        (String, String);
extern Length           strAMatch       (String, String);

extern String      strIsPrefix     (String pre, String);
extern String      strAIsPrefix    (String pre, String);

extern String      strIsSuffix     (String suf, String);
extern String      strAIsSuffix    (String suf, String);

extern MutString           strUpper        (MutString);
extern MutString           strLower        (MutString);

extern String      strnToAsciiStatic(String, Length);
extern MutString           strnFrAsciiStatic(MutString, Length);

extern int              strPrint        (FILE *, String,
                                         int oq, int cq,
                                         int e, const char *fmt);
                        /*
                         * Print string with quotes (oq, cq) and escape
                         * character e.  fmt is used to print the unprintables.
                         * The character count is returned.
                         */

#endif /* !_STROPS_H_ */
