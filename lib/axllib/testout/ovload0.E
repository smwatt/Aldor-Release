"ovload0.as", line 36: 
        b := left(b, z)                 -- lbi-b+zi or lbb-b+zb: ambiguous
.............^
[L36 C14] #1 (Error) There are 2 meanings for the operator `left'.
	Meaning 1: (Boolean, Boolean) -> Boolean
	Meaning 2: (Boolean, Integer) -> Boolean

"ovload0.as", line 37:         b := left(i, i)                 -- no meaning
                       ..................^
[L37 C19] #2 (Error) Argument 1 of `left' did not match any possible parameter type.
    The rejected type is Integer.
    Expected type Boolean.

