"error0.as", line 14:         b := left(b, z) -- ambiguous
                      .............^
[L14 C14] #1 (Error) There are 2 meanings for the operator `left'.
	Meaning 1: (a: Boolean, b: Boolean) -> Boolean
	Meaning 2: (b: Boolean, i: Integer) -> Boolean

"error0.as", line 15:         b := left(i, i) -- no meaning
                      ..................^
[L15 C19] #2 (Error) Argument 1 of `left' did not match any possible parameter type.
    The rejected type is Integer.
    Expected type Boolean.

