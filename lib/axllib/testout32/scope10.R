*** Starting "scobind" phase...
*** Result of scobind:
[Sequ: 
  [Sequ: 
    _
    _
    ]
    
  [Sequ: 
    _
    _
    _
    _
    _
    _
    _
    _
    _
    _
    ]
    
  [Sequ: 
    [Impo: _ AxlLib]
    [Inli: _ AxlLib]
    ]
    
  [Impo: _ Boolean]
  [Impo: [With: _ [Sequ: 
    [Decl: string [Appl: -> Literal %]]
    [Decl: << [Appl: -> [Comm: TextWriter %] TextWriter]]
    [Decl: << [Appl: -> % [Appl: -> TextWriter TextWriter]]]
    ]
    ] String]
  [Impo: [With: _ [Sequ: 
    [Decl: newline %]
    [Decl: << [Appl: -> [Comm: TextWriter %] TextWriter]]
    [Decl: << [Appl: -> % [Appl: -> TextWriter TextWriter]]]
    ]
    ] Character]
  [Impo: [With: _ [Sequ: 
    [Decl: print %]
    [Decl: error %]
    ]
    ] TextWriter]
  [Impo: _ FormattedOutput]
  [Defi: [Decl: Flotsam [With: _ [Decl: + [Appl: -> [Comm: % %] %]]]] [Add: _ [Sequ: 
    [Impo: _ Integer]
    [Decl: + [Appl: -> [Comm: % %] %]]
    [Decl: - [Appl: -> [Comm: % %] %]]
    [Decl: - [Appl: -> % %]]
    [Decl: normalize [Appl: -> % %]]
    [Decl: negate [Appl: -> % %]]
    [Decl: plus [Appl: -> [Comm: % %] %]]
    [Defa: [Decl: [Comm: x y] %]]
    [Defi: [Decl: - [*Appl: -> [Decl: x %] %]] [Lamb: [Comm: [Decl: x %]] % [Labe: - [Appl: normalize [Appl: negate x]]]]]
    [Defi: [Decl: + [*Appl: -> [Comm: [Decl: x %] [Decl: y %]] %]] [Lamb: [Comm: [Decl: x %] [Decl: y %]] % [Labe: + [Appl: normalize [Appl: plus x y]]]]]
    ]
    ]]
  ]
  

