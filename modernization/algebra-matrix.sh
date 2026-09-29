#!/bin/bash
#
# algebra-matrix.sh: Aldor build and development support.
#
# This file is part of Aldor.
#
# Aldor is licensed under the Apache License, Version 2.0.
#
#
# See legal/LICENSE in the Aldor distribution for details.
#
# Copyright (C) 2026 Stephen M. Watt.
#
set -u

LABEL="${1:?label required}"
ROOT="${2:?ALDORROOT required}"
SRCROOT="${3:?source root required}"
OUTROOT="${4:?output root required}"
TIMEOUT_SECS="${5:-60}"

mkdir -p "$OUTROOT"
RESULTS="$OUTROOT/results.tsv"
FINGERPRINT="$OUTROOT/input.sha256"

sha256_file() {
        if command -v sha256sum >/dev/null 2>&1; then
                sha256sum "$1" | awk '{print $1}'
        elif command -v shasum >/dev/null 2>&1; then
                shasum -a 256 "$1" | awk '{print $1}'
        elif command -v openssl >/dev/null 2>&1; then
                openssl dgst -sha256 "$1" | awk '{print $NF}'
        else
                echo "no SHA-256 utility available" >&2
                return 1
        fi
}

sha256_files() {
        if command -v sha256sum >/dev/null 2>&1; then
                sha256sum "$@" | sha256sum | awk '{print $1}'
        elif command -v shasum >/dev/null 2>&1; then
                shasum -a 256 "$@" | shasum -a 256 | awk '{print $1}'
        elif command -v openssl >/dev/null 2>&1; then
                for f in "$@"; do
                        openssl dgst -sha256 "$f"
                done | openssl dgst -sha256 | awk '{print $NF}'
        else
                echo "no SHA-256 utility available" >&2
                return 1
        fi
}

run_with_timeout() {
        local seconds="$1"
        shift
        local timer_pid cmd_pid rc

        if command -v timeout >/dev/null 2>&1; then
                timeout --signal=TERM --kill-after=5 "${seconds}s" "$@"
                return $?
        fi
        if command -v gtimeout >/dev/null 2>&1; then
                gtimeout --signal=TERM --kill-after=5 "${seconds}s" "$@"
                return $?
        fi

        "$@" &
        cmd_pid=$!
        (
                sleep "$seconds"
                kill -TERM "$cmd_pid" 2>/dev/null || exit 0
                sleep 5
                kill -KILL "$cmd_pid" 2>/dev/null || :
        ) &
        timer_pid=$!

        wait "$cmd_pid"
        rc=$?
        kill "$timer_pid" 2>/dev/null || :
        wait "$timer_pid" 2>/dev/null || :
        return "$rc"
}

INPUTS=(
        "$ROOT/bin/aldor"
        "$ROOT/lib/libaldor.al" "$ROOT/lib/libaldor.a"
        "$ROOT/lib/libalgebra.al" "$ROOT/lib/libalgebra.a"
        "$ROOT/lib/libalgebra-gmp.al" "$ROOT/lib/libalgebra-gmp.a"
        "$ROOT/lib/libfoam.a" "$ROOT/lib/libfoam-gmp.a"
)
for f in "${INPUTS[@]}"; do
        if [ ! -f "$f" ]; then
                echo "missing validation input: $f" >&2
                exit 1
        fi
done
CURRENT_FP=$(sha256_files "${INPUTS[@]}") || exit 1

if [ -f "$RESULTS" ] && [ ! -f "$FINGERPRINT" ]; then
        rm -f "$RESULTS"
        rm -rf "$OUTROOT"/work-*
elif [ -f "$FINGERPRINT" ] && \
     [ "$(cat "$FINGERPRINT")" != "$CURRENT_FP" ]; then
        rm -f "$RESULTS"
        rm -rf "$OUTROOT"/work-*
fi
printf '%s\n' "$CURRENT_FP" > "$FINGERPRINT"

if [ ! -f "$RESULTS" ]; then
        header='label\tbackend\ttest\tcompile_rc\trun_rc'
        header="$header\tref_match\toutput_sha256"
        printf '%b\n' "$header" > "$RESULTS"
fi

export ALDORROOT="$ROOT"
export PATH="$ROOT/bin:$ROOT/toolbin:$PATH"
EXTRACT="$ROOT/bin/extract"
[ -x "$EXTRACT" ] || EXTRACT="$ROOT/toolbin/extract"
if [ ! -x "$EXTRACT" ]; then
        echo "missing installed extract tool" >&2
        exit 1
fi
TESTDIR="$SRCROOT/lib/algebra/test"
SRCDIR="$SRCROOT/lib/algebra/src"
files=()
while IFS= read -r f; do
        if grep -q ALDORTEST "$f"; then
                files[${#files[@]}]="$f"
        fi
done < <(find "$SRCDIR" -name '*.as' -print | sort)

already_done() {
        awk -F '\t' -v b="$1" -v t="$2" \
                'NR > 1 && $2 == b && $3 == t { found=1 } END { exit !found }' \
                "$RESULTS"
}

for backend in std gmp; do
        for f in "${files[@]}"; do
                t=$(basename "$f" .as)
                if already_done "$backend" "$t"; then
                        printf '%s %-3s %-18s resumed\n' \
                                "$LABEL" "$backend" "$t"
                        continue
                fi

                w="$OUTROOT/work-$backend-$t"
                rm -rf "$w"
                mkdir -p "$w"
                cd "$w"
                "$EXTRACT" -mALDORTEST -o "$t.test.as" "$f" \
                        >extract.log 2>&1

                if [ "$backend" = std ]; then
                        "$ROOT/bin/aldor" -fx -y"$ROOT/lib" -lalgebra -laldor \
                                -q1 -qinline-all "$t.test.as" \
                                >compile.out 2>&1
                else
                        "$ROOT/bin/aldor" -fx -y"$ROOT/lib" -lalgebra-gmp \
                                -laldor -q1 -qinline-all \
                                -cruntime=foam-gmp,gmp -dGMP "$t.test.as" \
                                >compile.out 2>&1
                fi
                crc=$?
                rrc=NA
                ref=NA
                hash=NA
                if [ "$crc" -eq 0 ] && [ -x "$t.test" ]; then
                        run_with_timeout "$TIMEOUT_SECS" ./$t.test \
                                >run.out 2>&1
                        rrc=$?
                        if testcmp "$TESTDIR/testout/$t.out" run.out; then
                                ref=YES
                        else
                                ref=NO
                        fi
                        hash=$(sha256_file run.out) || exit 1
                fi

                printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\n' \
                        "$LABEL" "$backend" "$t" "$crc" "$rrc" "$ref" \
                        "$hash" >> "$RESULTS"
                case "$ref" in
                        YES) ref_text=match ;;
                        NO)  ref_text=mismatch ;;
                        *)   ref_text=not-run ;;
                esac
                printf '%s %-3s %-18s compiler rc=%s run rc=%s reference=%s\n' \
                        "$LABEL" "$backend" "$t" "$crc" "$rrc" \
                        "$ref_text"
                rm -f "$t.test" "$t.test.as"
        done
done
