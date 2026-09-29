*** Starting "macex" phase...
*** Result of macex:
[Sequ: 
  _
  _
  [Expo: [Decl: ComplexDoubleFloat [With: _ [Sequ: 
    _
    [Decl: complex [Appl: -> [Comm: DoubleFloat DoubleFloat] ComplexDoubleFloat]]
    [Decl: real [Appl: -> ComplexDoubleFloat DoubleFloat]]
    [Decl: imag [Appl: -> ComplexDoubleFloat DoubleFloat]]
    [Decl: 0 ComplexDoubleFloat]
    [Decl: 1 ComplexDoubleFloat]
    [Decl: + [Appl: -> [Comm: ComplexDoubleFloat ComplexDoubleFloat] ComplexDoubleFloat]]
    [Decl: - [Appl: -> [Comm: ComplexDoubleFloat ComplexDoubleFloat] ComplexDoubleFloat]]
    [Decl: * [Appl: -> [Comm: ComplexDoubleFloat ComplexDoubleFloat] ComplexDoubleFloat]]
    [Decl: / [Appl: -> [Comm: ComplexDoubleFloat ComplexDoubleFloat] ComplexDoubleFloat]]
    ]
    ]] _ _]
  [Defi: ComplexDoubleFloat [Add: _ [Sequ: 
    [Sequ: 
      _
      _
      ]
      
    [Expo: [Sequ: 
      [Decl: complex [Appl: -> [Comm: DoubleFloat DoubleFloat] ComplexDoubleFloat]]
      [Decl: real [Appl: -> ComplexDoubleFloat DoubleFloat]]
      [Decl: imag [Appl: -> ComplexDoubleFloat DoubleFloat]]
      [Decl: 0 ComplexDoubleFloat]
      [Decl: 1 ComplexDoubleFloat]
      [Decl: + [Appl: -> [Comm: ComplexDoubleFloat ComplexDoubleFloat] ComplexDoubleFloat]]
      [Decl: - [Appl: -> [Comm: ComplexDoubleFloat ComplexDoubleFloat] ComplexDoubleFloat]]
      [Decl: * [Appl: -> [Comm: ComplexDoubleFloat ComplexDoubleFloat] ComplexDoubleFloat]]
      [Decl: / [Appl: -> [Comm: ComplexDoubleFloat ComplexDoubleFloat] ComplexDoubleFloat]]
      ]
       _ _]
    [Sequ: 
      _
      [Loca: [Sequ: 
        [Decl: Record [Appl: -> [Comm: Type Type] Type]]
        [Decl: First Type]
        [Decl: Second Type]
        ]
        ]
      [Loca: [Sequ: 
        [Decl: first First]
        [Decl: second Second]
        ]
        ]
      [Buil: [Sequ: 
        [Decl: RecNew [Appl: -> [Comm: DoubleFloat DoubleFloat] [Appl: Record DoubleFloat DoubleFloat]]]
        [Decl: RecElt [Appl: -> [Comm: [Appl: Record DoubleFloat DoubleFloat] First] DoubleFloat]]
        [Decl: RecElt [Appl: -> [Comm: [Appl: Record DoubleFloat DoubleFloat] Second] DoubleFloat]]
        ]
        ]
      ]
      
    [Defi: [Decl: complex [Appl: -> [Comm: [Decl: r DoubleFloat] [Decl: i DoubleFloat]] ComplexDoubleFloat]] [Lamb: [Comm: [Decl: r DoubleFloat] [Decl: i DoubleFloat]] ComplexDoubleFloat [Labe: complex [Pret: [Appl: RecNew r i] ComplexDoubleFloat]]]]
    [Defi: [Decl: real [Appl: -> [Decl: a ComplexDoubleFloat] DoubleFloat]] [Lamb: [Comm: [Decl: a ComplexDoubleFloat]] DoubleFloat [Labe: real [Appl: RecElt [Pret: a [Appl: Record DoubleFloat DoubleFloat]] first]]]]
    [Defi: [Decl: imag [Appl: -> [Decl: a ComplexDoubleFloat] DoubleFloat]] [Lamb: [Comm: [Decl: a ComplexDoubleFloat]] DoubleFloat [Labe: imag [Appl: RecElt [Pret: a [Appl: Record DoubleFloat DoubleFloat]] second]]]]
    [Defi: [Decl: 0 ComplexDoubleFloat] [Appl: complex 0 0]]
    [Defi: [Decl: 1 ComplexDoubleFloat] [Appl: complex 1 0]]
    [Defi: [Decl: + [Appl: -> [Comm: [Decl: a ComplexDoubleFloat] [Decl: b ComplexDoubleFloat]] ComplexDoubleFloat]] [Lamb: [Comm: [Decl: a ComplexDoubleFloat] [Decl: b ComplexDoubleFloat]] ComplexDoubleFloat [Labe: + [Appl: complex [Appl: + [Appl: real a] [Appl: real b]] [Appl: + [Appl: imag a] [Appl: imag b]]]]]]
    [Defi: [Decl: - [Appl: -> [Comm: [Decl: a ComplexDoubleFloat] [Decl: b ComplexDoubleFloat]] ComplexDoubleFloat]] [Lamb: [Comm: [Decl: a ComplexDoubleFloat] [Decl: b ComplexDoubleFloat]] ComplexDoubleFloat [Labe: - [Appl: complex [Appl: - [Appl: real a] [Appl: real b]] [Appl: - [Appl: imag a] [Appl: imag b]]]]]]
    [Defi: [Decl: * [Appl: -> [Comm: [Decl: a ComplexDoubleFloat] [Decl: b ComplexDoubleFloat]] ComplexDoubleFloat]] [Lamb: [Comm: [Decl: a ComplexDoubleFloat] [Decl: b ComplexDoubleFloat]] ComplexDoubleFloat [Labe: * [Appl: complex [Appl: - [Appl: * [Appl: real a] [Appl: real b]] [Appl: * [Appl: imag a] [Appl: imag b]]] [Appl: + [Appl: * [Appl: real a] [Appl: imag b]] [Appl: * [Appl: imag a] [Appl: real b]]]]]]]
    [Defi: [Decl: / [Appl: -> [Comm: [Decl: a ComplexDoubleFloat] [Decl: b ComplexDoubleFloat]] ComplexDoubleFloat]] [Lamb: [Comm: [Decl: a ComplexDoubleFloat] [Decl: b ComplexDoubleFloat]] ComplexDoubleFloat [Labe: / [Sequ: 
      [Defi: [Decl: d DoubleFloat] [Appl: + [Appl: * [Appl: real b] [Appl: real b]] [Appl: * [Appl: imag b] [Appl: imag b]]]]
      [Appl: complex [Appl: / [Appl: + [Appl: * [Appl: real a] [Appl: real b]] [Appl: * [Appl: imag a] [Appl: imag b]]] d] [Appl: / [Appl: - [Appl: * [Appl: imag a] [Appl: real b]] [Appl: * [Appl: real a] [Appl: imag b]]] d]]
      ]
      ]]]
    ]
    ]]
  ComplexDoubleFloat
  F
  [Appl: R ComplexDoubleFloat]
  ]
  

