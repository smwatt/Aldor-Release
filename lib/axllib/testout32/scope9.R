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
  [Sequ: 
    _
    _
    _
    ]
    
  [Defi: [Decl: RationalNumber [With: _ [Sequ: 
    [Decl: ratio [Appl: -> [Comm: Integer Integer] RationalNumber]]
    [Decl: numer [Appl: -> RationalNumber Integer]]
    [Decl: denom [Appl: -> RationalNumber Integer]]
    [Decl: 0 RationalNumber]
    [Decl: 1 RationalNumber]
    [Decl: + [Appl: -> [Comm: RationalNumber RationalNumber] RationalNumber]]
    [Decl: - [Appl: -> [Comm: RationalNumber RationalNumber] RationalNumber]]
    [Decl: * [Appl: -> [Comm: Integer RationalNumber] RationalNumber]]
    [Decl: * [Appl: -> [Comm: RationalNumber RationalNumber] RationalNumber]]
    [Decl: inv [Appl: -> RationalNumber RationalNumber]]
    [Decl: / [Appl: -> [Comm: Integer Integer] RationalNumber]]
    [Decl: / [Appl: -> [Comm: RationalNumber RationalNumber] RationalNumber]]
    [Decl: cancelGcd [Appl: -> [Comm: Integer Integer] [Comm: Integer Integer]]]
    [Decl: cancelGcd! [Appl: -> [*Appl: Record [Decl: numer Integer] [Decl: denom Integer]] Integer]]
    [Decl: coerce [Appl: -> Integer RationalNumber]]
    ]
    ]] [Add: _ [Sequ: 
    [Sequ: 
      [Impo: _ Integer]
      [Impo: _ [*Appl: Record [Decl: numer Integer] [Decl: denom Integer]]]
      ]
      
    [Defa: [Decl: [Comm: n d g] Integer]]
    [Defa: [Decl: [Comm: a b r] RationalNumber]]
    [Defi: [Decl: reduce [*Appl: -> [Comm: [Decl: n Integer] [Decl: d Integer]] RationalNumber]] [Lamb: [Comm: [Decl: n Integer] [Decl: d Integer]] RationalNumber [Labe: reduce [Sequ: 
      [Assi: g [Appl: gcd n d]]
      [Appl: ratio [Appl: quo n g] [Appl: quo d g]]
      ]
      ]]]
    [Defi: [Decl: ratio [*Appl: -> [Comm: [Decl: n Integer] [Decl: d Integer]] RationalNumber]] [Lamb: [Comm: [Decl: n Integer] [Decl: d Integer]] RationalNumber [Labe: ratio [Pret: [Appl: bracket n d] RationalNumber]]]]
    [Defi: [Decl: reduce [*Appl: -> [Decl: r RationalNumber] RationalNumber]] [Lamb: [Comm: [Decl: r RationalNumber]] RationalNumber [Labe: reduce [Sequ: 
      [Assi: g [Appl: gcd [Appl: numer r] [Appl: denom r]]]
      [Appl: ratio [Appl: quo [Appl: numer r] g] [Appl: quo [Appl: denom r] g]]
      ]
      ]]]
    [Defi: [Decl: cancelGcd [*Appl: -> [Comm: [Decl: n Integer] [Decl: d Integer]] [Comm: Integer Integer]]] [Lamb: [Comm: [Decl: n Integer] [Decl: d Integer]] [Comm: Integer Integer] [Labe: cancelGcd [Sequ: 
      [Assi: g [Appl: gcd n d]]
      [Comm: [Appl: quo n g] [Appl: quo d g]]
      ]
      ]]]
    [Defi: [Decl: cancelGcd! [*Appl: -> [Decl: r [*Appl: Record [Decl: numer Integer] [Decl: denom Integer]]] Integer]] [Lamb: [Comm: [Decl: r [*Appl: Record [Decl: numer Integer] [Decl: denom Integer]]]] Integer [Labe: cancelGcd! [Sequ: 
      [Assi: g [Appl: gcd [Appl: apply r numer] [Appl: apply r denom]]]
      [Appl: set! r numer [Appl: quo [Appl: apply r numer] g]]
      [Appl: set! r denom [Appl: quo [Appl: apply r denom] g]]
      g
      ]
      ]]]
    [Defi: [Decl: coerce [*Appl: -> [Decl: n Integer] RationalNumber]] [Lamb: [Comm: [Decl: n Integer]] RationalNumber [Labe: coerce [Appl: ratio n 1]]]]
    [Defi: [Decl: numer [*Appl: -> [Decl: a RationalNumber] Integer]] [Lamb: [Comm: [Decl: a RationalNumber]] Integer [Labe: numer [Appl: apply [Pret: a [*Appl: Record [Decl: numer Integer] [Decl: denom Integer]]] numer]]]]
    [Defi: [Decl: denom [*Appl: -> [Decl: a RationalNumber] Integer]] [Lamb: [Comm: [Decl: a RationalNumber]] Integer [Labe: denom [Appl: apply [Pret: a [*Appl: Record [Decl: numer Integer] [Decl: denom Integer]]] denom]]]]
    [Defi: [Decl: 0 RationalNumber] [Appl: ratio 0 1]]
    [Defi: [Decl: 1 RationalNumber] [Appl: ratio 1 1]]
    [Defi: [Decl: + [*Appl: -> [Comm: [Decl: a RationalNumber] [Decl: b RationalNumber]] RationalNumber]] [Lamb: [Comm: [Decl: a RationalNumber] [Decl: b RationalNumber]] RationalNumber [Labe: + [Sequ: 
      [Assi: [Decl: z [*Appl: Record [Decl: numer Integer] [Decl: denom Integer]]] [Appl: bracket [Appl: denom a] [Appl: denom b]]]
      [Assi: g [Appl: cancelGcd! z]]
      [Assi: [Decl: zz [*Appl: Record [Decl: numer Integer] [Decl: denom Integer]]] [Appl: bracket [Appl: + [Appl: * [Appl: apply z denom] [Appl: numer a]] [Appl: * [Appl: apply z numer] [Appl: numer b]]] g]]
      [Appl: cancelGcd! zz]
      [Appl: set! zz denom [Appl: * [Appl: * [Appl: apply zz denom] [Appl: apply z numer]] [Appl: apply z denom]]]
      [Pret: zz RationalNumber]
      ]
      ]]]
    [Defi: [Decl: - [*Appl: -> [Comm: [Decl: a RationalNumber] [Decl: b RationalNumber]] RationalNumber]] [Lamb: [Comm: [Decl: a RationalNumber] [Decl: b RationalNumber]] RationalNumber [Labe: - [Appl: ratio [Appl: - [Appl: * [Appl: denom b] [Appl: numer a]] [Appl: * [Appl: denom a] [Appl: numer b]]] [Appl: * [Appl: denom a] [Appl: denom b]]]]]]
    [Defi: [Decl: * [*Appl: -> [Comm: [Decl: a RationalNumber] [Decl: b RationalNumber]] RationalNumber]] [Lamb: [Comm: [Decl: a RationalNumber] [Decl: b RationalNumber]] RationalNumber [Labe: * [Sequ: 
      [Assi: [Decl: a1 RationalNumber] [Appl: reduce [Appl: numer a] [Appl: denom b]]]
      [Assi: [Decl: a2 RationalNumber] [Appl: reduce [Appl: numer b] [Appl: denom a]]]
      [Appl: ratio [Appl: * [Appl: numer a] [Appl: numer b]] [Appl: * [Appl: denom a] [Appl: denom b]]]
      ]
      ]]]
    [Defi: [Decl: * [*Appl: -> [Comm: [Decl: n Integer] [Decl: b RationalNumber]] RationalNumber]] [Lamb: [Comm: [Decl: n Integer] [Decl: b RationalNumber]] RationalNumber [Labe: * [Sequ: 
      [Assi: [Decl: g Integer] [Appl: gcd n [Appl: denom b]]]
      [Appl: ratio [Appl: * [Appl: quo n g] [Appl: numer b]] [Appl: quo [Appl: denom b] g]]
      ]
      ]]]
    [Defi: [Decl: / [*Appl: -> [Comm: [Decl: n Integer] [Decl: d Integer]] RationalNumber]] [Lamb: [Comm: [Decl: n Integer] [Decl: d Integer]] RationalNumber [Labe: / [Appl: reduce [Appl: ratio n d]]]]]
    [Defi: [Decl: inv [*Appl: -> [Decl: a RationalNumber] RationalNumber]] [Lamb: [Comm: [Decl: a RationalNumber]] RationalNumber [Labe: inv [Appl: ratio [Appl: denom a] [Appl: numer a]]]]]
    [Defi: [Decl: / [*Appl: -> [Comm: [Decl: a RationalNumber] [Decl: b RationalNumber]] RationalNumber]] [Lamb: [Comm: [Decl: a RationalNumber] [Decl: b RationalNumber]] RationalNumber [Labe: / [Appl: * a [Appl: inv b]]]]]
    ]
    ]]
  ]
  

"scope9.as", line 55:         cancelGcd!(r:R):I ==
                      ...................^
[L55 C20] #1 (Warning) `r' has a default type and a different explicit type declaration.

