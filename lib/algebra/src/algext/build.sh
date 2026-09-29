#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_algext  "$@"
    compileAldorIntoLib sit_upmod   "$@"
    compileAldorIntoLib sit_saexcpt "$@"
    compileAldorIntoLib sit_sae     "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
