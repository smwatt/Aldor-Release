"macex7.as", line 10: macro f(a,b) == a + b
                      ..................^
[L10 C19] #2 (Error) (After Macro Expansion) There are no suitable meanings for the operator `+'.
The following could be suitable if imported:
  +: (Integer, Integer) -> Integer from Integer
Expanded expression was: +

"macex7.as", line 17: g(1$Integer, 1)           -- bad, after macro expansion
                      .............^
[L17 C14] #3 (Error) (After Macro Expansion) No meaning for identifier `1'.
Expanded expression was: 1

"macex7.as", line 19: 1$Integer + 1$Integer     -- bad, no macro expansion
                      ..........^
[L19 C11] #4 (Error) There are no suitable meanings for the operator `+'.
The following could be suitable if imported:
  +: (Integer, Integer) -> Integer from Integer

"macex7.as", line 23: macro zero?(zzz) == zzz = 0
                      ..........................^
[L23 C27] #1 (Error) (After Macro Expansion) No meaning for identifier `0'.
Expanded expression was: 0

"macex7.as", line 30:   == add
                      .....^
[L30 C6] #5 (Error) The domain is missing some exports.
	Missing =: (%, %) -> Boolean
	Missing 0: %

