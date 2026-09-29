#!/bin/sh
#
# This script tests $ALDOR -Fs option.

rm -f "$TMPDIR/test0.fm"

ALDOR=aldor

echo '== Compiling test0.as to $TMPDIR/test0.fm'
$ALDOR -R "$TMPDIR" -F fm test0.as > /dev/null
cat "$TMPDIR/test0.fm"
rm  "$TMPDIR/test0.fm"
