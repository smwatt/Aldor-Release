*** Starting "include" phase...
*** Result of include:
[[1] include0.as, line 1: -- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
, [2] include0.as, line 2: --> testphase include
, [3] include0.as, line 3: 
, [4] include0.as, line 4: --% Include0: Basic including (incl vs. reincl, relative paths, default ftype)
, [5] include0.as, line 5: #includeDir "incl"
, [6] include0.as, line 6: #include "includeA.as"
, [7] incl/includeA.as, line 1: -- Start file "test/incl/includeA.as"
, [8] incl/includeA.as, line 2: 
, [9] incl/includeA.as, line 3: -- This should include include0.as from this directory, not the test directory.
, [10] incl/includeA.as, line 4: #include "include0"
, [11] incl/include0.as, line 1: -- Start file "test/incl/include0.as"
, [12] incl/include0.as, line 2: -- End   file "test/incl/include0.as"
, [13] incl/include0.as, line 3: 
, [14] incl/includeA.as, line 5: 
, [15] incl/includeA.as, line 6: -- This should not reinclude
, [16] incl/includeA.as, line 7: #include "include0"
, [17] incl/includeA.as, line 8: 
, [18] incl/includeA.as, line 9: -- This should reinclude
, [19] incl/includeA.as, line 10: #reinclude "include0.as"
, [20] incl/include0.as, line 1: -- Start file "test/incl/include0.as"
, [21] incl/include0.as, line 2: -- End   file "test/incl/include0.as"
, [22] incl/include0.as, line 3: 
, [23] incl/includeA.as, line 11: 
, [24] incl/includeA.as, line 12: -- This should include relative to the directory of this file.
, [25] incl/includeA.as, line 13: #reinclude "Z/includeZ"
, [26] incl/Z/includeZ.as, line 1: -- Start file "test/incl/Z/includeZ.as"
, [27] incl/Z/includeZ.as, line 2: -- End   file "test/incl/Z/includeZ.as"
, [28] incl/includeA.as, line 14: 
, [29] incl/includeA.as, line 15: -- End   file "test/incl/includeA.as"
, [30] include0.as, line 7: 
, [31] include0.as, line 8: The End
]

