#!/bin/bash
#
# build.sh: Build and test the Aldor distribution.
#
# This file is part of Aldor.
#
# Aldor is licensed under the Apache License, Version 2.0.
#
# See legal/LICENSE in the Aldor distribution for details.
#
# Copyright (C) 2021, 2026 Stephen M. Watt.
#
set -o pipefail
#
# Run as build.sh [ | build | junk | test | reset | all ]
#
# No argument implies "build".
#
# Locate the distro from this script rather than from the caller's cwd.  This
# makes `bash /path/to/distro/build.sh build` safe and also avoids dependence
# on an external dirname command (Aldor's toolbin has its own dirname).
#
# If Bash inherited an already-stale cwd, it may print its own "shell-init:
# error retrieving current directory" before this script starts.  An absolute
# build.sh pathname can still recover because it gives us a valid anchor; a
# relative pathname cannot be resolved reliably once the cwd has disappeared.
script_ref=${BASH_SOURCE[0]}
case "$script_ref" in
    /*)
        script_parent=${script_ref%/*}
        [ -n "$script_parent" ] || script_parent=/
        ;;
    */*)
        if ! caller_dir=$(pwd -P 2>/dev/null); then
            echo "Cannot resolve a relative build.sh from a stale current directory." >&2
            echo "Change to an existing directory and invoke build.sh again." >&2
            exit 2
        fi
        script_parent="$caller_dir/${script_ref%/*}"
        ;;
    *)
        if ! caller_dir=$(pwd -P 2>/dev/null); then
            echo "Cannot resolve build.sh from a stale current directory." >&2
            echo "Change to an existing distro directory and run build.sh again." >&2
            exit 2
        fi
        script_parent=$caller_dir
        ;;
esac

if ! ALDORDISTRO_PHYS=$(cd -P -- "$script_parent" 2>/dev/null && pwd -P); then
    echo "Cannot locate the distro directory containing build.sh." >&2
    exit 2
fi


sanitize_component() {
    printf '%s' "$1" | tr '[:upper:]' '[:lower:]' | tr -c 'a-z0-9._-' '_'
}

aldorVersionString() {
    local header="$ALDORDISTRO_PHYS/aldor/src/version.h" value=
    if [ -r "$header" ]; then
        value=$(sed -n \
            's/^#define[[:space:]][[:space:]]*ALDOR_VERSION_STRING[[:space:]][[:space:]]*"\([^"]*\)".*/\1/p' \
            "$header" | head -1)
    fi
    [ -n "$value" ] || value=unknown
    printf '%s\n' "$value"
}

aldorOsName() {
    local system name=
    system=$(uname -s 2>/dev/null || printf unknown)
    case "$system" in
        Darwin)  name=macos ;;
        CYGWIN*) name=cygwin ;;
        Linux)
            if [ -r /etc/os-release ]; then
                name=$(sed -n 's/^ID=//p' /etc/os-release | head -1 | tr -d '"')
            fi
            [ -n "$name" ] || name=linux
            ;;
        *) name=$system ;;
    esac
    name=$(sanitize_component "$name")
    [ -n "$name" ] || name=unknown
    printf '%s\n' "$name"
}

aldorToolchainName() {
    local requested=${ALDOR_TOOLCHAIN:-auto} cc defs name=
    case "$requested" in
        ''|auto)
            cc=${ALDOR_CC:-${CC:-cc}}
            defs=$("$cc" -dM -E -x c - </dev/null 2>/dev/null || true)
            if printf '%s\n' "$defs" | grep -q '^#define __clang__ '; then
                name=clang
            elif printf '%s\n' "$defs" | grep -q '^#define __GNUC__ '; then
                name=gcc
            else
                name=unknown
            fi
            ;;
        *) name=$requested ;;
    esac
    name=$(sanitize_component "$name")
    [ -n "$name" ] || name=unknown
    printf '%s\n' "$name"
}

defaultBuildRoot() {
    local package_top version os arch toolchain
    package_top=$(cd -P -- "$ALDORDISTRO_PHYS/.." && pwd -P) || return 1
    version=$(sanitize_component "$(aldorVersionString)")
    os=$(aldorOsName)
    arch=$(sanitize_component "$(uname -m 2>/dev/null || printf unknown)")
    [ -n "$arch" ] || arch=unknown
    toolchain=$(aldorToolchainName)
    printf '%s/build/aldor-%s-%s-%s-%s\n' \
        "$package_top" "$version" "$os" "$arch" "$toolchain"
}

