#!/usr/bin/env bash
# Copyright (C) 2026 Stephen M. Watt.
# See legal/LICENSE in the Aldor distribution for licensing terms.
set -euo pipefail

ROOT=${1:?usage: bigint64-regression.sh ALDORROOT [source-root]}
SRCROOT=${2:-$(cd "$(dirname "$0")/../.." && pwd -P)}
WORK="$ROOT/testout/bigint64-regression"

export ALDORROOT="$ROOT"
export PATH="$ROOT/bin:$ROOT/toolbin:$PATH"

rm -rf "$WORK"
mkdir -p "$WORK"
cd "$WORK"

report_failure() {
    rc=$?
    [ "$rc" -eq 0 ] && return 0
    echo "bigint64-regression: FAIL rc=$rc" >&2
    echo "retained diagnostic directory: $WORK" >&2
    for f in compile.out compile.err run.out run.err expected.out diff.out; do
        [ -f "$f" ] || continue
        echo "--- $f ---" >&2
        cat "$f" >&2 || true
    done
    return "$rc"
}
trap report_failure EXIT

"$ROOT/bin/aldor" -Fx -q1 -y"$ROOT/lib" -laldor \
    "$SRCROOT/modernization/tests/bigint64-regression.as" \
    >compile.out 2>compile.err

if [ -s compile.err ]; then
    echo "bigint64-regression: compiler/linker warnings retained in $WORK/compile.err"
fi

./bigint64-regression >run.out 2>run.err
if [ -s run.err ]; then
    echo "bigint64-regression: executable wrote stderr; retained in $WORK/run.err"
fi

cat >expected.out <<'EOF_EXPECTED'
274877906951 eq=T rem=1
274877906951 eq=T rem=1
274877906952 eq=T rem=1
274877906952 eq=T rem=1
274878003719 eq=T rem=1
274878003719 eq=T rem=1
EOF_EXPECTED

diff -u expected.out run.out >diff.out || exit 1
printf 'PASS: 64-bit BInt normalization/remainder regression\n'
