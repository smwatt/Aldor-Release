-- Recursive domain/category fixed-point regression.
--> testrun -l axllib

#include "axllib"

SelfRecursive(F: BasicType): with {
        probe: () -> Boolean;
} == add {
        R: Type == Union(n: Pointer, r: Record(first: F, rest: R));

        probe(): Boolean == true;
}

GraphRecursive(F: BasicType): with {
        probe: () -> Boolean;
} == add {
        A1: Type == A2;
        A2: Type == A3;
        A3: Type == Record(first: F, next: A4);
        A4: Type == A5;
        A5: Type == Union(n: Pointer,
                         r: Record(first: F, rest: A1));

        probe(): Boolean == true;
}

import from TextWriter;

print << probe()$SelfRecursive(Integer) << newline;
print << probe()$GraphRecursive(Integer) << newline;
