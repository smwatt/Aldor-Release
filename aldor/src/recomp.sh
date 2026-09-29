#!/bin/bash

# Recompile some source files and rebuild the library
if [ -z "$ALDORROOT" ] ; then echo "ALDORROOT not defined" ; exit 1; fi

CXX=${HOST_CXX:-${CXX:-g++}}
CXXFLAGS="-std=c++20 -g -O3 -DUNUSED_LABELS -DNDEBUG -DTEST_ALL"

AR=doar
ARFLAGS=r
OBJEXT=${ALDOR_OBJEXT:-o}
LIBEXT=${ALDOR_LIBEXT:-a}
EXEEXT=${ALDOR_EXEEXT:-}
if [ "${ALDOR_TOOLCHAIN:-auto}" = msvc ]; then
    echo "recomp.sh does not yet translate MSVC command-line options." >&2
    exit 2
fi

function buildlib () {
    Lib=$1; shift

    cp $ALDORROOT/lib/$Lib .

    for SrcFile in $* ; do
        echo "Compiling $SrcFile"
        ObjFile=`echo $SrcFile | sed -e "s/\\.[cs]/.$OBJEXT/"`
        $CXX $CXXFLAGS -c $SrcFile
        $AR $ARFLAGS $Lib $ObjFile
        rm $ObjFile
    done
    doranlib "$Lib"
    mv $Lib $ALDORROOT/lib
}

# Libraries
echo "--- library"
buildlib libascomp.$LIBEXT $*

# Main programs
echo "--- aldor_t"
$CXX $CXXFLAGS main_t.cpp -L$ALDORROOT/lib -lascomp -lm -o aldor_t$EXEEXT
mv aldor_t$EXEEXT $ALDORROOT/bin/aldor_t$EXEEXT

echo "--- aldor"
$CXX $CXXFLAGS main.cpp   -L$ALDORROOT/lib -lascomp -lm -o aldor$EXEEXT
mv aldor$EXEEXT $ALDORROOT/bin/aldor$EXEEXT
