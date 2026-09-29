#!/bin/bash

doBuild() {
    compileAldorIntoLib alg_bivarpk   "$@"
    compileAldorIntoLib alg_ZpUVres   "$@"
    compileAldorIntoLib alg_mresbiv   "$@"

    # compileAldorIntoLib alg_disolve "$@"
    # compileAldorIntoLib alg_mhensel "$@"
    # compileAldorIntoLib alg_ezgcd   "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
