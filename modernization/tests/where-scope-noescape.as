#include "axllib.as"
#pile

import from Integer

f(): Integer == n where
        n ==> 7

-- This must fail: n belongs only to f's where context.
g(): Integer == n
