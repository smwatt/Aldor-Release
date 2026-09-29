-------------------------------------------------------------------------------
----
---- pointer.as: Extend the Pointer type with basic operations.
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

+++ Pointer is the type of pointers to opaque objects.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: pointer

extend Pointer: Conditional with {
	nil:	 %;
  	nil?:	 % -> Boolean;

	coerce:	 BPtr -> %;
	coerce:	 % -> BPtr;
}
== add {
	Rep == BPtr;
	import from Machine;
	import { formatSInt: BSInt -> String } from Foreign;

	sample: %                == per nil;

	nil: %		         == per nil;
  	nil?(p: %): Boolean      == nil?(rep p)::Boolean;
	(p: %) = (q: %): Boolean == (rep p = rep q)::Boolean;
	(w: TextWriter) << (p: %): TextWriter ==
		{ write!(w, formatSInt convert rep p); w }

	coerce(p: BPtr): % == per p;
	coerce(p: %): BPtr == rep p;

	test (p: %) : Boolean == not nil? p;
}
