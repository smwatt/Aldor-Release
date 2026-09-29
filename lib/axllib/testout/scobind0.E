"scobind0.as", line 18: local y
                        ......^
[L18 C7] #26 (Error) Cannot declare `y' both free and local.

"scobind0.as", line 19: 
free  y               -- Triggers free/local conflict error
......^
[L19 C7] #1 (Error) Cannot find scope in which free variable `y' is bound.

"scobind0.as", line 21: 
free  z : Integer     -- Triggers 'no outer scope' error.
......^
[L21 C7] #2 (Error) Cannot find scope in which free variable `z' is bound.

"scobind0.as", line 22: 
local z : Integer     -- Triggers free/local conflict error
......^
[L22 C7] #3 (Error) Cannot declare `z' both free and local.

"scobind0.as", line 24: 
free  w               -- Triggers 'no outer scope' error.
......^
[L24 C7] #4 (Error) Cannot find scope in which free variable `w' is bound.

"scobind0.as", line 25: 
local w : Integer     -- Triggers free/local conflict error
......^
[L25 C7] #5 (Error) Cannot declare `w' both free and local.

"scobind0.as", line 27: 
free  u : Integer     -- Triggers 'no outer scope' error.
......^
[L27 C7] #6 (Error) Cannot find scope in which free variable `u' is bound.

"scobind0.as", line 28: 
local u               -- Triggers free/local conflict error
......^
[L28 C7] #7 (Error) Cannot declare `u' both free and local.

"scobind0.as", line 31:     local x
                        ..........^
[L31 C11] #8 (Error) Cannot declare `x' both free and local.

"scobind0.as", line 38: 
    free q            -- Cannot declare a parameter free
.........^
[L38 C10] #9 (Error) Parameters cannot be declared local or free.
[L38 C10] #10 (Error) Cannot find scope in which free variable `q' is bound.

"scobind0.as", line 39: 
    local r           -- Cannot declare a parameter local
..........^
[L39 C11] #11 (Error) Parameters cannot be declared local or free.

"scobind0.as", line 40:     free x: Integer
                        .........^
[L40 C10] #13 (Note 1)  (cf. L41 C5)

"scobind0.as", line 41: 
    x == 1            -- Cannot define a free constant in an inner scope.
....^
[L41 C5] #12 (Error) A constant declared free in an inner scope (`x') cannot be defined in that scope.
(see Note 1)

"scobind0.as", line 43:     qq : Integer := 1
                        ....^
[L43 C5] #15 (Note 2)  (cf. L44 C10)

"scobind0.as", line 44:     free qq
                        .........^
[L44 C10] #14 (Error) It is illegal to declare an identifier free or local once it has already been used, defined or assigned.
(see Note 2)

"scobind0.as", line 45:     rr == 2
                        ....^
[L45 C5] #17 (Note 3)  (cf. L46 C10)

"scobind0.as", line 46:     free rr
                        .........^
[L46 C10] #16 (Error) It is illegal to declare an identifier free or local once it has already been used, defined or assigned.
(see Note 3)
[L46 C10] #18 (Error) Cannot find scope in which free variable `rr' is bound.

"scobind0.as", line 49: 
        x : Integer := 3   -- Trigger warning about implicit local
........^
[L49 C9] #19 (Warning) Implicit local `x' is a parameter, local or explicit free in an outer scope. Add a `local' declaration if this is what you intended.

"scobind0.as", line 58:     free getenv: String -> String
                        .........^
[L58 C10] #20 (Error) Cannot find scope in which free variable `getenv' is bound.
[L58 C10] #22 (Error) A built-in or foreign function cannot be declared `free' or `local'.

"scobind0.as", line 59:     local puts:   String -> SingleInteger
                        ..........^
[L59 C11] #21 (Error) A built-in or foreign function cannot be declared `free' or `local'.

"scobind0.as", line 67:     free BoolFalse:    () -> BBool
                        .........^
[L67 C10] #23 (Error) Cannot find scope in which free variable `BoolFalse' is bound.
[L67 C10] #25 (Error) A built-in or foreign function cannot be declared `free' or `local'.

"scobind0.as", line 68:     local BoolTrue:     () -> BBool
                        ..........^
[L68 C11] #24 (Error) A built-in or foreign function cannot be declared `free' or `local'.

