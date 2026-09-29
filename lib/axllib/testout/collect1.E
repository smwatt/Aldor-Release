"collect1.as", line 30: generate yield 1
                        ^
[L30 C1] #1 (Error) Have determined 3 possible types for the expression.
	Meaning 1: Generator(DoubleFloat)
	Meaning 2: Generator(SingleInteger)
	Meaning 3: Generator(Integer)

"collect1.as", line 33: (generate yield si)@Generator(Integer)
                        ................^
[L33 C17] #2 (Error) There are 0 meanings for `si' in this context.
The possible types were:
	  si: SingleInteger, a local
  The context requires an expression of type Integer.

"collect1.as", line 36: yield si
                        ^
[L36 C1] #3 (Error) The `yield' is not inside a `generate'.

"collect1.as", line 39: generate ( ((): Boolean +-> (yield si; false)) () )
                        .............................^
[L39 C30] #4 (Error) The `yield' is not inside a `generate'.

"collect1.as", line 42: generate ( (yield si; false) while true )
                        ............^
[L42 C13] #5 (Error) The `yield' is not inside a `generate'.

