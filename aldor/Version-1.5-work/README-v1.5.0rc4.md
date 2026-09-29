# Aldor v1.5.0rc4

This candidate fixes the generated-C prototype defect exposed by GCC 15 in
v1.5.0rc3.  RC3 correctly separated user `Foreign C` imports from Aldor's
FOAM runtime adapters, but plain `Foreign` functions were still emitted with
historical declarations such as:

    extern FiWord fiFputc();

That spelling did not describe the known Aldor signature and is no longer a
safe compatibility device.  In C23 an empty parameter list means no
parameters; in earlier C it was an old-style declaration with unspecified
parameters.

## Changes since v1.5.0rc3

1. Both plain `Foreign` and headerless `Foreign C` map imports retain a CSig
   format derived from the Aldor signature.
2. The C backend emits complete prototypes from that format.  For example,
   `(W, P) -> W` generates two parameter types rather than `f()`.
3. A genuinely zero-argument foreign function is declared as `f(void)`.
4. Dead-variable elimination and inlining preserve/remap the signature format
   for plain `Foreign`, just as they already did for `Foreign C`.
5. Export-to-C wrappers no longer receive a preceding historical `extern
   f()` declaration; the wrapper definition itself supplies its full
   parameter list.
6. Shipped sources that require the FOAM word-sized libc boundary explicitly
   call `fiFopen`, `fiFputc`, `fiFputs`, `fiFgetc`, `fiFseek`, `fiFtell`,
   `fiPowf`, `fiSystemVoid`, and related adapters rather than depending on
   compiler-side spelling rewrites.
7. The debugger runtime boundary now accepts `FiWord` event values at the
   generated-code ABI and converts to `FiDbgTag` internally.
8. Regression coverage generates both `Foreign` and `Foreign C` declarations
   with zero and nonzero arities and checks the resulting C prototypes.

The intended Foreign-C rules remain:

- `Foreign C;` is a raw, headerless C import.
- `Foreign C "filename";` uses that named header; the subordinate C compiler
  diagnoses a missing header or an incompatible C interface.
- `Foreign C "";` is an Aldor compile-time error.

RC4 retains the Darwin `unicl` and build-log provenance changes from RC2/RC3.

Run a clean release validation with:

    ./build.sh all
