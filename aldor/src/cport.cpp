///////////////////////////////////////////////////////////////////////////////
//
// cport.cpp: Small support routines for the Aldor portability environment.
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

#include "cport.h"

int _dont_assert = 0;

void
_do_assert(const char *str, const char *file, int line)
{
        if (!str) str = "Details not available.";
        fprintf(stderr,
                "Assertion failed, file \"%s\" line %d: %s\n",
                file, line, str);
        abort();
}
