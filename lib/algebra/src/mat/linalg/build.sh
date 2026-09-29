#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_bsolve   "$@"
    compileAldorIntoLib sit_overdet  "$@"
    compileAldorIntoLib sit_laring   "$@"
    compileAldorIntoLib sit_linalg   "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
