# Aldor v1.5.0rc6

RC6 corrects the regressions exposed by the first full RC5 run on grice while
retaining the RC4 generated-C prototype work and the RC3 Foreign-C semantics.

## Changes since v1.5.0rc5

1. The FOAM interpreter now recognizes the explicit runtime adapter names
   (`fiFputc`, `fiFputs`, `fiFopen`, `fiFclose`, `fiFflush`, `fiFgetc`,
   `fiFgets`, `fiFseek`, `fiFtell`, `fiPowf`, and `fiSystemVoid`).  These are
   interpreter implementation aliases/operations only; they do not alter
   `Foreign C` name resolution or generated C spelling.
2. The declaration buffer in `ccode.cpp` is restored to preserve the historic
   stable formatting of generated C, but all spelling-based libc redirection
   has been removed.  It only buffers and emits the declaration unchanged.
3. Link-file options are rebased correctly after `ccLinkProgram` changes into
   the output directory.  In particular, the requested `-o` file is expressed
   relative to the directory in which the linker is actually run.
4. A successful linker return is now accepted only if the requested executable
   exists.  `emitLink` also guarantees that object cleanup cannot remove the
   executable and verifies that it survives cleanup.  `ccGoProgram` checks the
   same invariant immediately before execution.
5. The RC4 complete prototypes remain unchanged: plain `Foreign` and
   headerless `Foreign C` carry their known signatures into generated C;
   zero-argument functions use `(void)`; `Foreign C "file.h"` relies on the
   named header; and `Foreign C ""` is an Aldor error.

Run a clean release validation with:

    ./build.sh all

The first external validation target should be grice, because its RC5 log
exercised both the interpreter regression and the macOS native `-grun` path.
