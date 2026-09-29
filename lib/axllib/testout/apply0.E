"apply0.as", line 22: 
f1 () : Integer == apply(b, false, y, false) -- 0 poss. types
...................^
[L22 C20] #1 (Error) There are no suitable meanings for the operator `apply'.
  apply: (b: Boolean, q: Boolean, i: DoubleFloat, s: Boolean) -> String, a local
      rejected because the context requires type `Integer'.
  apply: (a: Integer, q: Byte, i: Byte, s: Boolean) -> Boolean, a local
      rejected because argument 2 did not match `q: Byte'.
  apply: (i: Integer, q: Integer, b: Boolean, s: Boolean) -> Integer, a local
      rejected because argument 2 did not match `q: Integer'.
          The available types for argument 2 were:
          -- Boolean
  apply: (b: Integer, q: Boolean, i: Integer, s: Boolean) -> Boolean, a local
      rejected because argument 3 did not match `i: Integer'.
  apply: (a: Integer, q: Boolean, b: Boolean, s: Boolean) -> Boolean, a local
      rejected because argument 3 did not match `b: Boolean'.
          The available types for argument 3 were:
          -- DoubleFloat
          -- SingleFloat
  apply: (b: Integer, q: Boolean, i: SingleFloat, s: Byte) -> Boolean, a local
      rejected because argument 4 did not match `s: Byte'.
          The available types for argument 4 were:
          -- Boolean
  apply: (a: Integer, q: Byte) -> Integer, a local
      rejected because it cannot take 4 arguments.

"apply0.as", line 23: f2 () : Integer == 1(false, y, false)
                      ...............................^
[L23 C32] #2 (Error) Argument 4 of `apply' did not match any possible parameter type.
    The rejected type is Boolean.
    Expected type Byte.

"apply0.as", line 25: f4 () : Integer == false(false, 1.2, 1)
                      .....................................^
[L25 C38] #3 (Error) Argument 4 of `apply' did not match any possible parameter type.
    These possible types were rejected:
      -- DoubleFloat
      -- SingleFloat
      -- Integer
    Expected type Boolean.

