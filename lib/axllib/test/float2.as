-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axldem -l axllib
#pile
#include "axllib.as"
#library  DemoLib "axldem"

R ==> DoubleFloat
I ==> SingleInteger

import from DemoLib
import from DoubleFloat
import from SingleInteger
import from DoubleFloatElementaryFunctions

a      := 0.0
b      := 2.0
relerr := 1.0e-10
abserr := 0.0

fun(x: R): R == if x = 0.0 then 1.0 else sin(x)/x

print<<"Integrating sin(x)/x from "<<a<<" to "<<b<<newline

(result, errest, nofun, flag) := quanc8(fun, a, b, abserr, relerr)

-- The final printed bits vary by one ulp across otherwise conforming IEEE-754
-- implementations.  Test the numerical contract instead of a machine-specific
-- decimal rendering.
expected: R == 1.6054129768026948
resultTolerance: R == 1.0e-14
errorTolerance: R == 1.0e-12

if abs(result - expected) <= resultTolerance then
        print << "Result within tolerance" << newline
else
        print << "Result outside tolerance" << newline

if errest >= 0.0 and errest <= errorTolerance then
        print << "Error estimate within tolerance" << newline
else
        print << "Error estimate outside tolerance" << newline

if flag = 0.0 then
        print << "Quadrature converged" << newline
else
        print << "Quadrature did not converge" << newline
