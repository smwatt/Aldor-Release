#!/bin/bash

doBuild() {
    compileAldorIntoLib sm_delist   "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
