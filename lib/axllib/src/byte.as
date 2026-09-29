-------------------------------------------------------------------------------
----
---- byte.as: Extend the Byte data with basic operations.
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

+++ Byte implements single byte integers. Typically 8 bits.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: byte, single byte integer

extend Byte: Join(
	Logic,
	OrderedFinite,
	OrderedRing,
	DenseStorageCategory
) with == add {
	Rep == BByte;
	import from Machine;

	import { string: Literal -> % } from String;
	import { formatSInt: BSInt -> String } from Foreign;

	0:   %			== per 0;
	1:   %			== per 1;
	min: %			== per min;
	max: %			== per max;

	#: Integer		== {
		import from SingleInteger;
		(convert rep max - convert rep min + 1)::SingleInteger::Integer
	}

	coerce(n: SingleInteger): % == error "coerce$Byte not implemented";
	coerce(n: Integer): %       == error "coerce$Byte not implemented";

	+ (a: %): %		== a;
	- (a: %): %		== error "-$Byte not implemented";
	(a: %) + (b: %): %	== error "+$Byte not implemented";
	(a: %) - (b: %): %	== a + (-b);
	(a: %) * (b: %): %	== error "*$Byte not implemented";
	(a: %) ^ (n: SingleInteger): % == error "^$Byte not implemented";
	(a: %) ^ (n: Integer): % == error "^$Byte not implemented";

	(a: %) =  (b: %): Boolean== error "=$Byte not implemented";
	(a: %) >  (b: %): Boolean== error ">$Byte not implemented";

	~ (a: %): %		== error "~$Byte not implemented";
	(a: %) /\ (b: %): %	== error "/\$Byte not implemented";
	(a: %) \/ (b: %): %	== error "\/$Byte not implemented";

	(w: TextWriter) << (b: %): TextWriter ==
		{ write!(w, formatSInt convert rep b); w }
}
