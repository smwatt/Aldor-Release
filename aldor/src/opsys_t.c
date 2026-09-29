/*****************************************************************************
 *
 * opsys_t.c: Test operating system dependent code.
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

#if !defined(TEST_OPSYS) && !defined(TEST_ALL)

void testOpsys(void) { }

#else

/*
 *!! This is very limited at the moment.  More tests are needed.
 */
# include "axlgen.h"

local void
showMemMap(struct osMemMap **mm)
{
        String s;

        if (!mm)
                printf("    Not available\n");
        else
                for ( ; (*mm)->use != OSMEM_END; mm++) {
                        switch ((*mm)->use) {
                        case OSMEM_IDATA:       s = "initial data"; break;
                        case OSMEM_DDATA:       s = "dynamic data"; break;
                        case OSMEM_STACK:       s = "stack";    break;
                        default:                s = "?????";    break;
                        }
#if EDIT_1_0_n1_07
                        printf("    [%p, %p)  %s\n", (*mm)->lo, (*mm)->hi, s);
#else
                        printf("    [%#10lx, %#10lx)  %s\n",
                                (long) (*mm)->lo, (long) (*mm)->hi, s);
#endif
                }
        printf("\n");
}

#define NALLOC  10      /* Must be even */

void
testOpsys(void)
{
        struct osMemMap **mm;
        int             i;
        ULong           n;
        void *          allocv[NALLOC];

        mm = osMemMap(~0);
        (void) mm;
        printf("Full memory map query: OK\n");

        mm = osMemMap(OSMEM_STACK);
        (void) mm;
        printf("Stack memory map query: OK\n");

        printf("osAlloc/osFree ascending: ");
        for (i = 0; i < NALLOC; i++) {
                n = i + 1;
                allocv[i] = osAlloc(&n);
                if (!allocv[i]) exitFailure();
        }
        for (i = 0; i < NALLOC; i++)
                osFree(allocv[NALLOC-i-1]);
        printf("OK\n");

        printf("osAlloc/osFree descending: ");
        for (i = 0; i < NALLOC; i++) {
                n = NALLOC - i;
                allocv[i] = osAlloc(&n);
                if (!allocv[i]) exitFailure();
        }
        for (i = 0; i < NALLOC; i++)
                osFree(allocv[i]);
        printf("OK\n");

        printf("osAlloc/osFree interleaved: ");
        for (i = 0; i < NALLOC; i++) {
                n = i + 1;
                allocv[i] = osAlloc(&n);
                if (!allocv[i]) exitFailure();
        }
        for (i = 0; i < NALLOC; i += 2) {
                osFree(allocv[i]);
                osFree(allocv[NALLOC-i-1]);
        }
        printf("OK\n");

#if defined(OS_MAC_OSX)
        /* A valid Darwin osFree must make the low-level piece reusable. */
        n = 4096;
        allocv[0] = osAlloc(&n);
        if (!allocv[0]) exitFailure();
        osFree(allocv[0]);
        n = 4096;
        allocv[1] = osAlloc(&n);
        if (allocv[1] != allocv[0]) exitFailure();
        osFree(allocv[1]);
#endif
}

#endif
