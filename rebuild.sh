#!/bin/bash
#
# rebuild.sh: Aldor build and development support.
#
# This file is part of Aldor.
#
# Aldor is licensed under the Apache License, Version 2.0.
#
# See legal/LICENSE in the Aldor distribution for details.
#
# Copyright (C) 2026 Stephen M. Watt.
#
set -uo pipefail

script_ref=${BASH_SOURCE[0]}
case "$script_ref" in
    /*)
        script_parent=${script_ref%/*}
        [ -n "$script_parent" ] || script_parent=/
        ;;
    */*)
        if ! caller_dir=$(pwd -P 2>/dev/null); then
            echo "Cannot resolve a relative top-level driver from a stale current directory." >&2
            exit 2
        fi
        script_parent="$caller_dir/${script_ref%/*}"
        ;;
    *)
        if ! caller_dir=$(pwd -P 2>/dev/null); then
            echo "Cannot resolve the top-level driver from a stale current directory." >&2
            exit 2
        fi
        script_parent=$caller_dir
        ;;
esac
here=$(cd -P -- "$script_parent" 2>/dev/null && pwd -P) || exit 2
cd "$here" || exit 2

sanitize_component() {
    printf '%s' "$1" | tr '[:upper:]' '[:lower:]' | tr -c 'a-z0-9._-' '_'
}

usage() {
    cat <<'USAGE'
Usage: bash rebuild.sh [help|--help|-h] [build.sh arguments ...]

With no arguments, perform a complete clean build and test ("all").

By default rebuild.sh builds once with each supported native compiler family
available on PATH:

  Unix/macOS/Linux   GNU GCC and Clang, when both are available
  Cygwin             GNU GCC and Clang, when available

The names "gcc" and "clang" below mean compiler *families*, not executable
spellings.  rebuild.sh identifies the front end from compiler predefined
macros, verifies that each C/C++ pair belongs to the same family and compiler
release, and rejects pairs whose target triples disagree.  Automatic mode also
compiles a small host-interface probe.  A viable pair is preferred, but an
identified compiler family is never silently suppressed merely because that
probe fails: if no viable pair in the family is found, the first coherent pair
is still built so the failure is recorded in its normal Aldor build log.

Each toolchain gets a separate clean build root:

  build/aldor-<version>-<os>-<arch>-<toolchain>

Each build root contains exactly one build log (build.log) and one test log
(test.log).  The logs contain their own host/date/toolchain headers and timing
footers.  Build and test write no log files outside build/.

After each successful complete "all" run, rebuild.sh also writes release
handoff artifacts under the workspace delivery/ directory:

  aldor-<version>-<os>-<arch>-<toolchain>.tgz
  aldor-<version>-<os>-<arch>-<toolchain>-build.log
  aldor-<version>-<os>-<arch>-<toolchain>-test.log

The .tgz contains the complete per-toolchain build directory, including its
build.log and test.log.  Delivery packaging failure makes that toolchain fail.

Set ALDOR_TOOLCHAIN to select exactly one tested native compiler family instead
of the automatic set.  Currently enabled selections are gcc, clang, and native.

The source portability layer recognizes future Windows environments for
mingw-gcc, mingw-clang, and msvc, but rebuild.sh deliberately does not attempt
those targets yet.

For an explicit gcc or clang selection, ALDOR_CC and ALDOR_CXX may be used to
name an exact C/C++ pair.  If either is supplied, both must be supplied and
the pair is validated.

Any non-help arguments are passed to the top-level build.sh for every selected
toolchain.  Thus "bash rebuild.sh all" is equivalent to the no-argument form.
USAGE
}

case "${1:-}" in
    help|--help|-h)
        usage
        exit 0
        ;;
esac

version_header="$here/aldor/src/version.h"
release_version=
if [ -r "$version_header" ]; then
    release_version=$(
        sed -n \
            's/^#define[[:space:]][[:space:]]*ALDOR_VERSION_STRING[[:space:]][[:space:]]*"\([^"]*\)".*/\1/p' \
            "$version_header" | head -1
    )
fi
if [ -n "$release_version" ]; then
    aldor_version=$(sanitize_component "$release_version")
