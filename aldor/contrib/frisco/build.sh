#!/bin/bash

doBuild() {
    ( cd foam ; bash build.sh build "$@" ) || return $?
}
    
source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
