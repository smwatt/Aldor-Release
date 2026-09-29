#!/bin/sh
#
# This script tests importing a C function as a packed map.

SRC=`pwd`
# If ${TMPDIR} is not defined, just use /tmp.
TMPDIR=${TMPDIR-/tmp}
cd "$TMPDIR"

OBJ=$(platform --object-suffix)
EXE=$(platform --executable-suffix)
LM=$(platform --math-library)
ALDOR=aldor

echo '== Compiling packfns.c into packfns.o'
unicl -c "-I$ALDORROOT/include" "$SRC/packfns.c"

echo '== Compiling pack0.as into pack0 executable'
cp "$SRC/pack0.as" .
$ALDOR -F x "-I$SRC" -l axllib $LM -Mno-ALDOR_W_CantUseArchive pack0.as packfns.$OBJ

echo '== Executing pack0'
ensureExecutable pack0$EXE
./pack0$EXE

echo '== Cleaning up'
rm -f packfns.$OBJ pack0.as pack0 pack0$EXE

echo '== Done'
