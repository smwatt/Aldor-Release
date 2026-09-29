#!/bin/bash

doBuild() {
    compileAldorIntoLib sal_gener   "$@"
    compileAldorIntoLib sal_base    "$@"
    compileAldorIntoLib sal_order   "$@"
    compileAldorIntoLib sal_copy    "$@"
    compileAldorIntoLib sal_bstream "$@"
    compileAldorIntoLib sal_tstream "$@"
    compileAldorIntoLib sal_htype   "$@"
    compileAldorIntoLib sal_torder  "$@"
    compileAldorIntoLib sal_serial  "$@"
    compileAldorIntoLib sal_syntax  "$@"
    compileAldorIntoLib sal_otype   "$@"
    compileAldorIntoLib sal_itype   "$@"
    compileAldorIntoLib sal_byte    "$@"
    compileAldorIntoLib sal_char    "$@"
    compileAldorIntoLib sal_manip   "$@"
    compileAldorIntoLib sal_partial "$@"
    compileAldorIntoLib ald_pfunc   "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
