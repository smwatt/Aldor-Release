#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_vector   "$@"
    compileAldorIntoLib sit_matcat   "$@"
    compileAldorIntoLib sit_dnsemat  "$@"

    (cd gauss ;   bash build.sh build "$@" )
    (cd modular ; bash build.sh build "$@" )
    (cd linalg ;  bash build.sh build "$@" )
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
