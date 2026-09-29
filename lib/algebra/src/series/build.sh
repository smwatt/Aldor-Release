#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_seqence  "$@"
    compileAldorIntoLib sit_sercat   "$@"
    compileAldorIntoLib alg_serpoly  "$@"
    compileAldorIntoLib sit_duts     "$@" -Q1 # FIXME compbug requires lower opt
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
