#!/bin/bash

doBuild() {
    # Tools delivered to end-user [bin directory]

    SRC=../../src

    NEED="
        $SRC/file.cpp $SRC/fname.cpp $SRC/opsys.cpp
        $SRC/store.cpp $SRC/sto_debug.cpp $SRC/memclim.cpp
        $SRC/btree.cpp $SRC/cport.cpp $SRC/strops.cpp
        $SRC/debug.cpp $SRC/xfloat.cpp $SRC/buffer.cpp $SRC/timer.cpp
        $SRC/util.cpp $SRC/list.cpp $SRC/format.cpp $SRC/cfgfile.cpp
    "

    installcpp bin unicl -g -I"$SRC" $NEED
    installcpp bin uniar -I"$SRC" $SRC/alar.cpp
    installcpp bin platform -I"$SRC"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
