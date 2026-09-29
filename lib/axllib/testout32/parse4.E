"parse4.as", line 14:   z
                      ..^
[L14 C3] #1 (Warning) Suspicious juxtaposition.  Check for missing `;'.
Check indentation if you are using `#pile'.

"parse4.as", line 17:   w
                      ..^
[L17 C3] #2 (Warning) Suspicious juxtaposition.  Check for missing `;'.
Check indentation if you are using `#pile'.

"parse4.as", line 18: a1: T
                      ^
[L18 C1] #3 (Warning) Suspicious `:'.  Do you mean `local' or `default'?

"parse4.as", line 19: v = w
                      ..^
[L19 C3] #4 (Warning) Suspicious `='.  Do you mean `==' or `:=' ?

"parse4.as", line 22: j(x) .
                      ^.^
[L22 C1] #6 (Warning) Local `j' is used without being assigned or defined.
[L22 C3] #9 (Warning) Local `x' is used without being assigned or defined.

"parse4.as", line 23:   z
                      ..^
[L23 C3] #8 (Warning) Local `z' is used without being assigned or defined.

"parse4.as", line 25: u(v) .
                      ^.^
[L25 C1] #7 (Warning) Local `u' is used without being assigned or defined.
[L25 C3] #11 (Warning) Local `v' is used without being assigned or defined.

"parse4.as", line 27: do a2: T
                      .......^
[L27 C8] #5 (Warning) Local `T' is used without being assigned or defined.

"parse4.as", line 28: do w = w
                      .......^
[L28 C8] #10 (Warning) Local `w' is used without being assigned or defined.

