#!/usr/bin/env bash
# Copyright (C) 2026 Stephen M. Watt.
# See legal/LICENSE in the Aldor distribution for licensing terms.
set -eo pipefail

script_ref=${BASH_SOURCE[0]}
here=$(cd -P -- "${script_ref%/*}" && pwd -P)
distro=$(cd -P -- "$here/../.." && pwd -P)
work=$(mktemp -d "${TMPDIR:-/tmp}/aldor-test-harness.XXXXXX")
trap 'rm -rf "$work"' EXIT

fail() { echo "FAIL: $*" >&2; exit 1; }

# Build a (kind,file)->classification map directly from the authoritative
# AxlLib arrays.  This makes the regression fail if the census changes without
# consciously updating the expected count below.
python3 - "$distro/lib/axllib/test/build.sh" "$work/map.tsv" <<'PY'
import re, sys
src, out = sys.argv[1:]
text = open(src).read()
kind_prefix = {
    'Script':'script', 'Phase':'phase', 'Gen':'gen', 'Comp':'comp',
    'Int':'int', 'Run':'run', 'Err':'errs'
}
rows=[]
for prefix, kind in kind_prefix.items():
    for mode_name, mode in [('Ok','ok'),('Fix','fix'),('Diff','diff'),('Err','err')]:
        name=prefix+mode_name
        m=re.search(r'^'+re.escape(name)+r'=\(\n(.*?)^\)', text, re.M|re.S)
        if not m:
            # The historical driver references ErrErr without declaring it;
            # ordinary Bash treats that unset array as empty.
            if name == 'ErrErr':
                continue
            raise SystemExit('missing array '+name)
        body=re.sub(r'#.*','',m.group(1))
        for f in body.split(): rows.append((kind,f,mode))
# doTestIf removes duplicate file names within each classification call.
rows=sorted(set(rows))
with open(out,'w') as fp:
    for r in rows: fp.write('\t'.join(r)+'\n')
print(len(rows))
PY
census=$(wc -l < "$work/map.tsv" | tr -d ' ')
[ "$census" -eq 686 ] || fail "AxlLib classified census is $census, expected 686"

mkdir -p "$work/root/toolbin" "$work/root/testout/lib/axllib"
cp "$distro/build-fns.sh" "$work/root/toolbin/build-fns.sh"
cat > "$work/root/toolbin/testaldor" <<'EOF2'
#!/usr/bin/env bash
set -e
kind=all
nodiff=no
files=()
while [ $# -gt 0 ]; do
  case "$1" in
    -only) kind=${2#test}; shift 2 ;;
    -nodiff) nodiff=yes; shift ;;
    -keep) shift ;;
    -outdir|-refdir) shift 2 ;;
    -debug) shift ;;
    -*) shift ;;
    *) files+=("$1"); shift ;;
  esac
done
printf '%s\n' "$kind $nodiff ${#files[@]}" >> "$SYN_CALLS"
for path in "${files[@]}"; do
  f=${path##*/}
  mode=$(awk -F '\t' -v k="$kind" -v f="$f" '$1==k && $2==f {print $3; exit}' "$SYN_MAP")
  case "$mode" in
    ok) status=OK ;;
    fix) status=DIFFERENT ;;
    diff)
      if [ ! -f "$SYN_MISSING_REF_SENTINEL" ]; then
        status='DIFFERENT < reference missing; new output follows >'
        : > "$SYN_MISSING_REF_SENTINEL"
      else
        status=DIFFERENT
      fi
      ;;
    err) status=ERROR ;;
    *) echo "unclassified synthetic request: $kind $f" >&2; exit 2 ;;
  esac
  echo ">> $f: (synthetic) $status"
done
EOF2
chmod +x "$work/root/toolbin/testaldor"

export ALDORROOT="$work/root"
export ALDORTMP="$work/root/tmp"
export SYN_MAP="$work/map.tsv"
export SYN_CALLS="$work/calls.log"
export SYN_MISSING_REF_SENTINEL="$work/missing-ref-seen"
export PATH="$work/root/toolbin:$PATH"
mkdir -p "$ALDORTMP"

