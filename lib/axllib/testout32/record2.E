"record2.as", line 17: y := ff
                       .....^
[L17 C6] #1 (Error) No meaning for identifier `ff'.

"record2.as", line 22: h.a := 1  -- $. def, .$ def $.$ undef
                       ..^
[L22 C3] #2 (Error) There are no suitable meanings for the operator `set!'.
  Possible types for the selector `a' were:
    -- Enumeration(a: Type)

"record2.as", line 23: g.a := false  -- type mismatch lhs,rhs
                       .......^
[L23 C8] #3 (Error) There are no suitable meanings for the operator `set!'.
  The rejected type for the right hand side is Boolean.
  The context requires an expression of type Integer.

"record2.as", line 25: a.a := 1  -- $ def $. no sense
                       ^
[L25 C1] #4 (Error) There are no suitable meanings for the operator `set!'.
  No `set!' found for any of the possible types for `a':
    -- Enumeration(a: Type)

"record2.as", line 28: g(a,a) := 1  -- too many args for set!
                       ^
[L28 C1] #5 (Error) There are no suitable meanings for the operator `set!'.
  There is no `set!' definition with this number of arguments.

"record2.as", line 30: uuu: Boolean == (g.a := 1)
                       ..................^
[L30 C19] #6 (Error) There are no suitable meanings for the operator `set!'.
  Possible return types for `set!' expression were:
          -- Integer
  The context requires an expression of type Boolean.

