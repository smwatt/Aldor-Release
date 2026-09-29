-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--  Triv4      -- A minimal exporting program, independent of any other code.
#pile

--> testcomp
--> testgen c
--> testgen l
--> testrun -l axllib

export
	Type:	      	Type
	Tuple:		Type -> Type
	->:  	      	(Tuple Type, Tuple Type) -> Type
	Literal:	Type
	Arr:		Type
	SInt:		Type

import
	printf:     	(Arr, SInt) -> SInt
	puts:		(Arr) -> SInt
from Foreign C "<stdio.h>"

import
	ArrToSInt:    	Arr -> SInt
from Builtin


integer(s: Literal): SInt   == ArrToSInt (s pretend Arr)
string (s: Literal): Arr    == s pretend Arr

printTriv(): () ==
	printf("%ld ", 42)
	puts "Skidoo"

printTriv()