aldorOsVersion() {
    local system value=
    system=$(uname -s 2>/dev/null || printf unknown)
    case "$system" in
        Darwin)
            value=$(sw_vers -productVersion 2>/dev/null || \
                uname -r 2>/dev/null || printf unknown)
            ;;
        CYGWIN*) value=$(uname -srvm 2>/dev/null || printf unknown) ;;
        Linux)
            if [ -r /etc/os-release ]; then
                value=$(sed -n 's/^PRETTY_NAME=//p' /etc/os-release | \
                    head -1 | sed 's/^"//;s/"$//')
            fi
            [ -n "$value" ] || value=$(uname -srvm 2>/dev/null || printf unknown)
            ;;
        *) value=$(uname -srvm 2>/dev/null || printf unknown) ;;
    esac
    printf '%s\n' "$value"
}

readProjectHead() {
    local project_top=$1 head ref ref_file

    head=$(git -C "$project_top" rev-parse HEAD 2>/dev/null || true)
    if [ -n "$head" ]; then
        printf '%s\n' "$head"
        return 0
    fi

    if [ -r "$project_top/.git/HEAD" ]; then
        head=$(cat "$project_top/.git/HEAD" 2>/dev/null || true)
        case "$head" in
            'ref: '*)
                ref=${head#ref: }
                ref_file="$project_top/.git/$ref"
                if [ -r "$ref_file" ]; then
                    cat "$ref_file"
                    return 0
                fi
                if [ -r "$project_top/.git/packed-refs" ]; then
                    head=$(awk -v r="$ref" '$2 == r { print $1; exit }' \
                        "$project_top/.git/packed-refs")
                    if [ -n "$head" ]; then
                        printf '%s\n' "$head"
                        return 0
                    fi
                fi
                ;;
            *)
                if [ -n "$head" ]; then
                    printf '%s\n' "$head"
                    return 0
                fi
                ;;
        esac
    fi

    printf 'unknown\n'
}

writeLogHeader() {
    local operation=$1 started=$2 version host system os arch toolchain
    local cc cxx cc_version cxx_version target head project_top

    version=$(aldorVersionString)
    host=$(hostname -s 2>/dev/null || hostname 2>/dev/null || printf unknown)
    system=$(uname -s 2>/dev/null || printf unknown)
    os=$(aldorOsName)
    arch=$(uname -m 2>/dev/null || printf unknown)
    toolchain=$(aldorToolchainName)
    cc=${ALDOR_CC:-${CC:-cc}}
    cxx=${ALDOR_CXX:-${CXX:-c++}}
    cc_version=$("$cc" --version 2>&1 | head -1 || printf unknown)
    cxx_version=$("$cxx" --version 2>&1 | head -1 || printf unknown)
    target=$("$cc" -dumpmachine 2>/dev/null | head -1 || true)
    [ -n "$target" ] || target=unknown
    project_top=$(cd -P -- "$ALDORDISTRO_PHYS/.." && pwd -P 2>/dev/null || true)
    head=$(readProjectHead "$project_top")

    printf 'Aldor %s log\n' "$operation"
    printf 'Version:        %s\n' "$version"
    printf 'Host:           %s\n' "$host"
    printf 'Date:           %s\n' "$started"
    printf 'OS:             %s\n' "$os"
    printf 'OS version:     %s\n' "$(aldorOsVersion)"
    printf 'System:         %s\n' "$system"
    printf 'Architecture:   %s\n' "$arch"
    printf 'Toolchain:      %s\n' "$toolchain"
    printf 'C compiler:     %s\n' "$cc"
    printf 'C version:      %s\n' "$cc_version"
    printf 'C++ compiler:   %s\n' "$cxx"
    printf 'C++ version:    %s\n' "$cxx_version"
    printf 'Target:         %s\n' "$target"
    if [ "$system" = Darwin ] && [ "$toolchain" = gcc ]; then
        if [ -n "${ALDOR_MACOS_SDK:-}" ]; then
            printf 'macOS SDK:      %s\n' "$ALDOR_MACOS_SDK"
        else
            printf 'macOS SDK:      compiler default\n'
        fi
        printf 'SDK selection:  %s\n' "${ALDOR_MACOS_SDK_SELECTION:-unknown}"
        printf 'C++ compat:     %s\n' "${ALDOR_MACOS_CXX_COMPAT_MODE:-none}"
        [ -z "${MACOSX_DEPLOYMENT_TARGET:-}" ] || \
            printf 'Deployment:     %s\n' "$MACOSX_DEPLOYMENT_TARGET"
    fi
    printf 'Git HEAD:       %s\n' "$head"
    printf 'Source distro:  %s\n' "$ALDORDISTRO_PHYS"
    printf 'Build dir:      %s\n' "$ALDORROOT"
    printf 'Log file:       %s/%s.log\n' "$ALDORROOT" "$operation"
    printf '\n'
}

