#!/bin/bash
# Copyright (C) 2026 Stephen M. Watt.
# See legal/LICENSE in the Aldor distribution for licensing terms.
set -euo pipefail

ROOT="${1:?usage: where-scope-regression.sh ALDORROOT [source-root]}"
SRCROOT="${2:-$(cd "$(dirname "$0")/../.." && pwd)}"
OUT="$ROOT/modernization-validation/where-scope"
rm -rf "$OUT"
mkdir -p "$OUT/tmp"
export ALDORROOT="$ROOT"
export ALDORTMP="$OUT/tmp"

cd "$OUT"
"$ROOT/bin/aldor" -l axllib "$SRCROOT/modernization/tests/where-scope.as" \
        >positive.out 2>positive.err
[ ! -s positive.out ]
[ ! -s positive.err ]
[ -s where-scope.ao ]

set +e
"$ROOT/bin/aldor" -l axllib \
        "$SRCROOT/modernization/tests/where-scope-noescape.as" \
        >negative.out 2>negative.err
rc=$?
set -e
[ "$rc" -ne 0 ]
grep -q "No meaning for identifier .*n" negative.out

echo "where scope regression: PASS"
