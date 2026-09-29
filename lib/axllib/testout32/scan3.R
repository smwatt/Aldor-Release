*** Starting "scan" phase...
*** Result of scan:
[-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org)., <NL>
, --> testphase scan, <NL>
, #pile
, <NL>
, --% Scan3: Exhaustive test of all forms of numbers, <NL>
, --, <NL>
, -- r = radix specifier:      36r         (2 <= radix <= 36), <NL>
, -- w = whole part:           999         (or 99FFEE    with radix), <NL>
, -- p = point, <NL>
, -- f = fraction part:        999         (or 99FFEE    with radix), <NL>
, -- e = exponent part:        [eE]99      (only e99     with radix), <NL>
, -- es= signed exponent part: [eE][+-]99  (only e[+-]99 with radix), <NL>
, <NL>
, -- The various forms of numbers, <NL>
, .1, --   pf, <NL>
, .98, --   pf, <NL>
, .98e7, --   pfe, <NL>
, .98E+7, --   pfes, <NL>
, 32, --  w   , <NL>
, 32E7, --  w  e, <NL>
, 32e-7, --  w  es, <NL>
, 0., --  wp, <NL>
, 32., --  wp   , <NL>
, 32.e7, --  wp e, <NL>
, 32.e-7, --  wp es, <NL>
, 32.98, --  wpf, <NL>
, 32.98e7, --  wpfe, <NL>
, 3.9e+7, --  wpfes, <NL>
, 15r.BE8, -- r pf, <NL>
, 8r.70e4, -- r pfe, <NL>
, 10r.98e+7, -- r pfes, <NL>
, 16rFFEA, -- rw   , <NL>
, 16rFFEAe3, -- rw  e, <NL>
, 16rFFEAe-4, -- rw  es, <NL>
, 17r32., -- rwp   , <NL>
, 2r01.e7, -- rwp e, <NL>
, 18rAG.e+12, -- rwp es, <NL>
, 12r12TE.TE, -- rwpf, <NL>
, 8r76.05e9, -- rwpfe, <NL>
, 10r9AB.DEe+987, -- rwpfes, <NL>
, <NL>
, -- Various forms of non-numbers, <NL>
, -- (These generate errors), <NL>
, <Error: improper number (no digits in exponent).> , --  wp es, <NL>
, <Error: bad radix specification (2 <= radix <= 36).> , r, -- r    , <NL>
, <Error: bad radix specification (2 <= radix <= 36).> , re9, -- r   e, <NL>
, <Error: improper number (after radix specification).> , e, |+|, 9, -- r   es, <NL>
, <Error: improper number (no digits).> , -- r p   , <NL>
, <Error: improper number (no digits).> , e4, -- r p e, <NL>
, <Error: improper number (no digits).> , e, |+|, 4, -- r p es, <NL>
, <Error: bad radix specification (2 <= radix <= 36).> , r9AB, |.|, DEe, |+|, 987, -- rwpfes, <NL>
, -- (These generate other junk), <NL>
, 3., |+|, --  wp  s, <NL>
, |.|, e7, --   p e, <NL>
, |.|, e, |+|, 7, --   p es, <NL>
]

