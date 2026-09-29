#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_OPand    "$@"
    compileAldorIntoLib sit_OPassgn  "$@"
    compileAldorIntoLib sit_OPbigO   "$@"
    compileAldorIntoLib sit_OPif     "$@"
    compileAldorIntoLib sit_OPcase   "$@"
    compileAldorIntoLib sit_OPcplex  "$@"
    compileAldorIntoLib sit_OPequal  "$@"
    compileAldorIntoLib sit_OPexpt   "$@"
    compileAldorIntoLib sit_OPfact   "$@"
    compileAldorIntoLib sit_OPless   "$@"
    compileAldorIntoLib sit_OPlist   "$@"
    compileAldorIntoLib sit_OPllist  "$@"
    compileAldorIntoLib sit_OPmatrx  "$@"
    compileAldorIntoLib sit_OPminus  "$@"
    compileAldorIntoLib sit_OPmore   "$@"
    compileAldorIntoLib sit_OPnoteq  "$@"
    compileAldorIntoLib sit_OPplus   "$@"
    compileAldorIntoLib sit_OPprefx  "$@"
    compileAldorIntoLib sit_OPquot   "$@"
    compileAldorIntoLib sit_OPsubsc  "$@"
    compileAldorIntoLib sit_OPtimes  "$@"
    compileAldorIntoLib sit_OPvect   "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
