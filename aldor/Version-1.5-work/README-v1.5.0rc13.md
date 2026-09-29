# Aldor v1.5.0rc13

RC13 is the consolidated all-platform cleanliness candidate following the
RC10 results from RISC-V and macOS and the RC11/RC12 portability fixes.

Changes from RC12:

- Generated-C Clang warning controls are now probed independently.  In
  particular, Clang 14 can enable `-Wno-unused-value` even though it predates
  `-Wno-deprecated-non-prototype`.  This removes the RISC-V Clang 14 warnings
  in generated `sfloat.c`, `dfloat.c`, and `rawrec5.c` without suppressing
  host C/C++ diagnostics.
- The portability regression now checks that the two generated-C warning
  probes remain independent and reports PASS only after all RC12/RC13 checks
  have completed.

RC12 fixes retained in full:

- native compiler viability probing with ordinary C headers and the Darwin
  Mach C++ probe;
- capability-based Darwin `ranlib -no_warning_for_no_symbols` handling;
- tolerance/property-based AxlLib `float2` testing;
- named `build-diagnostics` and auxiliary modernization gates;
- Cygwin-only `sbrk` compatibility declaration;
- separate host C/C++ warning-option probing;
- compiler-family/release/target validation and per-toolchain build roots.

Expected validation target for the pre-MinGW pass is zero build diagnostics,
870/870 primary tests, all six generated-runtime smokes PASS, and all named
modernization gates PASS on every supported native platform/toolchain that
passes the viability probe.
