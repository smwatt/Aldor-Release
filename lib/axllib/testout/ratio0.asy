((|Declare|
    |f|
    (|Apply| -> (|Comma|) (|Comma|))
    ((|symeNameCode| . 200143) (|symeTypeCode| . 778822608)))
  (|Declare|
    |Ratio|
    (|Apply|
      ->
      (|Declare|
        i
        |IntegerNumberSystem|
        ((|symeNameCode| . 200114) (|symeHashCode| . 265443874)))
      (|With|
        (|Apply| |Join| |OrderedRing| |Field|)
        (|Sequence|
          (|Declare|
            *
            (|Apply| -> (|Comma| i %) %)
            ((|symeNameCode| . 200083) (|symeHashCode| . 353238574)))
          (|Declare|
            /
            (|Apply| -> (|Comma| i i) %)
            ((|symeNameCode| . 200088) (|symeHashCode| . 353231442)))
          (|Declare|
            |numer|
            (|Apply| -> % i)
            ((|symeNameCode| . 880264500) (|symeHashCode| . 143999008)))
          (|Declare|
            |denom|
            (|Apply| -> % i)
            ((|symeNameCode| . 882795296) (|symeHashCode| . 143999008)))
          (|Declare|
            |coerce|
            (|Apply| -> i %)
            ((|symeNameCode| . 770345191) (|symeHashCode| . 143989792))))))
    ((|symeNameCode| . 279190796)
      (|symeTypeCode| . 334304266)
      (|domExports|
        (|Declare|
          %%
          |OrderedRing|
          ((|symeNameCode| . 51482908) (|symeTypeCode| . 816910963)))
        (|Declare|
          %%
          |OrderedAbelianMonoid|
          ((|symeNameCode| . 51482908) (|symeTypeCode| . 383243686)))
        (|Declare|
          %%
          |Order|
          ((|symeNameCode| . 51482908) (|symeTypeCode| . 105187748)))
        (|Declare|
          >
          (|Apply| -> (|Comma| % %) |Boolean|)
          ((|documentation| . " Greater than test.
")
            (|symeNameCode| . 200103)
            (|symeTypeCode| . 21900315)))
        (|Declare|
          <
          (|Apply| -> (|Comma| % %) |Boolean|)
          ((|documentation| . " Less than test.
")
            (|symeNameCode| . 200101)
            (|symeTypeCode| . 21900315)))
        (|Declare|
          >=
          (|Apply| -> (|Comma| % %) |Boolean|)
          ((|documentation| . " Greater than or equal test.
")
            (|symeNameCode| . 51492941)
            (|symeTypeCode| . 21900315)))
        (|Declare|
          <=
          (|Apply| -> (|Comma| % %) |Boolean|)
          ((|documentation| . " Less than or equal test.
")
            (|symeNameCode| . 51492427)
            (|symeTypeCode| . 21900315)))
        (|Declare|
          |max|
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| . " Maximum argument.
")
            (|symeNameCode| . 318461825)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          |min|
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| . " Minimum argument.
")
            (|symeNameCode| . 318459775)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          |abs|
          (|Apply| -> % %)
          ((|documentation| . " Absolute value.
")
            (|symeNameCode| . 315051633)
            (|symeTypeCode| . 143998972)))
        (|Declare|
          |negative?|
          (|Apply| -> % |Boolean|)
          ((|documentation| . " Test whether value is negative?
")
            (|symeNameCode| . 424301635)
            (|symeTypeCode| . 884043277)))
        (|Declare|
          |positive?|
          (|Apply| -> % |Boolean|)
          ((|documentation| . " Test whether value is positive?
")
            (|symeNameCode| . 733146723)
            (|symeTypeCode| . 884043277)))
        (|Declare|
          |sign|
          (|Apply| -> % %)
          ((|documentation| . " -1, 0, or 1.
")
            (|symeNameCode| . 61935445)
            (|symeTypeCode| . 143998972)))
        (|Declare|
          %%
          |Field|
          ((|symeNameCode| . 51482908) (|symeTypeCode| . 627245452)))
        (|Declare|
          %%
          |GcdDomain|
          ((|symeNameCode| . 51482908) (|symeTypeCode| . 50139826)))
        (|Declare|
          %%
          |Ring|
          ((|symeNameCode| . 51482908) (|symeTypeCode| . 118610639)))
        (|Declare|
          %%
          |Monoid|
          ((|symeNameCode| . 51482908) (|symeTypeCode| . 656786807)))
        (|Declare|
          \1
          %
          ((|documentation| . " Identity for multiplication.
")
            (|symeNameCode| . 200090)
            (|symeTypeCode| . 1015195433)))
        (|Declare|
          *
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| . " Multiplication.
")
            (|symeNameCode| . 200083)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          %%
          |AbelianGroup|
          ((|symeNameCode| . 51482908) (|symeTypeCode| . 269400384)))
        (|Declare|
          %%
          |AbelianMonoid|
          ((|symeNameCode| . 51482908) (|symeTypeCode| . 744293378)))
        (|Declare|
          %%
          |BasicType|
          ((|symeNameCode| . 51482908) (|symeTypeCode| . 866929616)))
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
            (|symeNameCode| . 51509389)
            (|symeTypeCode| . 21900315)))
        (|Declare|
          |apply|
          (|Apply| -> (|Comma| |OutPort| %) |OutPort|)
          ((|documentation| . " Basic output.
")
            (|symeNameCode| . 306472243)
            (|symeTypeCode| . 672733286)))
        (|Declare|
          |sample|
          %
          ((|documentation| . " Example element.
")
            (|symeNameCode| . 255806968)
            (|symeTypeCode| . 1015195433)))
        (|Declare|
          \0
          %
          ((|documentation| . " Identity for addition.
")
            (|symeNameCode| . 200089)
            (|symeTypeCode| . 1015195433)))
        (|Declare|
          +
          (|Apply| -> % %)
          ((|documentation| . " Identity function.
")
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
          |zero?|
          (|Apply| -> % |Boolean|)
          ((|documentation| . " Test whether value equals 0.
")
            (|symeNameCode| . 206475020)
            (|symeTypeCode| . 884043277)))
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
            (|symeNameCode| . 200086)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          %%
          |QuotientRemainder|
          ((|symeNameCode| . 51482908) (|symeTypeCode| . 285334404)))
        (|Declare|
          |gcd|
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| . " Greatest commond divisor.
")
            (|symeNameCode| . 318604649)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          |quo|
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| . " Quotient leaving remainder.
")
            (|symeNameCode| . 318204816)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          |rem|
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| . " Remainder.
")
            (|symeNameCode| . 318266239)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          |divide|
          (|Apply| -> (|Comma| % %) (|Comma| % %))
          ((|documentation| . " Quotient-remainder pair.
")
            (|symeNameCode| . 42114539)
            (|symeTypeCode| . 624266776)))
        (|Declare|
          |\\|
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| . " a\\b = inv(a)*b
")
            (|symeNameCode| . 200133)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          /
          (|Apply| -> (|Comma| % %) %)
          ((|documentation| . " Division.
")
            (|symeNameCode| . 200088)
            (|symeTypeCode| . 355597834)))
        (|Declare|
          ^
          (|Apply| -> (|Comma| % |Integer|) %)
          ((|documentation| . " Exponentiation.
")
            (|symeNameCode| . 200135)
            (|symeTypeCode| . 455750953)))
        (|Declare|
          |inv|
          (|Apply| -> % %)
          ((|documentation| . " Inverse.
")
            (|symeNameCode| . 318722696)
            (|symeTypeCode| . 143998972)))
        (|Declare|
          *
          (|Apply| -> (|Comma| i %) %)
          ((|symeNameCode| . 200083) (|symeTypeCode| . 353238574)))
        (|Declare|
          /
          (|Apply| -> (|Comma| i i) %)
          ((|symeNameCode| . 200088) (|symeTypeCode| . 353231442)))
        (|Declare|
          |numer|
          (|Apply| -> % i)
          ((|symeNameCode| . 880264500) (|symeTypeCode| . 143999008)))
        (|Declare|
          |denom|
          (|Apply| -> % i)
          ((|symeNameCode| . 882795296) (|symeTypeCode| . 143999008)))
        (|Declare|
          |coerce|
          (|Apply| -> i %)
          ((|symeNameCode| . 770345191) (|symeTypeCode| . 143989792)))))))
(Import () Basic)
(Import () Exit)
(Import () Segment)
(Import () IntegerNumberSystem)
(Import () ->)
(Import () (Comma I I))
(Import () (Comma % %))
(Import () Record)
(Import () OutPort)
(Import () Boolean)
(Import () I)
(Import () %)
(Import () (Comma))
(Import () Integer)
(Import () SingleInteger)
(Import () Ratio)
(Inline () Pointer)
(Inline () Character)
(Inline () HalfInteger)
(Inline () SingleInteger)
(Inline () SingleFloat)
(Inline () DoubleFloat)
(Inline () Integer)
(Inline () Segment)
(Inline () Generator)
