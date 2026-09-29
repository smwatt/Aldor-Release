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

echo '== Compiling expfact.c into expfact.o'
unicl -c "-I$ALDORROOT/include" "$SRC/expfact.c" > /dev/null

echo '== Compiling exptoc.as into exptoc executable'
cp "$SRC/exptoc.as" .
aldor $LM -Mno-ALDOR_W_CantUseArchive -laxllib -F x exptoc.as expfact.$OBJ

echo '== Executing exptoc'
ensureExecutable exptoc$EXE
./exptoc$EXE

echo '== Cleaning up'
rm -f expfact.$OBJ exptoc.as exptoc$EXE

echo '== Done'