formatElapsed() {
    local total=$1 h m s
    h=$((total / 3600))
    m=$(((total % 3600) / 60))
    s=$((total % 60))
    printf '%02d:%02d:%02d' "$h" "$m" "$s"
}

appendTimingFooter() {
    local log=$1 label=$2 started=$3 start_epoch=$4 status=$5
    local finished end_epoch elapsed
    finished=$(date '+%Y-%m-%d %H:%M:%S %z' 2>/dev/null || date)
    end_epoch=$(date '+%s' 2>/dev/null || printf 0)
    if [ "$start_epoch" -gt 0 ] 2>/dev/null && [ "$end_epoch" -ge "$start_epoch" ] 2>/dev/null; then
        elapsed=$((end_epoch - start_epoch))
    else
        elapsed=0
    fi
    {
        printf '\n======================= %s Timing\n' "$label"
        printf 'Started:         %s\n' "$started"
        printf 'Finished:        %s\n' "$finished"
        printf 'Elapsed:         %s\n' "$(formatElapsed "$elapsed")"
        printf 'Elapsed seconds: %s\n' "$elapsed"
        printf 'Exit status:     %s\n' "$status"
    } | tee -a "$log"
}

# `all` is the release/portability entry point formerly provided by the
# top-level rebuild.sh.  It is deliberately hermetic: an inherited Aldor
# installation must never redirect, contaminate, or be removed by this build.
if [ "${1:-}" = all ]; then
    package_top=$(cd -P -- "$ALDORDISTRO_PHYS/.." && pwd -P) || exit 2
    package_root=${ALDOR_BUILD_ROOT:-$(defaultBuildRoot)}
    unset ALDORROOT ALDORTMP INCPATH LIBPATH ALGEBRAROOT ALGEBRA
    export ALDORROOT="$package_root"
    export ALDORTMP="$ALDORROOT/tmp"
    # Preserve complete diff evidence in the first cross-platform run.
    # The main log is intentionally verbose enough to diagnose and bless
    # deterministic portable differences without rerunning the machine.
    export ALDOR_TEST_BRIEF_DIFFS=0
    bash "$ALDORDISTRO_PHYS/build.sh" reset || exit $?
    bash "$ALDORDISTRO_PHYS/build.sh" build || exit $?
    bash "$ALDORDISTRO_PHYS/build.sh" test || exit $?
    printf '
Complete clean build and test finished.
'
    printf 'Build root: %s
' "$ALDORROOT"
    exit 0
fi

# All relative paths below are relative to the distro, regardless of the
# directory from which this script was invoked.
cd "$ALDORDISTRO_PHYS" || exit 2

selectPackageTestRoot() {
    [ "${1:-build}" = test ] || return 0
    [ "${ALDOR_NO_PACKAGE_ROOT_FALLBACK:-0}" != 1 ] || return 0

    # A top-level rebuild.sh cannot export ALDORROOT back into its caller's
    # shell.  It is therefore common for a later `bash build.sh test` to
    # inherit some unrelated, older ALDORROOT.  If this package has a completed
    # sibling root stamped as having been built by this exact distro, prefer
    # that root.  Never substitute an unstamped or mismatched root.
    package_top=$(cd -P -- "$ALDORDISTRO_PHYS/.." && pwd -P) || return 0
    package_root=${ALDOR_BUILD_ROOT:-$(defaultBuildRoot)}
    package_stamp="$package_root/.aldor-distro-source"

    current_ok=false
    if [ -n "${ALDORROOT:-}" ] && [ -f "$ALDORROOT/.aldor-distro-source" ] &&
       [ "$(cat "$ALDORROOT/.aldor-distro-source" 2>/dev/null)" = "$ALDORDISTRO_PHYS" ]; then
        current_ok=true
    fi
    $current_ok && return 0

    if [ -f "$package_stamp" ] &&
       [ "$(cat "$package_stamp" 2>/dev/null)" = "$ALDORDISTRO_PHYS" ]; then
        if [ -n "${ALDORROOT:-}" ] && [ "$ALDORROOT" != "$package_root" ]; then
            echo "Ignoring inherited ALDORROOT for test: $ALDORROOT" >&2
        fi
        echo "Using this package's completed test root: $package_root" >&2
        export ALDORROOT="$package_root"
        export ALDORTMP="$package_root/tmp"
    fi
}