else
    aldor_version=unknown
fi
workspace=$(cd -P -- "$here/.." 2>/dev/null && pwd -P) || exit 2
host_raw=$(hostname -s 2>/dev/null || hostname 2>/dev/null || printf unknown)
host=$(sanitize_component "$host_raw")
[ -n "$host" ] || host=unknown

system=$(uname -s 2>/dev/null || printf unknown)
arch_raw=$(uname -m 2>/dev/null || printf unknown)
arch_name=$(sanitize_component "$arch_raw")
[ -n "$arch_name" ] || arch_name=unknown
case "$system" in
    Darwin)  os_name=macos ;;
    CYGWIN*) os_name=cygwin ;;
    Linux)
        os_name=
        if [ -r /etc/os-release ]; then
            os_name=$(sed -n 's/^ID=//p' /etc/os-release | head -1 | tr -d '"')
        fi
        [ -n "$os_name" ] || os_name=linux
        ;;
    *) os_name=$system ;;
esac
os_name=$(sanitize_component "$os_name")
[ -n "$os_name" ] || os_name=unknown

# Shared Darwin SDK capability selection.  Automatic SDK fallback is
# GNU-GCC-only; Clang and non-Darwin behavior remain unchanged.
ALDOR_BUILD_FNS_PLATFORM_ONLY=1
source "$here/build-fns.sh"
unset ALDOR_BUILD_FNS_PLATFORM_ONLY

# Return a stable front-end fingerprint determined from compiler predefined
# macros, never from the executable's name or --version prose.
# Output: <family>|<major.minor.patch>
compiler_fingerprint() {
    local cmd=$1 lang=$2 defs family major minor patch

    command -v "$cmd" >/dev/null 2>&1 || return 1
    defs=$("$cmd" -dM -E -x "$lang" - </dev/null 2>/dev/null) || return 1

    if printf '%s\n' "$defs" | grep -q '^#define __clang__ '; then
        family=clang
        major=$(printf '%s\n' "$defs" | awk '$1=="#define" && $2=="__clang_major__" {print $3; exit}')
        minor=$(printf '%s\n' "$defs" | awk '$1=="#define" && $2=="__clang_minor__" {print $3; exit}')
        patch=$(printf '%s\n' "$defs" | awk '$1=="#define" && $2=="__clang_patchlevel__" {print $3; exit}')
    elif printf '%s\n' "$defs" | grep -q '^#define __GNUC__ '; then
        family=gcc
        major=$(printf '%s\n' "$defs" | awk '$1=="#define" && $2=="__GNUC__" {print $3; exit}')
        minor=$(printf '%s\n' "$defs" | awk '$1=="#define" && $2=="__GNUC_MINOR__" {print $3; exit}')
        patch=$(printf '%s\n' "$defs" | awk '$1=="#define" && $2=="__GNUC_PATCHLEVEL__" {print $3; exit}')
    else
        return 1
    fi

    [ -n "$major" ] || return 1
    [ -n "$minor" ] || minor=0
    [ -n "$patch" ] || patch=0
    printf '%s|%s.%s.%s\n' "$family" "$major" "$minor" "$patch"
}

compiler_target() {
    local cmd=$1 target
    target=$("$cmd" -dumpmachine 2>/dev/null | head -1) || target=
    printf '%s\n' "$target"
}

FOUND_CC=
FOUND_CXX=
FOUND_RELEASE=
FOUND_TARGET=
PAIR_ERROR=

