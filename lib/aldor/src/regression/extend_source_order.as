-- Regression for source-order selection of locally defined extensions.
-- Kept separate from imported-extensions test so both cases are visible.
#if ALDORTEST
#include "aldor"
#include "aldortest"

SourceOrderProbe: with {
    marker: () -> MachineInteger;
} == add {
    marker(): MachineInteger == 1;
}

beforeExtension(): MachineInteger == marker()$SourceOrderProbe;

extend SourceOrderProbe: with {} == add {
    marker(): MachineInteger == 2;
}

afterExtension(): MachineInteger == marker()$SourceOrderProbe;

import from MachineInteger;

stdout << "Testing local extension frontier..." << endnl;
stdout << (if beforeExtension() = 1 and afterExtension() = 2
           then "OK" else "FAIL") << endnl;
#endif
