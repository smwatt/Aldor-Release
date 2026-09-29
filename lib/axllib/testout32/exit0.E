"exit0.as", line 19: (x : Integer) := ( x := 1; x < 0 => x )
                     ...................^
[L19 C20] #1 (Error) There is no suitable interpretation for the expression 
                        x := 1
                        x < 0 ...
  The context requires an expression of type Integer.
     The possible types of the right hand side (`
                        x := 1
                        x < 0 ...') are:
          -- (0 types)

