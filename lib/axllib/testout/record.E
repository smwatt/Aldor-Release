"record.as", line 16: a.13 := 3
                      ..^
[L16 C3] #1 (Error) There are no suitable meanings for the operator `set!'.
  Possible types for the selector `13' were:
    -- Integer

"record.as", line 17: f().1 := 1
                      ^
[L17 C1] #2 (Error) There are no suitable meanings for the operator `set!'.
  No `set!' found for any of the possible types for `f()':
    -- Record(t: Integer)

"record.as", line 18: f().t := 2
                      ....^
[L18 C5] #3 (Error) No meaning for identifier `t'.

"record.as", line 19: a.w := 3
                      ..^
[L19 C3] #4 (Error) No meaning for identifier `w'.

"record.as", line 21: print a.c
                      ........^
[L21 C9] #5 (Error) No meaning for identifier `c'.

"record.as", line 23: g.e.y := 2
                      ....^
[L23 C5] #6 (Error) There are no suitable meanings for the operator `set!'.
  Possible types for the selector `y' were:
    -- Enumeration(y: Type)

"record.as", line 24: g.e.e := 1
                      ....^
[L24 C5] #7 (Error) There are no suitable meanings for the operator `set!'.
  Possible types for the selector `e' were:
    -- Enumeration(e: Type)

"record.as", line 25: g.z.a := 1
                      ..^
[L25 C3] #8 (Error) No meaning for identifier `z'.

