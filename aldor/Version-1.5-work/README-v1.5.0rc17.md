# Aldor v1.5.0rc17

RC17 is the cleanup release following the full portability consolidation in
RC16.  It includes the RC16 Ice portability patch and completes the
`stdc.cpp` -> `cport.cpp` transition in the secondary runtime builds.

## Corrections since RC16

- macOS now trusts the SDK's native `sbrk(int)` declaration; the portability
  layer supplies `sbrk(intptr_t)` only on environments that need it.
- Warning-option capability probing now handles GCC's special treatment of
  unknown negative `-Wno-*` options, so Clang-only options are not passed to
  GCC merely because an otherwise-clean probe stayed silent.
- `aldor/lib/libfoam/build.sh` now builds `cport.cpp`, not the removed
  `stdc.cpp`.
- `aldor/contrib/gmp/build.sh` likewise uses the `cport` runtime-source stem.
- The five historical K&R definitions in `subcmd/testaldor/tx_windows.c0`
  (`osRun`, `osRunOutput`, `osRunScript`, `osMakeDir`, `osRemoveDir`) now use
  ordinary prototypes.  This removes the Cygwin-Clang
  `-Wdeprecated-non-prototype` warnings without suppressing diagnostics.
- The portability regression now rejects stale secondary-runtime references
  to `stdc.cpp`/`stdc` and rejects reintroduction of those five K&R
  definitions.

## Portability model retained

The supported/tested model remains:

- Linux: GCC or Clang on x86-64, AArch64 and RISC-V64.
- macOS: GCC or Clang on x86-64 and AArch64.
- Windows/Cygwin: GCC or Clang on x86-64.

Future identities remain reserved but are deliberately not attempted yet:
`mingw-gcc`, `mingw-clang`, and `msvc` (with `msvc-clang` reserved for a later
Clang/MSVC-runtime target if desired).

The portability structure remains:

    platform.h -> cconfig.h -> cport.h -> ordinary source

`fenvport.h` is the focused floating-point platform service.  There are no
`.h0` wrappers and no `stdc.h`/`stdc.cpp` portability layer.

## Evidence leading to RC17

After the RC16 Ice patch, GCC and Clang on Ice, GCC and Clang on Ubuntu/AArch64,
and GCC and Clang on Cygwin all compiled the main Aldor compiler successfully
before reaching the same stale `stdc.cpp` reference in the secondary libfoam
build.  Cygwin Clang additionally exposed the three active K&R definitions in
`tx_windows.c0`; the audit found and modernized all five definitions in that
file.

The earlier Asteroid RC15 GCC/Clang runs completed the full 870-test matrix
cleanly and remain useful controls for the pre-RC16 compiler/runtime state.

## Validation before packaging

The RC17 working tree passes:

- `portability-regression.sh`
- `test-harness-regression.sh` (686 classified entries)
- `build-entry-regression.sh`
- `rebuild-driver-test.sh`
- shell syntax checks for the changed build scripts/regressions

The finished RC17 archive is also re-extracted and checked for release identity,
root README/RESUME discipline, silent clean-tree `build.sh junk`, and the same
host-independent release gates.  External RC17 platform builds are still
required; MinGW and MSVC variants are not to be attempted yet.
