#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_heugcd   "$@"
    compileAldorIntoLib sit_modpgcd  "$@"
    compileAldorIntoLib sit_modgcd   "$@"
    compileAldorIntoLib sit_gcdint   "$@"
    compileAldorIntoLib sit_gcdintg  "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
