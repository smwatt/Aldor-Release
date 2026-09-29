#!/bin/bash

doBuild() {
    compileAldorIntoLib sm_exppkgt   "$@"
    compileAldorIntoLib sm_dmp0pkgt  "$@"
    compileAldorIntoLib sm_pr0pkgt   "$@"
    compileAldorIntoLib alg_bivtst1  "$@"
    compileAldorIntoLib alg_bivtst2  "$@"
    compileAldorIntoLib alg_bivtst3  "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
