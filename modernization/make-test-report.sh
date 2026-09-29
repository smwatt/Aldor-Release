#!/bin/bash
#
# make-test-report.sh: Aldor build and development support.
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
set -euo pipefail

ROOT=${1:?usage: make-test-report.sh ALDORROOT [source-root]}
SRCROOT=${2:-$(cd "$(dirname "$0")/.." && pwd)}
OUT="$ROOT/test-report.txt"

count_log() {
    local f=$1 pattern=$2
    if [ -f "$f" ]; then
        grep -Ec "$pattern" "$f" 2>/dev/null || true
    else
        printf '0\n'
    fi
}

count_self_status() {
    local status=$1 f n=0 m
    for f in "$ROOT/testout/aldor/tout-ok.log" "$ROOT/testout/aldor/tout-diff.log"; do
        case "$status" in
            OK)        m=$(count_log "$f" '^>> .* OK$') ;;
            DIFFERENT) m=$(count_log "$f" '^>> .* DIFFERENT') ;;
            ERROR)     m=$(count_log "$f" '^>> .* ERROR$') ;;
        esac
        n=$((n + m))
    done
    printf '%s\n' "$n"
}

count_tsv_status() {
    local f=$1 col=$2 status=$3
    if [ -f "$f" ]; then
        awk -F '\t' -v c="$col" -v s="$status" 'NR > 1 && $c == s {n++} END {print n+0}' "$f"
    else
        printf '0\n'
    fi
}

count_tsv_class_status() {
    local f=$1 cls=$2 status=$3
    if [ -f "$f" ]; then
        awk -F '\t' -v c="$cls" -v s="$status" \
            'NR > 1 && $2 == c && $5 == s {n++} END {print n+0}' "$f"
    else
        printf '0\n'
    fi
}

count_tsv_unparsed() {
    local f=$1 col=$2
    if [ -f "$f" ]; then
        awk -F '\t' -v c="$col" \
            'NR > 1 && $c != "OK" && $c != "DIFFERENT" && $c != "ERROR" {n++} END {print n+0}' "$f"
    else
        printf '0\n'
    fi
}

extract_time() {
    local f=$1 key=$2
    if [ -f "$f" ]; then
        grep -E "^${key}[[:space:]]" "$f" | tail -1 | awk '{print $2}' || true
    fi
}

compiler_hash() {
    local compiler=$1
    if command -v sha256sum >/dev/null 2>&1; then
        sha256sum "$compiler" | awk '{print $1}'
    elif command -v shasum >/dev/null 2>&1; then
        shasum -a 256 "$compiler" | awk '{print $1}'
    else
        printf 'unavailable'
    fi
}

self_ok=$(count_self_status OK)
self_diff=$(count_self_status DIFFERENT)
self_err=$(count_self_status ERROR)
axf="$ROOT/testout/lib/axllib/results.tsv"
salf="$ROOT/testout/lib/aldor/results.tsv"
algf="$ROOT/testout/lib/algebra/results.tsv"

ax_ok=$(count_tsv_status "$axf" 5 OK)
ax_diff=$(count_tsv_status "$axf" 5 DIFFERENT)
ax_err=$(count_tsv_status "$axf" 5 ERROR)
ax_unknown=$(count_tsv_unparsed "$axf" 5)
sal_ok=$(count_tsv_status "$salf" 4 OK)
sal_diff=$(count_tsv_status "$salf" 4 DIFFERENT)
sal_err=$(count_tsv_status "$salf" 4 ERROR)
sal_unknown=$(count_tsv_unparsed "$salf" 4)
alg_ok=$(count_tsv_status "$algf" 4 OK)
alg_diff=$(count_tsv_status "$algf" 4 DIFFERENT)
alg_err=$(count_tsv_status "$algf" 4 ERROR)
alg_unknown=$(count_tsv_unparsed "$algf" 4)

all_ok=$((self_ok + ax_ok + sal_ok + alg_ok))
all_diff=$((self_diff + ax_diff + sal_diff + alg_diff))
all_err=$((self_err + ax_err + sal_err + alg_err))
all_unknown=$((ax_unknown + sal_unknown + alg_unknown))
all_recorded=$((all_ok + all_diff + all_err + all_unknown))
blocked=0
cf="$ROOT/test-classification.tsv"
if [ -f "$cf" ]; then
    blocked=$(awk -F '\t' 'NR > 1 && $6 == "BLOCKED" {n++} END {print n+0}' "$cf")
fi

