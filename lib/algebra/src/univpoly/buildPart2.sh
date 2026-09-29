#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_upolc   "$@"  -Mno-ALDOR_W_GenDomFunNotConst
    compileAldorIntoLib sit_spread  "$@"
    compileAldorIntoLib alg_sup     "$@"
    compileAldorIntoLib sit_dup     "$@"
    compileAldorIntoLib sit_ufacpol "$@"
    compileAldorIntoLib alg_unitool "$@"
}
    
source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
