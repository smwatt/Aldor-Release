#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_fhensel  "$@"
    compileAldorIntoLib sit_zfactor  "$@"
    compileAldorIntoLib sit_zfring   "$@"
    compileAldorIntoLib sit_zfringg  "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
