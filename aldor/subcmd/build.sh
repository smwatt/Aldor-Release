#!/bin/bash

doBuild() {
    for dir in lno testaldor unitools ; do
        (cd $dir ; bash build.sh build $*) || return $?
    done
}
    
source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
