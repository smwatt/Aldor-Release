#!/bin/bash

doBuild() {
    installc bin unnum
    
    # Only on some platforms (e.g. AIX)
    #installc bin renum
}
    
source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
