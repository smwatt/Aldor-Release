# Aldor modernization validation

This directory contains the current release-validation machinery shipped
with the public Aldor distribution. Historical development, checkpoint,
triage and audit material is maintained outside the standalone distribution.

Top-level files
---------------

- `validate.sh` -- master post-build modernization/release gate.
- `make-test-report.sh` -- consolidated primary-test and gate report.
- `classify-results.sh` -- classification helper for test results.
- `runtime-smoke.sh` -- generated-runtime smoke tests for AxlLib, Salli and
  Algebra release/debug/GMP configurations.
- `algebra-matrix.sh` -- Algebra release/debug/GMP validation matrix.
- `generated-c-compare.sh` -- optional compiler-to-compiler generated-C
  comparison.

Subdirectories
--------------

- `baselines/` -- current baseline/reference data consumed by validation.
- `benchmark/` -- generated-code performance benchmark and historical-support
  bridge used for relative timing.
- `tests/` -- focused regression gates and their source fixtures.

The intended release condition is a warning/error-clean build, 870/870 primary
tests, all generated-runtime smokes passing, and every named modernization
release gate passing.
