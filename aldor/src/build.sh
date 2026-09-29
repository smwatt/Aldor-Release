#!/bin/bash

# This sets a number of variable, and echos the OkFiles list.

source OkFiles.sh >/dev/null

doBuild() {
    # Compiler implementation files are C++20.  Do not feed the C dialect
    # selector from GLOBAL_CFLAGS to HOST_CXX; compileCppIntoLib already adds
    # GLOBAL_CXXFLAGS.  C runtime/generated-C builds use compileCIntoLib in
    # their own build scripts and retain their C standard policy.
    export CFLAGS="-g -DUNUSED_LABELS -DNDEBUG -DTEST_ALL"

    #CFLAGS="-g -DTEST_ALL -DSTO_DEBUG_DISPLAY" # -DSTO_CAN_BLACKLIST
    #CFLAGS="-O -DTEST_ALL -DUnBPack"
    #CFLAGS="-pg -DTEST_ALL -DNLOCAL"
    #CFLAGS="-O -DNDEBUG"
    #CFLAGS="-O -DNDEBUG -DTEST_ALL -DUSE_MEMORY_CLIMATE"
    #CFLAGS="-g -O3  -DNDEBUG -DTEST_ALL"
    #CFLAGS="-g -O3  -DUNUSED_LABELS -DNDEBUG -DTEST_ALL"


    #
    # Ensure target directories exist
    #
    echo "--- target directories"
    mkdir -p "$ALDORROOT/include"
    mkdir -p "$ALDORROOT/bin"
    mkdir -p "$ALDORROOT/lib"

    #
    # The include directory
    #
    echo "--- include directories"
    cpCmd basic.typ          "$ALDORROOT/include"
    cpCmd aldor.conf         "$ALDORROOT/include"
    cpCmd sample.terminfo    "$ALDORROOT/include"

    # foam_c.h is the public generated-C/runtime ABI header.  Install its
    # complete quoted-header dependency closure so a genuinely fresh
    # ALDORROOT does not depend on stale headers from an earlier build.
    for file in \
        foam_c.h foam_c0.h cport.h platform.h cconfig.h fenvport.h optcfg.h foamopt.h
    do
        cpCmd "$file" "$ALDORROOT/include"
    done

    #
    # Grammar
    #
    # axl_y.c is checked-in release source.  Ordinary builds must not
    # regenerate it merely because checkout or patching changed mtimes:
    # doing so makes the source depend on the host yacc/bison version.
    # Regeneration is therefore an explicit developer action.
    echo "--- grammar"
    if [ "${ALDOR_REGENERATE_GRAMMAR:-0}" = 1 ] ; then
        echo "--- Regenerating axl_y.c ---"
        if [ ! -w . ] ; then
            echo "*** Error *** Need writable source directory" >&2
            return 1
        fi
        (
            srcdir=`pwd`
            cd "$ALDORTMP"

            zacc -p -y axl_y.yt -c axl_y_temp.c "$srcdir/axl.y"

            sed -f "$srcdir/axl_y.sed" axl_y_temp.c > "$srcdir/axl_y.c"
            rmCmd axl_y_temp.c
        ) || return $?
    elif [ ! -r axl_y.c ] ; then
        echo "*** Error *** Missing checked-in generated parser axl_y.c" >&2
        return 1
    fi

    #
    # Libraries
    #
    echo "--- library"
    rmCmd "$LIBPATH/libascomp.$ALDOR_LIBEXT"
    local file
    for file in $TopCpp $PhaseCpp $ObjectsCpp $GeneralCpp $PortCpp $LayoutCpp
    do
        compileCppIntoLib ${file%.cpp} libascomp
    done
    # Shared implementation files retain .c because they are also C artifacts
    # or are built as C elsewhere.  The compiler consumes them as C++20.
    for file in $ObjectsSharedC $MadeC $GeneralC $PortC $TestC
    do
        compileCAsCppIntoLib ${file%.c} libascomp
    done
    for file in $TestCpp
    do
        compileCppIntoLib ${file%.cpp} libascomp
    done
    (cd "$LIBPATH" ; ranlibCmd "libascomp.$ALDOR_LIBEXT" )


    #
    # Main programs
    #
    buildmain() { (
        local SrcDir=`pwd`
        cd "$ALDORROOT/bin"
        local target="$2$ALDOR_EXEEXT"
        "$HOST_CXX" $GLOBAL_CXXFLAGS $CFLAGS -I"$SrcDir" -o "$target" \
            "$SrcDir/$1.cpp" "$LIBPATH/libascomp.$ALDOR_LIBEXT" -lm
        ensureExecutable "$target"
    ) }

    echo "--- aldor_t"
    buildmain main_t aldor_t
    echo "--- aldor"
    buildmain main   aldor

    # Hook to pause so that one can unquarantine by hand, if needed.
    if [ -n "$ALDOR_PAUSE_BUILD" ] ; then
        echo -n "Pausing build.  Hit enter to continue."
        read
    fi

    #
    # Message databases
    #
    echo "--- message databases"
    (
        local SrcDir=`pwd`
        cd "$ALDORROOT/lib"

        # The compiler sources carry the generated default message database,
        # but the low-level message self-test also exercises loading an
        # external catalog.  Rebuild and install its Pig Latin catalog on
        # every compiler build so a clean ALDORROOT contains the fixture.
        "$ALDORROOT/toolbin/atinlay" < "$SrcDir/comsgdb.msg" >comsgpig.msg
        "$ALDORROOT/bin/msgcat" -cat -detab comsgpig
        rmCmd comsgpig.msg
    )

    return 0
}

source "$ALDORROOT/toolbin/build-fns.sh" ; doMain "$@"
