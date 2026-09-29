#!/bin/bash

doBuild() {
    compileAldorIntoLib sal_bool    "$@"
    compileAldorIntoLib sal_arith   "$@"
    compileAldorIntoLib sal_oarith  "$@"
    compileAldorIntoLib sal_intcat  "$@"
    compileAldorIntoLib sal_bsearch "$@"
    compileAldorIntoLib sal_segment "$@"
    compileAldorIntoLib sal_binpow  "$@"
    compileAldorIntoLib sal_itools  "$@"
    compileAldorIntoLib sal_mint    "$@"
    compileAldorIntoLib sal_random  "$@"
    compileAldorIntoLib sal_int     "$@"
    compileAldorIntoLib sal_pointer "$@"
    compileAldorIntoLib sal_fltcat  "$@"
    compileAldorIntoLib sal_ftools  "$@"
    compileAldorIntoLib sal_sfloat  "$@"
    compileAldorIntoLib sal_dfloat  "$@"
    compileAldorIntoLib sal_lincomb "$@"
    compileAldorIntoLib sal_complex "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
