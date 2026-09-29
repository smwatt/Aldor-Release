#!/bin/sh
#
# This script tests the aldor -Fc++ command line argument.

SRC=`pwd`
ALDOR=aldor
OBJ=$(platform --object-suffix)
EXE=$(platform --executable-suffix)

SINK=/dev/null
cd "${TMPDIR-/tmp}"
if [ "$CPP" = "" ] ; then CPP=g++ ; fi
echo "== Generating C++ from mylist.as"
$ALDOR -Fc++ "$SRC/mylist.as" >> $SINK
echo "-- The files are:" mylist_*
for f in mylist_* ; do
  echo "**************************" $f
  cat "$f"
done

echo "== Compiling generated C++ "
$ALDOR "-I$SRC" -fo mylist_as.as
ls mylist_as.as mylist_as.$OBJ mylist_cc.h | sed "s/\.obj/.o/"

echo "== Linking program"
$CPP -Wno-deprecated -I. "-I$ALDORROOT/include" "$SRC/appli2.C" \
    mylist_as.$OBJ "-L$ALDORROOT/lib" -laxllib -lfoam -o appli2 >> $SINK

echo "== Running program"
./appli2$EXE
echo "== Removing all output files."
rm -f mylist_* appli2$EXE

