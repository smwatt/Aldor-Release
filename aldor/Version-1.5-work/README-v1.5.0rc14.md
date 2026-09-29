# Aldor v1.5.0rc14

RC14 is the reorganized native-platform cleanliness candidate before the
serious MinGW pass.

Changes from RC13
-----------------

- Fix the macOS warning-capability probe exposed by Homebrew GCC 16 on Ice.
  Warning controls are tested exactly as they will be used, separately for C
  and C++, and a candidate option is accepted only when compilation succeeds
  with zero diagnostic output.  This prevents the C-only
  `-Wno-int-conversion` option from leaking into C++ builds.
- Retain the RC13 independent generated-C Clang warning probes, including
  Clang 14 support for `-Wno-unused-value` when the newer
  `-Wno-deprecated-non-prototype` option is unavailable.
- Reorganize historical README/RESUME/release/checkpoint/report material into
  stable archival directories.  Only the current README and RESUME are copied
  at package root.
- Consolidate current modernization validation under
  `distro/modernization/{tests,baselines,benchmark}` and remove obsolete
  checkpoint-era aggregator scripts.
- Clean the source manifests: keep and register active `uniar.cpp`,
  `sal_gmpabi.c`, and `test0.as`; remove the accidental
  `ccomp.cpp.before-syslibs` backup and obsolete `dounicl` wrapper.
- Add the current V0.19/V0.20 design notes and platform descriptions under
  package-level `notes/`.

Retained RC12/RC13 portability work includes compiler viability probing,
Darwin no-symbol archive handling, portable `float2` numerical testing, named
modernization gates, the Cygwin `sbrk` compatibility declaration, and separate
host C/C++ warning-option selection.

Expected native validation target
---------------------------------

For every usable GCC/Clang toolchain selected by `rebuild.sh`:

- zero build/toolchain warnings and errors;
- 870/870 primary tests, with zero DIFFERENT and zero ERROR;
- all six generated-runtime smokes PASS;
- all named modernization release gates PASS;
- a pristine `bash build.sh junk` reports no unexpected paths.
