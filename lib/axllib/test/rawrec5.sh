#!/bin/sh
#
# This script tests passing raw records to C.

# If ${TMPDIR} is not defined, just use /tmp.
TMPDIR=${TMPDIR-/tmp}

OBJ=$(platform --object-suffix)
EXE=$(platform --executable-suffix)
LM=$(platform --math-library)

echo '== Creating temporary copies ...'
cp rawrec5.as "$TMPDIR"
cp cpxadd.c "$TMPDIR"

echo '== Moving to temporary directory ...'
cd "$TMPDIR"

echo '== Compiling C file'
unicl -c cpxadd.c > /dev/null

echo '== Running Aldor program ...'
aldor $LM -Mno-ALDOR_W_CantUseArchive -Grun -Q3 -laxllib rawrec5.as cpxadd.$OBJ 2>&1 | grep -v 'makes integer from pointer without a cast' | grep -v 'rawrec5.c: In function'

echo '== Cleaning up ...'
/bin/rm -f rawrec5.as
/bin/rm -f cpxadd.c
/bin/rm -f cpxadd.$OBJ

echo '== Done!'

