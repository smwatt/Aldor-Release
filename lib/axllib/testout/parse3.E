"parse3.as", line 17: 
   rationalApproximation(f,d) == rationalApproximation(f,d,10)
.........................^.^
[L17 C26] #3 (Error) Parameter type (for f) must be specified explicitly or with default.
[L17 C26] #10 (Error) Function return type must be specified.
[L17 C28] #4 (Error) Parameter type (for d) must be specified explicitly or with default.

"parse3.as", line 19:    rationalApproximation(f,d,b) ==
                      .........................^.^.^
[L19 C26] #5 (Error) Parameter type (for f) must be specified explicitly or with default.
[L19 C28] #6 (Error) Parameter type (for d) must be specified explicitly or with default.
[L19 C30] #7 (Error) Parameter type (for b) must be specified explicitly or with default.

"parse3.as", line 20:       t: Integer
                      ......^
[L20 C7] #1 (Warning) Suspicious `:'.  Do you mean `local' or `default'?

"parse3.as", line 27:       p0,p1,q0,q1 : Integer
                      ...............^
[L27 C16] #2 (Warning) Suspicious `:'.  Do you mean `local' or `default'?
[L27 C16] #13 (Fatal Error) Too many errors (use `-M emax=n' or `-M no-emax' to change the limit).

