"incl/includeB.as", line 2: 
#reinclude "include2.as"
^
[L2 C1] #1 (Error) Circular include files: 'include2.as'->'incl/includeB.as'.

"include2.as", line 12: #include "nonexistent.as"
                        ^
[L12 C1] #2 (Error) Could not open file `nonexistent.as'.

"include2.as", line 15: #elseif
                        ^
[L15 C1] #3 (Error) Improper use of `elseif' system command.

"include2.as", line 16: #else
                        ^
[L16 C1] #4 (Error) Unbalanced `#else'.

"include2.as", line 21: #elif YesWay
                        ^
[L21 C1] #5 (Warning) Unknown system command.

"include2.as", line 28: -- End of file
                        ^
[L28 C1] #6 (Error) End of file encountered in `#if'.

