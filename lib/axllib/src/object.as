-------------------------------------------------------------------------------
----
---- object.as: Dynamic objects (OO).
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

+++ Object implements dynamic objects, pairing data values with
+++ associated domains.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: object

Object(C: Category): with {
	object:		(T: C, T) -> %;
	avail:		% -> (T: C, T);
}
== add {
	Rep == Record(T: C, val: T);
	import from Rep;

	object	(T: C, t: T) : %	== per [T, t];
	avail	(ob: %) : (T: C, T)	== explode rep ob;
}
