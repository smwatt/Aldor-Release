"scobind5.as", line 12:         free a,b
                        ...............^
[L12 C16] #1 (Error) Cannot find scope in which free variable `b' is bound.

"scobind5.as", line 13:         a : Integer := 1
                        ........^
[L13 C9] #2 (Error) Free variable `a' is bound elsewhere with a different type.

"scobind5.as", line 14:         b : Integer := a
                        ........^
[L14 C9] #3 (Error) Cannot find scope in which free variable `b' is bound.

