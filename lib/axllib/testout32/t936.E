"t936.as", line 27: ANYTHING==>List FileName;
                    ...........^
[L27 C12] #1 (Error) (After Macro Expansion) Argument 2 of `FractionSExprOperations' did not match any possible parameter type.
    The rejected type is 
                 with 
                        %%: ListCatego....
    Expected type  with 
                        numerator: % -> Integer
       ....
Expanded expression was: List(FileName)

"t936.as", line 30: print << (foo$(FractionSExprOperations(Integer, ANYTHING))
                    .............^
[L30 C14] #2 (Error) Have determined 0 possible types for the expression.

