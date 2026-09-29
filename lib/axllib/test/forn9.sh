#!/bin/sh
#
# This script tests the aldor -Fc++ command line argument.

S=`pwd`
SRC=$S
ALDOR=aldor
OBJ=$(platform --object-suffix)
EXE=$(platform --executable-suffix)

SINK=/dev/null
cd "${TMPDIR-/tmp}"
rm -f *ao
if [ "$CPP" = "" ]
then CPP=g++
fi

echo "== Generating C++ from db.as"
$ALDOR -Fc++ "$SRC/db.as" >> $SINK
echo "-- The files are:" db_*
for f in db_* ; do
  echo "**************************" $f
  cat "$f"
done

echo "== Compiling generated C++ "
$ALDOR "-I$S" -fo db_as.as
ls db_as.as db_as.$OBJ db_cc.h | sed "s/\.obj/.o/"

echo "== Linking program"
$CPP -I. "-I$ALDORROOT/include" "$SRC/appli1.C" db_as.$OBJ \
    "-L$ALDORROOT/lib" -laxllib -lfoam -o appli1 >> $SINK
echo "== Running program"
./appli1$EXE
echo "== Removing all output files."
rm -f db_* appli1$EXE

