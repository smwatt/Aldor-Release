#!/bin/bash

doBuild() {
    compileAldorIntoLib sm_vt       "$@"
    compileAldorIntoLib sm_fvt      "$@"
    compileAldorIntoLib sm_osymbol  "$@"
    compileAldorIntoLib sm_listovar "$@"
    compileAldorIntoLib sm_tuplovar "$@"
    compileAldorIntoLib sm_expocat  "$@"
    compileAldorIntoLib sm_dirprodc "$@"
    compileAldorIntoLib sm_mievc    "$@"
    compileAldorIntoLib sm_zevc     "$@"
    compileAldorIntoLib sm_dirprod  "$@"
    compileAldorIntoLib sm_midrl    "$@"
    compileAldorIntoLib sm_midl     "$@"
    compileAldorIntoLib sm_milex    "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
