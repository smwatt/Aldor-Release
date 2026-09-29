#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_linelim "$@"
    compileAldorIntoLib sit_oge     "$@"
    compileAldorIntoLib sit_dfge    "$@"
    compileAldorIntoLib sit_hermge  "$@"
    compileAldorIntoLib sit_ffge    "$@"
    compileAldorIntoLib sit_ff2ge   "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
