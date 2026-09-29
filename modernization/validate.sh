#!/bin/bash
#
# validate.sh: Aldor build and development support.
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

ROOT="${1:?usage: validate.sh ALDORROOT [source-root]}"
SRCROOT="${2:-$(cd "$(dirname "$0")/.." && pwd)}"
OUT="$ROOT/modernization-validation"
if command -v sha256sum >/dev/null 2>&1; then
        CURRENT_FP=$(sha256sum "$ROOT/bin/aldor" | awk '{print $1}')
elif command -v shasum >/dev/null 2>&1; then
        CURRENT_FP=$(shasum -a 256 "$ROOT/bin/aldor" | awk '{print $1}')
else
        echo "no SHA-256 utility available (need sha256sum or shasum)" >&2
        exit 1
fi

if [ -f "$OUT/compiler.sha256" ] && \
   [ "$(cat "$OUT/compiler.sha256")" != "$CURRENT_FP" ]; then
        rm -rf "$OUT"
fi
mkdir -p "$OUT"
printf '%s\n' "$CURRENT_FP" > "$OUT/compiler.sha256"

if [ ! -x "$ROOT/bin/aldor" ]; then
        echo "missing $ROOT/bin/aldor" >&2
        exit 1
fi

# Build log must be genuinely clean if a canonical build produced one.  Record
# this as a named gate as well: historically a warning here could terminate
# validate.sh before the named auxiliary phases, yielding an anonymous failure.
build_diag_status=0
if [ -f "$ROOT/build.log" ]; then
        nw=$(grep -c 'warning:' "$ROOT/build.log" || true)
        ne=$(grep -c 'error:' "$ROOT/build.log" || true)
        echo "build diagnostics: errors=$ne warnings=$nw"
        if [ "$nw" -ne 0 ] || [ "$ne" -ne 0 ]; then
                build_diag_status=1
        fi
fi
printf '%s\n' "$build_diag_status" > "$OUT/build-diagnostics.status"
if [ "$build_diag_status" -eq 0 ]; then
        echo "modernization release gate: build-diagnostics PASS"
else
        echo "modernization release gate: build-diagnostics FAIL" >&2
        exit "$build_diag_status"
fi

VALIDATION_HOST_PATH=$PATH
export ALDORROOT="$ROOT"
export PATH="$ROOT/bin:$ROOT/toolbin:$PATH"

# Core named release gates are run first.  They are independent of the
# auxiliary host/C99 validation below.  Run all six before deciding whether
# the core gate set failed, so one failure cannot make the remaining gates
# misleadingly appear as `NOT RUN` in the final report.
core_gate_status=0

run_core_gate() {
        name=$1
        shift
        if "$@" >"$OUT/$name.out" 2>"$OUT/$name.err"; then
                rc=0
                [ ! -s "$OUT/$name.err" ] || rc=1
        else
                rc=$?
        fi
        printf '%s\n' "$rc" > "$OUT/$name.status"
        if [ "$rc" -ne 0 ]; then
                core_gate_status=1
        fi
}

run_core_gate where-scope \
        "$SRCROOT/modernization/tests/where-scope-regression.sh" \
        "$ROOT" "$SRCROOT"
run_core_gate aldorhash32 \
        "$SRCROOT/modernization/tests/aldorhash32-regression.sh" \
        "$ROOT" "$SRCROOT"
run_core_gate bigint64-regression \
        "$SRCROOT/modernization/tests/bigint64-regression.sh" \
        "$ROOT" "$SRCROOT"
run_core_gate domain-specialization \
        "$SRCROOT/modernization/tests/domain-specialization-regression.sh" \
        "$ROOT" "$SRCROOT"
run_core_gate defgcd-determinism \
        "$SRCROOT/modernization/tests/defgcd-determinism-regression.sh" \
        "$ROOT" "$SRCROOT" 20
run_core_gate semantic-recovery \
        "$SRCROOT/modernization/tests/semantic-recovery-regression.sh" \
        "$ROOT" "$SRCROOT"

if [ "$core_gate_status" -ne 0 ]; then
        echo "one or more core modernization release gates failed" >&2
        exit "$core_gate_status"
fi

