#!/bin/bash

doBuild() {
    export PATH="$ALDORROOT/bin:$PATH"

    #
    # Set up directory variables
    #
    local AldorSrc=`( cd ../../src ; pwd )`
    
    #
    # Install include files
    #
    # FIXME: Should clean so no .h needed other than foam_c.h
    cpCmd "$AldorSrc/foam_c.h"   "$ALDORROOT/include"
    cpCmd "$AldorSrc/foam_c0.h"  "$ALDORROOT/include"
    cpCmd "$AldorSrc/optcfg.h"   "$ALDORROOT/include"
    cpCmd "$AldorSrc/foamopt.h"  "$ALDORROOT/include"
    cpCmd "$AldorSrc/cconfig.h"  "$ALDORROOT/include"
    cpCmd "$AldorSrc/platform.h" "$ALDORROOT/include"


    #
    # Install run time for lisp
    #
    cpCmd foam_l.lsp "$ALDORROOT/lib"

    
    #
    # Build library
    #

    ( cd "$LIBPATH" ; rmCmd libfoam.$ALDOR_LIBEXT )

    # Compile the remaining C substrate and the C++20 runtime units.

    # Keep pathname-bearing options out of scalar CFLAGS: the generic build
    # helper intentionally word-splits CFLAGS, which breaks source paths that
    # contain spaces (for example, "Aldor Spin" on macOS).  Pass the include
    # directory as a separately quoted argument below.
    export CFLAGS="-DFOAM_RTS -O -DNDEBUG"

    local CSrcFiles="
    "
    local CppSrcFiles="
        cport.cpp opsys.cpp store.cpp sto_debug.cpp memclim.cpp
        foam_c.cpp foam_cfp.cpp foam_i.cpp
        dword.cpp util.cpp xfloat.cpp output.cpp foamopt.cpp compopt.cpp
        timer.cpp format.cpp table.cpp btree.cpp bigint.cpp
    "

    local f
    for f in $CSrcFiles ; do
        compileCIntoLib "$AldorSrc"/${f%.c} libfoam
    done
    for f in $CppSrcFiles ; do
        compileCppIntoLib "$AldorSrc"/${f%.cpp} libfoam "-I$AldorSrc"
    done


    # Optional allocator-debug runtime.  Production libfoam.$ALDOR_LIBEXT has no
    # per-allocation debug wrappers or tracking machinery.
    ( cd "$LIBPATH" ; rmCmd libfoam-debug.$ALDOR_LIBEXT )
    local saveCFLAGS="$CFLAGS"
    export CFLAGS="$CFLAGS -DSTO_DEBUG_ALLOCATOR"
    for f in $CSrcFiles ; do
        compileCIntoLib "$AldorSrc"/${f%.c} libfoam-debug
    done
    for f in $CppSrcFiles ; do
        compileCppIntoLib "$AldorSrc"/${f%.cpp} libfoam-debug "-I$AldorSrc"
    done
    export CFLAGS="$saveCFLAGS"
    ( cd "$LIBPATH" ; ranlibCmd libfoam-debug.$ALDOR_LIBEXT )

    # Prepend with UNICL=-Wv=1 to see exact compile command executed.
    # Prepend with UNICL=-Wv=3 to also unicl command line.
    compileAldorIntoLib runtime libfoam -Fao -Fo -Flsp -Fc \
    	-Q3 -Qinline-all -Qno-cc -Csmax=0 -Cargs=-g \
    	-W runtime -W check -M no-ALDOR_W_OverRideLibraryFile

    # Check there are no "gets".
    grep 'fiFileInitializer(' "$LIBPATH/runtime.c" | \
      sed -e 's/^[^"]*"\([^"]*\).*$$/*** Error: gets not allowed from \1./' | \
      grep -v rtexns || true
    rmCmd "$LIBPATH/runtime.c"
    
    ( cd "$LIBPATH" ; ranlibCmd libfoam.$ALDOR_LIBEXT )
}
    
source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
