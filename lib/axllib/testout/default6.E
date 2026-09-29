"default6.as", line 18: a1: Integer == foo(1)
                        ...............^
[L18 C16] #1 (Error) There are no suitable meanings for the operator `foo'.
  foo: (a: Integer, c: Integer == 1, d: DoubleFloat) -> Integer, a local
     rejected because parameter 3 (d: DoubleFloat) is missing.

"default6.as", line 19: a2: Integer == foo(2,b)
                        ...............^
[L19 C16] #2 (Error) There are no suitable meanings for the operator `foo'.
  foo: (a: Integer, c: Integer == 1, d: DoubleFloat) -> Integer, a local
     rejected because parameter 3 (d: DoubleFloat) is missing.

"default6.as", line 20: a3: Integer == foo(false, 1,1)
                        ...................^
[L20 C20] #3 (Error) Argument 1 of `foo' did not match any possible parameter type.
    The rejected type is Boolean.
    Expected type Integer.

"default6.as", line 21: a4: Integer == foo()
                        ...............^
[L21 C16] #4 (Error) There are no suitable meanings for the operator `foo'.
  foo: (a: Integer, c: Integer == 1, d: DoubleFloat) -> Integer, a local
     rejected because parameter 1 (a: Integer) is missing.

