*** Starting "parse" phase...
*** Result of parse:
[Sequ: 
  [Macr: [Sequ: 
    [Defi: [Appl: safePretend [Pare: [Comm: a T1 T2]]] [Pret: [Rest: a T1] T2]]
    [Defi: [Appl: rep x] [Pret: [Rest: x %] Rep]]
    [Defi: [Appl: per r] [Pret: [Rest: r Rep] %]]
    ]
    ]
  [Macr: [Sequ: 
    [Defi: BBool [Qual: Bool Machine]]
    [Defi: BChar [Qual: Char Machine]]
    [Defi: BArr [Qual: Arr Machine]]
    [Defi: BPtr [Qual: Ptr Machine]]
    [Defi: BByte [Qual: XByte Machine]]
    [Defi: BHInt [Qual: HInt Machine]]
    [Defi: BSInt [Qual: SInt Machine]]
    [Defi: BBInt [Qual: BInt Machine]]
    [Defi: BSFlo [Qual: SFlo Machine]]
    [Defi: BDFlo [Qual: DFlo Machine]]
    ]
    ]
  [Sequ: 
    [Impo: _ AxlLib]
    [Inli: _ AxlLib]
    ]
    
  [Impo: _ Boolean]
  [Impo: [Sequ: 
    [Decl: string [Appl: -> Literal %]]
    [Decl: << [Appl: -> [Pare: [Comm: TextWriter %]] TextWriter]]
    [Decl: << [Appl: -> % [Appl: -> TextWriter TextWriter]]]
    ]
     String]
  [Impo: [Sequ: 
    [Decl: newline %]
    [Decl: << [Appl: -> [Pare: [Comm: TextWriter %]] TextWriter]]
    [Decl: << [Appl: -> % [Appl: -> TextWriter TextWriter]]]
    ]
     Character]
  [Impo: [Sequ: 
    [Decl: print %]
    [Decl: error %]
    ]
     TextWriter]
  [Impo: _ FormattedOutput]
  [Defi: [Decl: [Appl: ListCat [Pare: [Decl: S Type]]] Category] [With: [Appl: Join [Pare: [Comm: BasicType [Appl: Aggregate S] Conditional]]] [Sequ: 
    [Docu: [Docu: [Decl: nil %] ++ that is an empty list
] ++ nil is a literal constant
]
    [Docu: [Docu: [Decl: cons [Appl: -> [Pare: [Comm: S %]] %]] ++ s to the front of the list k.
] ++ cons(s,k) appends
]
    [Docu: [Decl: list [Appl: -> [Appl: Tuple S] %]] ++ list(t) generates a list from a tuple.
]
    [Docu: [Decl: list [Appl: -> [Appl: Generator S] %]] ++ list(i) generates a list from a generator.
]
    [Docu: [Decl: first [Appl: -> % S]] ++ first k returns the first element of the list k.
]
    [Docu: [Decl: rest [Appl: -> % %]] ++ rest k returns the list consisting of all elements
	    of k after the first.
]
    [Docu: [Decl: setFirst! [Appl: -> [Pare: [Comm: % S]] S]] ++ setFirst!(k, s) destructively modifies the list k
	so that the first element is s.
]
    [Docu: [Decl: setRest! [Appl: -> [Pare: [Comm: % %]] %]] ++ setRest!(k1, k2) destructively
 modifies the list k1
 so that rest(k1) = k2.
]
    [Docu: [Decl: reverse! [Appl: -> % %]] ++ reverse! k destructively
 reverses the elements of k.
]
    [Docu: [Decl: concat! [Appl: -> [Pare: [Comm: % %]] %]] ++ concat!(k1, k2) destructively (to k1) appends k1 to the
	    front of k2.
]
    [Docu: [Decl: concat [Appl: -> [Pare: [Comm: % %]] %]] ++ concat(k1, k2) returns a new list that contains the
	    elements of k1 appended to the front of k2.
]
    [Docu: [Docu: [Decl: reduce [Appl: -> [Pare: [Comm: [Appl: -> [Pare: [Comm: S S]] S] % S]] S]] ++ r2 = (fun first rest k, r1)
 and so, returning the
 final value computed.
] ++ reduce(fun, k, s) computes r1 = fun(first k, s), then
]
    [Docu: [Decl: member? [Appl: -> [Pare: [Comm: S %]] Boolean]] ++ member?(s, k) returns
 true if s is contained in k,
 false otherwise.
]
    [Docu: [Decl: apply [Appl: -> [Pare: [Comm: % SingleInteger]] S]] ++ apply(k, i) returns the i-th element of k
]
    ]
    ]]
  [Docu: [Defi: [Decl: [Appl: List [Pare: [Decl: S BasicType]]] [Appl: ListCat S]] [Pret: [Add: _ [Sequ: 
    [Macr: [Defi: Rep0 P]]
    [Macr: [Defi: Rep P]]
    [Macr: [Defi: R [Appl: Record [Pare: [Comm: [Decl: first S] [Decl: rest Rep0]]]]]]
    ]
    ] [Appl: ListCat S]]] ++ List(S) provides an implementation of linked lists.
]
  [Docu: [Defi: [Decl: TestSet Category] [With: _ [Docu: [Decl: = [Appl: -> [Pare: [Comm: % %]] Boolean]] ++ a = b tests for equality of a and b.
]]] ++ TestSet is a very basic category exporting only one operation.
]
  [Docu: [Defi: [Decl: TestMonoid Category] [With: _ [Sequ: 
    [Docu: [Decl: 1 %] ++ 1 is an exported constant.
]
    [Docu: [Decl: * [Appl: -> [Pare: [Comm: % %]] %]] ++ a * b is the monoid operation.
]
    ]
    ]] ++ TestMonoid is slightly more complicated.
]
  [Docu: [Defi: [Decl: [Appl: TestOperator [Pare: [Decl: T Type]]] Category] [With: _ [Docu: [Decl: * [Appl: -> [Pare: [Comm: T %]] %]] ++ t * o is the operation that we want to see.
]]] ++ TestOperator is now reporting its operations.
]
  ]
  

