#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_popov   "$@"
    compileAldorIntoLib alg_ffupla  "$@"
    compileAldorIntoLib sit_upcrtla "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
