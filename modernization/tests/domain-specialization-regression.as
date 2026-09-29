#include "aldor"
#include "aldorio"

EqBox(R: PrimitiveType): with {
        eq?: (R, R) -> Boolean;
} == add {
        eq?(a: R, b: R): Boolean == a = b;
}

MIBox == EqBox(MachineInteger);
ZZBox == EqBox(Integer);

sameMI(a: MachineInteger, b: MachineInteger): Boolean == {
        import from MIBox;
        eq?(a, b);
}

sameZZ(a: Integer, b: Integer): Boolean == {
        import from ZZBox;
        eq?(a, b);
}

import from MachineInteger;
import from Integer;
import from Boolean;
stdout << sameMI(17, 17) << newline;
stdout << sameZZ(17, 17) << newline;
