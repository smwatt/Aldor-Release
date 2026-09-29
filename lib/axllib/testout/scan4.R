*** Starting "scan" phase...
*** Result of scan:
[-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org)., <NL>
, --> testphase scan, <NL>
, #pile
, <NL>
, --% Scan4: Determination of float vs ./int by context, <NL>
, <NL>
, -- These are not floats, <NL>
, A, |.|, 2, |.|, 3, <NL>
, A, |.|, 2, |.|, i, |.|, 3, <NL>
, f, |(|, 3, |)|, |.|, 3, <NL>
, i, |for|, i, |in|, 1, |..|, 10, <NL>
, |[|, 1, |..|, 10, |,|, 1, |..|, 10, |]|, <NL>
, <NL>
, -- These are floats, <NL>
, sin, 2.3, <NL>
, |[|, A, |.|, 1e7, |.|, 3e-4, |,|, A, |.|, 2r10e12, |]|, -- Strange combo [ A . 1e7 . 3e-4 , A . 2r10e12 ], <NL>
, f, |(|, .2, |)|, <NL>
, f, |(|, .2, |,|, 2., |,|, .2, |)|, <NL>
, |for|, i, |in|, .1, |..|, .2, |repeat|, .7, <NL>
, |for|, i, |in|, 1., |..|, 2., |repeat|, 8., <NL>
, |-|, .7, <NL>
, <NL>
, -- These have a mixture, <NL>
, f, 2, 3.2, <NL>
, g, |(|, .2, |)|, |.|, 2, <NL>
, A, |.|, 2, 3.2, <NL>
, A, |.|, 3, 3., <NL>
, |for|, i, |in|, 1., |..|, 2, |repeat|, |if|, 5, |then|, .7, |else|, 0., <NL>
, |[|, 1., |,|, .1, |..|, 2, |]|, <NL>
]

