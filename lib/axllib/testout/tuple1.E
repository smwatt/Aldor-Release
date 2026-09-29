"tuple1.as", line 27: local y: (3, Integer) -> String
                      ......^...^
[L27 C7] #1 (Warning) Local `y' is not assigned, defined, or used.
[L27 C11] #2 (Error) No meaning for integer-style literal `3'.
[L27 C11] #3 (Error) Have determined 1 possible types for the expression.
	Meaning 1: SingleInteger, Join(
                 with 
                  ...
  The context requires an expression of type Tuple(Type).
[L27 C11] #4 (Error) Argument 1 of `->' did not match any possible parameter type.
    The rejected type is SingleInteger, Join(
                 with 
                  ....
    Expected type Tuple(Type).

