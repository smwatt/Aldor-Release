#!/bin/sh
#
# This script tests replacement compilation of archive members.

DOALDOR=doaldor


# If $ALDORTMP is not defined, use /tmp.
if [ Z"$ALDORTMP" = Z ] ; then TMPDIR=/tmp ;  else TMPDIR="$ALDORTMP" ; fi


LIB=${TMPDIR}/lib
mkdir -p "${LIB}"
LAXLREP=${LIB}/librepl.al
LOBREP=${LIB}/librepl.a
LOBREPLIB=${LIB}/librepl.lib

# Start clean, in case the test is being re-run.
rm -rf "$LAXLREP" "$LOBREP" "$LOBREPLIB"

# 'ar rcv' avoids the archive creation warning from ar.
uniar rcv "$LAXLREP" /dev/null >/dev/null 2>&1
doar rcv "$LOBREP" /dev/null >/dev/null 2>&1

#
# The perl -p -e 's/-R *[^ ]* //g' removes the use of a -R option with an 
# absolute path so that it will remain platform independent
#
# The perl -p -e 's/-M no-ALDOR_W_OverRideLibraryFile //' eliminates the 
# -M no-ALDOR_W_OverRideLibraryFile so that it will match the installed output
#
$DOALDOR -l "$LIB" arrepl1a repl 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g' | sed -e 's/-M no-ALDOR_W_OverRideLibraryFile //'
$DOALDOR -l "$LIB" arrepl1b repl 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g' | sed -e 's/-M no-ALDOR_W_OverRideLibraryFile //'
$DOALDOR -l "$LIB" arrepl1c repl 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g' | sed -e 's/-M no-ALDOR_W_OverRideLibraryFile //'
$DOALDOR -l "$LIB" arrepl1a repl -Mno-warnings -D AddExport 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g' | sed -e 's/-M no-ALDOR_W_OverRideLibraryFile //'
#$DOALDOR arrepl1b repl -Mno-warnings
$DOALDOR -l "$LIB" arrepl1c repl -Mno-warnings 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g' | sed -e 's/-M no-ALDOR_W_OverRideLibraryFile //'

rm -rf "$LAXLREP" "$LOBREP" "$LOBREPLIB"
