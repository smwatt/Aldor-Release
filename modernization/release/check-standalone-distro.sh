#!/bin/bash
#
# check-standalone-distro.sh: Aldor build and development support.
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
set -eu

here=$(cd -P -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)
distro=$(cd -P -- "$here/../.." && pwd -P)

require_file()
{
    [ -e "$distro/$1" ] || {
        echo "standalone distro: missing required path: $1" >&2
        exit 1
    }
}

for p in \
    README.md legal/LICENSE legal/COPYRIGHT legal/LICENSE-AGPL3 \
    legal/LICENSE-RUNTIME legal/LICENSE-HISTORY.md \
    build.sh rebuild.sh build-fns.sh \
    aldor lib modernization
 do
    require_file "$p"
 done

# The source distribution must not obtain an executable/source dependency from
# its parent.  Historical text may mention ../; active shell drivers/tests may
# not resolve required project files above distro.
if grep -RInE \
    '(source|bash|exec|cp|cat|test|-f|-x)[[:space:]].*\.\./.*(build|rebuild|macos-sdk|modernization|aldor|lib)' \
    "$distro"/*.sh "$distro/modernization" --include='*.sh' 2>/dev/null
then
    echo "standalone distro: active script appears to depend on parent source" >&2
    exit 1
fi

# The compact source distribution is intentionally Git-free.  The full
# project package carries the repository at its top level.
if [ -e "$distro/.git" ]; then
    echo "standalone distro: unexpected .git directory" >&2
    exit 1
fi

# Source-tree housekeeping must remain silent.
out=$(mktemp "${TMPDIR:-/tmp}/aldor-standalone-junk.XXXXXX")
err=$(mktemp "${TMPDIR:-/tmp}/aldor-standalone-junk-err.XXXXXX")
trap 'rm -f "$out" "$err"' EXIT HUP INT TERM
env -u ALDORROOT bash "$distro/build.sh" junk >"$out" 2>"$err"
[ ! -s "$out" ] && [ ! -s "$err" ] || {
    echo "standalone distro: build.sh junk produced output" >&2
    cat "$out" >&2
    cat "$err" >&2
    exit 1
}

# The build-orchestration smoke, multi-toolchain driver and generic portability
# harness are source-only checks and therefore must run from the standalone
# tree itself.  Keep successful checks quiet, but never hide the check that
# failed or its diagnostics.
run_quiet()
{
    label=$1
    shift
    : >"$out"
    : >"$err"

    if "$@" >"$out" 2>"$err"; then
        return 0
    else
        rc=$?
    fi

    echo "standalone distro: FAIL: $label (status $rc)" >&2
    [ ! -s "$out" ] || cat "$out" >&2
    [ ! -s "$err" ] || cat "$err" >&2
    exit "$rc"
}

parent=$(cd -P "$distro/.." && pwd -P)

run_quiet "harness smoke" \
    bash "$distro/modernization/tests/harness-smoke.sh" "$parent"

run_quiet "rebuild driver" \
    bash "$distro/modernization/tests/rebuild-driver-test.sh"

run_quiet "portability regression" \
    env HOST_CC="${HOST_CC:-cc}" \
    bash "$distro/modernization/tests/portability-regression.sh" "$distro"

run_quiet "varargs ABI audit" \
    python3 "$distro/modernization/tests/varargs-abi-audit.py"

run_quiet "CCode integer-format ABI audit" \
    python3 "$distro/modernization/tests/ccode-int-format-abi.py"

run_quiet "message varargs audit" \
    python3 "$distro/modernization/tests/message-varargs-audit.py"

echo "standalone distro: PASS"
