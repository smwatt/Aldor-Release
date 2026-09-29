"scobind3.as", line 6: x: Integer == 1
                       ^
[L6 C1] #6 (Note 3)  (cf. L10 C1)

"scobind3.as", line 10: x: Boolean := 1 
                        ^
[L10 C1] #4 (Note 2)  (cf. L14 C1)
[L10 C1] #5 (Error) Cannot both assign and define `x' in the same scope.
Choose `==', `:=', or use as a `for' variable.
(see Note 3)

"scobind3.as", line 11: @Natascia
                        .^
[L11 C2] #2 (Note 1)  (cf. L16 C2)

"scobind3.as", line 14: x: Integer := 1
                        ^
[L14 C1] #3 (Error) Variables cannot have different types in the same scope.
(see Note 2)

"scobind3.as", line 16: @Natascia
                        .^
[L16 C2] #1 (Error) Cannot use label `Natascia' more than once in a given scope.
(see Note 1)

