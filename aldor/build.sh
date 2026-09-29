#!/bin/bash

doBuild() {
    (cd tools;   bash build.sh build "$@" ) || return $?
    (cd subcmd;  bash build.sh build "$@" ) || return $?
    (cd src;     bash build.sh build "$@" ) || return $?
    (cd lib;     bash build.sh build "$@" ) || return $?
    (cd contrib; bash build.sh build "$@" ) || return $?
    (cd test;    bash build.sh build "$@" ) || return $?
}
    
doTest() {
    (cd test;    bash build.sh test  "$@" ) || return $?
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
