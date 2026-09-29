"default7.as", line 14: f( d==false, c==3, a==1 ) -- Missing par. 1
                        ...^
[L14 C4] #1 (Error) Argument 1 of `f' did not match any possible parameter type.
    The rejected type is Boolean.
    Expected type Integer.

"default7.as", line 32: g( 1, d==1, b==3, 1 )  -- Par. missing c:Integer
                        ^
[L32 C1] #2 (Error) There are no suitable meanings for the operator `g'.
  g: (a: Integer, b: Integer, c: Integer, d: Integer) -> (), a local
     rejected because parameter 3 (c: Integer) is missing.

"default7.as", line 34: g( d==1, 1, 2, 3 )     -- Par. missing a:Integer
                        ^
[L34 C1] #3 (Error) There are no suitable meanings for the operator `g'.
  g: (a: Integer, b: Integer, c: Integer, d: Integer) -> (), a local
     rejected because parameter 1 (a: Integer) is missing.

"default7.as", line 47: h( d==1, 2 )    -- Par. missing c:Integer
                        ^
[L47 C1] #4 (Error) There are no suitable meanings for the operator `h'.
  h: (a: Integer == 1, b: Integer, c: Integer, d: Integer) -> (), a local
     rejected because parameter 3 (c: Integer) is missing.

"default7.as", line 64: 
i1: Integer == l(1, x: Integer == 3) -- Bad arg type; "should use x==3 ...
....................^
[L64 C21] #5 (Error) Argument 2 of `l' did not match any possible parameter type.
    The rejected type is Join(
                 with 
                        integer: ....
    Expected type Integer.

"default7.as", line 65: 
i2: Integer == l(1, 2, x: Integer == 3) -- Bad arg type; no "should ...
.......................^
[L65 C24] #6 (Error) Argument 3 of `l' did not match any possible parameter type.
    The rejected type is Join(
                 with 
                        integer: ....
    Expected type C0.

