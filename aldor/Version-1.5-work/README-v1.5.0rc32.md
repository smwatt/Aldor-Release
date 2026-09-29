# Aldor v1.5.0rc32

Portability release candidate following rc31.

Fixes a generated-C corruption on GCC/AArch64 caused by a variadic format
mismatch in `ccoIntOf`: `%ld` requires a `long`, while the macro commonly
received `int`.  The argument is now explicitly converted to `long`.
