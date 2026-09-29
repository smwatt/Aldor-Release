"funct1.as", line 19: 
f1 () : Integer == left(b, false, y, false)     -- 0 poss. types
...........................^
[L19 C28] #1 (Error) Argument 2 of `left' did not match any possible parameter type.
    The rejected type is Boolean.
    Expected type Integer.

"funct1.as", line 20: 
f2 () : Boolean  == left(b, false, y, false)    -- 0 poss. types
............................^
[L20 C29] #2 (Error) Argument 2 of `left' did not match any possible parameter type.
    The rejected type is Boolean.
    Expected type Integer.

"funct1.as", line 23: 
foo() : Boolean == left( 1, false, b, false )   -- 2 poss. meanings 
...................^
[L23 C20] #3 (Error) There are 2 meanings for the operator `left'.
	Meaning 1: (a: Integer, q: Boolean, b: Boolean, s: Boolean) -> Boolean
	Meaning 2: (b: Integer, q: Boolean, i: Integer, s: Boolean) -> Boolean

