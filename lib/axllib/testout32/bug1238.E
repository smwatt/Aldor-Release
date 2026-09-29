"bug1238.as", line 9: Foo:Cross(C:Category,X:C) == (Ring,Integer)
                      .....................^.^......^
[L9 C22] #3 (Error) Argument 2 of `Cross' did not match any possible parameter type.

    Expected type Type.
[L9 C24] #1 (Error) There are 2 meanings for `C' in this context.
The possible types were:
	  C: Category, a local
	  C: 
                 with 
                    ==  add ()
       ... from AxlLib
  The context requires an expression of type Tuple(Type).
[L9 C31] #4 (Error) Have determined 1 possible types for the expression.
	Meaning 1: Category == Join(ArithmeticSystem, Monoid, AbelianGroup), Join...
  The context requires an expression of type Cross(Category, C).

