#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_hensela   "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
