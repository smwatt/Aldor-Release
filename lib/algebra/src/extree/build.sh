#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_extree   "$@" -Mno-ALDOR_W_GenDomFunNotConst
    compileAldorIntoLib sit_optools  "$@"

    (cd operators; bash build.sh build "$@" ) || return $?
    (cd parser;    bash build.sh build "$@" ) || return $?
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
