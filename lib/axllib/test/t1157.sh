#!/bin/sh
#
# This script tests optimisation of character literals.

SRC=`pwd`
SINK=/dev/null
cd "${TMPDIR-/tmp}"

OBJ=$(platform --object-suffix)
EXE=$(platform --executable-suffix)
LM=$(platform --math-library)

cp "$SRC/t1157s.as" "$TMPDIR"
cp "$SRC/t1157m.as" "$TMPDIR"

aldor -laxllib -q2 -fo -fao t1157s.as
# aldor -laxllib -q1 -fx t1157m.as t1157s.$OBJ
aldor -Mno-ALDOR_W_CantUseArchive -laxllib $LM -q1 -fx t1157m.as t1157s.$OBJ
ensureExecutable t1157m$EXE
./t1157m$EXE | diff - t1157s.as
#aldor -laxllib -q2 -fx t1157m.as t1157s.$OBJ
aldor -Mno-ALDOR_W_CantUseArchive -laxllib $LM -q2 -fx t1157m.as t1157s.$OBJ
ensureExecutable t1157m$EXE
./t1157m$EXE | diff - t1157s.as
#aldor -laxllib -q3 -fx t1157m.as t1157s.$OBJ
aldor -Mno-ALDOR_W_CantUseArchive -laxllib $LM -q3 -fx t1157m.as t1157s.$OBJ
ensureExecutable t1157m$EXE
./t1157m$EXE | diff - t1157s.as