# Auxiliary validations are independent release gates.  Run all of them even
# if one fails so a portability or harness problem cannot hide later evidence.
# ERR remains useful for recording the first failure in each phase, but errexit
# is disabled until the complete auxiliary set has been exercised.
aux_current=
aux_current_failed=0
aux_gate_status=0
aux_finish_previous() {
        if [ -n "$aux_current" ]; then
                if [ "$aux_current_failed" -eq 0 ]; then
                        printf '0\n' > "$OUT/$aux_current.status"
                        echo "auxiliary modernization gate: $aux_current PASS"
                fi
        fi
}
aux_begin() {
        aux_finish_previous
        aux_current=$1
        aux_current_failed=0
        rm -f "$OUT/$aux_current.status"
        echo "auxiliary modernization gate: $aux_current"
}
aux_failed() {
        rc=$?
        if [ -n "$aux_current" ]; then
                if [ "$aux_current_failed" -eq 0 ]; then
                        printf '%s\n' "$rc" > "$OUT/$aux_current.status"
                        echo "auxiliary modernization gate: $aux_current FAIL (status $rc)" >&2
                fi
                aux_current_failed=1
                aux_gate_status=1
        else
                echo "auxiliary modernization gate failed before phase identification (status $rc)" >&2
                aux_gate_status=1
        fi
        return 0
}
set +e
trap aux_failed ERR

aux_begin portability
# Host portability invariants that do not require an Aldor program build.
"$SRCROOT/modernization/tests/portability-regression.sh" "$SRCROOT" \
        >"$OUT/portability.out" 2>"$OUT/portability.err"
[ ! -s "$OUT/portability.err" ]

aux_begin varargs-abi
# Raw C/C++ varargs contracts are ABI-sensitive.  Keep every extraction site
# inventoried and reject the historical pointer/integer mismatch patterns.
python3 "$SRCROOT/modernization/tests/varargs-abi-audit.py" \
        >"$OUT/varargs-abi.out" 2>"$OUT/varargs-abi.err"
[ ! -s "$OUT/varargs-abi.err" ]

aux_begin ccode-int-format-abi
# CCode integer literal formatting previously passed int through %ld varargs.
python3 "$SRCROOT/modernization/tests/ccode-int-format-abi.py" \
        >"$OUT/ccode-int-format-abi.out" 2>"$OUT/ccode-int-format-abi.err"
[ ! -s "$OUT/ccode-int-format-abi.err" ]

aux_begin message-varargs
# Message formats are selected indirectly by catalogue id, so compiler format
# attributes cannot validate them.  Resolve the catalogue and audit arity.
python3 "$SRCROOT/modernization/tests/message-varargs-audit.py" \
        >"$OUT/message-varargs.out" 2>"$OUT/message-varargs.err"
[ ! -s "$OUT/message-varargs.err" ]

aux_begin test-harness
# Build/test driver invariants: classification census, option propagation,
# concise DIFF handling, and continuation across failing library suites.
"$SRCROOT/modernization/tests/test-harness-regression.sh" \
        >"$OUT/test-harness.out" 2>"$OUT/test-harness.err"
[ ! -s "$OUT/test-harness.err" ]

aux_begin build-entry
# Top-level root selection and build-entry safety invariants.
env \
        -u ALDORROOT -u ALDORTMP -u INCPATH -u LIBPATH \
        -u ALGEBRAROOT -u ALGEBRA \
        -u HOST_CC -u HOST_CXX -u HOST_AR -u HOST_RANLIB \
        -u ALDOR_TOOLCHAIN -u ALDOR_CC -u ALDOR_CXX \
        -u ALDOR_AR -u ALDOR_RANLIB -u ALDOR_LIBTOOL \
        -u CC -u CXX -u AR -u RANLIB \
        PATH="$VALIDATION_HOST_PATH" \
        "$SRCROOT/modernization/tests/build-entry-regression.sh" \
        >"$OUT/build-entry.out" 2>"$OUT/build-entry.err"
[ ! -s "$OUT/build-entry.err" ]

aux_begin foreign-c-empty
# `Foreign C ""' is invalid.  Headerless raw C is spelled `Foreign C'.
# This must be rejected by Aldor itself, before any subordinate C compile.
EMPTY_FOREIGN="$OUT/foreign-c-empty.as"
cat >"$EMPTY_FOREIGN" <<'EOF'
import { foreignEmptyProbe: () -> (); } from Foreign C "";
EOF
if "$ROOT/bin/aldor" -Fc "$EMPTY_FOREIGN" \
        >"$OUT/foreign-c-empty.out" 2>"$OUT/foreign-c-empty.err"; then
        echo 'Foreign C "" unexpectedly compiled successfully' >&2
        false
