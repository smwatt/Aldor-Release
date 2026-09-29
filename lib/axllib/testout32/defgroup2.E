"defgroup2.as", line 28:    A:SingleInteger ==
                         ...^
[L28 C4] #5 (Warning) Implementation restriction: the value of `A:SingleInteger' depends on the value of `B:SingleInteger'. Perhaps you ought to define `A:SingleInteger' later?

"defgroup2.as", line 40:    B:SingleInteger == C + 3;
                         ...^
[L40 C4] #1 (Warning) Illegal recursive definition: <B:SingleInteger, C:SingleInteger, D:SingleInteger, B:SingleInteger>
[L40 C4] #4 (Warning) Implementation restriction: the value of `B:SingleInteger' depends on the value of `C:SingleInteger'. Perhaps you ought to define `B:SingleInteger' later?

"defgroup2.as", line 44:    C:SingleInteger ==
                         ...^
[L44 C4] #3 (Warning) Implementation restriction: the value of `C:SingleInteger' depends on the value of `D:SingleInteger'. Perhaps you ought to define `C:SingleInteger' later?

"defgroup2.as", line 54:    D:SingleInteger ==
                         ...^
[L54 C4] #2 (Warning) Implementation restriction: the value of `D:SingleInteger' depends on the value of `E:SingleInteger'. Perhaps you ought to define `D:SingleInteger' later?

