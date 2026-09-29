# Aldor v1.5.0rc11

RC11 is a portability and release-driver checkpoint derived from RC10.

Changes:

- `rebuild.sh` automatic toolchain discovery now checks compiler family/release
  from predefined macros, matching C/C++ target triples, and actual host
  viability.  On macOS the viability probe compiles a C++20 source including
  `<mach/mach.h>`, so an installed GNU GCC that cannot consume the current
  Apple SDK is skipped automatically.  Explicit `ALDOR_TOOLCHAIN=gcc` still
  forces the requested family for diagnosis or alternate-SDK use.
- Automatic discovery continues past an unusable compiler pair to later
  versioned candidates instead of abandoning the compiler family.
- macOS warning options are probed independently for C and C++; C-only options
  such as GCC's `-Wno-int-conversion` no longer leak into C++ compilation.
- `memlay.c` restores the legacy `sbrk(intptr_t)` declaration only on Cygwin,
  where current feature-test settings hide it, while Darwin/Linux use their
  system declarations.
- `modernization/validate.sh` names every auxiliary release gate and records
  `<gate>.status`, so `set -e` can no longer produce an anonymous release-gate
  failure.  `make-test-report.sh` reports these auxiliary statuses.
- `rebuild-driver-test.sh` covers unusable macOS GCC, explicit forcing, and
  fallback from an unusable plain GCC to a viable versioned GCC.

The compiler/language semantics are otherwise unchanged from RC10.
