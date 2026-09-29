"macex2.as", line 28:         aa : bb == cc
                      ........^
[L28 C9] #1 (Error) Macros must not be given return types.

"macex2.as", line 29:         ff(a: T) == a + b
                      ...........^
[L29 C12] #3 (Error) Macro parameters must not have type declarations.

"macex2.as", line 30:         gg(a: T): R == a + b
                      ........^..^
[L30 C9] #2 (Error) Macros must not be given return types.
[L30 C12] #4 (Error) Macro parameters must not have type declarations.

"macex2.as", line 32: f 3
                      ^
[L32 C1] #5 (Error) Macro used with incorrect number of arguments.

