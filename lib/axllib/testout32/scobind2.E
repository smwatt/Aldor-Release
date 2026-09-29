"scobind2.as", line 16: for i : Integer in 1..10 for j in 2..8 repeat
                        .............................^
[L16 C30] #1 (Error) Cannot both assign and define `j' in the same scope.
Choose `==', `:=', or use as a `for' variable.
(see Note 1)

"scobind2.as", line 18:     i := i + 1
                        ....^
[L18 C5] #3 (Error) Cannot explicitly assign a `for' variable.

"scobind2.as", line 19:     j == 9
                        ....^
[L19 C5] #2 (Note 1)  (cf. L16 C30)

"scobind2.as", line 22: [(z := 12) for z in 1..10]
                        ..^
[L22 C3] #4 (Error) Cannot explicitly assign a `for' variable.

"scobind2.as", line 23: [(z == 12) for z in 1..10]
                        ..^............^
[L23 C3] #6 (Note 2)  (cf. L23 C16)
[L23 C16] #5 (Error) Cannot both assign and define `z' in the same scope.
Choose `==', `:=', or use as a `for' variable.
(see Note 2)

