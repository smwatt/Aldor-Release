#include "axllib.as"
#pile

import from Integer

-- The context of `where' scopes over the complete attached definition.
-- It can therefore supply both a formal parameter type and the result type.
f(x): T == x where
        default x: Integer
        T ==> Integer

-- The where context is also visible in the definition body.
seven(): Integer == n where
        n ==> 7

-- The name defined by the attached definition is introduced outside the
-- where, while names belonging only to the where context do not escape.
g(): Integer == f(5) + seven()
