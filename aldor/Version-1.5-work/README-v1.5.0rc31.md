# Aldor v1.5.0rc31

rc31 follows the rc30 varargs-ABI candidate.  The first Grice rc30 runs showed
that both GNU GCC and Apple Clang stopped while compiling the CCode self-test:
the new typed CCode constructor correctly rejected historical raw `NULL`
arguments in `ccode_t.c`, and that test still contained one direct call to the
removed raw-varargs `ccoNew` constructor.

Principal changes relative to rc30:

- migrate every CCode self-test null argument to typed `CCode(nullptr)`;
- replace the test's last direct `ccoNew` use with `ccoNewNodeT`;
- make default-constructed `CCode` explicitly contain a null representation;
- correct `vxprintf` so `%u`, `%o`, `%x`, and `%X` retrieve unsigned arguments
  with `va_arg` rather than signed arguments;
- add `modernization/tests/varargs-abi-audit.py`, which inventories every raw
  `va_arg` extraction in `aldor/src` and fails when that reviewed inventory
  changes or when selected forbidden historical patterns return.

The rc30 build-log preservation change remains in place, so an early toolchain
failure still leaves a per-toolchain log under the package/workspace `logs/`
directory and under the corresponding build root's `evidence/` directory.
