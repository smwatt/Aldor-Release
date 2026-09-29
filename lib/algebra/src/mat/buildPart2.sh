#!/bin/bash

doBuild() {
    (cd linalg2 ;  bash build.sh build "$@" )
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
