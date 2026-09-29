#!/bin/bash

doBuild() {
    echo "= = = = = = = = = = = = Building the Base Aldor Library"
    ( cd aldor ;        bash build.sh build ) || return $?
    
    echo "= = = = = = = = = = = = Building the AxlLib Library"
    ( cd axllib ;       bash build.sh build ) || return $?

    echo "= = = = = = = = = = = = Building the Ax0 Axiom Library"
    ( cd ax0 ;          bash build.sh build ) || return $?

    echo "= = = = = = = = = = = = Building the AxlDem Library"
    ( cd axldem ;       bash build.sh build ) || return $?
    
    echo "= = = = = = = = = = = = Building the Debug Library"
    ( cd debuglib ;     bash build.sh build ) || return $?

    echo "= = = = = = = = = = = = Building the Algebra Library"
    ( cd algebra ;      bash build.sh build ) || return $?
}

doTest() {
    local overall=0 status

    echo "= = = = = = = = = = = = Aldor Compiler Tests Using Axllib"
    ( cd axllib ;       bash build.sh test "$@" )
    status=$?
    if [ $status -ne 0 ]; then
        overall=$status
        echo "AxlLib test suite reported failures; continuing."
    fi

    if [ "${ALDOR_RUNTIME_BLOCKED:-0}" = 1 ]; then
        echo "= = = = = = = = = = = = Testing Aldor Salli Library"
        echo "BLOCKED: generated-runtime smoke failed"
        mkdir -p "$ALDORROOT/testout/lib/aldor"
        : > "$ALDORROOT/testout/lib/aldor/BLOCKED"

        echo "= = = = = = = = = = = = Testing Sumit Algebra Library"
        echo "BLOCKED: generated-runtime smoke failed"
        mkdir -p "$ALDORROOT/testout/lib/algebra"
        : > "$ALDORROOT/testout/lib/algebra/BLOCKED"
    else
        echo "= = = = = = = = = = = = Testing Aldor Salli Library"
        ( cd aldor/test ;   bash build.sh test "$@" )
        status=$?
        if [ $status -ne 0 ]; then
            [ $overall -ne 0 ] || overall=$status
            echo "Aldor library test suite reported failures; continuing."
        fi

        echo "= = = = = = = = = = = = Testing Sumit Algebra Library"
        ( cd algebra/test ; bash build.sh test "$@" )
        status=$?
        if [ $status -ne 0 ]; then
            [ $overall -ne 0 ] || overall=$status
            echo "Algebra test suite reported failures; continuing."
        fi
    fi

    return $overall
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
