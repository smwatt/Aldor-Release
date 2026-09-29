#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_prfcat   "$@"
    compileAldorIntoLib sit_sprfgcd  "$@"
    compileAldorIntoLib sit_sprfmat  "$@"
    compileAldorIntoLib sit_sprfcat  "$@"
    compileAldorIntoLib sit_spf      "$@"
    compileAldorIntoLib sit_zpf      "$@"
    compileAldorIntoLib alg_pf2      "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
