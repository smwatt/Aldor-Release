"macex6.as", line 10: macro y == y
                      ...........^
[L10 C12] #1 (Error) (After Macro Expansion) Circular macro expansion: y->y.
Expanded expression was: y

"macex6.as", line 13:         q == e(s)
                      ...............^
[L13 C16] #2 (Error) (After Macro Expansion) Circular macro expansion: q->s->s.
Expanded expression was: s
[L13 C16] #3 (Error) (After Macro Expansion) Circular macro expansion: r->s->s.
Expanded expression was: s
[L13 C16] #4 (Error) (After Macro Expansion) Circular macro expansion: s->s.
Expanded expression was: s

