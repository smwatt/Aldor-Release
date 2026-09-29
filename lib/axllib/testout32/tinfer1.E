"tinfer1.as", line 19: foo(seg: XXX,  s:Type): Type == s
                       ....^....^
[L19 C5] #3 (Error) Have determined 1 possible types for the expression.
	Meaning 1: ?, Type
  The context requires an expression of type Tuple(Type).
[L19 C5] #4 (Error) Argument 1 of `->' did not match any possible parameter type.
    The rejected type is ?, Type.
    Expected type Tuple(Type).
[L19 C10] #2 (Error) No meaning for identifier `XXX'.

"tinfer1.as", line 20: baz(seg: Type, s:Type): YYY  == y
                       ........................^.......^
[L20 C25] #5 (Error) No meaning for identifier `YYY'.
[L20 C33] #11 (Error) No meaning for identifier `y'.

"tinfer1.as", line 24: local f: (t: Type) -> with BadStuff
                       ......^....................^
[L24 C7] #1 (Warning) Local `f' is not assigned, defined, or used.
[L24 C28] #7 (Error) No meaning for identifier `BadStuff'.
[L24 C28] #12 (Fatal Error) Too many errors (use `-M emax=n' or `-M no-emax' to change the limit).

