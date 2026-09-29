#!/bin/sh
#
# This script tests importing from archives of compiler libraries. 

# If ${TMPDIR} is not defined, just use /tmp.
TMPDIR=${TMPDIR-/tmp}

rm -rf "$TMPDIR/lib"
mkdir "$TMPDIR/lib"

ALDOR=aldor
OBJ=$(platform --object-suffix)
LM=$(platform --math-library)

echo '== Compiling triv*.as into triv*.ao and triv*.o'
$ALDOR -Mno-ALDOR_W_CantUseArchive $LM -l axllib -R "$TMPDIR" -F ao -F o triv*.as 2>&1  | grep -v "warning: conflicting types for built-in function"

echo '== Building an archive containing triv*.ao'
uniar cr "$TMPDIR/lib/libtriv.al" "$TMPDIR"/triv*.ao
rm -f "$TMPDIR"/triv*.ao

echo '== Building an archive containing triv*.o'
doar cr "$TMPDIR/lib/libtriv.a" "$TMPDIR"/triv*.$OBJ
doranlib "$TMPDIR/lib/libtriv.a"

rm -f "$TMPDIR"/triv*.$OBJ

echo '== Creating a client for the archive'
cat << END_libarch.as > "$TMPDIR/libarch.as"

-- A minimal program depending on archives of compiler libraries

#library Triv "triv4.ao"

import from Triv;

printTriv();

END_libarch.as

echo '== Testing that the client imports from the archive'
cd "$TMPDIR"
$ALDOR -Mno-ALDOR_W_CantUseArchive $LM -l axllib -Grun -Y "$TMPDIR/lib" -ltriv "$TMPDIR/libarch.as"

rm -rf "$TMPDIR/lib" "$TMPDIR/libarch.as"

echo '== Done'
