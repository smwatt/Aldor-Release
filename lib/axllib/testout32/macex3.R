*** Starting "macex" phase...
*** Result of macex:
[Sequ: 
  [Sequ: 
    _
    _
    _
    _
    ]
    
  [Defi: [Decl: f [Appl: -> [Decl: u Integer] Integer]] [Lamb: [Comm: [Decl: u Integer]] Integer [Labe: f [Sequ: 
    _
    [Defi: [Decl: g [Appl: -> [Decl: y Integer] Integer]] [Lamb: [Comm: [Decl: y Integer]] Integer [Labe: g [Sequ: 
      _
      [Appl: concat Lit: Number [Appl: stringify y]]
      ]
      ]]]
    [Appl: g [Appl: c1 u [Appl: - a2 a1]]]
    ]
    ]]]
  [Appl: - a1 a2]
  ]
  

"macex3.as", line 14:   macro a == a2 - a1
                      ................^
[L14 C17] #1 (Warning) Definition of macro `a' hides an outer definition.

"macex3.as", line 16:     macro i + j == concat(i,j)
                      ..........^
[L16 C11] #2 (Warning) Definition of macro `+' hides an outer definition.

