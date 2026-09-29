#!/bin/sh
#
# This script tests finding '..' from Segment SingleInteger.

ALDOR=aldor

echo '== Compiling segext[34].as.'
$ALDOR -laxllib -R "$TMPDIR" -Y "$TMPDIR" -lAxiomLib=ax0 segext3.as segext4.as

echo '== The files are:'
(cd "$TMPDIR" ; ls segext*.ao)

echo '== Cleaning up'
rm "$TMPDIR"/segext*.ao
