#!/bin/sh
#
# This script tests an invalid empty archive.

# If $ALDORTMP is not defined, use /tmp.
if [ Z"$ALDORTMP" = Z"" ] ; then TMPDIR=/tmp ;  else TMPDIR="$ALDORTMP" ; fi

rm -rf "$TMPDIR/lib"
mkdir "$TMPDIR/lib"

ALDOR=aldor

echo '== Building an archive containing triv*.ao'
touch "$TMPDIR/lib/libtriv.al"

echo '== Creating a client for the archive'
cat << END_libarch.as > "$TMPDIR/libarch.as"

-- A minimal program depending on archives of compiler libraries

#library Triv "triv4.ao"

import from Triv;

printTriv();

END_libarch.as

echo '== Testing that the client imports from the archive'
cd "$TMPDIR"
$ALDOR -Ccc=unicl -Grun -Y "$TMPDIR/lib" -ltriv libarch.as

rm -rf "$TMPDIR/lib" "$TMPDIR/libarch.as"

echo '== Done'
