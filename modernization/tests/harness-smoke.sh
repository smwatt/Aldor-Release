#!/usr/bin/env bash
# Fast end-to-end smoke test for the Aldor build orchestration layer.
set -euo pipefail

script_ref=${BASH_SOURCE[0]}
here=$(cd -P -- "${script_ref%/*}" && pwd -P)
distro=$(cd -P -- "$here/../.." && pwd -P)
package=${1:-$(cd -P -- "$distro/.." && pwd -P)}
if [ -n "${ALDOR_HARNESS_SMOKE_WORK:-}" ]; then
    work=$ALDOR_HARNESS_SMOKE_WORK
    rm -rf "$work"
    mkdir -p "$work"
else
    work=$(mktemp -d "${TMPDIR:-/tmp}/aldor-harness-smoke.XXXXXX")
fi

# Use the physical pathname so comparisons agree with build.sh, which
# deliberately canonicalizes its own location with pwd -P.  This matters on
# systems such as macOS where /var is a symlink to /private/var.
work=$(cd -P -- "$work" && pwd -P)

trap 'rm -rf "$work"' EXIT

fail() { echo "harness smoke FAIL: $*" >&2; exit 1; }

# 1. The production driver must bind its helper by source path, never PATH.
grep -Fq 'source "$ALDORDISTRO_PHYS/build-fns.sh"' "$distro/build.sh" || \
    fail 'distro/build.sh does not source its own build-fns.sh absolutely'

# 2. Run the full build-entry regression in a deliberately hostile inherited
# Aldor environment.  A poison build-fns.sh in PATH must never be consulted.
mkdir -p "$work/old-root/toolbin"
cat > "$work/old-root/toolbin/build-fns.sh" <<'POISON'
echo 'POISON build-fns.sh was sourced' >&2
exit 97
POISON
chmod +x "$work/old-root/toolbin/build-fns.sh"

for tc in gcc clang; do
    case "$tc" in
        gcc)   cc=gcc;   cxx=g++ ;;
        clang) cc=clang; cxx=clang++ ;;
    esac
    command -v "$cc" >/dev/null 2>&1 || continue
    command -v "$cxx" >/dev/null 2>&1 || continue
    out="$work/$tc.out"
    err="$work/$tc.err"
    env \
        ALDORROOT="$work/old-root" \
        ALDORTMP="$work/old-root/tmp" \
        INCPATH=/poison/include LIBPATH=/poison/lib \
        ALGEBRAROOT=/poison/algebra ALGEBRA=/poison/algebra \
        HOST_CC=false HOST_CXX=false HOST_AR=false HOST_RANLIB=false \
        ALDOR_TOOLCHAIN="$tc" CC="$cc" CXX="$cxx" \
        PATH="$work/old-root/toolbin:$PATH" TMPDIR="$work" \
        bash "$here/build-entry-regression.sh" >"$out" 2>"$err" || \
        fail "build-entry regression failed under hostile $tc environment"
    [ ! -s "$err" ] || {
        cat "$err" >&2
        fail "build-entry regression wrote stderr under $tc"
    }
done

# 3. Exercise the public wrapper itself with stale state.  The wrapper must
# reconstruct build state from this package and remove the inherited root's
# toolbin from PATH before it invokes distro/build.sh.
wrapper="$work/wrapper-package"
mkdir -p "$wrapper/distro/aldor/src" "$wrapper/distro"
cp "$package/build.sh" "$wrapper/build.sh"
cp "$distro/build-fns.sh" "$wrapper/distro/build-fns.sh"
printf '#define ALDOR_VERSION_STRING "9.9.9-smoke"\n' > \
    "$wrapper/distro/aldor/src/version.h"
cat > "$wrapper/distro/build.sh" <<'WRAP'
#!/bin/sh
{
    printf 'ALDORROOT=%s\n' "${ALDORROOT:-}"
    printf 'ALDORTMP=%s\n' "${ALDORTMP:-}"
    printf 'INCPATH=%s\n' "${INCPATH:-}"
    printf 'LIBPATH=%s\n' "${LIBPATH:-}"
    printf 'ALGEBRAROOT=%s\n' "${ALGEBRAROOT:-}"
    printf 'HOST_CC=%s\n' "${HOST_CC:-}"
    printf 'PATH=%s\n' "$PATH"
} > "$TRACE"
WRAP
chmod +x "$wrapper/distro/build.sh"

for tc in gcc clang; do
    case "$tc" in
        gcc)   cc=gcc;   cxx=g++ ;;
        clang) cc=clang; cxx=clang++ ;;
    esac
    command -v "$cc" >/dev/null 2>&1 || continue
    command -v "$cxx" >/dev/null 2>&1 || continue
    trace="$work/wrapper-$tc.trace"
    env -u ALDOR_BUILD_ROOT \
        ALDORROOT="$work/old-root" ALDORTMP="$work/old-root/tmp" \
        INCPATH=/poison/include LIBPATH=/poison/lib \
        ALGEBRAROOT=/poison/algebra ALGEBRA=/poison/algebra \
        HOST_CC=false HOST_CXX=false HOST_AR=false HOST_RANLIB=false \
        ALDOR_TOOLCHAIN="$tc" CC="$cc" CXX="$cxx" \
        ALDOR_SKIP_HARNESS_SMOKE=1 TRACE="$trace" \
        PATH="$work/old-root/toolbin:$PATH" \
        bash "$wrapper/build.sh" all >/dev/null 2>"$work/wrapper-$tc.err" || \
        fail "public wrapper failed under hostile $tc environment"
    [ ! -s "$work/wrapper-$tc.err" ] || {
        cat "$work/wrapper-$tc.err" >&2
        fail "public wrapper wrote stderr under $tc"
    }
    grep -q "^ALDORROOT=$wrapper/build/aldor-9.9.9-smoke-" "$trace" || \
        fail "public wrapper retained stale ALDORROOT under $tc"
    grep -q '^INCPATH=$' "$trace" || fail "public wrapper retained INCPATH"
    grep -q '^LIBPATH=$' "$trace" || fail "public wrapper retained LIBPATH"
    grep -q '^ALGEBRAROOT=$' "$trace" || fail "public wrapper retained ALGEBRAROOT"
    grep -q '^HOST_CC=$' "$trace" || fail "public wrapper retained HOST_CC"
    if grep -Fq "$work/old-root/toolbin" "$trace"; then
        fail "public wrapper retained stale Aldor toolbin in PATH"
    fi
done

# 4. Syntax-check the public orchestration scripts.
if [ -f "$package/build.sh" ]; then
    bash -n "$package/build.sh" || fail 'top-level build.sh syntax'
fi
bash -n "$distro/build.sh" || fail 'distro/build.sh syntax'
bash -n "$distro/modernization/validate.sh" || fail 'validate.sh syntax'

echo 'Aldor harness smoke PASS'
