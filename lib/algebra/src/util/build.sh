#!/bin/bash

doBuild() {
    compileAldorIntoLib alg_version   "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
