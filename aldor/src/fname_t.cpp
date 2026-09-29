///////////////////////////////////////////////////////////////////////////////
//
// fname_t.cpp: Test file names.
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

#if !defined(TEST_FNAME) && !defined(TEST_ALL)

void testFname(void) { }

#else

# include "axlgen.h"

#define NFNTEMP 40      /* number of temp files to allocate, */
                        /* > 36 to test base 36 conversion */

static String      ftv[] = {"a", "x", "i", "o", "m", 0};

void
testFname(void)
{
        FileName        fn;
        FileName        buf[NFNTEMP];
        FileName *      fnv;
        int             i, j, nvec;
        FILE *          f;

        printf("fnameParse: ");
        fn = FileName::parse("newfile");
        printf("\"%s\"\n", fn.name());

        printf("fnameUnparseStatic: ");
        printf("\"%s\"\n", fn.unparseStatic());

        printf("fnameFree: ");
        fn.free();
        printf("DONE.\n");

        for (i = 0; i < NFNTEMP; i++) {
                buf[i] = FileName::temp("", "", "");
                for (j = 0; j < i; j++)
                        if (strcmp(buf[i].name(), buf[j].name()) == 0) {
                                printf("fnameTemp: ERROR duplicate name\n");
                                exitFailure();
                        }
                f = fileTryOpen(buf[i], osIoWrMode);
                if (!f) {
                        printf("fnameTemp: ERROR opening temp file\n");
                        exitFailure();
                }
                fclose(f);
        }
        printf("fnameTemp: %d unique files OK\n", NFNTEMP);

        fnv = FileName::tempVector("", "", ftv);
        for (nvec = 0; fnv[nvec]; nvec++)
                fnv[nvec].free();
        if (nvec != 5) {
                printf("fnameTempVector: ERROR expected 5, got %d\n", nvec);
                exitFailure();
        }
        printf("fnameTempVector: %d files OK\n", nvec);

        for (i = 0; i < NFNTEMP; i++) {
                fileRemove(buf[i]);
                buf[i].free();
        }
        stoFree((void *) fnv);
}

#endif
