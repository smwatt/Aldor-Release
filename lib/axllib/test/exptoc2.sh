#!/bin/sh
#
# This script tests exporting an aldor function to a C file

SRC=`pwd`
# If ${TMPDIR} is not defined, just use /tmp.
TMPDIR=${TMPDIR-/tmp}
cd "$TMPDIR"

EXE=$(platform --executable-suffix)
OBJ=$(platform --object-suffix)
LM=$(platform --math-library)

echo '== Compiling expquo.c into expquo.o'
unicl -c "-I$ALDORROOT/include" "$SRC/expquo.c" > /dev/null

echo '== Compiling exptoc2.as into exptoc2 executable'
cp "$SRC/exptoc2.as" .
aldor $LM -Mno-ALDOR_W_CantUseArchive -laxllib -F x exptoc2.as expquo.$OBJ

echo '== Executing exptoc2'
ensureExecutable exptoc2$EXE
./exptoc2$EXE

echo '== Cleaning up'
rm -f expquo.$OBJ exptoc2.as exptoc2$EXE

echo '== Done'
