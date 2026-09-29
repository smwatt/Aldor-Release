-------------------------------------------------------------------------------
----
---- ref.as: Extension of Ref(T) with ops for explicit manipulation.
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

+++ Ref(T) is a type that allows T lvalues to be aliased
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1998
+++ Keywords: ref, deref, Ref

extend Ref(T: Type): with {
	deref: % -> T;
		++ deref(R) extracts the value from the object which
		++ is being referenced by R.

	update!: (%, T) -> T;
		++ update!(R, v) assigns the value v to the object
		++ being referenced by R.

	export from T;
}
== add {
    RepF ==> () -> (()->T, T->T);

	Rep  ==  RepF;

    --FIXME should not be needed
    repf(r: %): RepF == rep(r) pretend RepF;

	deref(r:%):T ==
	{
		(getter, setter) := repf(r)();
		z := getter();
		z;
	}


	update!(r:%, v:T):T ==
	{
		(getter, setter) := repf(r)();
		z := setter(v);
		z;
	}
}

