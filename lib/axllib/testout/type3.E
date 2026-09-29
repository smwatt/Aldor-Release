"type3.as", line 8:         sin:    F -> F
                    .....................^
[L8 C22] #1 (Error) No meaning for identifier `F'.

"type3.as", line 16:                 X__sin: F -> F
                     .............................^
[L16 C30] #3 (Error) No meaning for identifier `F'.

"type3.as", line 21:                         sin: F -> F
                     ..................................^
[L21 C35] #9 (Error) No meaning for identifier `F'.

"type3.as", line 26:                 X__sin(x: F): F == sin x
                     ..........................^........^
[L26 C27] #7 (Error) No meaning for identifier `F'.
[L26 C36] #14 (Error) There are 2 meanings for `sin' in this context.
The possible types were:
	  sin: F -> F, a local
	  sin: (x: F) -> F, a local
  The context requires an expression of type (x: F) -> F.

"type3.as", line 27:                 X__cos(x: F): F == cos x
                     ..........................^........^
[L27 C27] #12 (Error) No meaning for identifier `F'.
[L27 C36] #15 (Error) There are 2 meanings for `cos' in this context.
The possible types were:
	  cos: F -> F, a local
	  cos: (x: F) -> F, a local
  The context requires an expression of type (x: F) -> F.

"type3.as", line 28:                 X__tan(x: F): F == tan x
                     ..........................^........^
[L28 C27] #13 (Error) No meaning for identifier `F'.
[L28 C36] #16 (Error) There are 2 meanings for `tan' in this context.
The possible types were:
	  tan: F -> F, a local
	  tan: (x: F) -> F, a local
  The context requires an expression of type (x: F) -> F.

"type3.as", line 31:         sin(x: F): F == X__sin(x)
                     ...............^
[L31 C16] #5 (Error) No meaning for identifier `F'.

"type3.as", line 32:         cos(x: F): F == X__cos(x)
                     ...............^
[L32 C16] #18 (Error) No meaning for identifier `F'.

"type3.as", line 33:         tan(x: F): F == X__tan(x)
                     ...............^
[L33 C16] #19 (Error) No meaning for identifier `F'.

