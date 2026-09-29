# What's New in Aldor 2026

Aldor in 2026 is not a new language so much as a renewed implementation and
a clarified platform for the next stage of the language.  The immediate goal
has been to preserve the character of Aldor while bringing its compiler,
runtime, build system, and generated-code interfaces onto a foundation that is
portable, maintainable, and testable on current systems.

The work has therefore concentrated first on implementation quality rather
than on visible language change.  The compiler is being modernized as a C++20
program, while Aldor-generated C remains deliberately conservative C99.  This
keeps generated code easy to compile with ordinary C toolchains and avoids
making users of Aldor depend on C++ merely because the compiler itself is now
implemented in C++.  Runtime interfaces remain explicit and stable across the
C/C++ boundary.

A major part of the modernization has been replacing implementation idioms
that were reasonable on 1990s systems but are fragile on current 64-bit ABIs.
In particular, several old uses of untyped C varargs, erased function-pointer
types, and integer/pointer interchange have been removed or made type-safe.
These issues became especially visible on ARM64, where code that happened to
work on older x86 systems could fail or silently misread arguments.  The
current tree includes regression checks intended to prevent those patterns
from returning.

The internal data structures of the compiler have also been moving toward
stronger C++ abstractions.  Lists, tables, symbols, source locations, buffers,
tokens, C-code trees, FOAM trees, and related structures are being given typed
interfaces in place of older generic C conventions.  The intent is not to
wrap the compiler in layers, but to make representation assumptions explicit
enough that the C++ type system can catch mistakes that previously survived
until runtime.

Portability is now treated as a release property rather than an afterthought.
The build system has been exercised across macOS and Linux on both x86-64 and
ARM64, with GCC and Clang, together with Cygwin and continuing Windows work.
The build machinery keeps toolchains in separate build roots, records precise
compiler and platform information, and checks that native static archives and
Aldor library archives are handled by the appropriate tools.  macOS-specific
compiler compatibility is capability-probed rather than implemented by broad
platform guesses.

The test regime has been expanded in parallel.  Compiler tests, AxlLib tests,
the base Aldor library, and multiple Algebra configurations are run as a
matrix, with warnings and errors treated as release-quality issues.  The aim
is not merely to obtain a successful executable, but to make a release
reproducible and diagnostically quiet across supported toolchains.

Aldor's archive and interpreter paths have also received attention.  Native
object archives use the platform's ordinary archive tools, while Aldor
archives use Aldor-specific handling.  The `.ao` representation is being
treated as a versioned format so that interpreter images can evolve without
silently depending on one historical in-memory layout.  This work is part of
making compiled and interpreted execution agree more reliably.

The 2026 work also prepares for a small, deliberately conservative language
and library update after the portability release.  The planned primitive
numeric family uses explicit-width names such as `Int16`, `Int32`, `Int64`,
`Word16`, `Word32`, `Word64`, `Float32`, and `Float64`, with wider forms where
the implementation supports them.  Historical names such as
`MachineInteger`, `SingleInteger`, and `DoubleFloat` are intended to remain
available through compatibility support rather than disappearing abruptly.

Unicode support is intended to arrive incrementally.  Strings and comments
are the natural first target, followed by a carefully restricted set of
Unicode identifiers and selected mathematical operator aliases.  The goal is
to make mathematical source more natural without turning lexical portability
into a moving target.

Documentation syntax is also being reconsidered.  The direction is toward
unconditional documentation comments, including lightweight structured forms,
rather than maintaining separate conditional ALDOC paths.  This should make
documentation ordinary source material and permit simpler downstream tools.

At the FOAM level, the modernization is clarifying primitive representation
types and generic carriers.  `Item` is the provisional name for an untagged
atomic carrier used when the static type determines the interpretation;
`Box` is reserved for a tagged carrier.  `Nil` is distinguished from `Void`:
the former is a pointer-like sentinel value, while the latter denotes the
absence of a returned value.  These distinctions are intended to make low-level
interfaces more regular without forcing premature generalization such as
arbitrary-width `Int(n)` and `Word(n)` FOAM types.

Large algebraic values are also being designed around immutable published
semantics with privately mutable, uniquely owned storage.  For values such as
big integers, this permits destructive operations and buffer reuse inside an
algorithm while preserving the functional semantics visible to Aldor code.
A uniquely owned result can be frozen and published without copying; mutation
after publication requires uniqueness or a copy.  This is intended to support
allocation-conscious algorithms without exposing mutation at the language
level.

The `1.5.0` release should therefore be read primarily as the portable
foundation.  Its most important changes are beneath the language
surface: a modern compiler implementation, stricter ABI discipline, cleaner
generated C, stronger testing, and a build system suitable for current
platforms.  The 1.6 and subsequent 1.x releases can then introduce selected language,
library, Unicode, FOAM, and documentation improvements without mixing those
changes with unresolved portability work.

The guiding principle of the 2026 effort is continuity with clarification.
Aldor's central ideas—strong abstraction, dependent and categorical
interfaces, efficient generic programming, and close control over generated
code—remain the point.  The modernization is intended to make those ideas
easier to preserve, test, and extend for another generation of systems.