set +e
(
  cd "$distro/lib/axllib/test"
  bash build.sh test -nodiff
) >"$work/axllib.out" 2>"$work/axllib.err"
rc=$?
set -e
# All active historical classifications have been promoted to OK.  The
# synthetic run therefore has no expected-debt failures.
[ "$rc" -eq 0 ] || fail "all-OK synthetic AxlLib classification failed"
[ ! -s "$work/axllib.err" ] || { cat "$work/axllib.err" >&2; fail "AxlLib synthetic run wrote stderr"; }

# Every classified (kind,file) is invoked exactly once and -nodiff reaches the
# real testaldor call after -only parsing.
invoked=$(awk '{n += $3} END {print n+0}' "$SYN_CALLS")
[ "$invoked" -eq 686 ] || fail "AxlLib invoked $invoked classified results, expected 686"
if grep -vE '^(script|phase|gen|comp|int|run|errs) yes [0-9]+$' "$SYN_CALLS" >/dev/null; then
  fail "-only/-nodiff propagation to testaldor is incorrect"
fi

grep -q '686 OK' "$work/axllib.out" || fail "expected 686 OK results not reported"
grep -q '  0 diff' "$work/axllib.out" || fail "unexpected synthetic differences reported"
grep -q '  0 err' "$work/axllib.out" || fail "unexpected synthetic errors reported"

results="$ALDORROOT/testout/lib/axllib/results.tsv"
[ -f "$results" ] || fail "AxlLib results.tsv was not written"
rows=$(awk 'END {print NR-1}' "$results")
[ "$rows" -eq 686 ] || fail "AxlLib results.tsv has $rows rows, expected 686"
if ! awk -F '\t' 'NR==1 {next} NF != 5 || $1 != "axllib" || $2 !~ /^(ok|fix|diff|err)$/ || $5 !~ /^(OK|DIFFERENT|ERROR)$/ {bad=1} END {exit bad}' "$results"; then
  fail "AxlLib results.tsv contains an unrecognized row"
fi

# analyzeLog must count both compact DIFFERENT and verbose
# "DIFFERENT < Old, > new" forms.
cat > "$work/count.log" <<'EOF2'
>> a: (synthetic) OK
>> b: (synthetic) DIFFERENT
>> c: (synthetic) DIFFERENT < Old, > new
>> d: (synthetic) ERROR
EOF2
set +u
source "$distro/build-fns.sh"
summary=$(analyzeLog "$work/count.log")
set -u
case "$summary" in
  *'1 OK +   2 diff +   1 err =   4 total'*) ;;
  *) fail "annotated DIFFERENT was not counted: $summary" ;;
esac

grep -q 'export ALDOR_TEST_BRIEF_DIFFS=0' "$distro/build.sh" || \
  fail "clean all does not preserve full inline diff evidence"
grep -q '\* )       diffArg="" ;;' "$distro/lib/axllib/test/build.sh" || \
  fail "AxlLib default does not preserve inline diffs for all classifications"

# Verify the library aggregate continues after an AxlLib failure and forwards
# caller options to all three suites.
fake="$work/lib"
mkdir -p "$fake/axllib" "$fake/aldor/test" "$fake/algebra/test"
cp "$distro/lib/build.sh" "$fake/build.sh"
for d in axllib aldor/test algebra/test; do
  cat > "$fake/$d/build.sh" <<'EOF2'
#!/usr/bin/env bash
printf '%s %s\n' "$PWD" "$*" >> "$SYN_LIB_CALLS"
case "$PWD" in */axllib) exit 7;; *) exit 0;; esac
EOF2
  chmod +x "$fake/$d/build.sh"
