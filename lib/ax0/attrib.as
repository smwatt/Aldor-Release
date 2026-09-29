-------------------------------------------------------------------------------
----
---- attrib.as: Attribute definitions used in the Axiom library.
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
	Attribute X == X: Category == with;
}

Attribute nil;
Attribute infinite;
Attribute arbitraryExponent;
Attribute approximate;
Attribute complex;
Attribute shallowMutable;
Attribute canonical;
Attribute noetherian;
Attribute central;
Attribute partiallyOrderedSet;
Attribute arbitraryPrecision;
Attribute canonicalsClosed;
Attribute noZeroDivisors;
Attribute rightUnitary;
Attribute leftUnitary;
Attribute additiveValuation;
Attribute unitsKnown;
Attribute canonicalUnitNormal;
Attribute multiplicativeValuation;
Attribute finiteAggregate;
Attribute shallowlyMutable;

Attribute commutative(T: Type);
