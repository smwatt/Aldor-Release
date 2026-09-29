#!/bin/bash

doBuild() {
   compileAldorIntoLib sit_token   "$@" -Mno-ALDOR_W_GenDomFunNotConst
   compileAldorIntoLib sit_scanner "$@"
   compileAldorIntoLib sit_parser  "$@"
   compileAldorIntoLib sit_infexpr "$@"
   compileAldorIntoLib sit_lspexpr "$@"
   compileAldorIntoLib sit_maple   "$@"
}

source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
