"funct4.as", line 22: 
f1 () : Integer == left(b, false, y, false) -- 0 poss. types
...........................^
[L22 C28] #1 (Error) Argument 2 of `left' did not match any possible parameter type.
    The rejected type is Boolean.
    Expected one of:
      -- Integer
      -- Byte

"funct4.as", line 23: 
f2 () : Boolean  == left(b, false, y, false)    -- 0 poss. types
............................^
[L23 C29] #2 (Error) Argument 2 of `left' did not match any possible parameter type.
    The rejected type is Boolean.
    Expected one of:
      -- Integer
      -- Byte

"funct4.as", line 24: f3 () : Boolean  == left( b ) -- par. 2 doesn't match
                      ..........................^
[L24 C27] #3 (Error) Argument 1 of `left' did not match any possible parameter type.
    These possible types were rejected:
      -- Boolean
      -- Integer
    Expected one of:
      -- i: Integer, q: Integer, b: Boolean, s: Boolean
      -- b: Integer, q: Boolean, i: SingleFloat, s: Byte
      -- a: Integer, q: Boolean, b: Boolean, s: Boolean
      -- b: Integer, q: Boolean, i: Integer, s: Boolean
      -- a: Integer, q: Byte, i: Byte, s: Boolean
      -- a: Integer, q: Byte

