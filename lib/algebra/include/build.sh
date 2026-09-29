#!/bin/bash

doBuild() {
    cp *.as "$ALDORROOT/include"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
