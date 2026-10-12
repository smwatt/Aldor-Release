# Aldor 1.5: A language from the 90s designed for the 20s

## What is Aldor?

Aldor is a programming language built around a relatively small collection of
general ideas, applied consistently. In particular, it treats types as
first-class values and uses that choice, together with other orthogonal
mechanisms, to express many programming structures that other languages
introduce separately.

Types may be passed as arguments, returned as results, constructed from other
values, and used to express relationships among the objects in a program.
Properties required of types are described independently of their
representation, allowing considerable semantic freedom without prescribing
how values must be implemented.

The insistence on orthogonality pays off in composability. Generic programming,
extension of existing types, programming with type constructors, dependent
relationships between values and types, and several styles of object-oriented
organization can coexist without each bringing along its own private
machinery. Aldor is parsimonious about primitives, but not about what can be
built from them.

Few of these ideas are novel in isolation. What is more characteristic is how
far Aldor is prepared to go with them. The language is reluctant to introduce
a new primitive notion for a particular feature when a more general mechanism
can do the job while covering other cases as well.

There is a practical side to this. The language has been used to build
substantial systems, so its abstractions have had to work under sustained use
rather than only in illustrative examples. This has put steady pressure on
Aldor to make powerful constructions feel like ordinary programming. The aim
has not been abstraction for its own sake, but to have a language that can
express rich relationships among values, types and their properties while
retaining enough information for aggressive optimization.

Many of the resulting programming patterns will look familiar to programmers
coming from modern generic or functional languages. Aldor approaches them
through its own particular combination of first-class types, dependent typing,
categories, extension and ordinary function application. The interest lies
less in any one of these than in what becomes possible when they compose
uniformly. The result is a language in which fairly sophisticated
constructions can often be expressed with little ceremony.

## What does this mean in practice?

A few consequences of this design are worth making concrete.

- **Types and their properties are first-class.** Types may be arguments to
  functions and results of computation. Their properties are described by
  *categories*, which can themselves be parameterized and combined. This
  supports generic programming over rich families of types without fixing how
  those types are represented.

- **Types may depend on values, and values on types.** A function can accept a
  type argument and then values of that type or related types; a result type
  can depend on other arguments. Such relationships arise naturally once types
  participate in the ordinary language rather than a separate compile-time
  world.

- **Existing types can acquire additional structure.** A type need not be
  designed in advance for every use that may later be made of it. Additional
  properties and operations can be supplied where they are needed, without
  introducing a wrapper representation merely to attach another interface.

- **What looks built in need not be.** Many important data types and operations
  are supplied by libraries and ultimately implemented in terms of a much
  smaller collection of machine-level facilities. Fixed-width hardware
  arithmetic, arbitrary-precision arithmetic and user-defined numeric domains
  can therefore participate in the same general mechanisms.

- **Overloading is a general name-binding mechanism.** Selection is based on
  type and context rather than being a privilege of a particular class of
  objects.
  Familiar notation can therefore be reused systematically, with its meaning
  determined by the types involved.  In particular, overloading is not based
  on whether a named value is a function.

- **Functional and imperative programming coexist naturally.** Functions are
  first-class, higher-order programming is routine, and iteration is expressed
  through general traversal mechanisms. Neither style is treated as an annex.

- **Type constructors are themselves objects of programming.** Code can be
  generic not only over types but over constructions such as "list of" or
  "array of". This permits useful abstractions one level higher without
  introducing separate template or template-template machinery -- it is a
  direct consequence of types as values and orthogonality.

- **Library abstraction is intended to compile well.** Aldor compiles through
  an optimizing intermediate representation and emits portable C for the host
  compiler to finish. The language and compiler were designed together so
  that programming through generic interfaces does not discard the
  information needed for efficient implementation.

- **The same language serves small experiments and large systems.** Aldor can
  be used interactively or as a conventional compiled language, and its module
  mechanisms are intended to scale from a quick program to sizeable libraries
  and applications.

The original proving ground for these ideas was symbolic mathematical
computation involving rich algebraic structures. This presented a severe
test: large families of related types, layers of parameterization, algorithms
stated over abstract properties, and representations chosen for performance.
Mechanisms developed to cope cleanly with that degree of structure have
proven useful well beyond the setting that originally required them.

The following examples show what some of these ideas look like in actual
Aldor programs.

## A Taste of Aldor

The examples below are in `examples-v1.5.1.as`. They begin with code that
should look quite ordinary and then expose some of the mechanisms underneath.

### Ordinary programming

Here are iterative and tail-recursive ways to sum a list of integers,
given an initial value.

```aldor
#include "aldor"
#include "aldorio"

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
```

The first version is imperative and the second is tail recursive. There is
nothing particularly exotic in either.  The last value in the statement
sequence is returned by the function.

There is less built into the language here than these examples might at first
suggest. The traversal is supplied by the type being traversed.

