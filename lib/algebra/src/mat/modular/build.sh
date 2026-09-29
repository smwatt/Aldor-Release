#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_modpoge  "$@" -Q1  # FIXME compbug so -Q1
    compileAldorIntoLib sit_zcrtla   "$@" -Q1  # FIXME compbug so -Q1
    compileAldorIntoLib sit_speclin  "$@" -Q1  # FIXME compbug so -Q1
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
