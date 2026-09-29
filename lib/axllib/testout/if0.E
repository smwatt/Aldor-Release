"if0.as", line 25: x := (if x < 0 then x)
                   ......^
[L25 C7] #1 (Error) There is no suitable interpretation for the expression 
                if (x < 0) then x
  The context requires an expression of type Integer.
     The possible types of the right hand side (`
                if (x < 0) then x') are:
          -- (0 types)

"if0.as", line 28: u := if x < 0 then 2 else "hello"
                   .....^
[L28 C6] #2 (Error) Have determined 0 possible types for the expression.
  Subexpression `2':
	Meaning 1: Integer
  Subexpression `"hello"':
	Meaning 1: String

