#!/bin/bash
    
doBuild() {
    (cd gcd ; bash build.sh build "$@" )
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
