///////////////////////////////////////////////////////////////////////////////
//
// fluid_t.cpp: Test C++ dynamically scoped bindings.
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

#if !defined(TEST_FLUID) && !defined(TEST_ALL)

void testFluid(void) { }

#else

#include "axlgen.h"

struct FluidTestJump final { };

int     testFluidI = 32;
double  testFluidF = 32.1;
bool    testFluidB = false;

void    testFluidPeek                   (String);
void    testFluidNormalReturn           (void);
void    testFluidNested                 (void);
void    testFluidMultiple               (void);
void    testFluidThrowInner             (void);
void    testFluidThrowOuter             (void);

void
testFluid(void)
{
        testFluidI = 42;
        testFluidF = 42.42;
        testFluidB = false;

        testFluidPeek("initial");

        testFluidNormalReturn();
        testFluidPeek("after return");

        testFluidNested();
        testFluidPeek("after nested");

        testFluidMultiple();
        testFluidPeek("after multiple");

        try {
                testFluidThrowOuter();
        }
        catch (const FluidTestJump&) {
                testFluidPeek("after throw");
        }

        {
                Fluid bindI(testFluidI, 77);
                Fluid bindF(testFluidF, 77.5);
                Fluid bindB(testFluidB, true);
                testFluidPeek("bind on entry");
        }
        testFluidPeek("final");
}

void
testFluidPeek(String title)
{
        printf("%-16s %d %.2f %d\n",
               title, testFluidI, testFluidF, (int) testFluidB);
}

void
testFluidNormalReturn(void)
{
        Fluid bindI(testFluidI);
        Fluid bindF(testFluidF);

        testFluidI = 1001;
        testFluidF = 1001.25;
        testFluidPeek("normal return");
}

void
testFluidNested(void)
{
        Fluid outer(testFluidI, 100);
        testFluidPeek("nested outer");

        {
                Fluid inner(testFluidI, 200);
                testFluidPeek("nested inner");
        }

        testFluidPeek("nested restored");
}

void
testFluidMultiple(void)
{
        Fluid bindI(testFluidI, -7);
        Fluid bindF(testFluidF, 3.5);
        Fluid bindB(testFluidB, true);

        testFluidPeek("multiple");
}

void
testFluidThrowOuter(void)
{
        Fluid bindI(testFluidI, 500);
        Fluid bindF(testFluidF, 500.5);
        testFluidPeek("throw outer");
        testFluidThrowInner();
}

void
testFluidThrowInner(void)
{
        Fluid bindI(testFluidI, 600);
        Fluid bindF(testFluidF, 600.5);
        Fluid bindB(testFluidB, true);

        testFluidPeek("throw inner");
        throw FluidTestJump{};
}

#endif