fi
# Aldor diagnostics are written on its ordinary diagnostic/output stream on
# some builds, not necessarily stderr.  Accept either stream, but require the
# specific front-end diagnostic and require that C generation never started.
cat "$OUT/foreign-c-empty.out" "$OUT/foreign-c-empty.err" \
        >"$OUT/foreign-c-empty.diag"
grep -q 'Empty header name' "$OUT/foreign-c-empty.diag"
[ ! -e "$OUT/foreign-c-empty.c" ]

aux_begin foreign-prototype
# RC4: generated declarations for headerless foreign functions must be real
# C prototypes.  This covers both historical plain `Foreign' and raw
# headerless `Foreign C'.  In particular, do not depend on the pre-C23
# interpretation of f() as an unspecified argument list.
FOREIGN_PROTO="$OUT/foreign-prototype.as"
cat >"$FOREIGN_PROTO" <<'EOF'
#pile
export
    Type: Type
    Tuple: Type -> Type
    ->: (Tuple Type, Tuple Type) -> Type
    W: Type
    P: Type

import
    rc4Two: (W, P) -> W
    rc4Zero: () -> W
from Foreign

import
    rc4CTwo: (W, P) -> W
    rc4CZero: () -> W
from Foreign C

w: W == rc4Zero()
p: P == rc4CZero() pretend P
rc4Two(w, p)
rc4CTwo(w, p)
EOF
mkdir -p "$OUT/foreign-prototype"
cd "$OUT/foreign-prototype"
"$ROOT/bin/aldor" -Fc "$FOREIGN_PROTO" \
        >"$OUT/foreign-prototype.out" 2>"$OUT/foreign-prototype.err"
[ ! -s "$OUT/foreign-prototype.err" ]
grep -Eq '^extern FiWord rc4Two\(FiWord [^,]+, FiWord [^)]+\);$' \
        foreign-prototype.c
grep -q '^extern FiWord rc4Zero(void);$' foreign-prototype.c
grep -Eq '^extern FiWord rc4CTwo\(FiWord [^,]+, FiWord [^)]+\);$' \
        foreign-prototype.c
grep -q '^extern FiWord rc4CZero(void);$' foreign-prototype.c
if grep -Eq '^extern .*rc4(C)?(Two|Zero)\(\);$' foreign-prototype.c; then
        echo 'generated foreign declaration still uses an empty parameter list' >&2
        false
fi
"${HOST_CC:-cc}" -I"$SRCROOT/aldor/src" -I"$ROOT/include" \
        -c foreign-prototype.c -o foreign-prototype.o \
        >"$OUT/foreign-prototype-cc.out" \
        2>"$OUT/foreign-prototype-cc.err"
[ ! -s "$OUT/foreign-prototype-cc.err" ]

aux_begin generated-c-smoke
# Dependency-free generated-C smoke test.
SMOKE_SRC="$SRCROOT/lib/axllib/test/none0.as"
SMOKE="$OUT/smoke"
rm -rf "$SMOKE"
mkdir -p "$SMOKE"
cd "$SMOKE"
"$ROOT/bin/aldor" -Fc "$SMOKE_SRC" >stdout 2>stderr
[ -s none0.c ]
[ ! -s stderr ]

aux_begin generated-c-hygiene
# Generated C must not contain an expression statement consisting only of an
# identifier.  Such a statement has no effect for Aldor-generated ordinary
# objects and triggers -Wunused-value on compilers which diagnose it.  Scan
# both a freshly optimized test and materialized/generated reference C.  The
# scanner recognizes wrapped constructs such as `goto\n L3;' and `p->\n f;'
# as continuations rather than standalone statements.
HYGIENE="$OUT/generated-c-hygiene"
rm -rf "$HYGIENE"
mkdir -p "$HYGIENE"
cd "$HYGIENE"
LIBPATH="$ROOT/lib" "$ROOT/bin/aldor" \
        -Mno-ALDOR_W_CantUseArchive -O -laxllib -Fc \
        "$SRCROOT/lib/axllib/test/builtin0.as" \
        >"$OUT/generated-c-hygiene.out" \
        2>"$OUT/generated-c-hygiene.err"
[ ! -s "$OUT/generated-c-hygiene.err" ]
python3 - builtin0.c "$SRCROOT/lib/axllib/src" \
        "$SRCROOT/lib/axllib/testout" \
        "$SRCROOT/lib/axllib/testout32" <<'PYSCAN'
import pathlib, re, sys

