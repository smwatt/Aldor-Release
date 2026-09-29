#!/bin/bash
# Copyright (C) 2026 Stephen M. Watt.
# See legal/LICENSE in the Aldor distribution for licensing terms.
set -euo pipefail

ROOT=${1:?usage: aldorhash32-regression.sh ALDORROOT [source-root]}
SRCROOT=${2:-$(cd "$(dirname "$0")/../.." && pwd)}
OUT="$ROOT/aldorhash32-regression"
rm -rf "$OUT"
mkdir -p "$OUT"

export ALDORROOT="$ROOT"
export ALDORTMP="$OUT/tmp"
export PATH="$ROOT/bin:$ROOT/toolbin:$PATH"
mkdir -p "$ALDORTMP"

# The hash remains an SInt carrier, but its operations are on the low 32-bit
# bit pattern.  In particular, bit 31 is data, not a numeric sign.
cat >"$OUT/bits.c" <<'C_EOF'
#include "foam_c.h"
int main(void)
{
        FiSInt min32 = -2147483647L - 1L;
        FiSInt r = fiAldorHash32Combine(min32, 0);
        return r == min32 ? 0 : 1;
}
C_EOF
${HOST_CC:-cc} -std=c99 -pedantic-errors \
        -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE \
        -I"$SRCROOT/aldor/src" -I"$ROOT/include" \
        "$OUT/bits.c" -o "$OUT/bits"
"$OUT/bits"

cat >"$OUT/mininterp.as" <<'AS_EOF'
#include "axllib"
import from SingleInteger;
print << 3 << newline;
AS_EOF

cd "$OUT"
"$ROOT/bin/aldor" -Fc -laxllib mininterp.as >compile.out 2>compile.err
[ ! -s compile.err ]
grep -q '477900237L, 2130877288L' mininterp.c
rm -f mininterp.c

"$ROOT/bin/aldor" -Ginterp -laxllib mininterp.as >ginterp.out 2>ginterp.err
[ ! -s ginterp.err ]
printf '3\n' >expected.out
cmp expected.out ginterp.out

printf '%s\n' \
    '#include "axllib"' \
    'import from SingleInteger;' \
    '1 + 2' >gloop.in
"$ROOT/bin/aldor" -Gloop -laxllib <gloop.in >gloop.out 2>gloop.err
[ ! -s gloop.err ]
grep -q '3 @ SingleInteger' gloop.out

echo 'AldorHash32 compiler/generated-C/interpreter regression: PASS'
