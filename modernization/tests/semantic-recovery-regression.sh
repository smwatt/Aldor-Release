#!/bin/bash
# Copyright (C) 2026 Stephen M. Watt.
# See legal/LICENSE in the Aldor distribution for licensing terms.
set -euo pipefail

ROOT=${1:?usage: semantic-recovery-regression.sh ALDORROOT [source-root]}
SRCROOT=${2:-$(cd "$(dirname "$0")/../.." && pwd -P)}
OUT="$ROOT/modernization-validation/semantic-recovery"
rm -rf "$OUT"
mkdir -p "$OUT"

export ALDORROOT="$ROOT"
export ALDORTMP="$OUT/tmp"
export INCPATH="$ROOT/include"
export LIBPATH="$ROOT/lib"
export PATH="$ROOT/bin:$ROOT/toolbin:$PATH"
mkdir -p "$ALDORTMP"

# First preserve the full historical recovery sequence.  It exercises failed
# redeclarations, failed definitions, successful definitions after failures,
# and restoration of a previously valid meaning after a rejected redefinition.
(
    cd "$SRCROOT/lib/axllib/test"
    bash ./recov1.sh
) >"$OUT/recov1.out" 2>"$OUT/recov1.err"

if grep -q 'Program fault' "$OUT/recov1.out" "$OUT/recov1.err"; then
    echo 'semantic-recovery-regression: recov1 faulted' >&2
    exit 1
fi
grep -q '^3 @ SingleFloat$' "$OUT/recov1.out"
grep -q '^10 @ Integer$' "$OUT/recov1.out"

# Stress repeated semantic aborts.  Each rejected SingleFloat definition may
# create semantic objects and caches, but it must not disturb the committed
# Integer binding.  A fresh function must still compile and run afterwards.
{
    cat <<'EOT'
#int timing off
#include "axllib.as"
I ==> Integer
F ==> SingleFloat
anchor : I := 7
EOT
    for i in $(seq 1 40); do
        printf 'bad%s : F == false\n' "$i"
        printf 'anchor\n'
    done
    cat <<'EOT'
survived(x:I):I == x + anchor
survived 5
#quit
EOT
} >"$OUT/stress.input"

"$ROOT/bin/aldor" -Gloop -Mno-release \
    <"$OUT/stress.input" >"$OUT/stress.out" 2>"$OUT/stress.err"

if grep -q 'Program fault' "$OUT/stress.out" "$OUT/stress.err"; then
    echo 'semantic-recovery-regression: stress session faulted' >&2
    exit 1
fi

anchor_count=$(grep -c '^7 @ Integer$' "$OUT/stress.out" || true)
if [ "$anchor_count" -ne 41 ]; then
    echo "semantic-recovery-regression: expected 41 anchor results, got $anchor_count" >&2
    exit 1
fi
grep -q '^12 @ Integer$' "$OUT/stress.out"

# A failed first extension replaces committed meanings in the Stab entry by a
# new extension Syme while semantic analysis is in progress.  Rollback must
# restore the committed extendees, retire the failed semantic objects, and
# allow a subsequent valid extension of the same domain.
cat >"$OUT/extend.input" <<'EOT'
#int timing off
#include "axllib.as"
SI ==> SingleInteger
define Foo: Category == with { foo: % -> SI }
extend SI: Foo == add { foo(x:%):SI == false }
extend SI: Foo == add { foo(x:%):SI == x }
import from SI
foo 3
#quit
EOT

"$ROOT/bin/aldor" -Gloop -Mno-release \
    <"$OUT/extend.input" >"$OUT/extend.out" 2>"$OUT/extend.err"

if grep -q 'Program fault' "$OUT/extend.out" "$OUT/extend.err"; then
    echo 'semantic-recovery-regression: failed extension recovery faulted' >&2
    exit 1
fi
grep -q 'context requires an expression of type SingleInteger' "$OUT/extend.out"
grep -q '^3 @ SingleInteger$' "$OUT/extend.out"

# A later failed extension must not corrupt the previously committed
# extension or crash the interactive compiler.  Specific precedence and
# visibility of still later sequential extensions is deferred to 1.6.0.
cat >"$OUT/extend-existing.input" <<'EOT'
#int timing off
#include "axllib.as"
SI ==> SingleInteger
define Foo: Category == with { foo: % -> SI }
extend SI: Foo == add { foo(x:%):SI == x }
import from SI
foo 3
four: SI == 4
extend SI: Foo == add { foo(x:%):SI == false }
import from SI
foo 3
extend SI: Foo == add { foo(x:%):SI == four }
import from SI
foo 3
#quit
EOT

"$ROOT/bin/aldor" -Gloop -Mno-release \
    <"$OUT/extend-existing.input" >"$OUT/extend-existing.out" \
    2>"$OUT/extend-existing.err"

if grep -q 'Program fault\|Unhandled Exception\|Export not found' \
    "$OUT/extend-existing.out" "$OUT/extend-existing.err"; then
    echo 'semantic-recovery-regression: failed later extension recovery faulted' >&2
    exit 1
fi
grep -q 'context requires an expression of type SingleInteger' \
    "$OUT/extend-existing.out"
# Aldor 1.5.1 guarantees interactive robustness, not a particular
# winner/visibility rule for multiple sequential extensions in -Gloop.
# In particular the final valid extension may select the earlier foo (3),
# the newer foo (4), or fail lookup; the 1.6.0 definitional/execution
# context design must determine that behavior.  Keep the first two
# committed 3 results as the rollback/recovery guarantee, and continue
# rejecting fatal runtime failures and missing expected type diagnostics.
# Do not silently assert 3 to be the intended precedence.
existing_count=$(grep -c '^3 @ SingleInteger$' "$OUT/extend-existing.out" || true)
if [ "$existing_count" -lt 2 ]; then
    echo "semantic-recovery-regression: earlier valid meaning lost ($existing_count/2)" >&2
    exit 1
fi
# NOTE: no assertion on the third result: deferred to Aldor 1.6.0.

printf 'semantic-recovery-regression: PASS\n'
