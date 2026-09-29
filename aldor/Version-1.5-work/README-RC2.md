# Aldor v0.18rc2

This release candidate supersedes and withdraws v0.18rc1.

RC1 exposed a fresh-build regression in parameterized `extend` declarations
(`extend Tuple(T: Type)` in `libfoamlib/tuple.as`).  RC2 corrects that ordering
bug and completes the semantic recovery/retirement model discovered through
`recov1` and repeated-extension reducers.

## Parameterized extension repair

RC1 inspected/synthesized `abTForm(type)` before `scobindDeclareId`.  For a
parameterized extension this happens before the declaration establishes the
parameter scope, so a synthetic TForm for `Tuple(T)` contains an unbound `T`.

RC2 restores the required ordering:

1. declare/bind the extension identifier and parameters;
2. then read the TForm attached to the type;
3. only in the interactive loop, if it is still absent, use the synthetic
   `tfSyntaxFrAbSyn` fallback needed by recovery.

A genuinely empty-root RC2 build passes the former RC1 failure point and builds
all of `libfoamlib`, including `tuple.as`.

## Semantic recovery and retirement

Aborted interactive semantic objects are now separated into two notions:
logical lifetime and physical lifetime.

- rollback immediately removes aborted meanings from active Stab state;
- aborted Symes are marked RETIRED and remain allocated until semantic
  shutdown so older non-owning TForm/import cache pointers remain valid;
- retired meanings and wrappers whose originals are retired are ignored by
  active lookup/insertion and lazily purged from StabEntry caches;
- aborted TForms are likewise retained to semantic shutdown;
- Stab rollback visits the file/global ancestor chain exactly once and each
  descendant tree exactly once.

This prevents a stale cache from reactivating an aborted Syme while avoiding
mid-session dangling semantic pointers.

## Repeated extension semantics

A later `extend` of an already committed extension creates a fresh extension
layer.  The committed layer becomes its extendee.  An older extension whose
`symeExtension(old)` points at its successor is shadowed and cannot be
reinstalled as an active top-level meaning by later import/cache processing.

The permanent semantic-recovery gate checks the observable sequence:

    first valid extension  -> foo 3 = 3
    rejected extension     -> diagnostic
    restored extension     -> foo 3 = 3
    later valid extension  -> foo 3 = 4

It also runs historical `recov1`, a 40-abort stress sequence, and failed-first-
extension recovery.

## Local Trixie clean validation

A completely empty product root was rebuilt from the RC2 source and the same
root was used for the integrated tests.

    compiler self-tests       29 / 29
    AxlLib                    761 / 761
    libaldor                    8 / 8
    Algebra std                24 / 24
    Algebra debug              24 / 24
    Algebra GMP                24 / 24
    ----------------------------------
    primary total             870 / 870

There are 0 DIFFERENT, 0 ERROR and 0 unparsed results.  Build diagnostics are
0 warnings / 0 errors.

All modernization core gates PASS, including semantic-recovery, and the
48-case Algebra std/GMP validation matrix is current-correct.

## Relative timing versus v0.01

The deterministic `sm_dmp0` comparison is active again.  The original v0.01
compiler was rebuilt from the supplied package.  Because v0.01 predates both
the corrected export hash and the later external `.ao` envelope, its support
side uses a modern C99-clean bridge built from the same RC2 high-level library
sources, retaining the historical export hash and writing the historical raw
`.ao` payload.  The bridge-only delta is retained under
`distro/modernization/benchmark/v0.01-support-bridge.patch`.

Both variants produce byte-identical output SHA-256:

    e39d0470a682e8b05192d5e7cccb8178ee6142bb867ec11c72cb1831e87628aa

The release measurement used three warmups and 21 paired execution runs on an
otherwise idle host:

    CPU median paired ratio current/v0.01: 1.006909  (+0.691%)
    wall median paired ratio:              1.006807  (+0.681%)

Nine paired compiler+backend measurements give:

    compile CPU median paired ratio:       0.471111 (-52.889%)
    compile wall median paired ratio:      0.472477 (-52.752%)

Two independent seven-pair idle execution runs gave -0.059% and +0.974% CPU,
so the practical conclusion is that RC2 generated-code execution is essentially
unchanged from v0.01 on this host, while compilation is substantially faster.
Earlier measurements taken while another build was consuming CPU are excluded
from the release record.

## Build

From the package top level:

    bash build.sh all

For a fresh acceptance run, start from an empty product root.  Release-candidate
logs should use the package/version form, for example:

    aldor-v0.18-rc2-<host>-YYYY-MM-DD--HH-MM.log
