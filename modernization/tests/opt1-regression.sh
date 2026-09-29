#!/bin/bash
# Copyright (C) 2026 Stephen M. Watt.
# See legal/LICENSE in the Aldor distribution for licensing terms.
set -euo pipefail

BUILD_ROOT=${1:?build root required}
SRCROOT=${2:?source root required}
ALDOR="$BUILD_ROOT/bin/aldor"
SRC="$SRCROOT/lib/axllib/test/opt1.as"
EMERGE="$SRCROOT/lib/axllib/test/emerge0.as"
OUT="$BUILD_ROOT/testout/opt1-regression"

mkdir -p "$OUT"

cat > "$OUT/expected.out" <<'EOT'
block 1:
list(Natascia
, Natascia
, Natascia
, Natascia
, Natascia
, Natascia
, Natascia
, Natascia
, Natascia
, Natascia
)
done
EOT

run_opt1() {
    local mode=$1
    local q=$2
    local name=$3
    local work="$OUT/$name"

    mkdir -p "$work"
    (
        cd "$work"
        "$ALDOR" -R "$work" -M "base=$SRCROOT/lib/axllib/test/" \
            "$mode" "$q" -l axllib "$SRC"
    ) >"$work.out" 2>"$work.err"

    cmp -s "$OUT/expected.out" "$work.out" || {
        echo "opt1-regression: $name output mismatch" >&2
        diff -u "$OUT/expected.out" "$work.out" >&2 || true
        cat "$work.err" >&2
        exit 1
    }
}

run_opt1 -ginterp -q1 ginterp-q1
run_opt1 -ginterp -q2 ginterp-q2
run_opt1 -ginterp -q3 ginterp-q3
run_opt1 -grun    -q2 native-q2
run_opt1 -grun    -q3 native-q3

# Preserve the historical emerge alias regression as a guard against
# weakening the optimization while fixing stable mutable aliases.
"$ALDOR" -R "$OUT" -M "base=$SRCROOT/lib/axllib/test/" \
    -ginterp -q3 -l axllib "$EMERGE" >"$OUT/emerge0.out" \
    2>"$OUT/emerge0.err"

grep -F 'result = 3.1415926535878294' "$OUT/emerge0.out" >/dev/null || {
    echo 'opt1-regression: emerge0 result mismatch' >&2
    cat "$OUT/emerge0.out" >&2
    cat "$OUT/emerge0.err" >&2
    exit 1
}
grep -F 'after 33 function evaluations.' "$OUT/emerge0.out" >/dev/null || {
    echo 'opt1-regression: emerge0 evaluation-count mismatch' >&2
    cat "$OUT/emerge0.out" >&2
    exit 1
}

echo 'opt1-regression: PASS'
