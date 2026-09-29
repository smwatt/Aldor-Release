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
  [Impo: _ TextWriter]
  [Impo: [With: _ [Sequ: 
    [Decl: string [Appl: -> Literal %]]
    [Decl: << [Appl: -> [Comm: TextWriter %] TextWriter]]
    ]
    ] String]
  [Impo: [With: _ [Decl: integer [Appl: -> Literal %]]] Integer]
  [Impo: [With: _ [Sequ: 
    [Decl: nil %]
    [Decl: cons [Appl: -> [Comm: Integer %] %]]
    [Decl: << [Appl: -> [Comm: TextWriter %] TextWriter]]
    ]
    ] [Appl: List Integer]]
  [Inli: [With: _ [Decl: integer [Appl: -> Literal %]]] Integer]
  [Inli: [With: _ [Sequ: 
    [Decl: nil %]
    [Decl: cons [Appl: -> [Comm: Integer %] %]]
    ]
    ] [Appl: List Integer]]
  [Expo: [Decl: f [Appl: -> Integer Integer]] _ _]
  [Defi: [Decl: f [*Appl: -> [Decl: n Integer] Integer]] [Lamb: [Comm: [Decl: n Integer]] Integer [Labe: f [Sequ: 
    [Impo: _ [Appl: List Integer]]
    [Appl: first [Appl: rest [Appl: cons n [Appl: cons n nil]]]]
    ]
    ]]]
  [Appl: << [Appl: << print Lit: hello] newline]
  [Appl: << [Appl: << [Appl: << print [Appl: cons Lit: 3 [Appl: cons Lit: 2 nil]]] newline] newline]
  ]
  

