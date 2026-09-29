-------------------------------------------------------------------------------
----
---- partial.as: A type which allows values or soft failures to be returned.
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

Partial(T: Type): with {
	failed : %;
	failed?: % -> Boolean;
	coerce:  T -> %;
	coerce:  % -> T;
	retract: % -> T;
	bracket: T -> %;
	-- following 2 are hacks to support old syntax for Union(%, "failed")
	case:   (%, String) -> Boolean;
	case:   (%, Type) -> Boolean;
}
== add {
	macro Rep == Pointer;
	macro Rec == Record(val:T);
	import from Rep, Rec, String;

	failed? (p: %): Boolean          == nil? rep p;
	failed: %                        == per nil;
	coerce(v: T): %                  == [v]$Rec pretend %;
	[v: T]: %                        == v::%;
	(p: %) case (f: String): Boolean == failed? p;
	(p: %) case (S: Type): Boolean   == not failed? p;
	coerce(p: %): T                  == retract p;
	retract(p: %): T == {
		failed? p => error "cannot retract failed";
		(p pretend Rec).val
	}
}
