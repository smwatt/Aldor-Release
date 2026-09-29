# Aldor v1.5.0rc2

This candidate is a portability repair of v1.5.0rc1 after clean rebuilds on six
platforms exposed two release-blocking issues.

## Changes since v1.5.0rc1

1. Generated Foreign-C declarations redirected to runtime adapters no longer
   duplicate the adapter declaration with an obsolete empty-parameter-list
   declaration.  `foam_c.h` is the single source of the adapter prototypes.
2. The C++ `unicl` driver now performs Darwin post-link executable preparation
   (mode, quarantine removal, ad-hoc signing), restoring `-grun` execution on
   macOS for both x86-64 and arm64.
3. Build-log filenames include lowercase host, OS, and toolchain names, and log
   headers record exact OS and compiler versions.

Run a release validation from a clean package with:

    ./build.sh all

A successful build should finish all compiler, runtime-smoke, library, and
modernization release-gate tests.