identifier_stmt = re.compile(r'^\s*([A-Za-z_]\w*);\s*$')
continuation_end = re.compile(
    r'(?:->|\.|,|=|\+|-|\*|/|%|&|\||\^|!|~|\?|:|<|>|<=|>=|==|!=|&&|\|\||'
    r'\(|\[|\b(?:goto|return|sizeof))\s*$')

def scan(path):
    lines = path.read_text(errors='replace').splitlines()
    bad = []
    for i, line in enumerate(lines):
        m = identifier_stmt.match(line)
        if not m:
            continue
        j = i - 1
        while j >= 0 and not lines[j].strip():
            j -= 1
        prev = lines[j].rstrip() if j >= 0 else ''
        if prev and continuation_end.search(prev):
            continue
        bad.append((i + 1, m.group(1)))
    return bad

bad = []
for arg in sys.argv[1:]:
    root = pathlib.Path(arg)
    paths = [root] if root.is_file() else sorted(root.glob('*.c'))
    for path in paths:
        for lineno, ident in scan(path):
            bad.append((path, lineno, ident))
if bad:
    for path, lineno, ident in bad:
        print(f'{path}:{lineno}: useless generated identifier statement: {ident};',
              file=sys.stderr)
    raise SystemExit(1)
PYSCAN

aux_begin generated-c-c99
# The generated-program ABI is C99, not merely "some modern C".  Compile both
# a direct foam_c.h consumer and actual generated Aldor C in strict C99 mode.
cat >foam-c99-client.c <<'EOF'
#include "foam_c.h"
int main(void) { FiWord w = 0; return (int) w; }
EOF
"${HOST_CC:-cc}" -std=c99 -pedantic-errors \
        -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE \
        -I"$SRCROOT/aldor/src" -I"$ROOT/include" \
        -c foam-c99-client.c -o foam-c99-client.o \
        >"$OUT/foam-c99-client.out" 2>"$OUT/foam-c99-client.err"
[ ! -s "$OUT/foam-c99-client.err" ]
"${HOST_CC:-cc}" -std=c99 -pedantic-errors \
        -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE \
        -I"$SRCROOT/aldor/src" -I"$ROOT/include" \
        -c "$SMOKE/none0.c" -o none0-c99.o \
        >"$OUT/none0-c99.out" 2>"$OUT/none0-c99.err"
[ ! -s "$OUT/none0-c99.err" ]

aux_begin compiler-self-tests
# Validate the actual compiler self-test phase rather than a smaller rerun.
# The primary phase has already populated these logs; any DIFFERENT/ERROR in
# the 29 shipped self-tests must make this auxiliary gate fail as well.
SELF_OK="$ROOT/testout/aldor/tout-ok.log"
SELF_DIFF="$ROOT/testout/aldor/tout-diff.log"
[ -f "$SELF_OK" ]
[ -f "$SELF_DIFF" ]
self_count=$(grep -Ec '^>> .*: \(script\) (OK|DIFFERENT|ERROR)$' \
        "$SELF_OK" || true)
diff_count=$(grep -Ec '^>> ' "$SELF_DIFF" || true)
[ "$self_count" -eq 29 ]
[ "$diff_count" -eq 0 ]
! grep -Eq '^>> .*(DIFFERENT|ERROR)$' "$SELF_OK"

aux_begin implementation-c99
# Every implementation still classified as GeneralC/PortC must genuinely
# remain valid C.  Compiler/unitool-only implementations are .cpp now.
(
        cd "$SRCROOT/aldor/src"
        source ./OkFiles.sh >/dev/null
        for f in $GeneralC $PortC; do
                "${HOST_CC:-cc}" -std=c99 -pedantic-errors \
                        -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE \
                        -fsyntax-only -I"$SRCROOT/aldor/src" -I"$ROOT/include" \
                        "$SRCROOT/aldor/src/$f" \
                        >"$OUT/c99-$f.out" 2>"$OUT/c99-$f.err"
                [ ! -s "$OUT/c99-$f.err" ]
        done
)

