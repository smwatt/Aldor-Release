#!/bin/sh
# Error recovery in interactive mode.
#
# Keep a local watchdog around the interactive session so a future recovery
# regression cannot wedge the entire portability suite.  The timeout may be
# overridden for unusually slow machines.

recov_timeout=${ALDOR_RECOV1_TIMEOUT:-300}

aldor -Gloop -Mno-release <<'ThatsAll' &
--int verbose on
#int timing off
#include "axllib.as"
I ==> Integer
SI ==> SingleInteger
ST ==> String
BO ==> Boolean
F ==> SingleFloat

x : I := 10
x : SI := 20
x
x : I := 1
x : ST := "tata"
x : I := "fads"
x

foo(x:BO):BO == 2
foo(x:BO):BO == not x
foo true

import from SI
foo(x:SI):SI == x
foo 32
foo false

foo(x:BO):BO == x
y
foo false

k : F == false
h := 1.0  	-- bug
k : F := false
h := 1.0
k : F := 3.0
h := 1.0
k

gg(x:I):I == x
gg 10
gg(x:I):I == false
y
gg 10
#quit
ThatsAll
aldor_pid=$!

(
    sleep "$recov_timeout"
    if kill -0 "$aldor_pid" 2>/dev/null; then
        echo "recov1.sh: timeout after ${recov_timeout}s; terminating -Gloop" >&2
        kill -TERM "$aldor_pid" 2>/dev/null || true
        sleep 2
        kill -KILL "$aldor_pid" 2>/dev/null || true
    fi
) &
watchdog_pid=$!

wait "$aldor_pid"
status=$?
kill "$watchdog_pid" 2>/dev/null || true
wait "$watchdog_pid" 2>/dev/null || true
exit "$status"
