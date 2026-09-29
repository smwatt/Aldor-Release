#!/usr/bin/env bash
# Copyright (C) 2026 Stephen M. Watt.
# See legal/LICENSE in the Aldor distribution for licensing terms.
set -euo pipefail

ROOT="${1:?usage: domain-specialization-regression.sh ALDORROOT [source-root]}"
SRCROOT="${2:-$(cd "$(dirname "$0")/../.." && pwd)}"
WORK="$ROOT/testout/domain-specialization-regression"

export ALDORROOT="$ROOT"
export PATH="$ROOT/bin:$ROOT/toolbin:$PATH"
rm -rf "$WORK"
mkdir -p "$WORK"
cp "$SRCROOT/modernization/tests/domain-specialization-regression.as" "$WORK/test.as"
cd "$WORK"

aldor -Ffm -Q3 -y"$ROOT/lib" -laldor test.as >foam-compile.out 2>foam-compile.err
[ ! -s foam-compile.err ]
[ "$(grep -c 'BCall SIntEQ' test.fm)" -eq 1 ]
[ "$(grep -c 'BCall BIntEQ' test.fm)" -eq 1 ]

# The FOAM assertions above are the regression: they prove that the two
# concrete instantiations resolve to distinct implementations.  Do not link
# this tiny standalone program here: libfoam and libfoamlib have a historical
# circular static-archive dependency whose link order is outside this test.
