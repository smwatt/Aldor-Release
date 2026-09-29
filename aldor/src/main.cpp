///////////////////////////////////////////////////////////////////////////////
//
// main.cpp: Compiler entry point.
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

# include "axl.h"

int
main(int argc, MutString *argv)
{
        osFixCmdLine(&argc, &argv);
        return compCmd(argc, argv);
}
