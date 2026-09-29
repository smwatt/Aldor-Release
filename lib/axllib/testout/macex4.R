*** Starting "macex" phase...
*** Result of macex:
[Sequ: 
  [Sequ: 
    _
    _
    _
    ]
    
  [Appl: expt v Lit: 10]
  [Wher: [Sequ: 
    _
    _
    ]
     [Appl: - [Appl: * Lit: 10 z] 1]]
  v
  v
  [Appl: expt v Lit: 10]
  [Wher: [Sequ: 
    _
    _
    ]
     [Appl: subt [Appl: mult Lit: 10 v] 1]]
  v
  [Appl: expt v Lit: 10]
  ]
  

"macex4.as", line 15:   macro u == z;
                      .............^
[L15 C14] #1 (Warning) Definition of macro `u' hides an outer definition.

"macex4.as", line 24:   macro x == v;
                      .............^
[L24 C14] #2 (Warning) Definition of macro `x' hides an outer definition.

