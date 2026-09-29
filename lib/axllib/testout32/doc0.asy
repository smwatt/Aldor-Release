((|Declare|
    |TestOperator|
    (|Apply|
      ->
      (|Declare|
        t
        |Type|
        ((|symeNameCode| . 200125)
          (|symeTypeCode| . 547582661)
          (|domExports|)))
      (|Define|
        (|Declare| (|Label| |TestOperator| ()) |Category|)
        (|With|
          ()
          (|Declare|
            *
            (|Apply| -> (|Comma| t %) %)
            ((|documentation| . " t * o is the operation that we want to see.
")
              (|symeNameCode| . 200083)
              (|symeTypeCode| . 343832121))))))
    ((|documentation| . " TestOperator is now reporting its operations.
")
      (|symeNameCode| . 134077144)
      (|symeTypeCode| . 925443798)
      (|catExports|
        (|Declare|
          *
          (|Apply| -> (|Comma| t %) %)
          ((|documentation| . " t * o is the operation that we want to see.
")
            (|symeNameCode| . 200083)
            (|symeTypeCode| . 343832121)))
        (|Declare|
          %%
          (|Apply| |TestOperator| t)
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 703614868))))))
  (|Declare|
    |TestMonoid|
    (|Define|
      (|Declare| (|Label| |TestMonoid| ()) |Category|)
      (|With|
        ()
        (|Sequence|
          (|Declare|
            \1
            %
            ((|documentation| . " 1 is an exported constant.
")
              (|symeNameCode| . 200090)
              (|symeTypeCode| . 1015195433)))
          (|Declare|
            *
            (|Apply| -> (|Comma| % %) %)
            ((|documentation| . " a * b is the monoid operation.
")
              (|symeNameCode| . 200083)
              (|symeTypeCode| . 355597834))))))
    ((|documentation| . " TestMonoid is slightly more complicated.
")
      (|symeNameCode| . 934432)
      (|symeTypeCode| . 30206363)
      (|catExports|
        (|Declare|
          \1
          %
          ((|documentation| . " 1 is an exported constant.
")
            (|symeNameCode| . 200090)
            (|symeTypeCode| . 1015195433)))
        (|Declare|
          *
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| . " a * b is the monoid operation.
")
            (|symeNameCode| . 200083)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          %%
          |TestMonoid|
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 1015929787))))))
  (|Declare|
    |TestSet|
    (|Define|
      (|Declare| (|Label| |TestSet| ()) |Category|)
      (|With|
        ()
        (|Declare|
          =
          (|Apply| -> (|Comma| % %) |Boolean|)
          ((|documentation| . " a = b tests for equality of a and b.
")
            (|symeNameCode| . 200102)
            (|symeTypeCode| . 21900315)))))
    ((|documentation| .
        " TestSet is a very basic category exporting only one operation.
")
      (|symeNameCode| . 230976939)
      (|symeTypeCode| . 628769541)
      (|catExports|
        (|Declare|
          =
          (|Apply| -> (|Comma| % %) |Boolean|)
          ((|documentation| . " a = b tests for equality of a and b.
")
            (|symeNameCode| . 200102)
            (|symeTypeCode| . 21900315)))
        (|Declare|
          %%
          |TestSet|
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 172230470))))))
  (|Declare|
    |List|
    (|Apply|
      ->
      (|Declare|
        s
        |BasicType|
        ((|symeNameCode| . 200124)
          (|symeTypeCode| . 866929616)
          (|domExports|
            (|Declare|
              |PackedArrayNew|
              (|Apply| -> (|Qualify| |SInt| |Machine|) (|Qualify| |Arr| |Machine|))
              ((|documentation| .
                  " Create a machine array with enough storage
 for the specified number of elements. There
 is no support for providing initial values.
")
                (|symeNameCode| . 757252399)
                (|symeTypeCode| . 230929250)))
            (|Declare|
              |PackedArrayGet|
              (|Apply|
                ->
                (|Comma| (|Qualify| |Arr| |Machine|) (|Qualify| |SInt| |Machine|))
                %)
              ((|documentation| .
                  " Given a machine array, return the element
 at the specified index. Note that zero-based
 indexing is used.
")
                (|symeNameCode| . 767785253)
                (|symeTypeCode| . 504190320)))
            (|Declare|
              |PackedArraySet|
              (|Apply|
                ->
                (|Comma| (|Qualify| |Arr| |Machine|) (|Qualify| |SInt| |Machine|) %)
                %)
              ((|documentation| .
                  " Given a machine array replace the value at
 the specified index with a new value. Note
 that zero-based indexing is used.
")
                (|symeNameCode| . 756527409)
                (|symeTypeCode| . 7666178)))
            (|Declare|
              |PackedRecordSet|
              (|Apply| -> (|Comma| (|Qualify| |Ptr| |Machine|) %) %)
              ((|documentation| .
                  " `PackedRecordSet(p, v)' writes the raw representation
 of `v' into the machine address `p'. Use `pretend' to
 view `p' as an Aldor record or array as appropriate.
")
                (|symeNameCode| . 940586746)
                (|symeTypeCode| . 278278893)))
            (|Declare|
              |PackedRecordGet|
              (|Apply| -> (|Qualify| |Ptr| |Machine|) %)
              ((|documentation| .
                  " `PackedRecordGet(p)' reads the raw representation of
 a value from the machine address `p'. Use `pretend'
 to view `p' as an Aldor record or array as appropriate.
")
                (|symeNameCode| . 939800302)
                (|symeTypeCode| . 632591839)))
            (|Declare|
              |PackedRepSize|
              (|Apply| -> (|Comma|) (|Qualify| |SInt| |Machine|))
              ((|documentation| .
                  " This function returns the amount of memory required
 to store a raw value in a raw record.
")
                (|symeNameCode| . 920971615)
                (|symeTypeCode| . 932039034)))
            (|Declare|
              %%
              |DenseStorageCategory|
              ((|default| . 1)
                (|symeNameCode| . 51482908)
                (|symeTypeCode| . 958849489)))
            (|Declare|
              =
              (|Apply| -> (|Comma| % %) |Boolean|)
              ((|documentation| . " Equality test.
")
                (|symeNameCode| . 200102)
                (|symeTypeCode| . 21900315)))
            (|Declare|
              ~=
              (|Apply| -> (|Comma| % %) |Boolean|)
              ((|documentation| . " Inequality test.
")
                (|default| . 1)
                (|symeNameCode| . 51509389)
                (|symeTypeCode| . 21900315)))
            (|Declare|
              <<
              (|Apply| -> (|Comma| |TextWriter| %) |TextWriter|)
              ((|documentation| . " Basic output.
")
                (|symeNameCode| . 51492426)
                (|symeTypeCode| . 561215334)))
            (|Declare|
              <<
              (|Apply| -> % (|Apply| -> |TextWriter| |TextWriter|))
              ((|documentation| . " Basic output.
")
                (|default| . 1)
                (|symeNameCode| . 51492426)
                (|symeTypeCode| . 112505899)))
            (|Declare|
              |sample|
              %
              ((|documentation| . " Example element.
")
                (|symeNameCode| . 255806968)
                (|symeTypeCode| . 1015195433)))
            (|Declare|
              |hash|
              (|Apply| -> % |SingleInteger|)
              ((|documentation| . " Hashing function.
")
                (|default| . 1)
                (|symeNameCode| . 746853960)
                (|symeTypeCode| . 286270707)))
            (|Declare|
              |case|
              (|Apply| -> (|Comma| % %) |Boolean|)
              ((|documentation| . " for 'select' statements;
")
                (|default| . 1)
                (|symeNameCode| . 864625472)
                (|symeTypeCode| . 21900315)))
            (|Declare|
              |coerce|
              (|Apply| -> % %)
              ((|documentation| . " Why not?
")
                (|default| . 1)
                (|symeNameCode| . 770345191)
                (|symeTypeCode| . 143998972)))
            (|Declare|
              %%
              |BasicType|
              ((|default| . 1)
                (|symeNameCode| . 51482908)
                (|symeTypeCode| . 866929616))))))
      (|Apply| |ListCat| s))
    ((|documentation| .
        " List(S) provides an implementation of linked lists.
")
      (|symeNameCode| . 144194112)
      (|symeTypeCode| . 753564397)
      (|domExports|
        (|Declare|
          |PackedArrayNew|
          (|Apply| -> (|Qualify| |SInt| |Machine|) (|Qualify| |Arr| |Machine|))
          ((|documentation| .
              " Create a machine array with enough storage
 for the specified number of elements. There
 is no support for providing initial values.
")
            (|symeNameCode| . 757252399)
            (|symeTypeCode| . 230929250)))
        (|Declare|
          |PackedArrayGet|
          (|Apply|
            ->
            (|Comma| (|Qualify| |Arr| |Machine|) (|Qualify| |SInt| |Machine|))
            %)
          ((|documentation| .
              " Given a machine array, return the element
 at the specified index. Note that zero-based
 indexing is used.
")
            (|symeNameCode| . 767785253)
            (|symeTypeCode| . 504190320)))
        (|Declare|
          |PackedArraySet|
          (|Apply|
            ->
            (|Comma| (|Qualify| |Arr| |Machine|) (|Qualify| |SInt| |Machine|) %)
            %)
          ((|documentation| .
              " Given a machine array replace the value at
 the specified index with a new value. Note
 that zero-based indexing is used.
")
            (|symeNameCode| . 756527409)
            (|symeTypeCode| . 7666178)))
        (|Declare|
          |PackedRecordSet|
          (|Apply| -> (|Comma| (|Qualify| |Ptr| |Machine|) %) %)
          ((|documentation| .
              " `PackedRecordSet(p, v)' writes the raw representation
 of `v' into the machine address `p'. Use `pretend' to
 view `p' as an Aldor record or array as appropriate.
")
            (|symeNameCode| . 940586746)
            (|symeTypeCode| . 278278893)))
        (|Declare|
          |PackedRecordGet|
          (|Apply| -> (|Qualify| |Ptr| |Machine|) %)
          ((|documentation| .
              " `PackedRecordGet(p)' reads the raw representation of
 a value from the machine address `p'. Use `pretend'
 to view `p' as an Aldor record or array as appropriate.
")
            (|symeNameCode| . 939800302)
            (|symeTypeCode| . 632591839)))
        (|Declare|
          |PackedRepSize|
          (|Apply| -> (|Comma|) (|Qualify| |SInt| |Machine|))
          ((|documentation| .
              " This function returns the amount of memory required
 to store a raw value in a raw record.
")
            (|symeNameCode| . 920971615)
            (|symeTypeCode| . 932039034)))
        (|Declare|
          %%
          |DenseStorageCategory|
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 958849489)))
        (|Declare|
          =
          (|Apply| -> (|Comma| % %) |Boolean|)
          ((|documentation| . " Equality test.
")
            (|symeNameCode| . 200102)
            (|symeTypeCode| . 21900315)))
        (|Declare|
          ~=
          (|Apply| -> (|Comma| % %) |Boolean|)
          ((|documentation| . " Inequality test.
")
            (|default| . 1)
            (|symeNameCode| . 51509389)
            (|symeTypeCode| . 21900315)))
        (|Declare|
          <<
          (|Apply| -> (|Comma| |TextWriter| %) |TextWriter|)
          ((|documentation| . " Basic output.
")
            (|symeNameCode| . 51492426)
            (|symeTypeCode| . 561215334)))
        (|Declare|
          <<
          (|Apply| -> % (|Apply| -> |TextWriter| |TextWriter|))
          ((|documentation| . " Basic output.
")
            (|default| . 1)
            (|symeNameCode| . 51492426)
            (|symeTypeCode| . 112505899)))
        (|Declare|
          |sample|
          %
          ((|documentation| . " Example element.
")
            (|symeNameCode| . 255806968)
            (|symeTypeCode| . 1015195433)))
        (|Declare|
          |hash|
          (|Apply| -> % |SingleInteger|)
          ((|documentation| . " Hashing function.
")
            (|default| . 1)
            (|symeNameCode| . 746853960)
            (|symeTypeCode| . 286270707)))
        (|Declare|
          |case|
          (|Apply| -> (|Comma| % %) |Boolean|)
          ((|documentation| . " for 'select' statements;
")
            (|default| . 1)
            (|symeNameCode| . 864625472)
            (|symeTypeCode| . 21900315)))
        (|Declare|
          |coerce|
          (|Apply| -> % %)
          ((|documentation| . " Why not?
")
            (|default| . 1)
            (|symeNameCode| . 770345191)
            (|symeTypeCode| . 143998972)))
        (|Declare|
          %%
          |BasicType|
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 866929616)))
        (|Declare|
          |empty?|
          (|Apply| -> % |Boolean|)
          ((|documentation| . " Is the element count zero?
")
            (|symeNameCode| . 506619364)
            (|symeTypeCode| . 884043277)))
        (|Declare|
          |generator|
          (|Apply| -> % (|Apply| |Generator| s))
          ((|documentation| . " Generic traversal of an aggregate.
")
            (|symeNameCode| . 113320568)
            (|symeTypeCode| . 228375914)))
        (|Declare|
          |map|
          (|Apply| -> (|Comma| (|Apply| -> s s) %) %)
          ((|documentation| . " Form new aggregate using function.
")
            (|symeNameCode| . 318461817)
            (|symeTypeCode| . 126341433)))
        (|Declare|
          %%
          (|Apply| |Aggregate| s)
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 71079667)))
        (|Declare|
          |test|
          (|Apply| -> % |Boolean|)
          ((|documentation| . " Test used in conditional context.
")
            (|symeNameCode| . 5509732)
            (|symeTypeCode| . 884043277)))
        (|Declare|
          %%
          |Conditional|
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 498431890)))
        (|Declare|
          |nil|
          %
          ((|documentation| .
              " nil is a literal constant
 that is an empty list
")
            (|symeNameCode| . 318525310)
            (|symeTypeCode| . 1015195433)))
        (|Declare|
          |cons|
          (|Apply| -> (|Comma| s %) %)
          ((|documentation| .
              " cons(s,k) appends
 s to the front of the list k.
")
            (|symeNameCode| . 865800279)
            (|symeTypeCode| . 343897656)))
        (|Declare|
          |list|
          (|Apply| -> (|Apply| |Tuple| s) %)
          ((|documentation| . " list(t) generates a list from a tuple.
")
            (|symeNameCode| . 683203168)
            (|symeTypeCode| . 235561001)))
        (|Declare|
          |list|
          (|Apply| -> (|Apply| |Generator| s) %)
          ((|documentation| . " list(i) generates a list from a generator.
")
            (|symeNameCode| . 683203168)
            (|symeTypeCode| . 472075114)))
        (|Declare|
          |first|
          (|Apply| -> % s)
          ((|documentation| .
              " first k returns the first element of the list k.
")
            (|symeNameCode| . 682776373)
            (|symeTypeCode| . 143999018)))
        (|Declare|
          |rest|
          (|Apply| -> % %)
          ((|documentation| .
              " rest k returns the list consisting of all elements
	    of k after the first.
")
            (|symeNameCode| . 715582562)
            (|symeTypeCode| . 143998972)))
        (|Declare|
          |setFirst!|
          (|Apply| -> (|Comma| % s) s)
          ((|documentation| .
              " setFirst!(k, s) destructively modifies the list k
	so that the first element is s.
")
            (|symeNameCode| . 143562022)
            (|symeTypeCode| . 355574886)))
        (|Declare|
          |setRest!|
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| .
              " setRest!(k1, k2) destructively
 modifies the list k1
 so that rest(k1) = k2.
")
            (|symeNameCode| . 76356179)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          |reverse!|
          (|Apply| -> % %)
          ((|documentation| .
              " reverse! k destructively
 reverses the elements of k.
")
            (|symeNameCode| . 25941349)
            (|symeTypeCode| . 143998972)))
        (|Declare|
          |concat!|
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| .
              " concat!(k1, k2) destructively (to k1) appends k1 to the
	    front of k2.
")
            (|symeNameCode| . 141720952)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          |concat|
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| .
              " concat(k1, k2) returns a new list that contains the
	    elements of k1 appended to the front of k2.
")
            (|symeNameCode| . 653165038)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          |reduce|
          (|Apply| -> (|Comma| (|Apply| -> (|Comma| s s) s) % s) s)
          ((|documentation| .
              " reduce(fun, k, s) computes r1 = fun(first k, s), then
 r2 = (fun first rest k, r1)
 and so, returning the
 final value computed.
")
            (|symeNameCode| . 752904942)
            (|symeTypeCode| . 164402531)))
        (|Declare|
          |member?|
          (|Apply| -> (|Comma| s %) |Boolean|)
          ((|documentation| .
              " member?(s, k) returns
 true if s is contained in k,
 false otherwise.
")
            (|symeNameCode| . 1065723030)
            (|symeTypeCode| . 10200137)))
        (|Declare|
          |apply|
          (|Apply| -> (|Comma| % |SingleInteger|) s)
          ((|documentation| . " apply(k, i) returns the i-th element of k
")
            (|symeNameCode| . 306472243)
            (|symeTypeCode| . 546428975)))
        (|Declare|
          %%
          (|Apply| |ListCat| s)
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 940940622))))))
  (|Declare|
    |ListCat|
    (|Apply|
      ->
      (|Declare|
        s
        |Type|
        ((|symeNameCode| . 200124)
          (|symeTypeCode| . 547582661)
          (|domExports|)))
      (|Define|
        (|Declare| (|Label| |ListCat| ()) |Category|)
        (|With|
          (|Apply| |Join| |BasicType| (|Apply| |Aggregate| s) |Conditional|)
          (|Sequence|
            (|Declare|
              |nil|
              %
              ((|documentation| .
                  " nil is a literal constant
 that is an empty list
")
                (|symeNameCode| . 318525310)
                (|symeTypeCode| . 1015195433)))
            (|Declare|
              |cons|
              (|Apply| -> (|Comma| s %) %)
              ((|documentation| .
                  " cons(s,k) appends
 s to the front of the list k.
")
                (|symeNameCode| . 865800279)
                (|symeTypeCode| . 343897656)))
            (|Declare|
              |list|
              (|Apply| -> (|Apply| |Tuple| s) %)
              ((|documentation| . " list(t) generates a list from a tuple.
")
                (|symeNameCode| . 683203168)
                (|symeTypeCode| . 235561001)))
            (|Declare|
              |list|
              (|Apply| -> (|Apply| |Generator| s) %)
              ((|documentation| . " list(i) generates a list from a generator.
")
                (|symeNameCode| . 683203168)
                (|symeTypeCode| . 472075114)))
            (|Declare|
              |first|
              (|Apply| -> % s)
              ((|documentation| .
                  " first k returns the first element of the list k.
")
                (|symeNameCode| . 682776373)
                (|symeTypeCode| . 143999018)))
            (|Declare|
              |rest|
              (|Apply| -> % %)
              ((|documentation| .
                  " rest k returns the list consisting of all elements
	    of k after the first.
")
                (|symeNameCode| . 715582562)
                (|symeTypeCode| . 143998972)))
            (|Declare|
              |setFirst!|
              (|Apply| -> (|Comma| % s) s)
              ((|documentation| .
                  " setFirst!(k, s) destructively modifies the list k
	so that the first element is s.
")
                (|symeNameCode| . 143562022)
                (|symeTypeCode| . 355574886)))
            (|Declare|
              |setRest!|
              (|Apply| -> (|Comma| % %) %)
              ((|documentation| .
                  " setRest!(k1, k2) destructively
 modifies the list k1
 so that rest(k1) = k2.
")
                (|symeNameCode| . 76356179)
                (|symeTypeCode| . 355597834)))
            (|Declare|
              |reverse!|
              (|Apply| -> % %)
              ((|documentation| .
                  " reverse! k destructively
 reverses the elements of k.
")
                (|symeNameCode| . 25941349)
                (|symeTypeCode| . 143998972)))
            (|Declare|
              |concat!|
              (|Apply| -> (|Comma| % %) %)
              ((|documentation| .
                  " concat!(k1, k2) destructively (to k1) appends k1 to the
	    front of k2.
")
                (|symeNameCode| . 141720952)
                (|symeTypeCode| . 355597834)))
            (|Declare|
              |concat|
              (|Apply| -> (|Comma| % %) %)
              ((|documentation| .
                  " concat(k1, k2) returns a new list that contains the
	    elements of k1 appended to the front of k2.
")
                (|symeNameCode| . 653165038)
                (|symeTypeCode| . 355597834)))
            (|Declare|
              |reduce|
              (|Apply| -> (|Comma| (|Apply| -> (|Comma| s s) s) % s) s)
              ((|documentation| .
                  " reduce(fun, k, s) computes r1 = fun(first k, s), then
 r2 = (fun first rest k, r1)
 and so, returning the
 final value computed.
")
                (|symeNameCode| . 752904942)
                (|symeTypeCode| . 164402531)))
            (|Declare|
              |member?|
              (|Apply| -> (|Comma| s %) |Boolean|)
              ((|documentation| .
                  " member?(s, k) returns
 true if s is contained in k,
 false otherwise.
")
                (|symeNameCode| . 1065723030)
                (|symeTypeCode| . 10200137)))
            (|Declare|
              |apply|
              (|Apply| -> (|Comma| % |SingleInteger|) s)
              ((|documentation| . " apply(k, i) returns the i-th element of k
")
                (|symeNameCode| . 306472243)
                (|symeTypeCode| . 546428975)))))))
    ((|symeNameCode| . 274718099)
      (|symeTypeCode| . 793702287)
      (|catExports|
        (|Declare|
          |PackedArrayNew|
          (|Apply| -> (|Qualify| |SInt| |Machine|) (|Qualify| |Arr| |Machine|))
          ((|documentation| .
              " Create a machine array with enough storage
 for the specified number of elements. There
 is no support for providing initial values.
")
            (|symeNameCode| . 757252399)
            (|symeTypeCode| . 230929250)))
        (|Declare|
          |PackedArrayGet|
          (|Apply|
            ->
            (|Comma| (|Qualify| |Arr| |Machine|) (|Qualify| |SInt| |Machine|))
            %)
          ((|documentation| .
              " Given a machine array, return the element
 at the specified index. Note that zero-based
 indexing is used.
")
            (|symeNameCode| . 767785253)
            (|symeTypeCode| . 504190320)))
        (|Declare|
          |PackedArraySet|
          (|Apply|
            ->
            (|Comma| (|Qualify| |Arr| |Machine|) (|Qualify| |SInt| |Machine|) %)
            %)
          ((|documentation| .
              " Given a machine array replace the value at
 the specified index with a new value. Note
 that zero-based indexing is used.
")
            (|symeNameCode| . 756527409)
            (|symeTypeCode| . 7666178)))
        (|Declare|
          |PackedRecordSet|
          (|Apply| -> (|Comma| (|Qualify| |Ptr| |Machine|) %) %)
          ((|documentation| .
              " `PackedRecordSet(p, v)' writes the raw representation
 of `v' into the machine address `p'. Use `pretend' to
 view `p' as an Aldor record or array as appropriate.
")
            (|symeNameCode| . 940586746)
            (|symeTypeCode| . 278278893)))
        (|Declare|
          |PackedRecordGet|
          (|Apply| -> (|Qualify| |Ptr| |Machine|) %)
          ((|documentation| .
              " `PackedRecordGet(p)' reads the raw representation of
 a value from the machine address `p'. Use `pretend'
 to view `p' as an Aldor record or array as appropriate.
")
            (|symeNameCode| . 939800302)
            (|symeTypeCode| . 632591839)))
        (|Declare|
          |PackedRepSize|
          (|Apply| -> (|Comma|) (|Qualify| |SInt| |Machine|))
          ((|documentation| .
              " This function returns the amount of memory required
 to store a raw value in a raw record.
")
            (|symeNameCode| . 920971615)
            (|symeTypeCode| . 932039034)))
        (|Declare|
          %%
          |DenseStorageCategory|
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 958849489)))
        (|Declare|
          =
          (|Apply| -> (|Comma| % %) |Boolean|)
          ((|documentation| . " Equality test.
")
            (|symeNameCode| . 200102)
            (|symeTypeCode| . 21900315)))
        (|Declare|
          ~=
          (|Apply| -> (|Comma| % %) |Boolean|)
          ((|documentation| . " Inequality test.
")
            (|default| . 1)
            (|symeNameCode| . 51509389)
            (|symeTypeCode| . 21900315)))
        (|Declare|
          <<
          (|Apply| -> (|Comma| |TextWriter| %) |TextWriter|)
          ((|documentation| . " Basic output.
")
            (|symeNameCode| . 51492426)
            (|symeTypeCode| . 561215334)))
        (|Declare|
          <<
          (|Apply| -> % (|Apply| -> |TextWriter| |TextWriter|))
          ((|documentation| . " Basic output.
")
            (|default| . 1)
            (|symeNameCode| . 51492426)
            (|symeTypeCode| . 112505899)))
        (|Declare|
          |sample|
          %
          ((|documentation| . " Example element.
")
            (|symeNameCode| . 255806968)
            (|symeTypeCode| . 1015195433)))
        (|Declare|
          |hash|
          (|Apply| -> % |SingleInteger|)
          ((|documentation| . " Hashing function.
")
            (|default| . 1)
            (|symeNameCode| . 746853960)
            (|symeTypeCode| . 286270707)))
        (|Declare|
          |case|
          (|Apply| -> (|Comma| % %) |Boolean|)
          ((|documentation| . " for 'select' statements;
")
            (|default| . 1)
            (|symeNameCode| . 864625472)
            (|symeTypeCode| . 21900315)))
        (|Declare|
          |coerce|
          (|Apply| -> % %)
          ((|documentation| . " Why not?
")
            (|default| . 1)
            (|symeNameCode| . 770345191)
            (|symeTypeCode| . 143998972)))
        (|Declare|
          %%
          |BasicType|
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 866929616)))
        (|Declare|
          |empty?|
          (|Apply| -> % |Boolean|)
          ((|documentation| . " Is the element count zero?
")
            (|symeNameCode| . 506619364)
            (|symeTypeCode| . 884043277)))
        (|Declare|
          |generator|
          (|Apply| -> % (|Apply| |Generator| s))
          ((|documentation| . " Generic traversal of an aggregate.
")
            (|symeNameCode| . 113320568)
            (|symeTypeCode| . 228375914)))
        (|Declare|
          |map|
          (|Apply| -> (|Comma| (|Apply| -> s s) %) %)
          ((|documentation| . " Form new aggregate using function.
")
            (|symeNameCode| . 318461817)
            (|symeTypeCode| . 126341433)))
        (|Declare|
          %%
          (|Apply| |Aggregate| s)
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 71079667)))
        (|Declare|
          |test|
          (|Apply| -> % |Boolean|)
          ((|documentation| . " Test used in conditional context.
")
            (|symeNameCode| . 5509732)
            (|symeTypeCode| . 884043277)))
        (|Declare|
          %%
          |Conditional|
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 498431890)))
        (|Declare|
          |nil|
          %
          ((|documentation| .
              " nil is a literal constant
 that is an empty list
")
            (|symeNameCode| . 318525310)
            (|symeTypeCode| . 1015195433)))
        (|Declare|
          |cons|
          (|Apply| -> (|Comma| s %) %)
          ((|documentation| .
              " cons(s,k) appends
 s to the front of the list k.
")
            (|symeNameCode| . 865800279)
            (|symeTypeCode| . 343897656)))
        (|Declare|
          |list|
          (|Apply| -> (|Apply| |Tuple| s) %)
          ((|documentation| . " list(t) generates a list from a tuple.
")
            (|symeNameCode| . 683203168)
            (|symeTypeCode| . 235561001)))
        (|Declare|
          |list|
          (|Apply| -> (|Apply| |Generator| s) %)
          ((|documentation| . " list(i) generates a list from a generator.
")
            (|symeNameCode| . 683203168)
            (|symeTypeCode| . 472075114)))
        (|Declare|
          |first|
          (|Apply| -> % s)
          ((|documentation| .
              " first k returns the first element of the list k.
")
            (|symeNameCode| . 682776373)
            (|symeTypeCode| . 143999018)))
        (|Declare|
          |rest|
          (|Apply| -> % %)
          ((|documentation| .
              " rest k returns the list consisting of all elements
	    of k after the first.
")
            (|symeNameCode| . 715582562)
            (|symeTypeCode| . 143998972)))
        (|Declare|
          |setFirst!|
          (|Apply| -> (|Comma| % s) s)
          ((|documentation| .
              " setFirst!(k, s) destructively modifies the list k
	so that the first element is s.
")
            (|symeNameCode| . 143562022)
            (|symeTypeCode| . 355574886)))
        (|Declare|
          |setRest!|
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| .
              " setRest!(k1, k2) destructively
 modifies the list k1
 so that rest(k1) = k2.
")
            (|symeNameCode| . 76356179)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          |reverse!|
          (|Apply| -> % %)
          ((|documentation| .
              " reverse! k destructively
 reverses the elements of k.
")
            (|symeNameCode| . 25941349)
            (|symeTypeCode| . 143998972)))
        (|Declare|
          |concat!|
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| .
              " concat!(k1, k2) destructively (to k1) appends k1 to the
	    front of k2.
")
            (|symeNameCode| . 141720952)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          |concat|
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| .
              " concat(k1, k2) returns a new list that contains the
	    elements of k1 appended to the front of k2.
")
            (|symeNameCode| . 653165038)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          |reduce|
          (|Apply| -> (|Comma| (|Apply| -> (|Comma| s s) s) % s) s)
          ((|documentation| .
              " reduce(fun, k, s) computes r1 = fun(first k, s), then
 r2 = (fun first rest k, r1)
 and so, returning the
 final value computed.
")
            (|symeNameCode| . 752904942)
            (|symeTypeCode| . 164402531)))
        (|Declare|
          |member?|
          (|Apply| -> (|Comma| s %) |Boolean|)
          ((|documentation| .
              " member?(s, k) returns
 true if s is contained in k,
 false otherwise.
")
            (|symeNameCode| . 1065723030)
            (|symeTypeCode| . 10200137)))
        (|Declare|
          |apply|
          (|Apply| -> (|Comma| % |SingleInteger|) s)
          ((|documentation| . " apply(k, i) returns the i-th element of k
")
            (|symeNameCode| . 306472243)
            (|symeTypeCode| . 546428975)))
        (|Declare|
          %%
          (|Apply| |ListCat| s)
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 940940622)))))))
(|Sequence|)
