///////////////////////////////////////////////////////////////////////////////
//
// debug.cpp: Debugging code.
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

# include "axlgen0.h"

bool    dbFlag  = false;

FILE    *dbIn, *dbOut;

void
dbInit(void)
{
        dbIn  = osStdin;
        dbOut = osStdout;
}
