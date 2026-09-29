#!/usr/bin/env bash
#
# classify-results.sh: Aldor build and development support.
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
ROOT=${1:?usage: classify-results.sh ALDORROOT}
out="$ROOT/test-classification.tsv"
printf 'suite\tclass\ttest\tkind\tstatus\tclassification\n' > "$out"
stale_list="$(cd "$(dirname "$0")" && pwd)/baselines/hash-reference-stale.txt"
smoke=PASS
[ -f "$ROOT/runtime-smoke/FAIL" ] && smoke=FAIL

classify_row() {
    suite=$1 cls=$2 test=$3 kind=$4 status=$5
    case "$status" in
      OK) c=PASS ;;
      *)
        if [ "$suite" = axllib ] && [ "$cls" = ok ] && [ "$kind" != execution ] && \
           grep -qx "$test" "$stale_list" 2>/dev/null; then
            c=REFERENCE_STALE
        elif [ "$smoke" = FAIL ] && { [ "$kind" = execution ] || [ "$kind" = script ]; }; then
            c=BLOCKED
        elif [ "$suite" = axllib ] && [ "$cls" != ok ]; then
            c=EXPECTED_FAIL
        else
            c=SEMANTIC_FAIL
        fi ;;
    esac
    printf '%s\t%s\t%s\t%s\t%s\t%s\n' "$suite" "$cls" "$test" "$kind" "$status" "$c" >> "$out"
}

for f in "$ROOT"/testout/lib/*/results.tsv; do
    [ -f "$f" ] || continue
    header=$(head -n 1 "$f")
    case "$header" in
      $'suite\tclass\ttest\tkind\tstatus')
        while IFS=$'\t' read -r suite cls test kind status; do
            classify_row "$suite" "$cls" "$test" "$kind" "$status"
        done < <(tail -n +2 "$f")
        ;;
      $'suite\tvariant\ttest\tstatus\treference\tactual')
        while IFS=$'\t' read -r suite variant test status reference actual; do
            classify_row "$suite" "$variant" "$test" execution "$status"
        done < <(tail -n +2 "$f")
        ;;
      *)
        echo "Unrecognized results schema in $f: $header" >&2
        exit 2
        ;;
    esac
done

if [ -f "$ROOT/testout/RUNTIME-BLOCKED" ]; then
  printf 'axllib\tblocked\texecution-dependent-tests\truntime\tNOT-RUN\tBLOCKED\n' >> "$out"
fi
if [ -f "$ROOT/testout/lib/aldor/BLOCKED" ]; then
  printf 'libaldor\tblocked\tall\truntime\tNOT-RUN\tBLOCKED\n' >> "$out"
fi
if [ -f "$ROOT/testout/lib/algebra/BLOCKED" ]; then
  printf 'algebra\tblocked\tall\truntime\tNOT-RUN\tBLOCKED\n' >> "$out"
fi
cat "$out"
