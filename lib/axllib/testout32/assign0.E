"assign0.as", line 10: t := k;
                       .....^
[L10 C6] #1 (Error) No meaning for identifier `k'.

"assign0.as", line 12: x := y;   -- lhs undeclared
                       ^
[L12 C1] #2 (Error) The type of the assignment cannot be inferred.
     The possible types of the right hand side (`y') are:
          -- Integer
          -- SingleInteger

"assign0.as", line 14: z: Boolean := y
                       ..............^
[L14 C15] #3 (Error) There is no suitable interpretation for the expression y
  The context requires an expression of type Boolean.
     The possible types of the right hand side (`y') are:
          -- Integer
          -- SingleInteger

