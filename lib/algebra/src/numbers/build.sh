#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_prmtabl "$@"
    compileAldorIntoLib sit_primes  "$@"
    compileAldorIntoLib sit_primgen "$@"
    compileAldorIntoLib sit_prmroot "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
