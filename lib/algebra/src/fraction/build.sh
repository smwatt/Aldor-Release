#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_quotcat   "$@"
    compileAldorIntoLib sit_qotbyc0   "$@"
    compileAldorIntoLib sit_qotfct0   "$@"
    compileAldorIntoLib sit_lcqotct   "$@"
    compileAldorIntoLib sit_vecquot   "$@"
    compileAldorIntoLib sit_matquot   "$@"
    compileAldorIntoLib sit_uflgqot   "$@"
    compileAldorIntoLib sit_qotfcat   "$@"
    compileAldorIntoLib sit_qotbyct   "$@"
    compileAldorIntoLib sit_qotient   "$@"
    compileAldorIntoLib sit_quotby    "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
