#!/usr/bin/env bash
#
# runtime-smoke.sh: Aldor build and development support.
#
# This file is part of Aldor.
#
# Aldor is licensed under the Apache License, Version 2.0.
#
#
# See legal/LICENSE in the Aldor distribution for details.
#
# Copyright (C) 2026 Stephen M. Watt.
#
set -euo pipefail
ROOT=${1:?usage: runtime-smoke.sh ALDORROOT}
export ALDORROOT="$ROOT"
work="$ROOT/runtime-smoke"
rm -rf "$work"; mkdir -p "$work"
export PATH="$ROOT/bin:$ROOT/toolbin:$PATH"

run_one() {
    name=$1 family=$2 include=$3
    shift 3
    src="$work/smoke-$name.as"
    if [ "$family" = axllib ]; then
        cat > "$src" <<SRC
#include "$include"
import from Integer;
print << 42 << newline;
SRC
    else
        cat > "$src" <<SRC
#include "$include"
#include "aldorio"
import from Integer, TextWriter, Character;
stdout << 42 << newline;
SRC
    fi
    (cd "$work" && aldor -grun "$@" "${src##*/}") \
        >"$work/$name.out" 2>"$work/$name.err" || return 1
    grep -qx '42' "$work/$name.out"
}

status=0
probe() {
    name=$1 family=$2 include=$3
    shift 3
    if run_one "$name" "$family" "$include" "$@"; then
        echo "SMOKE PASS $name"
    else
        echo "SMOKE FAIL $name"
        [ ! -s "$work/$name.out" ] || {
            echo "--- $name stdout ---"
            cat "$work/$name.out"
        }
        [ ! -s "$work/$name.err" ] || {
            echo "--- $name stderr ---"
            cat "$work/$name.err"
        }
        status=1
    fi
}

probe axllib        axllib axllib  -laxllib
probe salli         salli  aldor   -laldor
probe salli-debug   salli  aldor   -laldord -DDEBUG
probe algebra       algebra algebra -lalgebra -laldor
probe algebra-debug algebra algebra -lalgebrad -laldord -DDEBUG
probe algebra-gmp   algebra algebra -lalgebra-gmp -laldor -cruntime=foam-gmp,gmp -DGMP

[ $status -eq 0 ] && : > "$work/PASS" || : > "$work/FAIL"
exit $status
