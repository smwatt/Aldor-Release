#!/bin/bash

doBuild() {
    (cd include; bash build.sh build "$@" ) || return $?
    (cd src;     bash build.sh build "$@" ) || return $?
    (cd test;    bash build.sh build "$@" ) || return $?
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