# Validate a proposed native C/C++ pair.  This intentionally asks each driver
# to preprocess the appropriate language, so a missing C++ front end cannot be
# mistaken for an available pair.
try_pair() {
    local wanted=$1 cc=$2 cxx=$3 cfp cxxfp cfam cxxfam crel cxxrel ctgt cxxtgt

    FOUND_CC=
    FOUND_CXX=
    FOUND_RELEASE=
    FOUND_TARGET=
    PAIR_ERROR=

    [ -n "$cc" ] && [ -n "$cxx" ] || {
        PAIR_ERROR="both C and C++ compiler names are required"
        return 1
    }

    cfp=$(compiler_fingerprint "$cc" c) || {
        PAIR_ERROR="cannot identify C compiler '$cc' from predefined macros"
        return 1
    }
    cxxfp=$(compiler_fingerprint "$cxx" c++) || {
        PAIR_ERROR="cannot identify C++ compiler '$cxx' from predefined macros"
        return 1
    }

    cfam=${cfp%%|*}; crel=${cfp#*|}
    cxxfam=${cxxfp%%|*}; cxxrel=${cxxfp#*|}

    [ "$cfam" = "$wanted" ] || {
        PAIR_ERROR="C compiler '$cc' is $cfam, not $wanted"
        return 1
    }
    [ "$cxxfam" = "$wanted" ] || {
        PAIR_ERROR="C++ compiler '$cxx' is $cxxfam, not $wanted"
        return 1
    }
    [ "$crel" = "$cxxrel" ] || {
        PAIR_ERROR="C/C++ compiler releases differ ($crel versus $cxxrel)"
        return 1
    }

    ctgt=$(compiler_target "$cc")
    cxxtgt=$(compiler_target "$cxx")
    if [ -n "$ctgt" ] && [ -n "$cxxtgt" ] && [ "$ctgt" != "$cxxtgt" ]; then
        PAIR_ERROR="C/C++ target triples differ ($ctgt versus $cxxtgt)"
        return 1
    fi

    FOUND_CC=$cc
    FOUND_CXX=$cxx
    FOUND_RELEASE=$crel
    if [ -n "$ctgt" ]; then FOUND_TARGET=$ctgt; else FOUND_TARGET=$cxxtgt; fi
    return 0
}

# Use an explicitly supplied pair only for an explicitly requested family.
validate_explicit_pair() {
    local family=$1
    if [ -n "${ALDOR_CC:-}" ] || [ -n "${ALDOR_CXX:-}" ]; then
        if [ -z "${ALDOR_CC:-}" ] || [ -z "${ALDOR_CXX:-}" ]; then
            echo "ALDOR_CC and ALDOR_CXX must be supplied together." >&2
            return 2
        fi
        if ! try_pair "$family" "$ALDOR_CC" "$ALDOR_CXX"; then
            echo "Invalid explicit $family compiler pair: $PAIR_ERROR." >&2
            return 2
        fi
        return 0
    fi
    return 1
}

find_compiler_pair() {
    local family=$1 use_env=${2:-yes} v
    FOUND_CC=
    FOUND_CXX=
    FOUND_RELEASE=
    FOUND_TARGET=

    # Conventional CC/CXX are useful hints, but only if they really form the
    # requested family.  Automatic discovery continues if they do not.
    if [ "$use_env" = yes ] && [ -n "${CC:-}" ] && [ -n "${CXX:-}" ] && \
       try_pair "$family" "$CC" "$CXX"; then
        return 0
    fi

    case "$family" in
        gcc)
            try_pair gcc gcc g++ && return 0
            for ((v=30; v>=4; v--)); do
                try_pair gcc "gcc-$v" "g++-$v" && return 0
                try_pair gcc "gcc-mp-$v" "g++-mp-$v" && return 0
                try_pair gcc "gcc$v" "g++$v" && return 0
            done
            try_pair gcc cc c++ && return 0
            ;;
        clang)
            try_pair clang clang clang++ && return 0
            for ((v=30; v>=4; v--)); do
                try_pair clang "clang-$v" "clang++-$v" && return 0
            done
            try_pair clang cc c++ && return 0
            # Apple has historically installed gcc/g++ aliases for Clang.
            try_pair clang gcc g++ && return 0
            ;;
    esac
    return 1
}