done
export SYN_LIB_CALLS="$work/lib-calls.log"
set +e
(
  cd "$fake"
  bash build.sh test -nodiff
) >"$work/lib.out" 2>"$work/lib.err"
librc=$?
set -e
[ "$librc" -eq 7 ] || fail "library aggregate did not preserve first failure ($librc)"
[ "$(wc -l < "$SYN_LIB_CALLS" | tr -d ' ')" -eq 3 ] || fail "later library suites were not attempted"
[ "$(grep -c 'test -nodiff' "$SYN_LIB_CALLS")" -eq 3 ] || fail "test options were not forwarded to all library suites"

# The final report must count actual test outcomes rather than confusing
# compiler build diagnostics with test ERRORs.  Exercise this on a tiny
# synthetic result set, including the verbose missing-reference DIFFERENT form.
reportroot="$work/report-root"
mkdir -p "$reportroot/testout/aldor" "$reportroot/testout/lib/axllib" \
         "$reportroot/testout/lib/aldor" "$reportroot/testout/lib/algebra"
cat > "$reportroot/testout/aldor/tout-ok.log" <<'EOF2'
>> s-ok: (script) OK
>> s-diff: (script) DIFFERENT < reference missing; new output follows >
>> s-err: (script) ERROR
EOF2
: > "$reportroot/testout/aldor/tout-diff.log"
cat > "$reportroot/testout/lib/axllib/results.tsv" <<'EOF2'
suite	class	test	kind	status
axllib	ok	a	generation .c	OK
axllib	ok	b	generation .c	DIFFERENT
axllib	ok	c	execution	ERROR
EOF2
cat > "$reportroot/testout/lib/aldor/results.tsv" <<'EOF2'
suite	variant	test	status	reference	actual
libaldor	regular	a	OK	-	-
EOF2
cat > "$reportroot/testout/lib/algebra/results.tsv" <<'EOF2'
suite	variant	test	status	reference	actual
algebra	std	a	DIFFERENT	-	-
EOF2
cat > "$reportroot/test-classification.tsv" <<'EOF2'
suite	class	test	kind	status	classification
algebra	blocked	all	runtime	NOT-RUN	BLOCKED
EOF2
: > "$reportroot/build.log"
bash "$distro/modernization/make-test-report.sh" "$reportroot" "$distro" \
  > "$work/report.out"
grep -A8 '^FINAL PRIMARY TEST RESULT TOTALS' "$work/report.out" > "$work/report.final"
grep -q '^OK:        3$' "$work/report.final" || fail "final report OK total is wrong"
grep -q '^DIFFERENT: 3$' "$work/report.final" || fail "final report DIFFERENT total is wrong"
grep -q '^ERROR:     2$' "$work/report.final" || fail "final report ERROR total is wrong"
grep -q '^UNPARSED:  0$' "$work/report.final" || fail "final report has unparsed rows"
grep -q '^RECORDED:  8$' "$work/report.final" || fail "final report recorded total is wrong"
grep -q '^UNTESTED GROUPS: 1$' "$work/report.final" || fail "final report untested count is wrong"

# Diagnostic cleanliness is a named validation gate.  A warning in build.log
# must leave a status artifact and an explicit failure line even though the
# later compiler-dependent gates cannot run in this synthetic root.
diagroot="$work/diag-root"
mkdir -p "$diagroot/bin"
printf '#!/bin/sh\nexit 0\n' > "$diagroot/bin/aldor"
chmod +x "$diagroot/bin/aldor"
printf 'synthetic.c:1: warning: deliberate regression probe\n' > "$diagroot/build.log"
set +e
bash "$distro/modernization/validate.sh" "$diagroot" "$distro" \
  > "$work/diag-validate.out" 2>&1
diagrc=$?
set -e
[ "$diagrc" -ne 0 ] || fail "warning-bearing build.log passed validation"
[ "$(cat "$diagroot/modernization-validation/build-diagnostics.status")" = 1 ] || \
  fail "build-diagnostics failure status was not recorded"
grep -q 'modernization release gate: build-diagnostics FAIL' \
  "$work/diag-validate.out" || fail "build-diagnostics failure was anonymous"

echo "PASS: test harness regression checks"
