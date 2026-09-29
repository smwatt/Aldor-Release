/*****************************************************************************
 *
 * strops_t.c: Test string operations.
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

#if !defined(TEST_STROPS) && !defined(TEST_ALL)

void testString(void) { }

#else

# include "axlgen.h"

MutString
testStrVPrintf(const char *fmt, ...)
{
        MutString  s;
        va_list argp;
        va_start(argp, fmt);
        s = strVPrintf(fmt, argp);
        va_end(argp);
        return s;
}

void
testString(void)
{
        MutString  s0, joined;
        String s, s1, s2, s3;
        int     n;

        s1 = "Pre";  s2 = "Prefix";  s3 = "Postfix";
        printf("strIsPrefix:\n");
        s = strIsPrefix(s1, s2);
        if (s != 0) {
                printf("The prefix is \"%s\", ",s1);
                printf("the rest is \"%s\".\n",s);
        }
        printf("strIsSuffix:\n");
        s = strIsSuffix("fix", s3);
        if (s != 0) {
                printf("\"%s\" is a suffix of \"%s\".\n",s,s3);
        }
        printf("strMatch:\n");
        printf("The first %d characters of \"%s\" and \"%s\" match.\n",
#if EDIT_1_0_n1_07
                (int) strMatch(s2, s3),s2,s3);
#else
                strMatch(s2, s3),s2,s3);
#endif

        printf("strCopy:\n");
        s0 = strCopy(s2);
        printf("\"%s\" and \"%s\" match\n",s2,s0);

        printf("strFree:\n");
        strFree(s0);

        n = 3;
        printf("strnCopy:\n");
        s0 = strnCopy(s2,n);
        printf("The first %d characters of \"%s\" are \"%s\".\n",n,s2,s0);
        strFree(s0);

        printf("strConcat, strLength:\n");
        printf("Concatenating \"%s\" and \"%s\" yields:\n",s1,s3);
        s0 = strConcat(s1,s3);
#if EDIT_1_0_n1_07
        printf("\"%s\", of length %d.\n",s0, (int) strLength(s0));
#else
        printf("\"%s\", of length %d.\n",s0,strLength(s0));
#endif
        strFree(s0);

        printf("strlConcat:\n");
        printf("Concatenating \"%s\", \"%s\" and \"%s\" yields:\n",s1,s2,s3);
        joined = strlConcat((String) s1, (String) s2, (String) s3, (String) NULL);
        printf("\"%s\"\n",joined);
        strFree(joined);

        printf("strPrintf/strVPrintf:\n");
        printf             ("%s--%s--%s--%d--%10.4g\n", s1, s2, s3, 13, 3.14);
        s0 = strPrintf     ("%s--%s--%s--%d--%10.4g\n", s1, s2, s3, 13, 3.14);
        printf("%s", s0);
        strFree(s0);
        s0 = testStrVPrintf("%s--%s--%s--%d--%10.4g\n", s1, s2, s3, 13, 3.14);
        printf("%s", s0);
        strFree(s0);

        printf("strPrint:\n");
        strPrint(osStdout, "The rain's flat by `\\/' gum!, he \"said\". \012",
                '"', '"', '\\', "\\%#o");
        printf("\n");
        strPrint(osStdout, "The rain's flat by `\\/' gum!, he \"said\". \012",
                '`', '\'', '!', "!x%x");
        printf("\n");
}

#endif
