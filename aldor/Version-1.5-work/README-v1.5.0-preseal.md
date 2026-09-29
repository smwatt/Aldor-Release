Aldor v1.5.0-preseal
====================

This records the preseal promotion point of the Aldor v1.5.0 portability
release.

The compiler, runtime and libraries are unchanged from v1.5.0-alpha3.  Alpha3
was validated with complete clean build/test runs under GCC and Clang on
x86-64, ARM64 and RISC-V systems across Linux, macOS and Cygwin.  Each reported
870/870 primary tests, zero warnings, zero errors, and passing modernization,
portability and Algebra validation gates.

The generated-code timing comparison against the v0.01 baseline found
essentially unchanged execution performance (about +0.7% paired CPU time and
flat paired wall time) while benchmark compilation was approximately twice as
fast.

Release-mechanics changes made at this checkpoint are limited to:

- canonical version identity `1.5.0` with an empty suffix;
- preseal checkpoint/tag naming `v1.5.0-preseal`, leaving `v1.5.0` free for the
  actual release seal;
- build/test roots under
  `build/aldor-<version>-<os>-<arch>-<toolchain>/`;
- separate `build.log` and `test.log` inside each configuration root;
- self-contained configuration headers in both logs; and
- elapsed-time/status footers written by the build scripts at the end of both
  logs.

No compiler, runtime, generated-C, Aldor-library or Algebra-library semantics
were changed in the promotion from alpha3 to this preseal checkpoint.
