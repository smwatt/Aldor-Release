# Aldor v0.18rc1

This is the first release candidate for Aldor v0.18.

It is cumulative on CP13 and adds the final semantic-recovery/incremental-
extension repair discovered while hardening `recov1` recovery.

## Release-candidate changes

### Semantic rollback and retirement

CP13 already separated logical rollback from physical reclamation: semantic
objects created by an aborted interactive step are removed from live lookup
structures immediately but retired until semantic shutdown, because surviving
TForms and caches can contain non-owning pointers to them.

RC1 keeps that model and extends rollback to extension mutations:

- failed extension layers are removed from Stab entries;
- previously committed extendees are restored;
- failed extendees attached to a surviving extension are detached;
- aborted Symes and TForms are retired rather than reclaimed mid-session;
- StabEntry possibility caches are invalidated when their Syme lists change.

### Repeated `extend`

A second extension of a domain/category exposed a separate pre-existing bug.
`scobindGetExtend` reused an already-completed extension TForm, causing type
inference to append another extendee to a completed join.

RC1 reuses an extension only while it is still being assembled.  Once an
extension is committed, a later `extend` forms a fresh extension layer whose
committed extension becomes an extendee.

The permanent recovery gate now checks:

1. the historical `recov1` sequence;
2. 40 consecutive rejected semantic statements followed by valid code;
3. a failed first extension followed by a valid extension;
4. valid extension -> failed later extension -> restoration of the committed
   extension -> subsequent valid extension.

The last sequence verifies observable values 3, 3, 4 across those stages.

## Local Trixie validation

Using the final RC1 compiler:

- semantic-recovery regression: PASS;
- compiler self-tests: 29/29;
- ordinary AxlLib `extend0` and `extend1`: PASS;
- modernization release gate: PASS;
- Algebra validation matrix: 48/48 std/GMP current-correct;
- build diagnostic gate: 0 warnings / 0 errors.

CP13's fresh empty-root acceptance immediately before these narrow RC1 changes
was 870/870 primary tests OK (761/761 AxlLib).  RC1 should therefore be rebuilt
from an empty root on the platform matrix before final v0.18 sealing.

## Relative timing

The v0.01 generated-code timing comparison is active again for RC1.  Because
the corrected export-hash / `.ao` ABI boundary introduced after v0.15 prevents
the original v0.01 compiler from consuming the current support root directly,
the benchmark requires an ABI-matched historical-hash support root, as in the
v0.16/v0.17 sealing runs.

Configure either the traditional variables

    ALDOR_BENCH_BASELINE_COMPILER
    ALDOR_BENCH_BASELINE_SUPPORT_ROOT

or their RC shorthand aliases

    ALDOR_V001_COMPILER
    ALDOR_V001_SUPPORT_ROOT

before `build.sh all`.  The current support root defaults to the freshly built
RC1 root.  RC1 no longer silently substitutes that corrected-hash root for the
v0.01 side.

The supplied v0.01 source package was rebuilt locally far enough to reproduce
its compiler (`Aldor version 1.2.-10(69)`).  A direct benchmark against the RC1
support root correctly fails with a library-format/ABI mismatch; this is why an
old-hash support root is required.

## Build

From the package top level:

    bash build.sh all

For the paired timing gate, configure the two v0.01 paths above first.
