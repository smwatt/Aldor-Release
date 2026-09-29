#!/bin/bash

doBuild() {
    local SrcDir=`pwd`
    local AldDir="$SrcDir/../../src"

    local opts=(-I"$AldDir" -DFOAM_RTS -O -DNDEBUG)

    # The FOAM runtime implementation is C++20 in v0.11.  Keep the GMP
    # bridge itself as C, but build the shared runtime units with the same
    # C++20 implementation language as the ordinary libfoam variant.
    for f in \
        cport opsys btree util foam_c output table xfloat foamopt \
        dword compopt store sto_debug bigint timer format foam_cfp
    do
        compileCppIntoLib "$AldDir/$f" libfoam-gmp "${opts[@]}" || return $?
    done

    local gmpCArgs=("${opts[@]}")
    [ -z "${ALDOR_GMP_INCLUDE_DIR:-}" ] || gmpCArgs+=("-I$ALDOR_GMP_INCLUDE_DIR")
    compileCIntoLib "$SrcDir/fm_gmp" libfoam-gmp "${gmpCArgs[@]}" || return $?

    (
        cd "$LIBPATH"
        uniar x "$LIBPATH/libfoam.al" runtime.ao

        aldor -Csmax=0 -Q3 -Qinline-all -M no-ALDOR_W_OverRideLibraryFile \
              -Fo -Flsp runtime.ao
        arReplaceCmd libfoam-gmp.$ALDOR_LIBEXT runtime.$ALDOR_OBJEXT
        rm -f runtime.$ALDOR_OBJEXT runtime.ao 

        ranlibCmd libfoam-gmp.$ALDOR_LIBEXT
    )
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
