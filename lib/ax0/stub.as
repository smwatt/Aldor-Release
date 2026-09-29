-------------------------------------------------------------------------------
----
---- stub.as: Stub domain/category definitions for the Axiom library.
----
-------------------------------------------------------------------------------
----
---- This file is part of Aldor.
----
---- Aldor is licensed under the Apache License, Version 2.0.
----
----
---- See legal/LICENSE in the Aldor distribution for details.
----
---- Copyright (C) 1990-2026 Stephen M. Watt.

import from AxiomLib;
inline from AxiomLib;

macro {
	StubDomain X == X: with == add;
}

StubDomain Boolean;
StubDomain InputForm;
StubDomain NonNegativeInteger;
StubDomain PositiveInteger;
StubDomain SingleInteger;
StubDomain String;

StubDomain Equation(T: Type);
StubDomain List(T: Type);
StubDomain SegmentBinding(T: Type);
StubDomain UniversalSegment(T: Type);
StubDomain Vector(T: Type);

SubsetCategory (C: Category, D: with) : Category == C with {
	coerce:		% -> D;
	retract:	D -> %;
}
