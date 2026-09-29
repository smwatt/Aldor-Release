-------------------------------------------------------------------------------
----
---- format.as: Conversion of numbers to and from strings.
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

#include "foamlib.as"

+++ Text conversion: the second argument to each of the format
+++ functions MUST be writable. Note that since String literals
+++ are usually stored in read-only memory so they must NOT be
+++ used. A copy of a string literal will be okay.
Format: with {
	format:	(SingleInteger, String, SingleInteger) -> SingleInteger;
	format:	(Integer,	String, SingleInteger) -> SingleInteger;

	scan:	(String, SingleInteger) -> (SingleInteger, SingleInteger);
	scan:	(String, SingleInteger) -> (Integer,       SingleInteger);
}
== add {
	import from Machine;

	default si: SingleInteger;
	default bi: Integer;

	default s:  String;
	default i:  SingleInteger;
	default b:  BSInt;

	format(si, s, i): SingleInteger == {
		format(si::BSInt, data s, (i-1)::BSInt)::SingleInteger + 1;
	}

	format(bi, s, i): SingleInteger == {
		format(bi::BBInt, data s, (i-1)::BSInt)::SingleInteger + 1;
	}

	scan(s, i): (SingleInteger, SingleInteger) == {
		(a: BSInt, b) := scan(data s, (i-1)::BSInt);
		(a::SingleInteger, b::SingleInteger + 1);
	}
	scan(s, i): (Integer,       SingleInteger) == {
		(a: BBInt, b) := scan(data s, (i-1)::BSInt);
		(a::Integer, b::SingleInteger + 1);
	}
}