selectPackageTestRoot "${1:-build}"

validateBuildRoot() {
    if [ -z "$ALDORROOT" ]; then
        echo "Set ALDORROOT to destination directory before proceeding." >&2
        return 1
    fi

    case "$ALDORROOT" in
        /)
            echo "Refusing unsafe ALDORROOT=/" >&2
            return 1
            ;;
    esac

    # Canonicalize after creating the root.  reset is allowed to remove this
    # directory later, but it must never contain the source distro itself.
    mkdir -p "$ALDORROOT" || return 1
    ALDORROOT_PHYS=$(cd "$ALDORROOT" && pwd -P) || return 1
    export ALDORROOT_PHYS

    case "$ALDORDISTRO_PHYS/" in
        "$ALDORROOT_PHYS/"*)
            echo "Refusing unsafe ALDORROOT: $ALDORROOT" >&2
            echo "ALDORROOT must not be the distro directory or one of its ancestors." >&2
            return 1
            ;;
    esac

    if [ -z "$ALDORTMP" ]; then
        export ALDORTMP="$ALDORROOT/tmp"
    fi

    # ALDORTMP is recursively removed at startup, so protect the source tree
    # from an accidentally broad setting here as well.
    mkdir -p "$ALDORTMP" || return 1
    ALDORTMP_PHYS=$(cd "$ALDORTMP" && pwd -P) || return 1
    case "$ALDORDISTRO_PHYS/" in
        "$ALDORTMP_PHYS/"*)
            echo "Refusing unsafe ALDORTMP: $ALDORTMP" >&2
            echo "ALDORTMP must not be the distro directory or one of its ancestors." >&2
            return 1
            ;;
    esac

    rm -rf "$ALDORTMP"
    mkdir -p "$ALDORTMP"
}

# The main work is done by the do* functions, defined in build-fns.sh
# and invoked by doMain below.
#
# It should be possible to run the build.sh in the various subdirectories,
# provided the prior directories are already built, but it may be necessary
# to source initializeAldorRoot to set the necessary environment variables.

doStart() {
    # Used to standardize messages and log file.
    if [ -z "$ALDORDISTRO" ] ; then export ALDORDISTRO="$ALDORDISTRO_PHYS" ; fi

    # Make sure the directory structure is set up.
    initializeAldorRoot
}


doReset() {
    rm -rf "$ALDORROOT"
    mkdir "$ALDORROOT"

    initializeAldorRoot
}

doBuild() {
    doBuildInner() {
        echo "======================= Build Toolchain"
        showAldorToolchain
        echo "======================= Checking GMP"
        requireGmp || return $?
        echo "======================= Ensuring Root Shape"
        initializeAldorRoot

        echo "======================= Building the Aldor Compiler"
        (cd aldor ; bash build.sh build) || return $?

        echo "======================= Building Libraries"
        (cd lib ;   bash build.sh build) || return $?
    }

    local log="$ALDORROOT/build.log" started start_epoch run_status scan_status status
    started=$(date '+%Y-%m-%d %H:%M:%S %z' 2>/dev/null || date)
    start_epoch=$(date '+%s' 2>/dev/null || printf 0)

    {
        writeLogHeader build "$started"
        doBuildInner "$@"
    } 2>&1 | tee "$log"
    run_status=${PIPESTATUS[0]}

    # Preserve absolute source/build paths in archival logs.  These logs are
    # intended to be self-contained records of the exact build configuration.
    scan_status=0
    checkForErrors "$log" || scan_status=$?
    status=$run_status
    if [ "$status" -eq 0 ] && [ "$scan_status" -ne 0 ]; then
        status=$scan_status
    fi

    appendTimingFooter "$log" Build "$started" "$start_epoch" "$status"

    if [ "$status" -eq 0 ]; then
        # Stamp the completed root with the physical source distro that produced
        # it.  This prevents a later `build.sh test` from silently exercising an
        # older installation through a stale ALDORROOT.
        printf '%s\n' "$ALDORDISTRO_PHYS" > "$ALDORROOT/.aldor-distro-source"
    fi
    return "$status"
}

