# Aldor v1.5.0rc18

RC18 is the anticipatory cleanup release following the RC17 Ice runs.  It
separates the generated-C ABI header from the Aldor implementation portability
umbrella and tightens dependency/manfiest validation so portability changes are
checked across the whole build rather than one exposed failure at a time.

## Changes since RC17

- `foam_c.h` no longer includes `cport.h`, directly or through `foamopt.h`.
  Generated C therefore does not inherit the compiler/runtime's stdio/POSIX
  namespace merely by including the FOAM ABI header.
- The `fiF*` stdio word adapters are now declared in `foam_c.h` and implemented
  in `cport.cpp`; generated C sees only the Aldor ABI names.
- Obsolete `CPORT_SF_IS_DOUBLE` branches are removed.  All supported targets use
  C `float` for Aldor single precision.
- The Homebrew/explicit GMP include directory is propagated to
  `aldor/contrib/gmp/fm_gmp.c` as well as `sal_gmpabi.c`.
- Raw `unlink` use in Salli is replaced by the `salUnlink` ABI adapter; tutorial
  and guide examples are updated consistently.
- `pow` in `sal_dfloat.as` now names `<math.h>` explicitly, and the unused raw
  `EOF` Foreign-C import is removed.
- `sal_util.c` is treated as Aldor implementation source and uses `cport.h`; its
  obsolete SunOS `powf` fallback and raw compiler-environment tests are gone.
- `cport.h` supplies the canonical POSIX `mkstemp` declaration on supported
  POSIX targets when feature-test settings hide it.
- The portability regression now compiles a synthetic generated-C namespace
  probe, checks transitive `cport.h` exclusion, rejects stale single-float
  switches and raw `unlink` imports, verifies both GMP include consumers, and
  validates secondary runtime source manifests against the source tree.

## Portability boundary

The intended split is now:

    platform.h -> cconfig.h -> cport.h -> Aldor implementation source

    foam_c.h   -> minimal generated-C ABI environment

`fenvport.h` remains a focused platform service.  MinGW and MSVC identities are
still structural only and are not attempted in RC18.

## Validation before packaging

The RC18 working tree passes the host-independent portability, test-harness,
build-entry and rebuild-driver regressions.  Targeted strict C99/C++20 compile
checks pass for `sal_util.c`, `cport.cpp`, and a synthetic generated-C unit that
includes `foam_c.h` and then declares libc-like names.  External platform builds
are still required.
