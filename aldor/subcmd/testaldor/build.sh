#!/bin/bash

doBuild() {
    installc toolbin testaldor -g -I../../src  util.c # To run compiler test suite.
}
    
source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