# Verify whether an identified compiler pair can consume the host interfaces
# Aldor needs.  The result is a preference signal, not permission to suppress a
# compiler family.  If no viable pair for an identified family exists, rebuild
# still runs the first coherent pair so the real build failure is logged.
pair_viable() {
    local family=$1 cc=$2 cxx=$3 tmp rc=0

    if [ "$system" = Darwin ] && [ "$family" = gcc ]; then
        if aldor_macos_select_sdk gcc "$cc" "$cxx"; then
            FOUND_SDK=$ALDOR_SELECTED_MACOS_SDK
            FOUND_SDK_MODE=$ALDOR_MACOS_SDK_SELECTION
            FOUND_SDK_ATTEMPTS=$ALDOR_MACOS_SDK_ATTEMPTS
            FOUND_CXX_COMPAT_FLAG=$ALDOR_MACOS_CXX_COMPAT_FLAG
            FOUND_CXX_COMPAT_MODE=$ALDOR_MACOS_CXX_COMPAT_MODE
            return 0
        fi
        FOUND_SDK=$ALDOR_SELECTED_MACOS_SDK
        FOUND_SDK_MODE=$ALDOR_MACOS_SDK_SELECTION
        FOUND_SDK_ATTEMPTS=$ALDOR_MACOS_SDK_ATTEMPTS
        FOUND_CXX_COMPAT_FLAG=$ALDOR_MACOS_CXX_COMPAT_FLAG
        FOUND_CXX_COMPAT_MODE=$ALDOR_MACOS_CXX_COMPAT_MODE
        return 1
    fi

    tmp=$(mktemp -d "${TMPDIR:-/tmp}/aldor-toolchain-probe.XXXXXX") || return 1
    trap 'rm -rf "$tmp"' RETURN
    cat >"$tmp/probe.c" <<'EOF_PROBE_C'
#include <stdio.h>
#include <unistd.h>
int main(void) { return 0; }
EOF_PROBE_C
    "$cc" -std=c99 -c "$tmp/probe.c" -o "$tmp/probe-c.o" \
        >/dev/null 2>&1 || rc=1
    if [ "$rc" -eq 0 ]; then
        if [ "$system" = Darwin ]; then
            cat >"$tmp/probe.cpp" <<'EOF_PROBE'
#include <mach/mach.h>
int main() { return 0; }
EOF_PROBE
        else
            printf 'int main() { return 0; }\n' >"$tmp/probe.cpp"
        fi
        "$cxx" -std=c++20 -c "$tmp/probe.cpp" -o "$tmp/probe-cxx.o" \
            >/dev/null 2>&1 || rc=1
    fi
    rm -rf "$tmp"
    trap - RETURN
    return "$rc"
}

canonical_toolchain() {
    case "$1" in
        gcc|gnu|g++)      printf '%s\n' gcc ;;
        clang|clang++)    printf '%s\n' clang ;;
        mingw|mingw64|mingw-gcc) printf '%s\n' mingw-gcc ;;
        mingw-clang)              printf '%s\n' mingw-clang ;;
        msvc)                     printf '%s\n' msvc ;;
        native)           printf '%s\n' native ;;
        ''|auto)          printf '%s\n' auto ;;
        *)                return 1 ;;
    esac
}

# Parallel arrays: label, C compiler, C++ compiler, verified release, target.
toolchains=()
ccs=()
cxxs=()
releases=()
targets=()
preflights=()
sdks=()
sdk_modes=()
sdk_attempts=()
cxx_compat_flags=()
cxx_compat_modes=()

add_found_pair() {
    local family=$1
    toolchains+=("$family")
    ccs+=("$FOUND_CC")
    cxxs+=("$FOUND_CXX")
    releases+=("$FOUND_RELEASE")
    targets+=("$FOUND_TARGET")
    preflights+=("${FOUND_PREFLIGHT:-unknown}")
    sdks+=("${FOUND_SDK:-}")
    sdk_modes+=("${FOUND_SDK_MODE:-not-applicable}")
    sdk_attempts+=("${FOUND_SDK_ATTEMPTS:-}")
    cxx_compat_flags+=("${FOUND_CXX_COMPAT_FLAG:-}")
    cxx_compat_modes+=("${FOUND_CXX_COMPAT_MODE:-none}")
}

FALLBACK_CC=
FALLBACK_CXX=
FALLBACK_RELEASE=
FALLBACK_TARGET=
FALLBACK_SDK=
FALLBACK_SDK_MODE=
FALLBACK_SDK_ATTEMPTS=
FALLBACK_CXX_COMPAT_FLAG=
FALLBACK_CXX_COMPAT_MODE=none
FOUND_PREFLIGHT=unknown
FOUND_SDK=
FOUND_SDK_MODE=not-applicable
FOUND_SDK_ATTEMPTS=
FOUND_CXX_COMPAT_FLAG=
FOUND_CXX_COMPAT_MODE=none

