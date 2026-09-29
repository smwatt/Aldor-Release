#!/bin/sh
#
# This script tests replacement compilation of archive members.

# If $ALDORTMP is not defined, use /tmp.
if [ Z"$ALDORTMP" = Z ] ; then TMPDIR=/tmp ;  else TMPDIR="$ALDORTMP" ; fi

DOALDOR=doaldor

LIB=${TMPDIR}/lib
mkdir -p "${LIB}"
LAXLREP=${LIB}/librepl.al

LOBREP=${LIB}/librepl.a
LOBREPLIB=${LIB}/librepl.lib

# Start clean, in case the test is being re-run.
rm -rf "$LAXLREP" "$LOBREP" "$LOBREPLIB"

doar rcv "$LOBREP" /dev/null >/dev/null 2>&1

# 'ar rcv' avoids the archive creation warning from ar.
uniar rcv "$LAXLREP" /dev/null >/dev/null 2>&1
#if [ "$P" != "win" ]; then 
#	ar rcv ${LOBREP} /dev/null 2>&1 | grep -v 'ar: writing'
#fi

#
# The sed -e 's/-R *[^ ]* //g' removes the use of a -R option with an 
# absolute path so that it will remain platform independent
#
$DOALDOR -l "$LIB" arrepla repl 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g'
$DOALDOR -l "$LIB" arreplb repl 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g'
$DOALDOR -l "$LIB" arreplc repl 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g'
$DOALDOR -l "$LIB" arrepla repl -Mno-warnings 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g'
$DOALDOR -l "$LIB" arreplb repl -Mno-warnings 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g'
$DOALDOR -l "$LIB" arreplc repl -Mno-warnings 2>&1 | grep -v 'ar: writing' | sed -e 's/-R *[^ ]* //g'

rm -rf "$LAXLREP" "$LOBREP" "$LOBREPLIB"