```aldor
for x in xs repeat ...
```

`for` is not a list-specific construct; other library or user-defined types
can provide generators and participate in the same notation.

Likewise, the bracket construction does not intrinsically mean "construct a
list".

```aldor
[1, 2, 3, 4, 5, 6]
```

The surrounding type context selects a suitable `bracket` operation. Here the
result is a `List Integer`; elsewhere the same notation can construct an array
or a user-defined aggregate.

Even the numerals are type-directed. Literal notation obtains values through
operations of the intended type rather than imposing one universal
representation.

The surface program is conventional, but much of its meaning comes from
typed operations supplied by libraries.

### Generic programming

Now make the arithmetic type itself an argument.

```aldor
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
```

`ArithmeticType` states the properties the computation requires. The same
source can therefore operate on many arithmetic domains rather than being
tied to `Integer`.

There is more going on in the function signature than ordinary parametric
polymorphism. The types of `ell`, `s0`, `s` and the result all depend on `R`,
and `R` is itself an argument to the function.

The default initial value is expressed in the same declaration:

```aldor
s0: R == 0
```

so a caller may provide an accumulator or use the zero belonging to the
chosen arithmetic type.

Nothing here depends on the provenance of `R` -- the algorithm depends only
on the properties it needs.

### Give an existing type a new capability

Suppose strings are to participate in algorithms requiring a total order.
It is possible to imbue them with additional properties without altering
their representation or modifying the environment used by others.

```aldor
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
```

This does not introduce an `OrderedString`, wrap strings in another
representation, or alter their existing representation. It says that in this
context `String` satisfies `TotallyOrderedType`, and supplies the operations
needed to make that true. A generic list sort can then be used directly on
strings.

The point is not merely that a type can be extended. Representation and
available structure are separate questions, so a simple type can acquire
richer properties where those properties are useful.

### Categories describe the structure required

Type categories may be constructed by specifying required exports and
inherited interfaces.

```aldor
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
```

Here, `AbelianGroup` and `Monoid` state required exports. `Ring` combines those
requirements while `Algebra(R)` is parameterized by a ring and adds a scalar
multiplication.

Because categories may themselves be parameterized and combined, interfaces
participate in the same style of abstraction as the types whose properties
they describe.

### Types can be constructed by ordinary functions

Here is a small quaternion domain constructor. It is a function that takes a
type belonging to one category as a parameter and returns a type belonging to
another.

```aldor
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
```

Internally the quaternion values are represented by records of four elements
of `R`; clients see the algebraic interface rather than that representation.

One export is conditional:

```aldor
if R has OutputType then OutputType;
```

If values of `R` can be written, then quaternions over `R` can be written as
well, and the corresponding implementation is provided under the same
condition.

### Passing related arguments

We saw generic functions earlier. Here dependent mapping types relate
arguments. Consider this small function: it takes a type as an argument and
returns a function that takes two values of that type and performs arithmetic
on them.

```aldor
commutator(R: Ring)(x: R, y: R): R == x * y - y * x;
```

The function first accepts a ring `R`, and then two values of that very `R`.
Supplying the first argument therefore determines both the operation and the
types of its remaining arguments. Because `R` is declared to be a `Ring`,
the required multiplication and subtraction are available. This does not
require `x` and `y` to be allocated objects: they may be passed as raw data
values.

To use this with integers, we first declare the existing `Integer` type to
belong to the new category `Ring`, using its existing exports to satisfy the
category requirements. We then specialize `commutator` in two ways.

```aldor
extend Integer: Ring;

IntegerCommutator    := commutator Integer;
QuaternionCommutator := commutator Quaternion Integer;

import from Integer, Quaternion Integer;

stdout << IntegerCommutator   (2,3)              << newline;
stdout << QuaternionCommutator(1+2*i+3*j-4*k, j) << newline;
```

The two calls to `commutator` are ordinary partial applications. Type arguments
do not belong to a separate template language or a special declaration phase.

### Higher-order programming, one level up

Since type constructors are ordinary functions, they can be arguments as well.

```aldor
Ag ==> (S: Type) -> BoundedFiniteLinearStructureType S;

swap(X: Ag, Y: Ag)(S: Type)(x: X Y S): Y X S == {
    import from Y S, X S;
    [[s for s in y] for y in x]
}

import from Array List Integer, List Array Integer;

a: Array List Integer := [[1, 2, 3], [4, 5, 6]];
b: List Array Integer := swap(Array, List)(Integer)(a);

stdout << a << " became " << b << newline;
```

`Ag` describes the type of a constructor such as `Array` or `List`: given an
element type, it produces a bounded finite linear aggregate type.

`swap` accepts two such constructors, an element type `S`, and a value of type
`X Y S`; it returns a value of type `Y X S`.