try_viable_pair() {
    local family=$1 cc=$2 cxx=$3
    if try_pair "$family" "$cc" "$cxx"; then
        FOUND_SDK=
        FOUND_SDK_MODE=not-applicable
        FOUND_SDK_ATTEMPTS=
        FOUND_CXX_COMPAT_FLAG=
        FOUND_CXX_COMPAT_MODE=none
        if pair_viable "$family" "$FOUND_CC" "$FOUND_CXX"; then
            FOUND_PREFLIGHT=pass
            return 0
        fi
        printf 'Host-interface preflight failed for %s candidate %s/%s; continuing search.\n' \
            "$family" "$cc" "$cxx" >&2
        if [ -z "$FALLBACK_CC" ]; then
            FALLBACK_CC=$FOUND_CC
            FALLBACK_CXX=$FOUND_CXX
            FALLBACK_RELEASE=$FOUND_RELEASE
            FALLBACK_TARGET=$FOUND_TARGET
            FALLBACK_SDK=$FOUND_SDK
            FALLBACK_SDK_MODE=$FOUND_SDK_MODE
            FALLBACK_SDK_ATTEMPTS=$FOUND_SDK_ATTEMPTS
            FALLBACK_CXX_COMPAT_FLAG=$FOUND_CXX_COMPAT_FLAG
            FALLBACK_CXX_COMPAT_MODE=$FOUND_CXX_COMPAT_MODE
        fi
    fi
    return 1
}

find_viable_compiler_pair() {
    local family=$1 v
    FOUND_CC=
    FOUND_CXX=
    FOUND_RELEASE=
    FOUND_TARGET=
    FOUND_PREFLIGHT=unknown
    FALLBACK_CC=
    FALLBACK_CXX=
    FALLBACK_RELEASE=
    FALLBACK_TARGET=
    FALLBACK_SDK=
    FALLBACK_SDK_MODE=
    FALLBACK_SDK_ATTEMPTS=
    FALLBACK_CXX_COMPAT_FLAG=
    FALLBACK_CXX_COMPAT_MODE=none

    if [ -n "${CC:-}" ] && [ -n "${CXX:-}" ] && \
       try_viable_pair "$family" "$CC" "$CXX"; then
        return 0
    fi

    case "$family" in
        gcc)
            try_viable_pair gcc gcc g++ && return 0
            for ((v=30; v>=4; v--)); do
                try_viable_pair gcc "gcc-$v" "g++-$v" && return 0
                try_viable_pair gcc "gcc-mp-$v" "g++-mp-$v" && return 0
                try_viable_pair gcc "gcc$v" "g++$v" && return 0
            done
            try_viable_pair gcc cc c++ && return 0
            ;;
        clang)
            try_viable_pair clang clang clang++ && return 0
            for ((v=30; v>=4; v--)); do
                try_viable_pair clang "clang-$v" "clang++-$v" && return 0
            done
            try_viable_pair clang cc c++ && return 0
            try_viable_pair clang gcc g++ && return 0
            ;;
    esac

    if [ -n "$FALLBACK_CC" ]; then
        FOUND_CC=$FALLBACK_CC
        FOUND_CXX=$FALLBACK_CXX
        FOUND_RELEASE=$FALLBACK_RELEASE
        FOUND_TARGET=$FALLBACK_TARGET
        FOUND_SDK=$FALLBACK_SDK
        FOUND_SDK_MODE=$FALLBACK_SDK_MODE
        FOUND_SDK_ATTEMPTS=$FALLBACK_SDK_ATTEMPTS
        FOUND_CXX_COMPAT_FLAG=$FALLBACK_CXX_COMPAT_FLAG
        FOUND_CXX_COMPAT_MODE=$FALLBACK_CXX_COMPAT_MODE
        FOUND_PREFLIGHT=fail
        printf 'No %s candidate passed the %s host-interface preflight; ' \
            "$family" "$os_name" >&2
        printf 'building %s/%s anyway so the result is logged.\n' \
            "$FOUND_CC" "$FOUND_CXX" >&2
        return 0
    fi
    return 1
}

