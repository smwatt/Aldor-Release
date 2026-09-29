#!/bin/bash

doBuild() {
    ( cd src ;     bash build.sh build ) || return $?
}

doTest() {
    ( cd test ; bash build.sh test "$@" ) || return $?
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
