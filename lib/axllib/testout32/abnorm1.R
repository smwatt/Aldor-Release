*** Starting "abnorm" phase...
*** Result of abnorm:
[Sequ: 
  [Sequ: 
    [MDef: rep [MLam: [Comm: x] [Pret: [Rest: x %] Rep]]]
    [MDef: per [MLam: [Comm: r] [Pret: [Rest: r Rep] %]]]
    ]
    
  [Sequ: 
    [MDef: BBool [Qual: Bool Machine]]
    [MDef: BChar [Qual: Char Machine]]
    [MDef: BArr [Qual: Arr Machine]]
    [MDef: BPtr [Qual: Ptr Machine]]
    [MDef: BByte [Qual: XByte Machine]]
    [MDef: BHInt [Qual: HInt Machine]]
    [MDef: BSInt [Qual: SInt Machine]]
    [MDef: BBInt [Qual: BInt Machine]]
    [MDef: BSFlo [Qual: SFlo Machine]]
    [MDef: BDFlo [Qual: DFlo Machine]]
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
  [Defi: [Decl: List [With: _ [Sequ: 
    [MDef: S Integer]
    [MDef: % List]
    [Decl: nil %]
    [Decl: cons [Appl: -> [Comm: S %] %]]
    [Decl: empty? [Appl: -> % Boolean]]
    [Decl: first [Appl: -> % S]]
    [Decl: rest [Appl: -> % %]]
    ]
    ]] [Add: _ [Sequ: 
    [MDef: S Integer]
    [MDef: % List]
    ]
    ]]
  ]
  

