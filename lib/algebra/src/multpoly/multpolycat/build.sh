#!/bin/bash

doBuild() {
    compileAldorIntoLib alg_stdfrng "$@"
    compileAldorIntoLib alg_poltype "$@"
    compileAldorIntoLib sm_polring0 "$@"
    compileAldorIntoLib alg_defgcd  "$@"
    compileAldorIntoLib sm_famr0    "$@"
    compileAldorIntoLib sm_polring  "$@"
    compileAldorIntoLib sm_rmpcat0  "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
