#!/bin/bash

doBuild() {
    ( cd gmp    ; bash build.sh build "$@" ) || return $?

    ( cd frisco ; bash build.sh build "$@" ) || return $?
}
    
source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
