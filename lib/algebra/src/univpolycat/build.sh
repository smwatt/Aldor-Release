#!/bin/bash

doBuild() {
    compileAldorIntoLib sit_umonom  "$@"
    compileAldorIntoLib sit_ufalg   "$@"
    compileAldorIntoLib sit_uffalg  "$@"
    compileAldorIntoLib sit_upolalg "$@"
    compileAldorIntoLib sit_upolc0  "$@"
    compileAldorIntoLib sit_froot   "$@"
    compileAldorIntoLib sit_zring   "$@"
    compileAldorIntoLib sit_fftring "$@"
    compileAldorIntoLib sit_sqfree  "$@"
    compileAldorIntoLib sit_resprs  "$@"
    compileAldorIntoLib sit_ugring  "$@"
    compileAldorIntoLib sit_fring   "$@"
    compileAldorIntoLib alg_polydio "$@"
    compileAldorIntoLib alg_chrem2  "$@"
    compileAldorIntoLib alg_modgcdp "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
