# Aldor v1.5.0rc9

RC9 preserves the clean RC8 compiler/runtime state and improves the top-level
portable rebuild driver so one invocation can validate every supported native
compiler family available on a host.

## Rebuild-driver changes from RC8

1. **Automatic multi-toolchain builds.**  With `ALDOR_TOOLCHAIN` unset (or
   `auto`), `rebuild.sh` detects GCC and Clang C/C++ pairs on Unix-like hosts
   and performs a complete clean build and test with each available family.
   Cygwin intentionally defaults to GCC only.

2. **Explicit single-toolchain override.**  Setting `ALDOR_TOOLCHAIN` selects
   exactly one toolchain.  In particular, a Cygwin MinGW validation can be run
   with:

       ALDOR_TOOLCHAIN=mingw bash rebuild.sh

   `ALDOR_CC` and `ALDOR_CXX` may select particular compiler executables.
   Versioned GCC/Clang installations are detected, and Apple's `gcc` spelling
   is not mistaken for GNU GCC.

3. **Independent roots.**  Each selected toolchain gets a fresh root named:

       build-<aldor-version>-<os>-<toolchain>

   For example, RC9 on Ubuntu uses roots such as
   `build-v1.5.0rc9-ubuntu-gcc` and `build-v1.5.0rc9-ubuntu-clang`.
   The lower-level `distro/build.sh all` now accepts the selected root through
   `ALDOR_BUILD_ROOT` while preserving its historical `build-root` fallback.

4. **Common log directory.**  All top-level logs remain under `./logs`.
   Existing log naming already includes host, OS and toolchain, so GCC and
   Clang runs do not collide.

5. **Help.**  `bash rebuild.sh help`, `bash rebuild.sh --help`, and `-h` print
   usage, toolchain-selection and build-root information without starting a
   build.

6. **Failure aggregation.**  If one automatically selected toolchain fails,
   the remaining selected toolchains are still attempted.  The final summary
   lists passes and failures and the overall command returns nonzero if any
   selected build failed.

## Host-independent regression testing

`distro/modernization/rebuild-driver-test.sh` exercises the driver with mock
host/compiler commands rather than depending on the test machine's compiler
inventory.  It verifies:

- macOS with Apple `gcc` plus a versioned real GCC and Clang selects real GCC
  and Clang exactly once each;
- Cygwin defaults to GCC even when Clang is visible;
- `ALDOR_TOOLCHAIN=mingw` selects one MinGW build on Cygwin;
- toolchain-specific root names are correct;
- a failed first toolchain does not suppress the second build;
- both requested help spellings are side-effect free;
- `ALDOR_BUILD_ROOT` is propagated through reset/build/test in the lower-level
  `all` path.

The driver regression tests pass on the RC9 package source.

## Compiler/runtime validation inheritance

No compiler/runtime semantic change is introduced by RC9.  RC8's final local
GCC 14.2 and Clang 17 validations were completely clean: 870/870 primary
cases, zero differences, zero errors and zero warnings with all generated
runtime smokes and modernization gates passing.  RC9 changes the release
identity and rebuild machinery so the next cross-platform runs exercise this
same source state through the new multi-toolchain driver.
