#!/bin/sh
#
# This script tests splitting generated C files.

SRC=`pwd`
cd "$TMPDIR"

ALDOR=aldor

echo '== Generating split C files for mandel.'
$ALDOR -Csmax=50 -Fc "$SRC/mandel.as"

echo '-- The files are:'
ls mand*

echo '-- The header is:'
cat mandel.h

echo '-- The first file is:'
cat mandel.c

echo '-- The second file is:'
cat mand*01.c

echo '-- Clean up.'
rm mand*.[ch]
