-------------------------------------------------------------------------------
----
---- hinteger.as: Half-word sized integers.
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

#include "axllib"

+++ HalfInteger implements half-precision integers.  Typically 16 bits.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: half-precision integer

extend HalfInteger: Join(
	Logic,
	OrderedFinite,
	OrderedRing,
	DenseStorageCategory
) with == add {
	Rep == BHInt;
	import from Machine;

	import { string: Literal -> % } from String;
	import { formatSInt: BSInt -> String } from Foreign;

	0:   %			== per 0;
	1:   %			== per 1;
	min: %			== per min;
	max: %			== per max;

	#: Integer == {
		import from SingleInteger;
		(convert rep max - convert rep min + 1)::SingleInteger::Integer
	}
	local sint(x:%):BSInt == convert(rep x);
	local hint(x:BSInt):% == per(convert x);

	+ (a: %): %		== a;
	- (a: %): %		== hint (- sint a);
	(a: %) + (b: %): %	== hint( sint a + sint b);
	(a: %) - (b: %): %	== hint( sint a - sint b);
	(a: %) * (b: %): %	== hint( sint a * sint b);

	(a: %) = (b: %): Boolean== (sint a = sint b)::Boolean;
	(a: %) > (b: %): Boolean== ((sint b) < (sint a))::Boolean;

	~ (a: %): %		== hint( ~ sint a);
	(a: %) /\ (b: %): %	== hint(sint a /\ sint b);
	(a: %) \/ (b: %): %	== hint(sint a \/ sint b);

	coerce(n: SingleInteger):%==hint (coerce n);
	coerce(n: Integer): %     ==hint (convert coerce n);

	(a: %) ^ (n:Integer): %       == hint  coerce ((coerce sint a)^n) ;

	(w: TextWriter) << (h: %): TextWriter ==
		{ write!(w, formatSInt convert rep h); w }
}