add_detected_native() {
    local family=$1
    if find_viable_compiler_pair "$family"; then
        add_found_pair "$family"
    fi
}

requested=${ALDOR_TOOLCHAIN:-auto}
if ! requested=$(canonical_toolchain "$requested"); then
    echo "Unknown ALDOR_TOOLCHAIN='${ALDOR_TOOLCHAIN:-}'." >&2
    echo "Use gcc, clang, native, or auto/unset." >&2
    exit 2
fi

if [ "$requested" = auto ]; then
    if [ -n "${ALDOR_CC:-}" ] || [ -n "${ALDOR_CXX:-}" ]; then
        echo "ALDOR_CC/ALDOR_CXX require an explicit ALDOR_TOOLCHAIN." >&2
        echo "For example: ALDOR_TOOLCHAIN=gcc ALDOR_CC=gcc-14 ALDOR_CXX=g++-14 bash rebuild.sh" >&2
        exit 2
    fi
    add_detected_native gcc
    add_detected_native clang
    if [ ${#toolchains[@]} -eq 0 ]; then
        echo "No supported native C/C++ compiler pair was found on PATH." >&2
        echo "Set ALDOR_TOOLCHAIN and, if necessary, ALDOR_CC/ALDOR_CXX." >&2
        exit 2
    fi
else
    case "$requested" in
        gcc|clang)
            validate_explicit_pair "$requested"
            explicit_status=$?
            if [ "$explicit_status" -eq 0 ]; then
                FOUND_PREFLIGHT=forced
                FOUND_SDK=
                FOUND_SDK_MODE=not-applicable
                FOUND_SDK_ATTEMPTS=
                if [ "$system" = Darwin ] && [ "$requested" = gcc ]; then
                    if aldor_macos_select_sdk gcc "$FOUND_CC" "$FOUND_CXX"; then
                        FOUND_PREFLIGHT=pass
                    else
                        FOUND_PREFLIGHT=fail
                    fi
                    FOUND_SDK=$ALDOR_SELECTED_MACOS_SDK
                    FOUND_SDK_MODE=$ALDOR_MACOS_SDK_SELECTION
                    FOUND_SDK_ATTEMPTS=$ALDOR_MACOS_SDK_ATTEMPTS
                    FOUND_CXX_COMPAT_FLAG=$ALDOR_MACOS_CXX_COMPAT_FLAG
                    FOUND_CXX_COMPAT_MODE=$ALDOR_MACOS_CXX_COMPAT_MODE
                fi
                add_found_pair "$requested"
            elif [ "$explicit_status" -eq 2 ]; then
                exit 2
            elif find_compiler_pair "$requested" yes; then
                FOUND_PREFLIGHT=forced
                FOUND_SDK=
                FOUND_SDK_MODE=not-applicable
                FOUND_SDK_ATTEMPTS=
                if [ "$system" = Darwin ] && [ "$requested" = gcc ]; then
                    if aldor_macos_select_sdk gcc "$FOUND_CC" "$FOUND_CXX"; then
                        FOUND_PREFLIGHT=pass
                    else
                        FOUND_PREFLIGHT=fail
                    fi
                    FOUND_SDK=$ALDOR_SELECTED_MACOS_SDK
                    FOUND_SDK_MODE=$ALDOR_MACOS_SDK_SELECTION
                    FOUND_SDK_ATTEMPTS=$ALDOR_MACOS_SDK_ATTEMPTS
                    FOUND_CXX_COMPAT_FLAG=$ALDOR_MACOS_CXX_COMPAT_FLAG
                    FOUND_CXX_COMPAT_MODE=$ALDOR_MACOS_CXX_COMPAT_MODE
                fi
                add_found_pair "$requested"
            else
                echo "Requested $requested C/C++ compiler family was not found on PATH." >&2
                echo "Set both ALDOR_CC and ALDOR_CXX to the desired pair if needed." >&2
                exit 2
            fi
            ;;
        mingw-gcc|mingw-clang|msvc)
            echo "ALDOR_TOOLCHAIN=$requested is recognized but not enabled for builds yet." >&2
            echo "Current tested targets are native gcc/clang on Linux, macOS, and Cygwin." >&2
            exit 2
            ;;
        native)
            cc=${ALDOR_CC:-${HOST_CC:-${CC:-cc}}}
            cxx=${ALDOR_CXX:-${HOST_CXX:-${CXX:-c++}}}
            toolchains+=(native)
            ccs+=("$cc")
            cxxs+=("$cxx")
            releases+=(unknown)
            targets+=(unknown)
            preflights+=(unknown)
            sdks+=("")
            sdk_modes+=(not-applicable)
            sdk_attempts+=("")
            cxx_compat_flags+=("")
            cxx_compat_modes+=(none)
            ;;
    esac
