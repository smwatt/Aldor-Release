#!/bin/bash

doBuild() {
    compileAldorIntoLib sm_rmp    "$@"
    compileAldorIntoLib sm_rmpzx  "$@"
    compileAldorIntoLib sm_rmpz   "$@"
    compileAldorIntoLib alg_smp   "$@"
    compileAldorIntoLib sm_dmp0   "$@"
    compileAldorIntoLib sm_dmp1   "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
