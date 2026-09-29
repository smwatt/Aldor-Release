# Aldor v1.5.0rc15 portability cleanup checkpoint 2

This checkpoint consolidates the compiler/runtime portability layer before the
next release candidate.

## Supported target model

The source portability layer now recognizes only these target families:

- Linux: GCC or Clang; x86-64, AArch64, RISC-V64.
- macOS: GCC or Clang; x86-64, AArch64.
- Windows/Cygwin: GCC or Clang; x86-64.
- Future native Windows identities are reserved for MinGW GCC, MinGW Clang,
  and MSVC, but rebuild.sh does not attempt those targets yet.

Compiler family and Windows environment/ABI are independent properties.
`platform.h` records compiler major/minor/patch versions as well as OS,
environment and architecture.

## Portability structure

- `platform.h` is the only hand-written source header which interprets compiler
  predefined environment macros.
- `cconfig.h` derives capabilities and defects from the normalized environment.
- `cport.h` presents the common C99/C++20/POSIX-facing environment to Aldor.
- `fenvport.h` is the one focused platform service retained for floating-point
  trap control.
- There are no remaining `.h0` wrapper files.
- `stdc.h` is gone and the old `stdc.cpp` support unit is now `cport.cpp`.
- Active hand-written source contains no unsupported-platform branches and no
  raw compiler/OS predefined macros outside `platform.h`.

The Cygwin `putenv`/`sbrk` issue is handled as POSIX capability normalization
in `cport.h`, not as source-local compiler conditionals.

## Build-driver policy

Automatic native rebuilds use GCC and Clang when available, including both on
Cygwin.  MinGW/MSVC names are recognized as future targets but are deliberately
rejected by `rebuild.sh` for now.

Recommended future Windows variant names are:

- `cygwin-gcc`
- `cygwin-clang`
- `mingw-gcc`
- `mingw-clang`
- `msvc`

If clang-cl/MSVC-ABI support is added later, use `msvc-clang`.

## Validation performed here

- portability-regression.sh: PASS
- test-harness-regression.sh: PASS (686 classified entries)
- build-entry-regression.sh: PASS
- rebuild-driver-test.sh: PASS
- GCC and Clang syntax/compile checks for the changed compiler/runtime,
  testaldor, and unitools sources: PASS
- `distro/build.sh junk` on a clean tree: silent PASS
- Linux GCC real build progressed through tools, testaldor, unitools and well
  into the compiler library without portability diagnostics before the local
  execution window ended.  This is not claimed as a completed platform build.

MinGW and MSVC builds have not been attempted.
