# Aldor v1.5.0rc5

RC5 is a narrow Darwin build correction on top of v1.5.0rc4.

On macOS, the Darwin post-link block in `subcmd/unitools/unicl.cpp` is
compiled.  RC4 referred there to `ccNoExecute` and `ccBuf` before those
file-scope objects were declared.  Linux validation did not see the defect
because that block is removed by preprocessing there.  In addition, the
`ccPuts` helper used by the Darwin block had an old commented-out
implementation.

## Changes since v1.5.0rc4

1. Declare the `unicl` command-builder state before the Darwin post-link
   routine uses it.
2. Restore the `ccPuts` helper.
3. Make `ccPutq` and `ccPuts` take `String` rather than `MutString`; this is
   const-correct and removes Clang writable-string warnings.
4. Add a validation check that preprocesses/compiles the Darwin branch even
   on a non-Darwin validation host.

RC5 otherwise retains RC4 unchanged, including the complete generated C
prototypes for `Foreign` and headerless `Foreign C`, the explicit `fi*`
runtime adapters, and the rule that `Foreign C ""` is an Aldor error.

Run a clean release validation with:

    ./build.sh all
