# Aldor v1.5.0rc3

This candidate corrects the Foreign-C adapter design in v1.5.0rc2 while
retaining the independent Darwin `unicl` repair and build-provenance changes.

## Changes since v1.5.0rc2

1. `Foreign C` identifiers are never rewritten by spelling.  In particular,
   a user's `fputc`, `fputs`, `fopen`, etc. continue to denote exactly the C
   identifiers declared by the user's Aldor signature/header contract.
2. The runtime/basic layer explicitly imports and calls `fiFputc` and
   `fiFputs` from `Foreign` where the FOAM word-sized ABI adapters are wanted.
3. The simple `fiF*` stdio adapters are `static inline` functions in
   `foam_c.h`, not macros.  Their out-of-line duplicates were removed from
   `output.cpp`.
4. `Foreign C;` remains the headerless raw-C form.
5. `Foreign C "filename";` remains the named-header form; missing headers and
   incompatible C declarations are diagnosed by the subordinate C compiler.
6. `Foreign C "";` is now a compiler error.  Historical shipped-library uses
   of that spelling were changed to `Foreign C;`.

The v1.5.0rc2 Darwin `unicl` post-link repair and log naming/version reporting
remain unchanged.

Run a release validation from a clean package with:

    ./build.sh all
