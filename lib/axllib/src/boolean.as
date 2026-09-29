-------------------------------------------------------------------------------
----
---- boolean.as: Extend the Boolean type with logical operations.
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

+++ The Boolean data type supports logical operations.
+++ Both arguments of the binary operations are always evaluated.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: logical operation

extend Boolean: Join(
	Conditional,
	Logic,
	OrderedFinite,
	DenseStorageCategory
) with {
	false: %;	++ false is the constant representing logical falsity.
	true:  %;	++ true is the constant representing logical truth.
}
== add {
	Rep == BBool;
	import from Machine;

	(a: %) =  (b: %): Boolean == per (rep a =  rep b);
	(a: %) ~= (b: %): Boolean == per (rep a ~= rep b);
	(a: %) >  (b: %): Boolean ==  a /\ ~b;
	(a: %) <  (b: %): Boolean == ~a /\  b;
	(a: %) <= (b: %): Boolean == ~a \/  b;
	(a: %) >= (b: %): Boolean ==  a \/ ~b;

	#: Integer	     == 2;
	min: %		     == per false;
	max: %		     == per true;

	false: %	     == per false;
	true:  %	     == per true;
	sample: %            == per true;

	~(a: %): %	     == per (~ rep a);
	(a: %) /\ (b: %): %  == per (rep a /\ rep b);
	(a: %) \/ (b: %): %  == per (rep a \/ rep b);

	max(a: %, b: %): %   == a \/ b;
	min(a: %, b: %): %   == a /\ b;

	test(b: %): Boolean  == b;

	(p: TextWriter) << (b: %): TextWriter == {
		import from String;
		p << (if b then "true" else "false");
	}


#if ExplicitPackedExports
	-- The compiler can generate these exports automatically
	-- but could not always inline them unless they were defined
	-- explicitly.
	import
	{
			ArrNew: (Rep, BSInt) -> BArr;
			ArrElt: (BArr, BSInt) -> Rep;
			ArrSet: (BArr, BSInt, Rep) -> Rep;
	} from Builtin;


	PackedArrayNew(n:BSInt):BArr ==
		ArrNew(false, n);

	PackedArrayGet(a:BArr, i:BSInt):% ==
		per ArrElt(a, i);

	PackedArraySet(a:BArr, i:BSInt, e:%):% ==
		per ArrSet(a, i, rep e);

	PackedRecordGet(p:BPtr):% ==
		per ArrElt(p pretend BArr, 0);

	PackedRecordSet(p:BPtr, e:%):% ==
		per ArrSet(p pretend BArr, 0, rep e);

	PackedRepSize():BSInt ==
	{
		import
		{
			TypeBool: () -> BSInt;
			RawRepSize: BSInt -> BSInt;
		} from Builtin;

		RawRepSize(TypeBool());
	}
#endif
}

