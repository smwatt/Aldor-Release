# Aldor v1.5.0rc15

RC15 is the cleaned native-platform candidate following the first RC14 Ice
run.  It is intended to restore an absolutely clean native build log before
the serious MinGW pass.

Changes from RC14
-----------------

- Centralize native `.a` archive operations behind `doar` and `doranlib`.
  `uniar` remains exclusively the Aldor `.al` archive tool.
- On Darwin, capability-probe and use `ar ...S` so archive creation/update does
  not implicitly build a symbol table and emit "has no symbols" diagnostics.
  The index is then built explicitly with capability-probed
  `ranlib -no_warning_for_no_symbols`.
- Add `c` automatically to native `ar` replacement/append operations when it
  is absent, suppressing the otherwise harmless "creating archive" notice.
- Convert all active shell build/test paths to the native archive wrappers;
  direct `ar`/`ranlib` invocations are now treated as a regression.
- Discover GMP explicitly.  `ALDOR_GMP_PREFIX` is the explicit override;
  otherwise pkg-config is used when available, followed on macOS by
  `brew --prefix gmp`, and finally the ordinary system search path.
- Add an early GMP compile-and-link gate before the expensive compiler build.
  A missing `gmp.h` or `libgmp` therefore produces one concise diagnostic
  immediately rather than failing later while compiling `sal_gmpabi.c`.
- Propagate a discovered non-default GMP include directory to `sal_gmpabi.c`
  and a non-default GMP library directory to generated-C links using GMP.
- Print the detailed top-level rebuild/package context only for actual
  `build`/`all` actions.  Housekeeping commands such as `junk`, `reset`, and
  `walk` remain terse.

Retained RC14 work includes the GCC-16 exact warning-option probes, the
Clang-14 generated-C compatibility probe, compiler viability checking, the
portable `float2` numerical test, the reorganized README/RESUME/checkpoint
layout, and the cleaned OkFiles manifests.

Expected native validation target
---------------------------------

For every usable GCC/Clang toolchain selected by `rebuild.sh`:

- zero build/toolchain/archive warnings and errors;
- 870/870 primary tests, with zero DIFFERENT and zero ERROR;
- all six generated-runtime smokes PASS;
- all named modernization release gates PASS;
- a pristine `bash build.sh junk` reports no unexpected paths.