fi

if [ "$#" -eq 0 ]; then
    set -- all
fi

case "${1:-}" in
    build|all) detailed_context=1 ;;
    *)         detailed_context=0 ;;
esac

if [ $detailed_context -eq 1 ]; then
    printf 'Aldor rebuild: %s on %s\n' "$aldor_version" "$os_name"
    printf 'Selected toolchains:'
    for tc in "${toolchains[@]}"; do
        printf ' %s' "$tc"
    done
    printf '\n'
fi

deliver_build() {
    local stem delivery build_name tmp_prefix

    stem="aldor-${aldor_version}-${os_name}-${arch_name}-${tc}"
    delivery="$workspace/delivery"
    build_name=${build_root##*/}

    if [ ! -f "$build_root/build.log" ] || [ ! -f "$build_root/test.log" ]; then
        echo "Delivery packaging requires build.log and test.log in $build_root." >&2
        return 1
    fi

    mkdir -p "$delivery" || return 1
    tmp_prefix="$delivery/.${stem}.tmp.$$"
    rm -f "${tmp_prefix}.tgz" \
          "${tmp_prefix}-build.log" \
          "${tmp_prefix}-test.log"

    if ! tar -czf "${tmp_prefix}.tgz" \
            -C "$workspace/build" "$build_name"; then
        rm -f "${tmp_prefix}.tgz" \
              "${tmp_prefix}-build.log" \
              "${tmp_prefix}-test.log"
        echo "Failed to create delivery archive for $tc." >&2
        return 1
    fi

    if ! cp "$build_root/build.log" "${tmp_prefix}-build.log" ||
       ! cp "$build_root/test.log" "${tmp_prefix}-test.log"; then
        rm -f "${tmp_prefix}.tgz" \
              "${tmp_prefix}-build.log" \
              "${tmp_prefix}-test.log"
        echo "Failed to copy delivery logs for $tc." >&2
        return 1
    fi

    rm -f "$delivery/${stem}.tgz" \
          "$delivery/${stem}-build.log" \
          "$delivery/${stem}-test.log"

    mv "${tmp_prefix}.tgz" "$delivery/${stem}.tgz" &&
    mv "${tmp_prefix}-build.log" "$delivery/${stem}-build.log" &&
    mv "${tmp_prefix}-test.log" "$delivery/${stem}-test.log" || {
        rm -f "${tmp_prefix}.tgz" \
              "${tmp_prefix}-build.log" \
              "${tmp_prefix}-test.log"
        echo "Failed to finalize delivery artifacts for $tc." >&2
        return 1
    }

    printf 'Delivery archive:   %s\n' "$delivery/${stem}.tgz"
    printf 'Delivery build log: %s\n' "$delivery/${stem}-build.log"
    printf 'Delivery test log:  %s\n' "$delivery/${stem}-test.log"
    return 0
}

# A rebuild describes what this host can build now.  Remove stale
# configuration directories for this version/OS/architecture before
# recreating the toolchains detected below.
build_parent="$workspace/build"
mkdir -p "$build_parent"
rm -rf "$build_parent"/aldor-"${aldor_version}"-"${os_name}"-"${arch_name}"-*

overall=0
passed=()
failed=()
for i in "${!toolchains[@]}"; do
    tc=${toolchains[$i]}
    cc=${ccs[$i]}
    cxx=${cxxs[$i]}
    release=${releases[$i]}
    target=${targets[$i]}
    preflight=${preflights[$i]}
    sdk=${sdks[$i]}
    sdk_mode=${sdk_modes[$i]}
    sdk_try=${sdk_attempts[$i]}
    cxx_compat=${cxx_compat_flags[$i]}
    cxx_compat_mode=${cxx_compat_modes[$i]}
    build_root="$workspace/build/aldor-${aldor_version}-${os_name}-${arch_name}-${tc}"

    run_toolchain() {
        if [ $detailed_context -eq 1 ]; then
            printf '\n============================================================\n'
            printf 'Rebuild toolchain: %s\n' "$tc"
            printf 'C compiler:        %s\n' "$cc"
            printf 'C++ compiler:      %s\n' "$cxx"
            if [ "$release" != unknown ] && [ -n "$release" ]; then
                printf 'Verified release:  %s %s\n' "$tc" "$release"
            fi
            if [ "$target" != unknown ] && [ -n "$target" ]; then
                printf 'Verified target:   %s\n' "$target"
            fi
            printf 'Host preflight:    %s\n' "$preflight"
            if [ "$system" = Darwin ] && [ "$tc" = gcc ]; then
                if [ -n "$sdk" ]; then
                    printf 'Selected SDK for Aldor: %s\n' "$sdk"
                else
                    printf 'Selected SDK for Aldor: compiler default\n'
                fi
                printf 'SDK selection:     %s\n' "$sdk_mode"
                printf 'C++ compatibility: %s\n' "$cxx_compat_mode"
                [ -z "$sdk_try" ] || printf 'SDK probe results: %s\n' "$sdk_try"
                [ -z "${MACOSX_DEPLOYMENT_TARGET:-}" ] || \
                    printf 'Deployment target: %s\n' "$MACOSX_DEPLOYMENT_TARGET"
            fi
            printf 'Architecture:      %s\n' "$arch_raw"
            printf 'Build root:        %s\n' "$build_root"
            printf 'Build log:         %s/build.log\n' "$build_root"
            printf 'Test log:          %s/test.log\n' "$build_root"
            printf '============================================================\n'
        fi

        # Preserve rebuild.sh's clean-root semantics for every toolchain while
        # isolating each invocation's environment from the next one.
        rm -rf "$build_root"
        (
            export ALDOR_TOOLCHAIN="$tc"
            export ALDOR_CC="$cc"
            export ALDOR_CXX="$cxx"
            export CC="$cc"
            export CXX="$cxx"
            export ALDOR_BUILD_ROOT="$build_root"
            if [ "$system" = Darwin ] && [ "$tc" = gcc ]; then
                if [ -n "$sdk" ]; then
                    export ALDOR_MACOS_SDK="$sdk"
                    export SDKROOT="$sdk"
                elif [ "$sdk_mode" = compiler-default ]; then
                    unset ALDOR_MACOS_SDK SDKROOT
                fi
                export ALDOR_MACOS_SDK_SELECTION="$sdk_mode"
                export ALDOR_MACOS_SDK_ATTEMPTS="$sdk_try"
                export ALDOR_MACOS_CXX_COMPAT_FLAG="$cxx_compat"
                export ALDOR_MACOS_CXX_COMPAT_MODE="$cxx_compat_mode"
            fi
            export ALDORROOT="$build_root"
            export ALDORTMP="$build_root/tmp"
            bash "$here/build.sh" "$@"
        )
    }

    run_toolchain "$@"
    status=$?

    if [ "$status" -eq 0 ] && [ "${1:-}" = all ]; then
        deliver_build
        status=$?
    fi

    if [ "$status" -eq 0 ]; then
        passed+=("$tc")
    else
        failed+=("$tc:$status")
        [ $overall -ne 0 ] || overall=$status
    fi
done

if [ $detailed_context -eq 1 ]; then
    printf '\nAldor rebuild summary\n'
    printf '  passed:'
    if [ ${#passed[@]} -eq 0 ]; then
        printf ' none'
    else
        printf ' %s' "${passed[@]}"
    fi
    printf '\n'
    printf '  failed:'
    if [ ${#failed[@]} -eq 0 ]; then
        printf ' none'
    else
        printf ' %s' "${failed[@]}"
    fi
    printf '\n'
fi

exit "$overall"
