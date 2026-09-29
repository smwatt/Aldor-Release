# Aldor v1.5.0rc19

RC19 corrects the FOAM/cport ownership mistake in RC18 and strengthens the
release gates around that architectural boundary.  It also fixes a separate
release-identity error found by the anticipatory audit.

## Changes since RC18

- `cport.h` and `cport.cpp` are again completely independent of FOAM.  They
  contain no `fi*`/`Fi*` names and do not include `foam_c.h`.
- The generated-C stdio word-adapter implementations now live in
  `foam_c.cpp`; only their ABI declarations remain in `foam_c.h`.
- The FOAM-specific `FiWord_UByte` helper moved out of `cport.h` and is local
  to `foam_c.cpp`.
- Stream flushing has one FOAM adapter, `fiFflush`, and it returns `void`.
  `sal_tstream.as`, `sal_bstream.as`, and `sal_file.as` import it as
  `fiFflush: Pointer -> ()`.  The interpreter has a dedicated void dispatch
  entry for the adapter; native C `fflush` remains the ordinary int-returning
  Foreign C function.
- The portability gate rejects any alternate FOAM flush adapter name and
  checks that `fiFflush` has the void ABI in both declaration and
  implementation.
- The portability gate also rejects FOAM names or `foam_c.h` dependencies in
  the portability layer (`platform.h`, `cconfig.h`, `cport.*`, and
  `fenvport.h`), checks the complete generated-C header include boundary, and
  checks all retained stdio adapter declarations/implementations.
- The release identity is corrected: RC18 accidentally had
  `ALDOR_VERSION_STRING "1.5.0rc18"` but `ALDOR_VERSION_SUFFIX "rc17"`.
  RC19 sets both consistently and adds an automatic consistency check.

## `fiFflush` ABI

The canonical FOAM stream-flush adapter is:

    void fiFflush(FiWord stream)

Aldor imports use the corresponding signature:

    fiFflush: Pointer -> ();

The implementation deliberately discards the C `fflush` status.  There is no
second FOAM flush adapter.  Native Foreign C code may still import C `fflush`
with its native integer return type when that status is required.

## Portability boundary

The intended dependency direction is:

    platform.h -> cconfig.h -> cport.h -> Aldor implementation source

    foam_c.h   -> minimal generated-C ABI environment

    foam_c.cpp -> Aldor implementation support (including cport transitively)

Thus FOAM may depend on the portability layer in its implementation, but the
portability layer never depends on FOAM, and generated C does not inherit the
broad implementation namespace.

## Validation before packaging

The host-independent portability, test-harness, and build-entry regressions pass
after the correction.  `cport.cpp` and `foam_c.cpp` compile as
C++20 with both GCC and Clang in the FOAM runtime configuration, and the
resulting `foam_c` object exports exactly the nine retained stdio adapters.

A clean Debian/GCC build completed through the compiler, runtimes, Salli,
AxlLib, Ax0, AxlDem, DebugLib, and release/debug/GMP Algebra.  The primary
suite reported 870 OK, 0 DIFFERENT, and 0 ERROR before the final flush-name
adjustment; the focused RC19 gates are rerun for the packaged source.

External platform validation is still required.  The next matrix is Linux
GCC/Clang, macOS GCC/Clang, and Cygwin GCC/Clang.  Do not attempt MinGW or MSVC
yet.