The implementation is just `[[s for s in y] for y in x]`.
The two traversals are supplied by the input aggregate types. The two bracket
constructions are selected according to the required result types. The same
short expression is therefore generic both in how it consumes its input and
in how it constructs its output.

### Types and values can travel together

Because types are values, a value may be packaged together with its type while
retaining a static constraint on which types are permitted.

```aldor
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
```

An `Object(C)` contains some type `T` satisfying category `C`, together with a
value of precisely that `T`.

The list above contains a string, an integer and a list of integers. They do
not share a representation or a common superclass. What they share is the
property `OutputType`.

Unpacking an object with `avail` produces the dependent pair

```aldor
(T: C, T)
```

consisting of a type known to satisfy `C` and a value known to have exactly
that type. Consequently

```aldor
for ob in objects repeat writeOne avail ob;
```

is statically justified. The relationship between the hidden type and the
hidden value is itself represented in the type system.

By this point the examples have used dependent types, generic algorithms,
open extension, higher-order type constructors and algebraic interfaces. None
required a parallel meta-language. They arise by composing the same small
collection of mechanisms used in the simpler examples.

## Source Distribution

The repository contains the public standalone source distribution for Aldor.

It is self-contained with respect to Aldor project material: a copy of this
repository is sufficient to configure, build and test Aldor, subject only to
documented host prerequisites such as a C/C++ compiler, Bison, Bash, standard
build utilities, and GMP development headers and libraries.

Release source archives and prebuilt platform packages are available from:

https://github.com/smwatt/Aldor-Release/releases

## Prebuilt Aldor packages

Prebuilt Aldor 1.5.1 packages are available for the following
platform/compiler combinations:

| Platform | Architecture | Toolchain |
| --- | --- | --- |
| Cygwin | x86_64 | GCC |
| Cygwin | x86_64 | Clang |
| Debian Linux | riscv64 | GCC |
| Debian Linux | riscv64 | Clang |
| macOS | arm64 | GCC |
| macOS | arm64 | Apple Clang |
| macOS | x86_64 | GCC |
| macOS | x86_64 | Apple Clang |
| Ubuntu Linux | aarch64 | GCC |
| Ubuntu Linux | aarch64 | Clang |
| Ubuntu Linux | x86_64 | GCC |
| Ubuntu Linux | x86_64 | Clang |

For these configurations all tests pass.

Package names have the form:

```text
aldor-1.5.1-<platform>-<architecture>-<toolchain>.tgz
```

For example:

```text
aldor-1.5.1-ubuntu-x86_64-gcc.tgz
aldor-1.5.1-macos-arm64-clang.tgz
```

### Using a prebuilt package

Unpack the package in a convenient location:

```sh
tar xzf aldor-1.5.1-ubuntu-x86_64-gcc.tgz
```

Aldor uses the environment variable `ALDORROOT` to identify the root of the
installed Aldor tree. Set `ALDORROOT` to the directory created by unpacking
the package -- the directory containing the Aldor `bin` and library
directories -- and put its `bin` directory on your search path.

For example:

```sh
export ALDORROOT=/path/to/aldor
export PATH="$ALDORROOT/bin:$PATH"
```

`ALDORROOT` tells the compiler where to find the libraries and other files
belonging to that Aldor installation.

You can then compile an Aldor source file with, for example:

```sh
aldor -Fx -laldor examples-v1.5.1.as
```

The `ALDORROOT` and `PATH` settings may be placed in your shell startup file
if Aldor is to be used regularly.

## Building from source

`ALDORROOT` is the destination directory where the compiled Aldor installation
will be placed. After the build it will contain subdirectories such as `bin/`,
`lib/`, and other installation files.

For a normal build of one selected or default toolchain:

```sh
export ALDORROOT=/path/to/aldor
bash build.sh
```

A particular compiler family can be selected, for example:

```sh
ALDORROOT=/path/to/aldor ALDOR_TOOLCHAIN=gcc bash build.sh
ALDORROOT=/path/to/aldor ALDOR_TOOLCHAIN=clang bash build.sh
```

For a complete clean build and test for each supported native compiler family
found on the host:

```sh
bash rebuild.sh
```

`rebuild.sh` selects build roots automatically; `ALDORROOT` does not need to
be set when using it.

Generated build roots used by `rebuild.sh` are placed outside the source tree.

## Documentation

Guides and manuals for earlier versions:

- [Aldor User Guide](doc/aldor-ug-2003/aldorug.pdf)
- [libaldor Reference Manual](lib/aldor/doc/libaldor.pdf)
- [Algebra Reference Manual](lib/algebra/doc/algebra.pdf)

Additional historical and technical documentation is under [`doc/`](doc/).

## Releases

The `v1.5.1` tag identifies the Aldor 1.5.1 public source snapshot.

The corresponding GitHub Release provides the standalone source archive and
validated binary packages for supported platform/compiler combinations.

## Licensing and provenance

See [`legal/`](legal/) for copyright and licensing information.