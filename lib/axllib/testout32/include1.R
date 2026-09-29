*** Starting "include" phase...
*** Result of include:
[[1] include1.as, line 1: -- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
, [2] include1.as, line 2: --> testphase include
, [3] include1.as, line 3: 
, [4] include1.as, line 4: --% Include1: Assertions and conditional inclusion
, [5] include1.as, line 5: --
, [6] include1.as, line 6: --  The lines 1, 2, 8, 9 and the contents of incl/include0.as should appear.
, [7] include1.as, line 7: 
, [8] include1.as, line 8: #assert Asserted
, [9] include1.as, line 9: 
, [10] include1.as, line 10: #if Asserted
, [11] include1.as, line 11: -- 1. This line should appear
, [12] include1.as, line 12: #endif
, [13] include1.as, line 13: 
, [14] include1.as, line 14: #if Asserted
, [15] include1.as, line 15: -- 2. This line should appear
, [16] include1.as, line 16: #else
, [18] include1.as, line 18: #endif
, [19] include1.as, line 19: 
, [20] include1.as, line 20: #if Unasserted
, [22] include1.as, line 22: #elseif AlsoUnasserted
, [29] include1.as, line 29: #else
, [30] include1.as, line 30: -- 8. This line should appear
, [31] include1.as, line 31: #   if Asserted
, [32] include1.as, line 32:     -- 9. This line should appear
, [33] include1.as, line 33: #   else
, [35] include1.as, line 35: #   endif
, [36] include1.as, line 36: #includeDir "incl"
, [37] include1.as, line 37: #include "includeC.as"
, [38] incl/includeC.as, line 1: -- Start file "test/incl/includeC.as"
, [39] incl/includeC.as, line 2: -- End   file "test/incl/includeC.as"
, [40] incl/includeC.as, line 3: 
, [41] include1.as, line 38: #endif
]

