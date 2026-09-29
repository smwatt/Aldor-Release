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
  [Defi: [Decl: test [*Appl: -> [Decl: t Boolean] Boolean]] [Lamb: [Comm: [Decl: t Boolean]] Boolean [Labe: test t]]]
  [Defi: [Decl: Lst [*Appl: -> [Decl: S [With: _ [Sequ: 
    [Decl: = [Appl: -> [Comm: S S] Boolean]]
    [Decl: ~= [Appl: -> [Comm: S S] Boolean]]
    [Decl: print [Appl: -> S [Comm: ]]]
    ]
    ]] [With: _ [Sequ: 
    [Decl: nil %]
    [Decl: cons [Appl: -> [Comm: S %] %]]
    [Decl: empty? [Appl: -> % Boolean]]
    [Decl: first [Appl: -> % S]]
    [Decl: rest [Appl: -> % %]]
    [Decl: test [Appl: -> % Boolean]]
    ]
    ]]] [Lamb: [Comm: [Decl: S [With: _ [Sequ: 
    [Decl: = [Appl: -> [Comm: S S] Boolean]]
    [Decl: ~= [Appl: -> [Comm: S S] Boolean]]
    [Decl: print [Appl: -> S [Comm: ]]]
    ]
    ]]] [With: _ [Sequ: 
    [Decl: nil %]
    [Decl: cons [Appl: -> [Comm: S %] %]]
    [Decl: empty? [Appl: -> % Boolean]]
    [Decl: first [Appl: -> % S]]
    [Decl: rest [Appl: -> % %]]
    [Decl: test [Appl: -> % Boolean]]
    ]
    ] [Labe: Lst [Add: _ [Sequ: 
    _
    _
    [Defi: [Decl: P [With: _ [Sequ: 
      [Decl: nil? [Appl: -> % Boolean]]
      [Decl: nilptr %]
      [Decl: recptr [Appl: -> [*Appl: Record [Decl: first S] [Decl: rest P]] %]]
      [Decl: value [Appl: -> % [*Appl: Record [Decl: first S] [Decl: rest P]]]]
      ]
      ]] [Add: _ [Sequ: 
      _
      [Impo: _ Pointer]
      [Defi: [Decl: nil? [*Appl: -> [Decl: p %] Boolean]] [Lamb: [Comm: [Decl: p %]] Boolean [Labe: nil? [Appl: nil? [Pret: [Rest: p %] Pointer]]]]]
      [Defi: [Decl: nilptr %] [Pret: [Rest: nil Pointer] %]]
      [Defi: [Decl: recptr [*Appl: -> [Decl: r [*Appl: Record [Decl: first S] [Decl: rest P]]] %]] [Lamb: [Comm: [Decl: r [*Appl: Record [Decl: first S] [Decl: rest P]]]] % [Labe: recptr [Pret: r %]]]]
      [Defi: [Decl: value [*Appl: -> [Decl: p %] [*Appl: Record [Decl: first S] [Decl: rest P]]]] [Lamb: [Comm: [Decl: p %]] [*Appl: Record [Decl: first S] [Decl: rest P]] [Labe: value [Pret: p [*Appl: Record [Decl: first S] [Decl: rest P]]]]]]
      ]
      ]]
    [Sequ: 
      [Impo: _ [*Appl: Record [Decl: first S] [Decl: rest P]]]
      [Impo: _ P]
      [Impo: _ Boolean]
      ]
      
    [Defi: [Decl: rec [*Appl: -> [Decl: x %] [*Appl: Record [Decl: first S] [Decl: rest P]]]] [Lamb: [Comm: [Decl: x %]] [*Appl: Record [Decl: first S] [Decl: rest P]] [Labe: rec [Appl: value [Pret: [Rest: x %] P]]]]]
    [Defi: [Decl: empty? [*Appl: -> [Decl: l %] Boolean]] [Lamb: [Comm: [Decl: l %]] Boolean [Labe: empty? [Appl: nil? [Pret: [Rest: l %] P]]]]]
    [Defi: [Decl: nil %] [Pret: [Rest: nilptr P] %]]
    [Defi: [Decl: cons [*Appl: -> [Comm: [Decl: a S] [Decl: l %]] %]] [Lamb: [Comm: [Decl: a S] [Decl: l %]] % [Labe: cons [Pret: [Rest: [Appl: recptr [Appl: bracket a [Pret: [Rest: l %] P]]] P] %]]]]
    [Defi: [Decl: first [*Appl: -> [Decl: l %] S]] [Lamb: [Comm: [Decl: l %]] S [Labe: first [Appl: [Appl: rec l] first]]]]
    [Defi: [Decl: rest [*Appl: -> [Decl: l %] %]] [Lamb: [Comm: [Decl: l %]] % [Labe: rest [Pret: [Rest: [Appl: [Appl: rec l] rest] P] %]]]]
    [Defi: [Decl: test [*Appl: -> [Decl: l %] Boolean]] [Lamb: [Comm: [Decl: l %]] Boolean [Labe: test [Not: [Test: [Appl: empty? l]]]]]]
    ]
    ]]]]
  ]
  

"scope7.as", line 32:                 macro  Rep == Pointer
                      ..............................^
[L32 C31] #1 (Warning) Definition of macro `Rep' hides an outer definition.

