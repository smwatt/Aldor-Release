#include "aldor"
#include "aldorio"

----------------------------------------------------------------------
-- Ordinary list reduction: iterative and tail recursive
----------------------------------------------------------------------

sumIter(xs: List Integer, s0: Integer): Integer == {
    s := s0;
    for x in xs repeat s := s + x;
    s
}

sumRec(ell: List Integer, s: Integer): Integer == {
    empty? ell => s;
    sumRec(rest ell, s + first ell);
}


import from Integer;

li: List Integer := [1, 2, 3, 4, 5, 6];

stdout << sumIter(li, 10) << newline;
stdout << sumRec (li, 10) << newline;

----------------------------------------------------------------------
-- Generic over an arithmetic type and with default initial value
----------------------------------------------------------------------

sumIter(R: ArithmeticType, ell: List R, s0: R == 0): R == {
    s := s0;
    for x in ell repeat s := s + x;
    s
}

sumRec(R: ArithmeticType, ell: List R, s: R == 0): R == {
    empty? ell => s;
    sumRec(R, rest ell, s + first ell);
}

import from DoubleFloat;

lf: List DoubleFloat := [1.0, 2.0, 3.0];

stdout << sumIter(DoubleFloat, lf) << newline;
stdout << sumRec(DoubleFloat, lf) << newline;

----------------------------------------------------------------------
-- Extending an existing representation with a new interface
----------------------------------------------------------------------

extend String: TotallyOrderedType == add {
    (u: %) < (v: %): Boolean == {
        import from MachineInteger;
        nu := #u;
        nv := #v;
        n  := min(nu, nv);
        for p in 1..n repeat {
            import from Character;
            u.p < v.p => return true;
            u.p > v.p => return false;
        }
        nu < nv;
    }
    (u: %) <= (v: %): Boolean == not (u > v);
}

import from List String;

words := ["animal", "aldor", "apple", "anaconda", "atlantic"];
sorted := sort! copy words;

----------------------------------------------------------------------
-- A mini category hierarchy
----------------------------------------------------------------------

AbelianGroup: Category == PrimitiveType with {
    0: %;
    +: (%, %) -> %;
    -: % -> %;
    -: (%, %) -> %;
}

Monoid: Category == PrimitiveType with {
    1: %;
    *: (%, %) -> %;
}

Ring: Category == Join(AbelianGroup, Monoid);

Algebra(R: Ring): Category == Ring with {
    *: (R, %) -> %;
}


----------------------------------------------------------------------
-- A domain constructor
----------------------------------------------------------------------

Quaternion(R: Ring): Algebra R with {
    quaternion: (R, R, R, R) -> %;
    i: %;
    j: %;
    k: %;

    if R has OutputType then OutputType;
}
== add {
    Rep == Record(a: R, b: R, c: R, d: R);
    import from Rep, R;

    quaternion(a: R, b: R, c: R, d: R): % == per [a, b, c, d];

    0: % == quaternion(0, 0, 0, 0);
    1: % == quaternion(1, 0, 0, 0);
    i: % == quaternion(0, 1, 0, 0);
    j: % == quaternion(0, 0, 1, 0);
    k: % == quaternion(0, 0, 0, 1);

    (x: %) = (y: %): Boolean == {
        rx := rep x; ry := rep y;
        rx.a = ry.a and rx.b = ry.b and rx.c = ry.c and rx.d = ry.d;
    }
    (x: %) + (y: %): % == {
        rx := rep x; ry := rep y;
        quaternion(rx.a + ry.a, rx.b + ry.b, rx.c + ry.c, rx.d + ry.d);
    }
    -(x: %): % == {
        rx := rep x;
        quaternion(-rx.a, -rx.b, -rx.c, -rx.d);
    }
    (x: %) - (y: %): % == x + (-y);
    (r: R) * (x: %): % == {
        rx := rep x;
        quaternion(r*rx.a, r*rx.b, r*rx.c, r*rx.d);
    }

    (x: %) * (y: %): % == {
        rx := rep x; ry := rep y;
        quaternion(
            rx.a*ry.a - rx.b*ry.b - rx.c*ry.c - rx.d*ry.d,
            rx.a*ry.b + rx.b*ry.a + rx.c*ry.d - rx.d*ry.c,
            rx.a*ry.c - rx.b*ry.d + rx.c*ry.a + rx.d*ry.b,
            rx.a*ry.d + rx.b*ry.c - rx.c*ry.b + rx.d*ry.a);
    }

    if R has OutputType then
        (out: TextWriter) << (x: %): TextWriter == {
            put? := false;
            put(a: R, s: String): () == {
                if a ~= 0 then {
                    out << (if put? then "+" else "") << a << s;
                    free put? := true;
                }
            }
            rx := rep x;
            put(rx.a, ""); put(rx.b, "i"); put(rx.c, "j"); put(rx.d, "k");
            out
        }
}

----------------------------------------------------------------------
-- A curried function of dependent type
----------------------------------------------------------------------

commutator(R: Ring)(x: R, y: R): R == x * y - y * x;

extend Integer: Ring;

IntegerCommutator    := commutator Integer;
QuaternionCommutator := commutator Quaternion Integer;

import from Integer, Quaternion Integer;

stdout << IntegerCommutator   (2,3)              << newline;
stdout << QuaternionCommutator(1+2*i+3*j-4*k, j) << newline;

----------------------------------------------------------------------
-- Higher-order functions on type constructors
----------------------------------------------------------------------

Ag ==> (S: Type) -> BoundedFiniteLinearStructureType S;

swap(X: Ag, Y: Ag)(S: Type)(x: X Y S): Y X S == {
    import from Y S, X S;
    [[s for s in y] for y in x]
}

import from Array List Integer, List Array Integer;

a: Array List Integer := [[1, 2, 3], [4, 5, 6]];
b: List Array Integer := swap(Array, List)(Integer)(a);

stdout << a << " became " << b << newline;

----------------------------------------------------------------------
-- OO semantics via dependent types
----------------------------------------------------------------------

Object(C: Category): with {
    ob:    (T: C, t: T) -> %;
    avail: % -> (T: C, T);
}
== add {
    Rep == Record(T: C, val: T);
    import from Rep;

    ob(T: C, t: T): % == per [T, t];
    avail(o: %): (T: C, T) == explode rep o;
}

import from Object OutputType, List Object OutputType;

objects: List Object OutputType := [
    ob(String,       "Aldor"),
    ob(Integer,      42),
    ob(List Integer, [2, 3, 5, 7])
];

writeOne(T: OutputType, t: T): () == { stdout << t << newline }

for ob in objects repeat writeOne avail ob;