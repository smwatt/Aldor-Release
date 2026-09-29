#!/bin/bash

doBuild() {
    (cd libfoamlib;  bash build.sh build "$@" ) || return $?
    (cd libfoam;     bash build.sh build "$@" ) || return $?
}
    
source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
