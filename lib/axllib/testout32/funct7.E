"funct7.as", line 19: (h false)(x)
                      ..........^
[L19 C11] #1 (Error) Argument 1 of `h(false)' did not match any possible parameter type.
    These possible types were rejected:
      -- SingleInteger
      -- DoubleFloat
    Expected type Integer -> Boolean.

"funct7.as", line 20: (h false)(x,true)
                      .^
[L20 C2] #2 (Error) There are no suitable meanings for the operator `h(false)'.
  (...): (x: Integer -> Boolean) -> Integer
  (...): (x: Integer -> Boolean) -> Boolean
      rejected because it cannot take 2 arguments.

"funct7.as", line 21: g(f) 
                      ..^
[L21 C3] #3 (Error) Argument 1 of `g' did not match any possible parameter type.
    The rejected type is (i: Integer -> Boolean) -> Integer.
    Expected type Integer -> Boolean.

"funct7.as", line 22: e (g:(x:Integer->Boolean)->Boolean):Boolean == g(f)
                      .................................................^
[L22 C50] #4 (Error) Argument 1 of `g' did not match any possible parameter type.
    The rejected type is (i: Integer -> Boolean) -> Integer.
    Expected type Integer -> Boolean.

