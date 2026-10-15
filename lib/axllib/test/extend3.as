-- Test bare extension and source-order binding.
--> testrun -l axllib
#include "axllib"

PyramidCat: Category == with {
        >>: (%, %) -> Boolean;
}

PyramidAgain: Category == with {
        >>: (%, %) -> Boolean;
}

Pyramid: PyramidCat == add {
        Rep ==> Integer;
        import from Rep;

        (a: %) >> (b: %): Boolean == rep a > rep b;
}

before(): () == {
        import from Integer;
        import from Pyramid;

        a: Pyramid == 5 pretend Pyramid;
        b: Pyramid == 7 pretend Pyramid;

        if a >> b then
                print << "before: true" << newline;
        else
                print << "before: false" << newline;
}

before();

extend Pyramid: PyramidAgain;

accept(T: PyramidAgain): () == ();
accept(Pyramid);

after(): () == {
        import from Integer;
        import from Pyramid;

        a: Pyramid == 5 pretend Pyramid;
        b: Pyramid == 7 pretend Pyramid;

        if a >> b then
                print << "after: true" << newline;
        else
                print << "after: false" << newline;
}

after();
