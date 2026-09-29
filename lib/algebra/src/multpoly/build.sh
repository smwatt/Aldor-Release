#!/bin/bash

doBuild() {
    (cd exponent;     bash build.sh build "$@" ) || return $?
    (cd multpolycat;  bash build.sh build "$@" ) || return $?
    # (cd multpolydata; bash build.sh build "$@" ) -- moved ahead
    (cd multpolydom;  bash build.sh build "$@" ) || return $?
    (cd multpolypkg;  bash build.sh build "$@" ) || return $?
    (cd multpolytest; bash build.sh build "$@" ) || return $?
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
