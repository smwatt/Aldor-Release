#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_indvar  "$@"
    compileAldorIntoLib sit_product "$@"
    compileAldorIntoLib sit_pring   "$@"
    compileAldorIntoLib sit_complex "$@"
    compileAldorIntoLib sit_mkpring "$@"
    compileAldorIntoLib sit_interp  "$@" -Q2  # FIXME compbug requires lower opt
    compileAldorIntoLib sit_shell   "$@" -Q2  # FIXME compbug requires lower opt
    compileAldorIntoLib sit_permut  "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
