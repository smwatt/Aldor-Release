# Aldor v1.5.0rc12

RC12 is a macOS portability/cleanliness follow-up to RC11, incorporating the
late RC10 results from `ice` (Intel macOS 26.6.2).

Changes:

- `rebuild.sh` host viability probing now compiles a C99 source including
  `<stdio.h>` and `<unistd.h>` before the existing C++20 host probe.  This
  catches stale separately-installed GCC include-fixed trees after an
  OS/Xcode upgrade, such as Ice's Homebrew GCC 13.2 installation whose
  `stdio.h` could no longer find Apple's `_stdio.h`.
- Darwin archive indexing now probes whether the selected `ranlib` supports
  Apple's `-no_warning_for_no_symbols` option.  When supported, the option is
  propagated through `ranlibCmd`, `doranlib`, compiler archive recompilation,
  and the standalone Aldor/Algebra library builders.  No log filtering and no
  dummy object symbols are used.
- AxlLib `float2` no longer treats the last decimal bits of a quadrature result
  as a portable golden string.  It checks the result, error estimate, and
  convergence status against explicit numerical tolerances and emits stable
  platform-independent output.  This addresses Ice/Clang's one-ulp result
  difference while continuing to fail genuine numerical regressions.
- Diagnostic cleanliness is itself now recorded as the named
  `build-diagnostics` modernization gate.  A warning-cleanliness failure can
  therefore no longer terminate validation anonymously before the auxiliary
  gate instrumentation starts.
- RC11's Cygwin-only `sbrk` compatibility declaration, independent C/C++
  warning-option probing, named auxiliary gates, and toolchain-family/target
  validation are retained.
- Corrected the release suffix macro while advancing the public compiler
  identity to `1.5.0rc12`.

Host-independent regressions pass:

- `portability-regression.sh`
- `test-harness-regression.sh`
- `build-entry-regression.sh`
- `rebuild-driver-test.sh`

The compiler/language semantics are otherwise unchanged from RC11.
