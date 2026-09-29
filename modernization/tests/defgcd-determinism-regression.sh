#!/usr/bin/env bash
# Copyright (C) 2026 Stephen M. Watt.
# See legal/LICENSE in the Aldor distribution for licensing terms.
set -euo pipefail

ROOT="${1:?usage: defgcd-determinism-regression.sh ALDORROOT [source-root] [runs]}"
SRCROOT="${2:-$(cd "$(dirname "$0")/../.." && pwd)}"
RUNS="${3:-20}"
WORK="$ROOT/testout/defgcd-determinism-regression"
SOURCE="$SRCROOT/lib/algebra/src/multpoly/multpolycat/alg_defgcd.as"
REFERENCE="$SRCROOT/lib/algebra/test/testout/alg_defgcd.out"

export ALDORROOT="$ROOT"
export PATH="$ROOT/bin:$ROOT/toolbin:$PATH"

rm -rf "$WORK"
mkdir -p "$WORK"
cd "$WORK"

sha256_file() {
    if command -v sha256sum >/dev/null 2>&1; then
        sha256sum "$1" | awk '{print $1}'
    else
        shasum -a 256 "$1" | awk '{print $1}'
    fi
}

run_with_timeout() {
    secs=$1
    shift
    if command -v timeout >/dev/null 2>&1; then
        timeout "$secs" "$@"
    elif command -v gtimeout >/dev/null 2>&1; then
        gtimeout "$secs" "$@"
    else
        "$@"
    fi
}

report_failure() {
    rc=$?
    [ "$rc" -eq 0 ] && return 0
    echo "defgcd-determinism-regression: FAIL rc=$rc" >&2
    echo "retained diagnostic directory: $WORK" >&2
    for f in compile.out compile.err runs.tsv mismatch.*.diff run.*.err; do
        [ -f "$f" ] || continue
        echo "--- $f ---" >&2
        cat "$f" >&2 || true
    done
    return "$rc"
}
trap report_failure EXIT

extract -mALDORTEST -o"$WORK/alg_defgcd.test.as" "$SOURCE"
aldor -Fx -y"$ROOT/lib" -lalgebra -laldor -q1 -qinline-all \
        alg_defgcd.test.as >compile.out 2>compile.err
if [ -s compile.err ]; then
    echo "defgcd-determinism-regression: compiler/linker warnings retained"
    echo "  in $WORK/compile.err"
fi

printf 'run\trc\tstdout_sha256\tstderr_sha256\n' >runs.tsv
for i in $(seq 1 "$RUNS"); do
    n=$(printf '%02d' "$i")
    set +e
    run_with_timeout 60 ./alg_defgcd.test >"run.$n.out" 2>"run.$n.err"
    rc=$?
    set -e
    so=$(sha256_file "run.$n.out")
    se=$(sha256_file "run.$n.err")
    printf '%s\t%s\t%s\t%s\n' "$n" "$rc" "$so" "$se" >>runs.tsv
    if [ "$rc" -ne 0 ]; then
        echo "defgcd-determinism-regression: run $n rc=$rc" >&2
        exit 1
    fi
    if ! cat "run.$n.err" "run.$n.out" | cmp -s - "$REFERENCE"; then
        {
            echo '--- reference'
            cat "$REFERENCE"
            echo "--- run $n stderr+stdout"
            cat "run.$n.err" "run.$n.out"
        } >"mismatch.$n.diff"
        echo "defgcd-determinism-regression: run $n differs from reference" >&2
        exit 1
    fi
done

[ "$(tail -n +2 runs.tsv | cut -f3 | sort -u | wc -l | tr -d ' ')" -eq 1 ]
[ "$(tail -n +2 runs.tsv | cut -f4 | sort -u | wc -l | tr -d ' ')" -eq 1 ]
printf 'PASS: defgcd deterministic over %s runs\n' "$RUNS"
