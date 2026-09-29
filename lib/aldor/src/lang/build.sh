#!/bin/bash

doBuild() {
    compileAldorIntoLib sal_lang "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
