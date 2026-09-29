#include "aldor"
#include "aldorio"

macro {
    Z  == Integer;
    MI == MachineInteger;
}

import from Z, MI, Boolean;

show(x: Z): () == {
    xc: Z := machine(x)::Z;
    y:  Z := x*x + 1;
    stdout << x << " eq=" << (x = xc) << " rem=" << (y rem x) << newline;
}

check(m: MI): () == {
    r: Z := m::Z;
    show(0@Z + r);
    show(r + 0@Z);
    show(1@Z + r);
    show(r + 1@Z);
    show(96768@Z + r);
    show(r + 96768@Z);
}

-- This 38-bit value is large enough to require three 16-bit BInt places when
-- temporarily stored, while still fitting the 64-bit immediate representation.
check(274877906951@MI);
