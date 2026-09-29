#!/bin/bash

doBuild() {
    (cd linalg3 ;  bash build.sh build "$@" )
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
