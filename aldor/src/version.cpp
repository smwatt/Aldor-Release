///////////////////////////////////////////////////////////////////////////////
//
// version.cpp: Compiler version number.
//
///////////////////////////////////////////////////////////////////////////////

// This file is part of Aldor.
//
// Aldor is licensed under the Apache License, Version 2.0.
//
//
// See legal/LICENSE in the Aldor distribution for details.
//
// Copyright (C) 1990-2026 Stephen M. Watt.

# include "axlobs.h"
# include "version.h"

/*
 * The version of this program.
 */

#if EDIT_1_0_n2_07
const char *    axiomxlName          = "Aldor";
#else
char *          axiomxlName          = "Aldor";
#endif

int     axiomxlMajorVersion  = ALDOR_VERSION_MAJOR;
int     axiomxlMinorVersion  = ALDOR_VERSION_MINOR;
int     axiomxlMinorFreeze   = ALDOR_VERSION_MICRO;
int     axiomxlEditNumber    = 0;


#if EDIT_1_0_n2_07
const char  *axiomxlPatchLevel    = ALDOR_VERSION_SUFFIX;
#else
char        *axiomxlPatchLevel    = (char *) ALDOR_VERSION_SUFFIX;
#endif

#ifndef axiomxlBuildVersion
#define axiomxlBuildVersion ""
#endif

local MutString verGetDate(String);

bool
verBannerWanted(void)
{
        if (strIsPrefix("internal", axiomxlBuildVersion))
                return false;
        if (strIsPrefix("release", axiomxlBuildVersion))
                return false;

        return true;
}

void
verPrint(void)
{
        if (strIsPrefix("prerelease", axiomxlBuildVersion))
                comsgFPrintf(osStdout, ALDOR_I_PreRelease, axiomxlName);

        if (strIsPrefix("demo", axiomxlBuildVersion)) {
                MutString date;
                date = verGetDate(axiomxlBuildVersion);
                comsgFPrintf(osStdout, ALDOR_I_DemoExpiry,
                             axiomxlName, date);
                strFree(date);
        }
}

local MutString
verGetDate(String s)
{
        char *ptr;
        MutString date;
        while (*s != ':') s++;
        s++;
        ptr = date = strCopy(s);
        while (*ptr != '\0') {
                if (*ptr == '-') *ptr=' ';
                ptr++;
        }
        return date;
}
