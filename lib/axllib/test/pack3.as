-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen f -Q3
--> testrun -Q3 -l axllib

#include "axllib"

macro {
        SI == SingleInteger;
        DF == DoubleFloat;
}

-- A value with an explicitly declared raw representation.  RawType is part
-- of the type itself rather than being guessed from the size of a Word.
PackableValue: Category == BasicType with {
        RawType: Type;
        raw: % -> RawType$%;
        box: RawType$% -> %;
}

-- The ordinary one-Word case.
extend SI: PackableValue == add {
        RawType: Type == SI;
        raw(x: %): RawType$% == x pretend RawType$%;
        box(x: RawType$%): % == x pretend %;
}

-- A floating-point Word is also immediate, but its machine raw type is BDFlo.
extend DF: PackableValue == add {
        RawType: Type == BDFlo;
        raw(x: %): RawType$% == (x::BDFlo) pretend RawType$%;
        box(x: RawType$%): % == ((x pretend BDFlo)::%);
}

-- WGS84 position: decimal-degree latitude/longitude and altitude in metres
-- above the WGS84 ellipsoid.  On supported 64-bit targets this is naturally
-- a three-Word raw value, so it cannot accidentally collapse to the common
-- one-Word representation.
WGS84Position: PackableValue with {
        new:       (DF, DF, DF) -> %;
        latitude:  % -> DF;
        longitude: % -> DF;
        altitude:  % -> DF;
        raise:     (%, DF) ->* %;
}
== add {
        Rep == Record(latitude: DF, longitude: DF, altitude: DF);
        import from Rep;

        RawType: Type == Rep;

        sample: % == per [0.0, 0.0, 0.0];
        new(lat: DF, lon: DF, alt: DF): % == per [lat, lon, alt];

        latitude(p: %): DF == rep(p).latitude;
        longitude(p: %): DF == rep(p).longitude;
        altitude(p: %): DF == rep(p).altitude;

        raise(p: %, delta: DF):* % ==
                new(latitude p, longitude p, altitude p + delta);

        raw(p: %): RawType$% == rep(p) pretend RawType$%;
        box(p: RawType$%): % == per(p pretend Rep);

        (out: TextWriter) << (p: %): TextWriter ==
                out << "(" << latitude(p) << ", " << longitude(p)
                    << ", " << altitude(p) << ")";

        (p: %) = (q: %): Boolean ==
                latitude(p) = latitude(q)
                and longitude(p) = longitude(q)
                and altitude(p) = altitude(q);
}

-- The generic immediate-call path must preserve the raw representation of S.
applyPacked(S: PackableValue, f: S ->* S, x: S): S == f x;

increment(x: SI):* SI == x + 1;

raise10(p: WGS84Position):* WGS84Position == {
        import from DF;
        raise(p, 10.0);
}

main(): () == {
        import from SI, DF, WGS84Position;

        print << applyPacked(SI, increment, 41) << newline;

        p := new(43.4723, -80.5449, 334.0);
        q := applyPacked(WGS84Position, raise10, p);
        ok := latitude(q) = 43.4723 and longitude(q) = -80.5449
              and altitude(q) = 344.0;
        print << ok << newline;
}

main();
