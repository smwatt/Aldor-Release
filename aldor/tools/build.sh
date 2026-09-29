#!/bin/bash

doBuild() {
    (cd unix ; bash build.sh build "$@" ) || return $?
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
