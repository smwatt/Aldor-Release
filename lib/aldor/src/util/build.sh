#!/bin/bash

doBuild() {
    compileAldorIntoLib sal_timer   "$@"
    compileAldorIntoLib sal_file    "$@"
    compileAldorIntoLib sal_version "$@"
    compileAldorIntoLib sal_cmdline "$@"
    compileAldorIntoLib sal_agat    "$@"
    compileAldorIntoLib ald_trace   "$@"
    compileAldorIntoLib rtexns      "$@"
    compileAldorIntoLib eio_rsto    "$@"

}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"

