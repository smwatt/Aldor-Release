"scobind1.as", line 14: local y : SingleInteger := 1
                        ......^
[L14 C7] #5 (Note 1)  (cf. L21 C17)

"scobind1.as", line 16: 
f(x: SingleInteger, w : SingleInteger):SingleInteger ==
..^
[L16 C3] #7 (Note 2)  (cf. L20 C17)

"scobind1.as", line 17: 
    local w                             -- bad, parameters cannot be declared free or local
..........^
[L17 C11] #3 (Error) Parameters cannot be declared local or free.

"scobind1.as", line 20: 
          local x : SingleInteger == 3  -- bad, local const with same name as parameter
................^
[L20 C17] #6 (Error) A local constant may not have the same name as an outer variable or parameter.
(see Note 2)

"scobind1.as", line 21: 
          local y : SingleInteger == 4  -- bad, local const with same name as outer variable
................^
[L21 C17] #4 (Error) A local constant may not have the same name as an outer variable or parameter.
(see Note 1)

"scobind1.as", line 29: 
    q : SingleInteger := 700            -- give implicit local message
....^
[L29 C5] #2 (Warning) Implicit local `q' is a parameter, local or explicit free in an outer scope. Add a `local' declaration if this is what you intended.

"scobind1.as", line 30: 
    r : SingleInteger := 700            -- give implicit local message
....^
[L30 C5] #1 (Warning) Implicit local `r' is a parameter, local or explicit free in an outer scope. Add a `local' declaration if this is what you intended.

"scobind1.as", line 34: 
        q : SingleInteger := 700        -- give implicit local message
........^
[L34 C9] #8 (Warning) Implicit local `q' is a parameter, local or explicit free in an outer scope. Add a `local' declaration if this is what you intended.

"scobind1.as", line 40: AxlLib := Array
                        ^
[L40 C1] #10 (Error) `AxlLib' is used in a type, so must be constant, and so cannot be assigned to.
[L40 C1] #11 (Error) Cannot assign to or redefine library or archive constant `AxlLib'.

"scobind1.as", line 43: AxlDem == Array
                        ^
[L43 C1] #9 (Error) Cannot assign to or redefine library or archive constant `AxlDem'.

