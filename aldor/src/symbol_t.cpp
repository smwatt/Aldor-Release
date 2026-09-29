///////////////////////////////////////////////////////////////////////////////
//
// symbol_t.cpp: Test symbol type.
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

#if !defined(TEST_SYMBOL) && !defined(TEST_ALL)

void testSymbol(void) { }

#else

# include "axlgen.h"

void
testSymbol(void)
{
        int     i;
        Symbol  sym;
        String str;


        for (i = 0; i < 3; i++) {
                switch(i) {
                        case 0:
                                sym = Symbol::probe("symbol", SYM_LOOK);
                                str = sym ? sym.string() : "-- failed --";
                                printf("symbol string: \"%s\" ", str);
                                printf("with option: SYM_LOOK\n");
                                break;
                        case 1:
                                sym = Symbol::internConst("internal symbol constant");
                                str = sym ? sym.string() : "-- failed --";
                                printf("symbol string: \"%s\" ", str);
                                printf("with option: SYM_ALLOC\n");
                                break;
                        case 2:
                                sym = Symbol::probe("copy symbol", SYM_STRCOPY);
                                str = sym ? sym.string() : "-- failed --";
                                printf("symbol string: \"%s\" ", str);
                                printf("with option: SYM_STRCOPY\n");
                                break;
                }
        }

        sym = Symbol::intern("internal symbol");
        str = sym ? sym.string() : "-- failed --";
        printf("symbol string: \"%s\" ", str);
        printf("with options: SYM_ALLOC and ,SYM_STRCOPY\n");
}

#endif
