#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_polkara "$@"
    compileAldorIntoLib alg_sup0    "$@"
    compileAldorIntoLib alg_sup1    "$@"
    compileAldorIntoLib alg_uprcr   "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
