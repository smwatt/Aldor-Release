"scope12.as", line 9: i:SingleInteger == 1
                      ^
[L9 C1] #6 (Note 1)  (cf. L10 C7)

"scope12.as", line 10: local i: SingleInteger == 2
                       ......^
[L10 C7] #5 (Error) It is illegal to declare an identifier free or local once it has already been used, defined or assigned.
(see Note 1)
[L10 C7] #7 (Error) Constant `i' cannot be redefined.
(see Note 2)

"scope12.as", line 12: j:SingleInteger := 1
                       ^
[L12 C1] #10 (Note 3)  (cf. L13 C7)

"scope12.as", line 13: local j
                       ......^
[L13 C7] #9 (Error) It is illegal to declare an identifier free or local once it has already been used, defined or assigned.
(see Note 3)

"scope12.as", line 15: local t: Integer == 10
                       ......^
[L15 C7] #12 (Note 4)  (cf. L16 C1)

"scope12.as", line 16: t == t+1
                       ^
[L16 C1] #11 (Error) Constant `t' cannot be redefined.
(see Note 4)

"scope12.as", line 32:    setchildren_!: (%,List %)->%
                       ..............^
[L32 C15] #1 (Warning) Escape character ignored.  Do you mean '__'?

"scope12.as", line 34:    setvalue_!: (%,S) -> S
                       ...........^
[L34 C12] #2 (Warning) Escape character ignored.  Do you mean '__'?

"scope12.as", line 48: 
    tree(s,ls) == per [s,ls]          -- problem, no return type
.........^
[L48 C10] #13 (Error) Function return type must be specified.

"scope12.as", line 49: 
    tree(s) == per [s,[]]             -- problem, no return type
.........^
[L49 C10] #14 (Error) Function return type must be specified.

"scope12.as", line 54: 
    setchildren_!(t:%,ls:List %):% == (set!(rep t,args,ls);t)
...............^
[L54 C16] #3 (Warning) Escape character ignored.  Do you mean '__'?

"scope12.as", line 55:     setvalue_!(t:%,s:S):S == set!(rep t,value,s)
                       ............^
[L55 C13] #4 (Warning) Escape character ignored.  Do you mean '__'?

"scope12.as", line 60:     print<<x<<newline
                       ...........^
[L60 C12] #16 (Note 5)  (cf. L61 C11)

"scope12.as", line 61:     local x: Integer :=2
                       ..........^
[L61 C11] #15 (Error) It is illegal to declare an identifier free or local once it has already been used, defined or assigned.
(see Note 5)

