#!/bin/sh
#
# This script tests the use of temporary files and warning messages in emit.c.

SRC=`pwd`
ALDOR=aldor
cd "${TMPDIR-/tmp}"

# Make sure the generated lisp code doesn't contain a main expression.
$ALDOR -Zdb -F c "$SRC/test0.as"
mv test0.c test0.c.bak

$ALDOR -Zdb "$SRC/test0.as"
$ALDOR -Zdb -F c test0.ao

diff test0.c.bak test0.c
rm -f test0.c.bak test0.c test0.ao
