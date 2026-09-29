# Aldor v1.5.0rc1

This is a light public-version/toolchain repackaging of the validated
v0.18rc2 modernization candidate.  The public version is 1.5.0rc1 because
Aldor 1.0, 1.1 and 1.2 have already been released.

## Correctness baseline

No language-semantic change is intended relative to v0.18rc2.  Its pristine
Trixie acceptance state was:

    compiler self-tests       29 / 29
    AxlLib                    761 / 761
    libaldor                    8 / 8
    Algebra std                24 / 24
    Algebra debug              24 / 24
    Algebra GMP                24 / 24
    ----------------------------------
    primary total             870 / 870

with 0 DIFFERENT, 0 ERROR and all modernization release gates PASS.

## Toolchain selection

`distro/build-fns.sh` now resolves a build-wide toolchain.  The normal entry
point remains:

    bash build.sh all

Explicit choices are:

    ALDOR_TOOLCHAIN=gcc   bash build.sh all
    ALDOR_TOOLCHAIN=clang bash build.sh all
    ALDOR_TOOLCHAIN=mingw bash build.sh all

The `mingw` choice defaults to the Cygwin-hosted native-Windows tools:

    x86_64-w64-mingw32-gcc
    x86_64-w64-mingw32-g++
    x86_64-w64-mingw32-ar
    x86_64-w64-mingw32-ranlib

and uses the historical `MACHINE=win32gcc` / `CONFIGSYS=win32gcc` native
Windows configuration.  The obsolete `-mno-cygwin` flags have been removed.

The following variables override individual resolved tools:

    ALDOR_CC
    ALDOR_CXX
    ALDOR_AR
    ALDOR_RANLIB

For example:

    ALDOR_TOOLCHAIN=gcc \
    ALDOR_CC=gcc-14 ALDOR_CXX=g++-14 \
    bash build.sh all

The conventional `CC`, `CXX`, `AR`, `RANLIB` and historical `HOST_*`
variables remain accepted as fallbacks.

Native filename conventions are centralized as:

    ALDOR_OBJEXT
    ALDOR_LIBEXT
    ALDOR_EXEEXT

GNU/Clang/MinGW use `.o` and `.a`; MinGW uses `.exe`.  Selecting
`ALDOR_TOOLCHAIN=msvc` establishes `cl`, `lib`, `.obj`, `.lib` and `.exe`
naming, but MSVC option/archive translation is deliberately still marked
experimental.  This package does not claim a complete MSVC build yet.

## Generated-C linking

When generated C is compiled directly by GCC/Clang/MinGW, the historical
`unicl` wrapper no longer supplies target system libraries.  The compiler now
accepts `ALDOR_SYSTEM_LIBS`, a comma-separated list of library names appended
after the Aldor runtime archive.  This preserves one-pass static-link order.
The GCC-like build defaults it to `m`; it can be overridden explicitly.

This is distinct from `-Clib=...`, whose libraries intentionally precede the
runtime.  Release builds continue to clear inherited `ALDORARGS` so an outside
compiler-option environment cannot contaminate a clean acceptance build.

For Clang, the build suppresses `-Wdeprecated-non-prototype` only for generated
Aldor C.  Some historical generated declarations use the C99-compatible
no-prototype form; Clang 17 warns about their removal in C23.  The C++20
compiler/runtime sources retain their normal diagnostics.

## Toolchain validation for this candidate

On Trixie/x86-64, GCC 14.2 was used for a clean `build.sh all`: all 870 primary
tests passed, all modernization release gates passed, and build diagnostics
contained zero warnings and zero errors.

Clang 17 was used for an isolated compiler/runtime bootstrap.  The compiler
self-tests passed 29/29.  After the generated-C warning selection described
above, `libfoamlib` and `libfoam` rebuilt with zero warnings and zero errors.

MinGW-w64 was not installed on the Trixie validation host.  Its tool names,
filename conventions, `MACHINE=win32gcc`, and compiler-side `_WIN32` platform
selection were checked at configuration/preprocessor level, but a native
Windows build remains to be run on Serwin.  MSVC remains filename/tooling
scaffolding only in this candidate.

See `reports/v1.5.0rc1-TOOLCHAIN-VALIDATION.txt` for the exact validation
summary.

## Historical provenance

The semantic recovery, repeated-extension, parameterized-extension and timing
work documented in `README-RC2.md`, `RESUME-RC2.md` and the RC2 reports is
retained unchanged as historical provenance.  The v0.01 deterministic timing
comparison recorded there found generated-code execution essentially unchanged
and compiler/backend time substantially reduced.
