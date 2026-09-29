"funct5.as", line 36: d:*H == foo( x,x,false) -- error
                      .............^
[L36 C14] #2 (Error) Argument 1 of `foo' did not match any possible parameter type.
    The rejected type is H.
    Expected type Boolean.
The following could be suitable if imported:
  foo: (H, H, Boolean) -> H from H

"funct5.as", line 38: e:*H == foo( x,x,false) -- error
                      .............^.^
[L38 C14] #3 (Error) Argument 1 of `foo' did not match any possible parameter type.
    The rejected type is H.
    Expected type Boolean.
The following could be suitable if imported:
  foo: (H, H, Boolean) -> H from H
[L38 C16] #1 (Warning) Local `x' is used without being assigned or defined.

"funct5.as", line 40: a:SI == id(1)  -- error
                      ........^
[L40 C9] #4 (Error) There are no suitable meanings for the operator `id'.

