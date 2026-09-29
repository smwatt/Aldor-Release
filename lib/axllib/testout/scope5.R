*** Starting "scobind" phase...
*** Result of scobind:
[Sequ: 
  [Sequ: 
    _
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
  [Loca: [Decl: g0 [Appl: -> [Comm: ] Integer]]]
  [Loca: [Decl: g1 [Appl: -> Integer Integer]]]
  [Loca: [Decl: g2 [Appl: -> [Comm: Integer Integer] Integer]]]
  [Appl: g1 [Assi: [Decl: a Integer] Lit: 4]]
  [Loca: [Decl: f1 [*Appl: -> [Defi: [Decl: n Integer] Lit: 5] [Appl: GaloisField n]]]]
  [Loca: [Decl: f2 [*Appl: -> [Comm: [Decl: n Integer] [Decl: m Integer]] [Appl: RectangularMatrix n m Integer]]]]
  [*Appl: h1 [Defi: aaa Lit: 3]]
  [*Appl: Record [Decl: s Integer] [Decl: t SingleFloat]]
  [*Appl: h2 [Assi: b Lit: 4] [Defi: c Lit: 5]]
  ]
  

"scope5.as", line 10: local g0: () -> Integer
                      ......^
[L10 C7] #5 (Warning) Local `g0' is not assigned, defined, or used.

"scope5.as", line 12: local g2: (Integer,Integer) -> Integer
                      ......^
[L12 C7] #3 (Warning) Local `g2' is not assigned, defined, or used.

"scope5.as", line 14: g1(a : Integer := 4)
                      ^
[L14 C1] #4 (Warning) Local `g1' is used without being assigned or defined.

"scope5.as", line 18: local f1: (n : Integer == 5) -> GaloisField(n)
                      ......^
[L18 C7] #2 (Warning) Local `f1' is not assigned, defined, or used.

"scope5.as", line 19: 
local f2: (n : Integer, m: Integer) -> RectangularMatrix(n,m,Integer)
......^
[L19 C7] #1 (Warning) Local `f2' is not assigned, defined, or used.

