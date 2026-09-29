# Aldor v1.5.0rc10

RC10 follows the first RC9 Grice run.  It hardens multi-toolchain discovery
and fixes the Darwin SDK 26 `sbrk` declaration conflict exposed by both real
MacPorts GCC 14 and Apple Clang 21.

## Toolchain discovery

`rebuild.sh` no longer decides what "gcc" or "clang" means from an executable
name or from free-form `--version` text.  Direct `build.sh` auto-mode log labeling
uses the same macro-based family rule.

For every proposed C/C++ pair it now:

1. preprocesses the appropriate language and inspects predefined macros;
2. classifies Clang by `__clang__` before considering GCC compatibility macros;
3. classifies GNU GCC by `__GNUC__` only when `__clang__` is absent;
4. requires the C and C++ drivers to be the same compiler family;
5. requires their compiler release fingerprints to agree; and
6. when `-dumpmachine` is available, requires their target triples to agree.

Thus a command spelled `gcc` is GNU GCC only when its front-end macros prove
that it is GNU GCC.  Apple's historical `gcc`/`g++` aliases are correctly
classified as Clang.  Conversely, on Grice the unversioned MacPorts `gcc` and
`g++` are correctly recognized as genuine GCC 14.3.0 and selected alongside
Apple Clang 21.

Automatic policy remains:

- ordinary Unix/macOS/Linux: build with one coherent GCC pair and one coherent
  Clang pair when both are available;
- Cygwin: build with native GCC only by default;
- `ALDOR_TOOLCHAIN=mingw`: explicitly select the MinGW GCC-family cross pair.

`ALDOR_CC` and `ALDOR_CXX` are now accepted only with an explicit
`ALDOR_TOOLCHAIN`, and must be supplied together.  An explicit GCC or Clang
pair undergoes the same family/release/target validation.  A MinGW pair must
also report a MinGW target.

Independent build roots and the common log directory remain as in RC9:

    build-<aldor-version>-<os>-<toolchain>
    logs/

## Darwin SDK 26 memlay fix

`distro/aldor/tools/unix/memlay.c` included `<unistd.h>` and then redeclared:

    extern void *sbrk(intptr_t increment);

Current Darwin SDK 26 declares `sbrk(int)`, so the private declaration
conflicted with the system declaration under both GCC and Clang.  RC10 removes
the private declaration and relies on `<unistd.h>` as the platform authority.
The now-unused `<stdint.h>` include is removed as well.

## Host-independent regression testing

`distro/modernization/rebuild-driver-test.sh` now covers:

- the exact Grice shape: unversioned MacPorts GCC plus Apple Clang;
- an Apple `gcc` alias whose banner is deliberately misleading, proving that
  classification comes from predefined macros rather than names or prose;
- rejection of mismatched GCC C/C++ releases and fallback to a coherent pair;
- rejection of explicit C/C++ pairs with different target triples;
- Cygwin GCC-only automatic policy;
- the explicit Cygwin MinGW override and MinGW target check;
- per-toolchain root naming;
- continuation to the second toolchain after a first-toolchain failure;
- `help`/`--help`; and
- propagation of `ALDOR_BUILD_ROOT` through the lower-level `all` path.

The regression suite passes on the RC10 package source.  `memlay.c` also
compiles and runs with the available local GCC and Clang using the Aldor C99
feature-test environment.

## Validation state

RC10 has not yet been declared cross-platform clean.  The compiler/runtime
baseline remains the clean RC8 state, with RC9/RC10 changes limited to rebuild
orchestration, release identity, and the portable `memlay` declaration fix.
The next real-host runs should start with Grice and then continue across the
remaining validation platforms.
