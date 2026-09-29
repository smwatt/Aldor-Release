*** Starting "abcheck" phase...
*** Result of abcheck:
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
    
  [Defi: [Decl: VecInt [With: _ [Sequ: 
    [Decl: new [Appl: -> [Comm: SingleInteger Integer] VecInt]]
    [Decl: apply [Appl: -> [Comm: VecInt SingleInteger] Integer]]
    [Decl: set! [Appl: -> [Comm: VecInt SingleInteger Integer] Integer]]
    [Decl: # [Appl: -> VecInt SingleInteger]]
    [Decl: empty? [Appl: -> VecInt Boolean]]
    [Decl: empty [Appl: -> [Comm: ] VecInt]]
    [Decl: + [Appl: -> [Comm: VecInt VecInt] VecInt]]
    [Decl: - [Appl: -> [Comm: VecInt VecInt] VecInt]]
    [Decl: - [Appl: -> VecInt VecInt]]
    [Decl: * [Appl: -> [Comm: VecInt VecInt] VecInt]]
    [Decl: * [Appl: -> [Comm: Integer VecInt] VecInt]]
    [Decl: = [Appl: -> [Comm: VecInt VecInt] Boolean]]
    ]
    ]] [Add: _ [Sequ: 
    [Buil: [Sequ: 
      [Decl: ArrNew [Appl: -> [Comm: Integer SingleInteger] VecInt]]
      [Decl: ArrElt [Appl: -> [Comm: VecInt SingleInteger] Integer]]
      [Decl: ArrSet [Appl: -> [Comm: VecInt SingleInteger Integer] Integer]]
      ]
      ]
    [Impo: _ SingleInteger]
    [Impo: _ Integer]
    [Defi: [Decl: new [Appl: -> [Comm: [Decl: size SingleInteger] [Decl: val Integer]] VecInt]] [Lamb: [Comm: [Decl: size SingleInteger] [Decl: val Integer]] VecInt [Labe: new [Appl: ArrNew val size]]]]
    [Defi: [Decl: new [Appl: -> [Decl: size SingleInteger] VecInt]] [Lamb: [Comm: [Decl: size SingleInteger]] VecInt [Labe: new [Appl: ArrNew 0 size]]]]
    [Defi: [Decl: apply [Appl: -> [Comm: [Decl: v VecInt] [Decl: index SingleInteger]] Integer]] [Lamb: [Comm: [Decl: v VecInt] [Decl: index SingleInteger]] Integer [Labe: apply [Appl: ArrElt v index]]]]
    [Defi: [Decl: set! [Appl: -> [Comm: [Decl: v VecInt] [Decl: index SingleInteger] [Decl: val Integer]] Integer]] [Lamb: [Comm: [Decl: v VecInt] [Decl: index SingleInteger] [Decl: val Integer]] Integer [Labe: set! [Appl: ArrSet v index val]]]]
    [Defi: [Decl: # [Appl: -> [Decl: v VecInt] SingleInteger]] [Lamb: [Comm: [Decl: v VecInt]] SingleInteger [Labe: # [Appl: # [Pret: [Rest: v %] Rep]]]]]
    [Defi: [Decl: empty [Appl: -> [Comm: ] VecInt]] [Lamb: [Comm: ] VecInt [Labe: empty [Appl: new 0]]]]
    [Defi: [Decl: empty? [Appl: -> [Decl: v VecInt] Boolean]] [Lamb: [Comm: [Decl: v VecInt]] Boolean [Labe: empty? [Appl: = [Appl: # v] 0]]]]
    [Defi: [Decl: map [Appl: -> [Comm: [Decl: f [Appl: -> Integer Integer]] [Decl: v VecInt]] VecInt]] [Lamb: [Comm: [Decl: f [Appl: -> Integer Integer]] [Decl: v VecInt]] VecInt [Labe: map [Sequ: 
      [Assi: [Decl: n SingleInteger] [Appl: # v]]
      [Assi: [Decl: vv VecInt] [Appl: new n]]
      [Repe: [Appl: set! vv i [Appl: f [Appl: apply v i]]] [For: [Decl: i SingleInteger] [Appl: .. 0 [Appl: - n 1]] _]]
      vv
      ]
      ]]]
    [Defi: [Decl: minimum [Appl: -> [Comm: [Decl: n1 SingleInteger] [Decl: n2 SingleInteger]] SingleInteger]] [Lamb: [Comm: [Decl: n1 SingleInteger] [Decl: n2 SingleInteger]] SingleInteger [Labe: minimum [Sequ: 
      [Exit: [Test: [Appl: < n1 n2]] n2]
      n2
      ]
      ]]]
    [Defi: [Decl: map [Appl: -> [Comm: [Decl: f [Appl: -> [Comm: Integer Integer] Integer]] [Decl: v1 VecInt] [Decl: v2 VecInt]] VecInt]] [Lamb: [Comm: [Decl: f [Appl: -> [Comm: Integer Integer] Integer]] [Decl: v1 VecInt] [Decl: v2 VecInt]] VecInt [Labe: map [Sequ: 
      [Assi: [Decl: n SingleInteger] [Appl: minimum [Appl: # v1] [Appl: # v2]]]
      [Assi: [Decl: vv VecInt] [Appl: new n]]
      [Repe: [Appl: set! vv i [Appl: f [Appl: apply v1 i] [Appl: apply v2 i]]] [For: [Decl: i SingleInteger] [Appl: .. 0 [Appl: - n 1]] _]]
      vv
      ]
      ]]]
    [Defi: [Decl: - [Appl: -> [Decl: v VecInt] VecInt]] [Lamb: [Comm: [Decl: v VecInt]] VecInt [Labe: - [Appl: map - v]]]]
    [Defi: [Decl: + [Appl: -> [Comm: [Decl: v1 VecInt] [Decl: v2 VecInt]] VecInt]] [Lamb: [Comm: [Decl: v1 VecInt] [Decl: v2 VecInt]] VecInt [Labe: + [Appl: map + v1 v2]]]]
    [Defi: [Decl: - [Appl: -> [Comm: [Decl: v1 VecInt] [Decl: v2 VecInt]] VecInt]] [Lamb: [Comm: [Decl: v1 VecInt] [Decl: v2 VecInt]] VecInt [Labe: - [Appl: map - v1 v2]]]]
    [Defi: [Decl: * [Appl: -> [Comm: [Decl: v1 VecInt] [Decl: v2 VecInt]] VecInt]] [Lamb: [Comm: [Decl: v1 VecInt] [Decl: v2 VecInt]] VecInt [Labe: * [Appl: map * v1 v2]]]]
    [Defi: [Decl: * [Appl: -> [Comm: [Decl: n Integer] [Decl: v VecInt]] VecInt]] [Lamb: [Comm: [Decl: n Integer] [Decl: v VecInt]] VecInt [Labe: * [Appl: map [Lamb: [Comm: [Decl: x Integer]] Integer [Appl: * n x]] v]]]]
    [Defi: [Decl: dot [Appl: -> [Comm: [Decl: v1 VecInt] [Decl: v2 VecInt]] Integer]] [Lamb: [Comm: [Decl: v1 VecInt] [Decl: v2 VecInt]] Integer [Labe: dot [Sequ: 
      [Assi: [Decl: n SingleInteger] [Appl: minimum [Appl: # v1] [Appl: # v2]]]
      [Assi: [Decl: vv VecInt] [Appl: new n]]
      [Assi: [Decl: sum Integer] 0]
      [Repe: [Assi: sum [Appl: + sum [Appl: * [Appl: apply v1 i] [Appl: apply v2 i]]]] [For: [Decl: i SingleInteger] [Appl: .. 0 [Appl: - n 1]] _]]
      sum
      ]
      ]]]
    ]
    ]]
  ]
  

