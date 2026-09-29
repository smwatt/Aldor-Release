# Aldor v1.5.0rc16

RC16 promotes the full portability cleanup from the RC15 portability CP2
checkpoint into a release candidate.

## Supported target model

The active portability layer now targets:

- Linux: GCC or Clang; x86-64, AArch64, RISC-V64.
- macOS: GCC or Clang; x86-64, AArch64.
- Windows/Cygwin: GCC or Clang; x86-64.

The portability layer also reserves coherent future identities for native
Windows builds with MinGW GCC, MinGW Clang and MSVC.  These native-Windows
variants are deliberately not attempted by the rebuild driver yet and are not
claimed as supported in RC16.

Recommended Windows variant names are:

- `cygwin-gcc`
- `cygwin-clang`
- `mingw-gcc`
- `mingw-clang`
- `msvc`

If Clang using the Microsoft ABI/runtime (`clang-cl`) is supported later, use
`msvc-clang`.

## Portability structure

The portability architecture is now:

    platform.h -> cconfig.h -> cport.h -> ordinary source

- `platform.h` is the only hand-written source header that interprets raw
  compiler/OS predefined macros.  It normalizes OS, Windows environment/ABI,
  architecture, compiler family and compiler major/minor/patch version.
- `cconfig.h` derives capabilities and defects from that normalized
  environment.
- `cport.h` constructs the standard C99/C++20/POSIX-facing environment used by
  ordinary Aldor source.
- `fenvport.h` is a focused floating-point platform service rather than a
  repaired standard-header wrapper.
- All historical `.h0` wrappers are gone.
- `stdc.h` is gone and the former `stdc.cpp` support unit is now `cport.cpp`.
- Active hand-written source contains no unsupported-platform branches and no
  raw compiler/OS predefined macros outside `platform.h` (generated
  parser/scanner files are the explicit exception).
- POSIX declarations such as `putenv` and `sbrk` are normalized centrally,
  rather than repaired with source-local Cygwin conditionals.

Compiler family is kept separate from Windows environment/ABI, so future
`mingw-clang` and `msvc-clang` targets do not need to be conflated.

## Other RC15 fixes retained

RC16 also retains the RC15 native-archive and GMP cleanup:

- Native `.a` operations are centralized in `doar`/`doranlib`; `uniar` remains
  for Aldor `.al` archives.
- Darwin uses capability-probed `ar ...S` and
  `ranlib -no_warning_for_no_symbols` to keep symbol-free archives quiet.
- GMP discovery includes an early compile/link gate and Homebrew-prefix
  discovery on macOS.
- Detailed top-level build context is printed only for actual build/all
  actions; clean `junk` checks remain silent.

## Validation before promotion

The portability CP2 tree passed:

- `portability-regression.sh`
- `test-harness-regression.sh` (686 classified entries)
- `build-entry-regression.sh`
- `rebuild-driver-test.sh`
- GCC and Clang compile/syntax checks for changed compiler/runtime, testaldor
  and unitools sources
- silent clean-tree `build.sh junk`

RC16 promotion is additionally validated from a fresh extraction of the final
archive.  A complete external platform matrix has not yet been run on RC16.
MinGW and MSVC variants have not been attempted.

During RC16 promotion, the top-level `junk` path was also tightened so that it
requires no `ALDORROOT`, creates no build-root directories, and is completely
silent on a clean package.
