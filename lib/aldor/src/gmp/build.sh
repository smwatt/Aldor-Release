#!/bin/bash

doBuild() {
    local LibBase="$1"
    shift

    compileAldorIntoLib sal_gmptls "$LibBase" "$@"
    compileAldorIntoLib sal_intgmp "$LibBase" "$@"
    compileAldorIntoLib sal_fltgmp "$LibBase" "$@"

    # GMP's comparison routines return C int, while MachineInteger is a
    # machine word (64 bits on LP64 hosts).  Use a C99 ABI shim to perform
    # the required sign extension instead of declaring mpz_cmp as returning
    # MachineInteger in Foreign C.
    gmpCArgs=()
    [ -z "${ALDOR_GMP_INCLUDE_DIR:-}" ] || gmpCArgs+=("-I$ALDOR_GMP_INCLUDE_DIR")
    compileCIntoLib sal_gmpabi "$LibBase" "${gmpCArgs[@]}"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
