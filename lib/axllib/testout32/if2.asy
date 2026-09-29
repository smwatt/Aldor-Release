((|Declare|
    p
    (|Apply|
      ->
      (|Declare|
        r
        |Ring|
        ((|symeNameCode| . 200123)
          (|symeTypeCode| . 118610639)
          (|domExports|
            (|Declare|
              \0
              %
              ((|documentation| . " Identity for `+'
")
                (|symeNameCode| . 200089)
                (|symeTypeCode| . 1015195433)))
            (|Declare|
              +
              (|Apply| -> % %)
              ((|documentation| . " Identity.
")
                (|default| . 1)
                (|symeNameCode| . 200084)
                (|symeTypeCode| . 143998972)))
            (|Declare|
              +
              (|Apply| -> (|Comma| % %) %)
              ((|documentation| . " Addition.
")
                (|symeNameCode| . 200084)
                (|symeTypeCode| . 355597834)))
            (|Declare|
              -
              (|Apply| -> % %)
              ((|documentation| . " Negation.
")
                (|symeNameCode| . 200086)
                (|symeTypeCode| . 143998972)))
            (|Declare|
              -
              (|Apply| -> (|Comma| % %) %)
              ((|documentation| . " Subtraction.
")
                (|default| . 1)
                (|symeNameCode| . 200086)
                (|symeTypeCode| . 355597834)))
            (|Declare|
              \1
              %
              ((|documentation| . " Identity for `*'
")
                (|symeNameCode| . 200090)
                (|symeTypeCode| . 1015195433)))
            (|Declare|
              *
              (|Apply| -> (|Comma| % %) %)
              ((|documentation| . " Multiplication
")
                (|symeNameCode| . 200083)
                (|symeTypeCode| . 355597834)))
            (|Declare|
              ^
              (|Apply| -> (|Comma| % |Integer|) %)
              ((|documentation| . " Exponentiation or error
")
                (|symeNameCode| . 200135)
                (|symeTypeCode| . 455750953)))
            (|Declare|
              |coerce|
              (|Apply| -> |Integer| %)
              ((|symeNameCode| . 770345191) (|symeTypeCode| . 363839259)))
            (|Declare|
              |coerce|
              (|Apply| -> |SingleInteger| %)
              ((|symeNameCode| . 770345191) (|symeTypeCode| . 352135667)))
            (|Declare|
              %%
              |ArithmeticSystem|
              ((|default| . 1)
                (|symeNameCode| . 51482908)
                (|symeTypeCode| . 297185210)))
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
                (|default| . 1)
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
              %%
              |Monoid|
              ((|default| . 1)
                (|symeNameCode| . 51482908)
                (|symeTypeCode| . 656786807)))
            (|Declare|
              |zero?|
              (|Apply| -> % |Boolean|)
              ((|documentation| . " Test whether value equals 0.
")
                (|default| . 1)
                (|symeNameCode| . 206475020)
                (|symeTypeCode| . 884043277)))
            (|Declare|
              %%
              |AbelianMonoid|
              ((|default| . 1)
                (|symeNameCode| . 51482908)
                (|symeTypeCode| . 744293378)))
            (|Declare|
              %%
              |AbelianGroup|
              ((|default| . 1)
                (|symeNameCode| . 51482908)
                (|symeTypeCode| . 269400384)))
            (|Declare|
              %%
              |Ring|
              ((|default| . 1)
                (|symeNameCode| . 51482908)
                (|symeTypeCode| . 118610639))))))
      (|Define|
        (|Declare|
          ()
          (|With|
            (|Apply| |Join| |Ring| (|If| (|Test| (|Has| r |Field|)) |Field| ()))
            (|Declare|
              |x|
              %
              ((|symeNameCode| . 200161) (|symeTypeCode| . 1015195433)))))
        (|PretendTo|
          (|Add| () ())
          (|With|
            (|Apply| |Join| |Ring| (|If| (|Test| (|Has| r |Field|)) |Field| ()))
            (|Declare|
              |x|
              %
              ((|symeNameCode| . 200161) (|symeTypeCode| . 1015195433)))))))
    ((|symeNameCode| . 200121)
      (|symeTypeCode| . 445937631)
      (|domExports|
        (|Declare|
          \0
          %
          ((|documentation| . " Identity for `+'
")
            (|symeNameCode| . 200089)
            (|symeTypeCode| . 1015195433)))
        (|Declare|
          +
          (|Apply| -> % %)
          ((|documentation| . " Identity.
")
            (|default| . 1)
            (|symeNameCode| . 200084)
            (|symeTypeCode| . 143998972)))
        (|Declare|
          +
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| . " Addition.
")
            (|symeNameCode| . 200084)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          -
          (|Apply| -> % %)
          ((|documentation| . " Negation.
")
            (|symeNameCode| . 200086)
            (|symeTypeCode| . 143998972)))
        (|Declare|
          -
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| . " Subtraction.
")
            (|default| . 1)
            (|symeNameCode| . 200086)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          \1
          %
          ((|documentation| . " Identity for `*'
")
            (|symeNameCode| . 200090)
            (|symeTypeCode| . 1015195433)))
        (|Declare|
          *
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| . " Multiplication
")
            (|symeNameCode| . 200083)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          ^
          (|Apply| -> (|Comma| % |Integer|) %)
          ((|documentation| . " Exponentiation or error
")
            (|symeNameCode| . 200135)
            (|symeTypeCode| . 455750953)))
        (|Declare|
          |coerce|
          (|Apply| -> |Integer| %)
          ((|symeNameCode| . 770345191) (|symeTypeCode| . 363839259)))
        (|Declare|
          |coerce|
          (|Apply| -> |SingleInteger| %)
          ((|symeNameCode| . 770345191) (|symeTypeCode| . 352135667)))
        (|Declare|
          %%
          |ArithmeticSystem|
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 297185210)))
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
            (|default| . 1)
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
          %%
          |Monoid|
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 656786807)))
        (|Declare|
          |zero?|
          (|Apply| -> % |Boolean|)
          ((|documentation| . " Test whether value equals 0.
")
            (|default| . 1)
            (|symeNameCode| . 206475020)
            (|symeTypeCode| . 884043277)))
        (|Declare|
          %%
          |AbelianMonoid|
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 744293378)))
        (|Declare|
          %%
          |AbelianGroup|
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 269400384)))
        (|Declare|
          %%
          |Ring|
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 118610639)))
        (|Declare|
          |gcd|
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| . " Greatest commond divisor.
")
            (|default| . 1)
            (|symeNameCode| . 318604649)
            (|symeTypeCode| . 355597834)
            (|condition| |And| (|Has| r |Field|))))
        (|Declare|
          |quo|
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| . " Quotient leaving remainder.
")
            (|default| . 1)
            (|symeNameCode| . 318204816)
            (|symeTypeCode| . 355597834)
            (|condition| |And| (|Has| r |Field|))))
        (|Declare|
          |rem|
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| . " Remainder.
")
            (|default| . 1)
            (|symeNameCode| . 318266239)
            (|symeTypeCode| . 355597834)
            (|condition| |And| (|Has| r |Field|))))
        (|Declare|
          |divide|
          (|Apply| -> (|Comma| % %) (|Comma| % %))
          ((|documentation| . " Quotient-remainder pair.
")
            (|default| . 1)
            (|symeNameCode| . 42114539)
            (|symeTypeCode| . 624266776)
            (|condition| |And| (|Has| r |Field|))))
        (|Declare|
          %%
          |EuclideanDomain|
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 78774180)
            (|condition| |And| (|Has| r |Field|))))
        (|Declare|
          /
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| . " Division: a/b = a*inv(b).
")
            (|default| . 1)
            (|symeNameCode| . 200088)
            (|symeTypeCode| . 355597834)
            (|condition| |And| (|Has| r |Field|))))
        (|Declare|
          |\\|
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| . " Division: a\\b = inv(a)*b
")
            (|default| . 1)
            (|symeNameCode| . 200133)
            (|symeTypeCode| . 355597834)
            (|condition| |And| (|Has| r |Field|))))
        (|Declare|
          |inv|
          (|Apply| -> % %)
          ((|documentation| . " Inverse.
")
            (|symeNameCode| . 318722696)
            (|symeTypeCode| . 143998972)
            (|condition| |And| (|Has| r |Field|))))
        (|Declare|
          %%
          |Group|
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 372253109)
            (|condition| |And| (|Has| r |Field|))))
        (|Declare|
          |unit?|
          (|Apply|
            ->
            (|Declare|
              |x|
              %
              ((|symeNameCode| . 200161) (|symeTypeCode| . 1015195433)))
            |Boolean|)
          ((|default| . 1)
            (|symeNameCode| . 896181516)
            (|symeTypeCode| . 884043277)
            (|condition| |And| (|Has| r |Field|))))
        (|Declare|
          %%
          |Field|
          ((|default| . 1)
            (|symeNameCode| . 51482908)
            (|symeTypeCode| . 627245452)
            (|condition| |And| (|Has| r |Field|))))
        (|Declare|
          |x|
          %
          ((|symeNameCode| . 200161) (|symeTypeCode| . 1015195433)))))))
(|Sequence|)