{
    echo "Aldor full test report"
    echo "======================"
    echo
    echo "Source: $SRCROOT"
    echo "Root:   $ROOT"
    if [ -x "$ROOT/bin/aldor" ]; then
        echo "Compiler SHA-256: $(compiler_hash "$ROOT/bin/aldor")"
    fi
    echo "Generated: $(date -u '+%Y-%m-%dT%H:%M:%SZ')"
    echo

    echo "Compiler self-tests"
    echo "-------------------"
    echo "OK:        $self_ok"
    echo "DIFFERENT: $self_diff"
    echo "ERROR:     $self_err"
    echo

    echo "AxlLib classified tests"
    echo "-----------------------"
    for cls in ok fix diff err; do
        cok=$(count_tsv_class_status "$axf" "$cls" OK)
        cdiff=$(count_tsv_class_status "$axf" "$cls" DIFFERENT)
        cerr=$(count_tsv_class_status "$axf" "$cls" ERROR)
        if [ -f "$axf" ]; then
            ctotal=$(awk -F '\t' -v c="$cls" 'NR > 1 && $2 == c {n++} END {print n+0}' "$axf")
        else
            ctotal=0
        fi
        cls_upper=$(printf '%s' "$cls" | tr '[:lower:]' '[:upper:]')
        printf '%-5s  OK=%-3s DIFFERENT=%-3s ERROR=%-3s TOTAL=%-3s\n' \
            "$cls_upper" "$cok" "$cdiff" "$cerr" "$ctotal"
    done
    [ "$ax_unknown" -eq 0 ] || echo "UNPARSED AxlLib result rows: $ax_unknown"
    echo

    echo "Result classification"
    echo "---------------------"
    if [ -f "$cf" ]; then
        for c in PASS SEMANTIC_FAIL HARNESS_FAIL REFERENCE_STALE EXPECTED_FAIL BLOCKED; do
            n=$(awk -F '\t' -v c="$c" 'NR > 1 && $6 == c {n++} END {print n+0}' "$cf")
            label=$c
            [ "$c" = BLOCKED ] && label=UNTESTED
            printf '%-16s %s\n' "$label" "$n"
        done
    else
        echo "not run"
    fi
    echo

    echo "libaldor embedded tests"
    echo "-----------------------"
    if [ -f "$ROOT/test.log" ]; then
        grep -E 'libaldor: (PASSED|FAILED)' "$ROOT/test.log" | tail -1 || true
    fi
    echo

    echo "Algebra embedded tests"
    echo "----------------------"
    if [ -f "$ROOT/test.log" ]; then
        grep -E '(aldorStd|aldorDebug|aldorGmp): (PASSED|FAILED)' "$ROOT/test.log" || true
    fi
    echo

    echo "Modernization release gates"
    echo "---------------------------"
    v="$ROOT/modernization-validation"
    for item in where-scope aldorhash32 bigint64-regression domain-specialization defgcd-determinism semantic-recovery; do
        if [ -f "$v/$item.status" ]; then
            rc=$(cat "$v/$item.status")
            if [ "$rc" = 0 ]; then
                printf '%-28s PASS\n' "$item"
            else
                printf '%-28s FAIL (status %s)' "$item" "$rc"
                [ -s "$v/$item.err" ] && echo ' / HAS DIAGNOSTICS' || echo
            fi
        elif [ -f "$v/$item.out" ]; then
            # Backward-compatible display for validation artifacts produced
            # before explicit status recording was introduced.
            printf '%-28s legacy result' "$item"
            [ ! -s "$v/$item.err" ] && echo ' / status unknown' || echo ' / HAS DIAGNOSTICS'
        else
            printf '%-28s NOT RUN\n' "$item"
        fi
    done
    echo
    echo "Auxiliary modernization gates"
    echo "-----------------------------"
    for item in build-diagnostics portability test-harness build-entry foreign-c-empty \
        foreign-prototype generated-c-smoke generated-c-hygiene generated-c-c99 \
        compiler-self-tests implementation-c99 unitools algebra-matrix \
        generated-c-compare; do
        if [ -f "$v/$item.status" ]; then
            rc=$(cat "$v/$item.status")
            if [ "$rc" = 0 ]; then
                printf '%-28s PASS\n' "$item"
            else
                printf '%-28s FAIL (status %s)\n' "$item" "$rc"
            fi
        elif [ "$item" != generated-c-compare ]; then
            printf '%-28s NOT RUN\n' "$item"
        fi
    done
    if [ -f "$v/algebra/results.tsv" ]; then
        n=$(awk 'END {print NR-1}' "$v/algebra/results.tsv")
        echo "Algebra validation matrix:     $n cases recorded"
    fi
    echo

    echo "Diagnostic artifacts"
    echo "--------------------"
    echo "Full inline test evidence: $ROOT/test.log"
    for f in "$axf" "$salf" "$algf"; do
        if [ -f "$f" ]; then
            n=$(awk 'END {print NR-1}' "$f")
            echo "$f: $n result rows"
        fi
    done
    echo

    echo "Build/test timing"
    echo "-----------------"
    for phase in build test; do
        f="$ROOT/$phase.log"
        if [ -f "$f" ]; then
            printf '%-6s real=%-10s user=%-10s sys=%-10s\n' \
                "$phase" "$(extract_time "$f" real)" "$(extract_time "$f" user)" "$(extract_time "$f" sys)"
        else
            echo "$phase log not found"
        fi
    done
    echo

    echo "Relative generated-code timing"
    echo "------------------------------"
    if [ -f "$ROOT/relative-timing/summary.txt" ]; then
        cat "$ROOT/relative-timing/summary.txt"
    else
        echo "not run"
    fi
    echo

    echo "Build diagnostics (compiler/toolchain messages only)"
    echo "---------------------------------------------------"
    if [ -f "$ROOT/build.log" ]; then
        echo "warnings: $(grep -c 'warning:' "$ROOT/build.log" || true)"
        echo "errors:   $(grep -c 'error:' "$ROOT/build.log" || true)"
    else
        echo "build.log not found"
    fi
    echo

    echo "FINAL PRIMARY TEST RESULT TOTALS"
    echo "--------------------------------"
    echo "Scope: compiler self-tests + AxlLib + libaldor + Algebra"
    echo "OK:        $all_ok"
    echo "DIFFERENT: $all_diff"
    echo "ERROR:     $all_err"
    echo "UNPARSED:  $all_unknown"
    echo "RECORDED:  $all_recorded"
    echo "UNTESTED GROUPS: $blocked"
} > "$OUT"

# Make the report relocatable like build.log/test.log without relying on the
# platform-specific syntax of sed -i.
tmp="$OUT.tmp.$$"
sed -e "s:${ROOT}:\$ALDORROOT:g" -e "s:${SRCROOT}:\$ALDORDISTRO:g" "$OUT" > "$tmp"
mv "$tmp" "$OUT"

cat "$OUT"
