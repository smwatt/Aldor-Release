#!/bin/bash
# Copyright (C) 2026 Stephen M. Watt.
# See legal/LICENSE in the Aldor distribution for licensing terms.
set -euo pipefail

BUILD_ROOT=${1:?build root required}
SRCROOT=${2:?source root required}
ALDOR="$BUILD_ROOT/bin/aldor"
SRC="$SRCROOT/lib/axllib/test/t1145.as"
OUT="$BUILD_ROOT/testout/t1145-regression"
EXPECT='q = 1099511627776'

mkdir -p "$OUT"

run_case() {
    local mode=$1
    local q=$2
    local name=$3
    local out="$OUT/$name.out"
    local err="$OUT/$name.err"

    "$ALDOR" -R "$OUT" -M "base=$SRCROOT/lib/axllib/test/" \
        "$mode" "$q" -l axllib "$SRC" >"$out" 2>"$err" || {
        cat "$out"
        cat "$err" >&2
        exit 1
    }
    grep -Fx "$EXPECT" "$out" >/dev/null || {
        cat "$out" >&2
        exit 1
    }
}

run_case -ginterp -q0 ginterp-q0
run_case -ginterp -q1 ginterp-q1
run_case -ginterp -q2 ginterp-q2
run_case -ginterp -q3 ginterp-q3
run_case -grun    -q2 native-q2
run_case -grun    -q3 native-q3

echo 't1145-regression: PASS'