doTest() {
    expected_root_source=$ALDORDISTRO_PHYS
    if [ ! -f "$ALDORROOT/.aldor-distro-source" ] ||
       [ "$(cat "$ALDORROOT/.aldor-distro-source" 2>/dev/null)" != "$expected_root_source" ]; then
        echo "Refusing to test a stale or mismatched ALDORROOT: $ALDORROOT" >&2
        echo "Build this distro into that root before running its tests." >&2
        return 2
    fi
    if [ ! -x "$ALDORROOT/toolbin/testaldor" ]; then
        echo "Cannot test: $ALDORROOT/toolbin/testaldor is missing or not executable." >&2
        return 2
    fi
    case ":$PATH:" in
        *":$ALDORROOT/toolbin:"*) ;;
        *) export PATH="$ALDORROOT/toolbin:$PATH" ;;
    esac

    doTestInner() {
        local overall=0 phase_status

        # A previous failed smoke in this same build root must not contaminate
        # a later successful test run.  Recreate these markers only if the
        # current smoke actually fails.
        rm -f "$ALDORROOT/testout/RUNTIME-BLOCKED" \
              "$ALDORROOT/testout/lib/aldor/BLOCKED" \
              "$ALDORROOT/testout/lib/algebra/BLOCKED"

        echo "======================= Aldor Self-Tests Without Libraries"
        (cd aldor ; bash build.sh test )
        phase_status=$?
        if [ $phase_status -ne 0 ]; then
            overall=$phase_status
            echo "Self-test phase reported failures; continuing with remaining test phases."
        fi

        echo "======================= Generated Runtime Smoke"
        bash "$ALDORDISTRO_PHYS/modernization/runtime-smoke.sh" "$ALDORROOT"
        smoke_status=$?

        echo "======================= Aldor Tests With Libraries"
        if [ $smoke_status -ne 0 ]; then
            [ $overall -ne 0 ] || overall=$smoke_status
            echo "Generated runtime smoke failed; execution-dependent library tests are BLOCKED."
            mkdir -p "$ALDORROOT/testout"
            : > "$ALDORROOT/testout/RUNTIME-BLOCKED"
            if [ "${ALDOR_TEST_BRIEF_DIFFS:-0}" = 1 ]; then
                ( cd lib ; ALDOR_RUNTIME_BLOCKED=1 bash build.sh test -nodiff )
            else
                ( cd lib ; ALDOR_RUNTIME_BLOCKED=1 bash build.sh test )
            fi
        else
            if [ "${ALDOR_TEST_BRIEF_DIFFS:-0}" = 1 ]; then
                ( cd lib ; bash build.sh test -nodiff )
            else
                ( cd lib ; bash build.sh test )
            fi
        fi
        phase_status=$?
        if [ $phase_status -ne 0 ]; then
            [ $overall -ne 0 ] || overall=$phase_status
            echo "Library-test phase reported failures; continuing with release gates."
        fi

        bash "$ALDORDISTRO_PHYS/modernization/classify-results.sh" "$ALDORROOT" >/dev/null || true

        echo "======================= Modernization Release Gates"
        bash "$ALDORDISTRO_PHYS/modernization/validate.sh" \
            "$ALDORROOT" "$ALDORDISTRO_PHYS"
        phase_status=$?
        if [ $phase_status -ne 0 ]; then
            [ $overall -ne 0 ] || overall=$phase_status
            echo "Modernization release gates reported failures; continuing."
        fi

        echo "======================= Relative Performance Timing"
        bench_baseline_compiler=${ALDOR_BENCH_BASELINE_COMPILER:-${ALDOR_V001_COMPILER:-}}
        bench_baseline_support=${ALDOR_BENCH_BASELINE_SUPPORT_ROOT:-${ALDOR_V001_SUPPORT_ROOT:-}}
        bench_current_support=${ALDOR_BENCH_CURRENT_SUPPORT_ROOT:-$ALDORROOT}

        if [ -n "$bench_baseline_compiler" ] && [ -n "$bench_baseline_support" ]; then
            bench_out="$ALDORROOT/relative-timing"
            rm -rf "$bench_out"
            mkdir -p "$bench_out"
            BENCH_BASELINE_SUPPORT_ROOT="$bench_baseline_support" \
            BENCH_CURRENT_SUPPORT_ROOT="$bench_current_support" \
            "$ALDORDISTRO_PHYS/modernization/benchmark/run-generated-code-benchmark.sh" \
                "$ALDORROOT" \
                "$bench_baseline_compiler" \
                "$ALDORROOT/bin/aldor" \
                "$bench_out"
            phase_status=$?
            if [ $phase_status -ne 0 ]; then
                [ $overall -ne 0 ] || overall=$phase_status
                echo "Relative timing phase reported a failure."
            fi
        elif [ -n "$bench_baseline_compiler" ]; then
            echo "NOT RUN: Aldor 1.2 reference compiler is configured but its ABI-matched support root is not."
            echo "Set ALDOR_BENCH_BASELINE_SUPPORT_ROOT or ALDOR_V001_SUPPORT_ROOT."
            echo "Do not use the corrected-hash current support root for the historical compiler."
        else
            echo "NOT RUN: configure the Aldor 1.2 reference compiler and historical-hash support root."
            echo "Use ALDOR_BENCH_BASELINE_COMPILER + ALDOR_BENCH_BASELINE_SUPPORT_ROOT"
            echo "or ALDOR_V001_COMPILER + ALDOR_V001_SUPPORT_ROOT."
        fi

        return $overall
    }

    local log="$ALDORROOT/test.log" started start_epoch status
    started=$(date '+%Y-%m-%d %H:%M:%S %z' 2>/dev/null || date)
    start_epoch=$(date '+%s' 2>/dev/null || printf 0)

    {
        writeLogHeader test "$started"
        doTestInner "$@"
    } 2>&1 | tee "$log"
    status=${PIPESTATUS[0]}

    # Preserve absolute source/build paths in archival logs.
    bash "$ALDORDISTRO_PHYS/modernization/make-test-report.sh" \
        "$ALDORROOT" "$ALDORDISTRO_PHYS" || true
    appendTimingFooter "$log" Test "$started" "$start_epoch" "$status"
    return $status
}

