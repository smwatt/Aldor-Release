#!/bin/bash

doBuild() {
    cp * "$ALDORROOT"/include
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