aux_begin unitools
# Unitools are C++20 products in v0.08.
[ -x "$ROOT/bin/unicl" ]
[ -x "$ROOT/bin/platform" ]
"$ROOT/bin/platform" >"$OUT/platform.out" 2>"$OUT/platform.err"
[ ! -s "$OUT/platform.err" ]
printf 'int main(void) { return 0; }\n' >"$OUT/unicl-smoke.c"
(
        cd "$OUT"
        rm -f unicl-smoke.o
        "$ROOT/bin/unicl" -c unicl-smoke.c \
                >unicl-smoke.out 2>unicl-smoke.err
        [ -s unicl-smoke.o ]
        [ ! -s unicl-smoke.err ]

        # Native -W options must pass through unicl.  In particular, -Wno-*
        # must not match unicl's private -Wn no-execute option.
        rm -f unicl-smoke
        "$ROOT/bin/unicl" -Wno-unused-value unicl-smoke.o -o unicl-smoke \
                >unicl-link-smoke.out 2>unicl-link-smoke.err
        [ -x unicl-smoke ]
        ./unicl-smoke
        [ ! -s unicl-link-smoke.err ]
)

aux_begin algebra-matrix
# The runtime matrix is resumable.  Compilation outcomes must still match the
# historical baseline, and every current Algebra case must now compile, run,
# and match its single portable reference exactly (modulo CRLF canonicalization
# performed by testcmp).
"$SRCROOT/modernization/algebra-matrix.sh" v008 "$ROOT" "$SRCROOT" \
        "$OUT/algebra" 60
base="$SRCROOT/modernization/baselines/v0.04-algebra-std-gmp.tsv"
cur="$OUT/algebra/results.tsv"
python3 - "$base" "$cur" <<'PY'
import csv, sys
bfile, cfile = sys.argv[1:]
def read(path):
    with open(path, newline='') as f:
        rows = list(csv.DictReader(f, delimiter='\t'))
    return {(r['backend'], r['test']): r for r in rows}
b = read(bfile)
c = read(cfile)
if set(b) != set(c):
    print('Algebra test-set changed', sorted(set(b) ^ set(c)))
    raise SystemExit(1)
diffs = []
for key in sorted(b):
    if b[key]['compile_rc'] != c[key]['compile_rc']:
        diffs.append((key, b[key]['compile_rc'], c[key]['compile_rc']))
if diffs:
    for key, old, new in diffs:
        print('NEW ALGEBRA COMPILE DIFFERENCE', key, old, '->', new)
    raise SystemExit(1)
print('Algebra compile outcomes match v0.04 baseline exactly:', len(c), 'cases')

bad_runtime = []
for key in sorted(c):
    r = c[key]
    if r['compile_rc'] != '0' or r['run_rc'] != '0' or r['ref_match'] != 'YES':
        bad_runtime.append((key, r['compile_rc'], r['run_rc'], r['ref_match']))
if bad_runtime:
    for key, crc, rrc, ref in bad_runtime:
        print('ALGEBRA RUNTIME/REFERENCE FAILURE', key,
              'compile=' + crc, 'run=' + rrc, 'ref=' + ref)
    raise SystemExit(1)
print('Algebra runtime/reference outcomes all current-correct:', len(c), 'cases')
PY

# Optional strong compiler-to-compiler comparison.  Point
# ALDOR_COMPARE_ROOT at the prior release product tree.
if [ -n "${ALDOR_COMPARE_ROOT:-}" ]; then
        aux_begin generated-c-compare
        "$SRCROOT/modernization/generated-c-compare.sh" v007-v008 \
                "$ALDOR_COMPARE_ROOT" "$ROOT" "$SRCROOT" \
                "$OUT/generated-c-compare"
        python3 - "$OUT/generated-c-compare/results.tsv" <<'PY'
import csv, sys
with open(sys.argv[1], newline='') as f:
    rows = list(csv.DictReader(f, delimiter='\t'))
bad = [r for r in rows if r['identical'] != 'YES']
if bad:
    for r in bad:
        print('GENERATED-C DIFFERENCE', r['backend'], r['test'])
    raise SystemExit(1)
print('Generated-C compiler comparison identical:', len(rows), 'cases')
PY
fi

aux_finish_previous
aux_current=
trap - ERR
set -e
if [ "$aux_gate_status" -ne 0 ]; then
        failed_gates=
        for status_file in "$OUT"/*.status; do
                [ -f "$status_file" ] || continue
                [ "$(cat "$status_file" 2>/dev/null)" = 0 ] && continue
                gate=${status_file##*/}
                gate=${gate%.status}
                failed_gates="$failed_gates $gate"
        done
        echo "one or more auxiliary modernization release gates failed" >&2
        echo "failed release gates:${failed_gates:- unknown}" >&2
        exit "$aux_gate_status"
fi
echo "modernization release gate: PASS"
