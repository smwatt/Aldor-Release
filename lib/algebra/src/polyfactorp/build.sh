#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_upfactp   "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
