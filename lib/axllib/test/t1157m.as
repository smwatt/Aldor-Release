------------------------  optbug.as --------------------------
#include "axllib"

#library foo "t1157s.ao"
import from foo;
inline from foo;

import {
        fiFopen: (String, String) -> Pointer;
        fiFgetc: Pointer -> SingleInteger;
} from Foreign;

local scan!(p:Pointer):MyChar == {
        n := fiFgetc p;
        char n;
}

main():() == {
        import from Pointer, MyChar;
        p := fiFopen("t1157s.as", "r");
        ~nil? p => {
                c := scan! p;
                while ~(c = eof) repeat { print << c pretend Character ; c := scan! p};
        }
}

main();

