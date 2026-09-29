#!/bin/bash

SrcDir=`pwd`
AldDir="$SrcDir/../../../src"
CmmDir="$SrcDir/../cmm"

FoamFiles="
        axlport.h platform.h cconfig.h cport.h fenvport.h
        os_unix.c0 os_macosx_vm.c0 os_windows.c0
        editlevels.h axlgen.h axlgen0.h debug.h
        fluid.h format.h test.h opsys.h
        btree.h bigint.h dword.h util.h table.h format.h xfloat.h
        cport.cpp opsys.cpp btree.cpp bigint.cpp dword.cpp
        util.cpp table.cpp format.cpp xfloat.cpp foam_c.h foam_c0.h foam_cfp.h
        foamopt.h compopt.h optcfg.h output.h
        foam_c.cpp foam_cfp.cpp foamopt.cpp compopt.cpp output.cpp
"

createTmpDir() {
    local Tmp="$1"
    mkdir -p "$Tmp"
    local f
    for f in $FoamFiles ; do cpCmd "$AldDir/$f" "$Tmp/$f" ; done
    cpCmd "$SrcDir/store.h" "$Tmp/store.h"
}

deleteTmpDir() {
    rm -rf "$1"
}

doCleanup() {
    deleteTmpDir
}

doBuild() {
    # Temporarily copy into a working directory in $LIBPATH so
    # #include "store.h" finds it, instead of $AldDir/store.h.
    local TmpDir="tmp.$$"
    if [ -z "$ALDORTMP" ] ; then
        TmpDir="/tmp/$TmpDir"
    else
        TmpDir="$ALDORTMP/$TmpDir"
    fi
    createTmpDir "$TmpDir"

    # May need to exclude -I$CmmDir from stdc/opsys
    local opts=(-I"$CmmDir" -DFOAM_RTS -O -DNDEBUG)
    local f
    for f in cport opsys foam_c ; do
        compileCppIntoLib "$TmpDir/$f" libfoam-car "${opts[@]}"
    done
    for f in btree bigint table util output xfloat foamopt dword compopt format ; do
        compileCppIntoLib "$TmpDir/$f" libfoam-car "${opts[@]}"
    done

    deleteTmpDir "$TmpDir"


    (
        cd "$LIBPATH"
        uniar x "$LIBPATH/libfoam.al" runtime.ao

        aldor -Csmax=0 -O -Fo -Flsp runtime.ao
        arReplaceCmd libfoam-car.$ALDOR_LIBEXT runtime.$ALDOR_OBJEXT
        rm -f runtime.$ALDOR_OBJEXT runtime.ao

        ranlibCmd libfoam-car.$ALDOR_LIBEXT
    )
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