# When distro/build.sh is used directly from a standalone source tarball,
# perform the same narrowly scoped Darwin GNU-GCC host-interface selection as
# the release driver.  This keeps `distro` self-contained.
case "$(uname -s 2>/dev/null):${ALDOR_TOOLCHAIN:-auto}" in
    Darwin:gcc|Darwin:gnu|Darwin:g++)
        ALDOR_BUILD_FNS_PLATFORM_ONLY=1
source "$ALDORDISTRO_PHYS/build-fns.sh"
unset ALDOR_BUILD_FNS_PLATFORM_ONLY
        if [ -z "${ALDOR_MACOS_SDK_SELECTION:-}" ]; then
            aldor_macos_select_sdk gcc \
                "${ALDOR_CC:-${CC:-gcc}}" "${ALDOR_CXX:-${CXX:-g++}}" || true
            if [ -n "$ALDOR_SELECTED_MACOS_SDK" ]; then
                ALDOR_MACOS_SDK=$ALDOR_SELECTED_MACOS_SDK
                SDKROOT=$ALDOR_SELECTED_MACOS_SDK
                export ALDOR_MACOS_SDK SDKROOT
            fi
            export ALDOR_MACOS_SDK_SELECTION ALDOR_MACOS_SDK_ATTEMPTS
            export ALDOR_MACOS_CXX_COMPAT_FLAG ALDOR_MACOS_CXX_COMPAT_MODE
        fi
        ;;
esac

# Validate destructive build-root settings before build-fns.sh gets a chance
# to initialize anything below ALDORROOT.  `junk` is intentionally a pure
# source-tree check and needs no build root.
if [ "${1:-build}" != junk ]; then
    validateBuildRoot || exit $?
fi

# Never search PATH for build-fns.sh.  A prior Aldor build may have placed an
# older toolbin at the front of PATH; the driver must use the helper beside
# this exact source distro.
source "$ALDORDISTRO_PHYS/build-fns.sh" ; doMain "$@"
