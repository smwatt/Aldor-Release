///////////////////////////////////////////////////////////////////////////////
//
// main_t.cpp: Low-level testing entry point.
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

# include "axltop.h"

int
main(int argc, MutString *argv)
{
        if (argc == 1) {
#if EDIT_1_0_n1_07
                osInit();
#endif
                sxiInit();
                sxiReadEvalPrintLoop(osStdin, osStdout, SXRW_Default);
        }
        else {
#if EDIT_1_0_n1_07
                osInit();
#endif
                dbInit();
                while (--argc) testSelf(*++argv);
        }

        return EXIT_SUCCESS;
}
