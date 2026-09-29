-------------------------------------------------------------------------------
----
---- char.as: Extend the Character type with basic operations.
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

+++ Characters for natural language text.
+++
+++ In the portable byte code files, characters are represented in ASCII.
+++ In a running program, characters are represented according to the
+++ machine's native character set, e.g. ASCII or EBCDIC.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: character, text, ASCII, EBCDIC

extend Character: Join(
	OrderedFinite,
	DenseStorageCategory
) with {
	space:	 %;
	newline: %;
	tab: %;
	digit?:	 % -> Boolean;
	letter?: % -> Boolean;
	lower:	 % -> %;
	upper:	 % -> %;
}
== add {
	Rep == BChar;
	import from Machine;
	import from SingleInteger;

	sample:  %                == per space;
	space:	 %		  == per space;
	newline: %		  == per newline;
	tab: 	 %		  == per tab;
	min:	 %		  == per min;
	max:	 %		  == per max;
	#:	 Integer	  ==
		(ord max - ord min + 1)@BSInt::SingleInteger::Integer;

	digit? (c: %): Boolean    == digit? (rep c)::Boolean;
	letter?(c: %): Boolean    == letter?(rep c)::Boolean;

	(a: %) =  (b: %): Boolean == (rep a =  rep b)::Boolean;
	(a: %) ~= (b: %): Boolean == (rep a ~= rep b)::Boolean;
	(a: %) <  (b: %): Boolean == (rep a <  rep b)::Boolean;
	(a: %) >  (b: %): Boolean == (rep b <  rep a)::Boolean;
	(a: %) <= (b: %): Boolean == (rep a <= rep b)::Boolean;
	(a: %) >= (b: %): Boolean == (rep b <= rep a)::Boolean;

	lower(c: %): % == per lower rep c;
	upper(c: %): % == per upper rep c;
	(w: TextWriter) << (c: %): TextWriter == 
		{ write!(w, c); w }
}
