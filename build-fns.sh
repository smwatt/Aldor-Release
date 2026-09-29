#
# This file is meant to be included in buid scripts -- it is not free-standing.
#
# Copyright (C) 2021, 2026 Stephen M. Watt.
#
# This file is part of Aldor.
#
# Aldor is licensed under the Apache License, Version 2.0.
#
# See legal/LICENSE in the Aldor distribution for details.


###############################################################################
#
# This file (saved in, say, "thiscmdfile") is used as:
#
#    #!/bin/bash
#    ...
#    ... Define any of doBuild, doTest, doCleanup
#    ...
#    source "$ALDORROOT"/toolbin/build-fns.sh ; doMain "$@"
#
# Use it as  "bash thiscmdfile arg1 ...", or, if executable, without "bash".
#
# Arg1, if given, should be one of   reset, build, test, junk, walk.
#
# Each of these causes a particular function to be executed, and
# that function is provided the remaining arguments, if any.
#
# These have the following effect:
#   reset -- Call doReset. No default.
#   build -- Call doBuild. Default is to recurse to subdirectories.
#   test  -- Call doTest.  Default is to recurse to subdirectories.
#   walk  -- Call doWalk.  Default is to recurse to subdirectories.
#   junk  -- Call doJunk.  Default recursively compares contents with OkFiles.
#   none  -- Do nothing.   Use to source build functions into current shell.
#
# If not given, the default is "build".
#
# The default doJunk looks for junk in the current directory then recurses.
# Junk is determined by OkFiles or OkFiles.sh, if one of them exists.
# OkFiles shd list expected files and subdirs. Lines may end with # comments.
# OkFiles.sh  when run, should output a list of expected files and subdirs.
#
# If it is defined, doCleanup is called on "break" (signals 1 2 15).
#
###############################################################################


# Cache the host identity once per source operation.  Platform helpers and the
# normal build initialization use the same fact instead of probing uname in
# several independent branches.
ALDOR_HOST_SYSTEM=$(uname -s 2>/dev/null || printf unknown)
export ALDOR_HOST_SYSTEM

###############################################################################
#
# macOS SDK selection
#
# These functions are needed by rebuild.sh before it has selected a toolchain.
# ALDOR_BUILD_FNS_PLATFORM_ONLY permits build-fns.sh to be sourced just far
# enough to define platform-selection helpers, without initializing the normal
# build environment.
#
###############################################################################

aldor_macos_ensure_deployment_target()
{
    [ "$ALDOR_HOST_SYSTEM" = Darwin ] || return 0
    if [ -z "${MACOSX_DEPLOYMENT_TARGET:-}" ] && \
       command -v sw_vers >/dev/null 2>&1; then
        MACOSX_DEPLOYMENT_TARGET=$(sw_vers -productVersion 2>/dev/null | \
            awk -F. '{print $1 "." $2}')
        export MACOSX_DEPLOYMENT_TARGET
    fi
}

aldor_macos_active_sdk()
{
    [ "$ALDOR_HOST_SYSTEM" = Darwin ] || return 1
    command -v xcrun >/dev/null 2>&1 || return 1
    xcrun --sdk macosx --show-sdk-path 2>/dev/null
}

aldor_macos_canonical_sdk()
{
    local p=$1 parent base
    [ -d "$p" ] || return 1
    parent=${p%/*}
    base=${p##*/}
    [ -n "$parent" ] || parent=/
    parent=$(cd -P -- "$parent" 2>/dev/null && pwd -P) || return 1
    (cd -P -- "$parent/$base" 2>/dev/null && pwd -P)
}

# Probe the ordinary C host interface and the Mach C++ interface.
# Argument 3 is an optional explicit SDK path.  Argument 4 is an optional
# C++-only compatibility definition.  The C probe never receives argument 4.
aldor_macos_sdk_probe()
{
    local cc=$1 cxx=$2 sdk=${3:-} cxx_compat=${4:-} tmp rc=0

    [ "$ALDOR_HOST_SYSTEM" = Darwin ] || return 1
    tmp=$(mktemp -d "${TMPDIR:-/tmp}/aldor-sdk-probe.XXXXXX") || return 1

    cat > "$tmp/probe.c" <<'EOF_C'
#include <stdio.h>
#include <unistd.h>
int main(void) { return 0; }
EOF_C
    cat > "$tmp/probe.cpp" <<'EOF_CXX'
#include <mach/mach.h>
int main() { return 0; }
EOF_CXX

    if [ -n "$sdk" ]; then
        "$cc" -std=c99 -isysroot "$sdk" "$tmp/probe.c" \
            -o "$tmp/probe-c" >/dev/null 2>&1 || rc=1
        if [ "$rc" -eq 0 ]; then
            if [ -n "$cxx_compat" ]; then
                "$cxx" -std=c++20 -isysroot "$sdk" "$cxx_compat" \
                    "$tmp/probe.cpp" -o "$tmp/probe-cxx" \
                    >/dev/null 2>&1 || rc=1
            else
                "$cxx" -std=c++20 -isysroot "$sdk" "$tmp/probe.cpp" \
                    -o "$tmp/probe-cxx" >/dev/null 2>&1 || rc=1
            fi
        fi
    else
        "$cc" -std=c99 "$tmp/probe.c" -o "$tmp/probe-c" \
            >/dev/null 2>&1 || rc=1
        if [ "$rc" -eq 0 ]; then
            if [ -n "$cxx_compat" ]; then
                "$cxx" -std=c++20 "$cxx_compat" "$tmp/probe.cpp" \
                    -o "$tmp/probe-cxx" >/dev/null 2>&1 || rc=1
            else
                "$cxx" -std=c++20 "$tmp/probe.cpp" \
                    -o "$tmp/probe-cxx" >/dev/null 2>&1 || rc=1
            fi
        fi
    fi

    rm -rf "$tmp"
    return "$rc"
}

# Select the macOS host-interface mode for one compiler pair.
#
# Results are returned in globals:
#   ALDOR_SELECTED_MACOS_SDK       empty means compiler default
#   ALDOR_MACOS_SDK_SELECTION      compiler-default, explicit, or none-usable
#   ALDOR_MACOS_SDK_ATTEMPTS       human-readable probe record
#   ALDOR_MACOS_CXX_COMPAT_FLAG    C++-only flag, normally empty
#   ALDOR_MACOS_CXX_COMPAT_MODE    none or xnu-static-assert
aldor_macos_select_sdk()
{
    local family=$1 cc=$2 cxx=$3 explicit= sdk=
    local compat='-D_Static_assert=static_assert'

    ALDOR_SELECTED_MACOS_SDK=
    ALDOR_MACOS_SDK_SELECTION=not-applicable
    ALDOR_MACOS_SDK_ATTEMPTS=
    ALDOR_MACOS_CXX_COMPAT_FLAG=
    ALDOR_MACOS_CXX_COMPAT_MODE=none

    [ "$ALDOR_HOST_SYSTEM" = Darwin ] || return 0
    aldor_macos_ensure_deployment_target

    if [ -n "${ALDOR_MACOS_SDK:-}" ] && [ -n "${SDKROOT:-}" ] && \
       [ "$ALDOR_MACOS_SDK" != "$SDKROOT" ]; then
        ALDOR_MACOS_SDK_SELECTION=conflicting-explicit
        ALDOR_MACOS_SDK_ATTEMPTS="ALDOR_MACOS_SDK and SDKROOT disagree"
        return 1
    fi
    if [ -n "${ALDOR_MACOS_SDK:-}" ]; then
        explicit=$ALDOR_MACOS_SDK
    elif [ -n "${SDKROOT:-}" ]; then
        explicit=$SDKROOT
    fi

    if [ -n "$explicit" ]; then
        sdk=$(aldor_macos_canonical_sdk "$explicit" 2>/dev/null || true)
        [ -n "$sdk" ] || sdk=$explicit
        ALDOR_SELECTED_MACOS_SDK=$sdk
        ALDOR_MACOS_SDK_SELECTION=explicit
        if aldor_macos_sdk_probe "$cc" "$cxx" "$sdk"; then
            ALDOR_MACOS_SDK_ATTEMPTS="$sdk=pass"
            return 0
        fi
        ALDOR_MACOS_SDK_ATTEMPTS="$sdk=fail"
        if [ "$family" = gcc ] && \
           aldor_macos_sdk_probe "$cc" "$cxx" "$sdk" "$compat"; then
            ALDOR_MACOS_CXX_COMPAT_FLAG=$compat
            ALDOR_MACOS_CXX_COMPAT_MODE=xnu-static-assert
            ALDOR_MACOS_SDK_ATTEMPTS="$sdk=fail;$sdk+cxx-compat=pass"
            return 0
        fi
        [ "$family" != gcc ] || \
            ALDOR_MACOS_SDK_ATTEMPTS="$sdk=fail;$sdk+cxx-compat=fail"
        return 1
    fi

    ALDOR_MACOS_SDK_SELECTION=compiler-default
    if aldor_macos_sdk_probe "$cc" "$cxx"; then
        ALDOR_MACOS_SDK_ATTEMPTS="compiler-default=pass"
        return 0
    fi
    ALDOR_MACOS_SDK_ATTEMPTS="compiler-default=fail"

    if [ "$family" = gcc ] && \
       aldor_macos_sdk_probe "$cc" "$cxx" "" "$compat"; then
        ALDOR_MACOS_CXX_COMPAT_FLAG=$compat
        ALDOR_MACOS_CXX_COMPAT_MODE=xnu-static-assert
        ALDOR_MACOS_SDK_ATTEMPTS="compiler-default=fail;compiler-default+cxx-compat=pass"
        return 0
    fi

    [ "$family" != gcc ] || \
        ALDOR_MACOS_SDK_ATTEMPTS="compiler-default=fail;compiler-default+cxx-compat=fail"
    ALDOR_MACOS_SDK_SELECTION=none-usable
    return 1
}

if [ "${ALDOR_BUILD_FNS_PLATFORM_ONLY:-0}" = 1 ]; then
    return 0
fi

###############################################################################
#
# Environment
#
###############################################################################

# Toolchain selection.
#
# The build historically used HOST_CC/HOST_CXX for the implementation and
# relied on bare ar/ranlib plus platform defaults elsewhere.  Keep those old
# variables working, but provide one explicit build-wide toolchain contract.
#
# Typical uses:
#   ALDOR_TOOLCHAIN=gcc   bash build.sh all
#   ALDOR_TOOLCHAIN=clang bash build.sh all
#   ALDOR_TOOLCHAIN=mingw bash build.sh all
#
# Individual commands may always be overridden, for example:
#   ALDOR_CC=gcc-14 ALDOR_CXX=g++-14 bash build.sh all
#
# ALDOR_TOOLCHAIN=msvc establishes native MSVC file-name conventions and tool
# names.  The main build still contains GCC-style option syntax in places, so
# MSVC command-line translation remains experimental; the variables below are
# deliberately the foundation for that later work.
export ALDOR_TOOLCHAIN="${ALDOR_TOOLCHAIN:-auto}"

# Platform variation is normalized here.  The rest of the build consumes the
# resolved tool names and flags without branching on the host OS.
aldor_platform_tool_defaults()
{
    ALDOR_DEFAULT_AR=ar
    ALDOR_DEFAULT_RANLIB=ranlib
    ALDOR_DEFAULT_LIBTOOL=

    case "$ALDOR_HOST_SYSTEM" in
        Darwin)
            [ ! -x /usr/bin/ar ] || ALDOR_DEFAULT_AR=/usr/bin/ar
            [ ! -x /usr/bin/ranlib ] || ALDOR_DEFAULT_RANLIB=/usr/bin/ranlib
            [ ! -x /usr/bin/libtool ] || ALDOR_DEFAULT_LIBTOOL=/usr/bin/libtool
            if [ -z "${ALDOR_LIBTOOL_FLAGS+x}" ]; then
                ALDOR_LIBTOOL_FLAGS="-no_warning_for_no_symbols"
            fi
            ;;
        *)
            if [ -z "${ALDOR_LIBTOOL_FLAGS+x}" ]; then
                ALDOR_LIBTOOL_FLAGS=
            fi
            ;;
    esac

    ALDOR_LIBTOOL="${ALDOR_LIBTOOL:-$ALDOR_DEFAULT_LIBTOOL}"
    export ALDOR_LIBTOOL_FLAGS
}

aldor_platform_tool_defaults

case "$ALDOR_TOOLCHAIN" in
    auto|native)
        ALDOR_CC="${ALDOR_CC:-${HOST_CC:-${CC:-cc}}}"
        ALDOR_CXX="${ALDOR_CXX:-${HOST_CXX:-${CXX:-c++}}}"
        ALDOR_AR="${ALDOR_AR:-${HOST_AR:-${AR:-$ALDOR_DEFAULT_AR}}}"
        ALDOR_RANLIB="${ALDOR_RANLIB:-${HOST_RANLIB:-${RANLIB:-$ALDOR_DEFAULT_RANLIB}}}"
        ;;
    gcc|gnu|g++)
        ALDOR_CC="${ALDOR_CC:-${CC:-gcc}}"
        ALDOR_CXX="${ALDOR_CXX:-${CXX:-g++}}"
        ALDOR_AR="${ALDOR_AR:-${AR:-$ALDOR_DEFAULT_AR}}"
        ALDOR_RANLIB="${ALDOR_RANLIB:-${RANLIB:-$ALDOR_DEFAULT_RANLIB}}"
        ;;
    clang|clang++)
        ALDOR_CC="${ALDOR_CC:-${CC:-clang}}"
        ALDOR_CXX="${ALDOR_CXX:-${CXX:-clang++}}"
        ALDOR_AR="${ALDOR_AR:-${AR:-$ALDOR_DEFAULT_AR}}"
        ALDOR_RANLIB="${ALDOR_RANLIB:-${RANLIB:-$ALDOR_DEFAULT_RANLIB}}"
        ;;
    mingw|mingw64)
        ALDOR_MINGW_PREFIX="${ALDOR_MINGW_PREFIX:-x86_64-w64-mingw32}"
        ALDOR_CC="${ALDOR_CC:-${ALDOR_MINGW_PREFIX}-gcc}"
        ALDOR_CXX="${ALDOR_CXX:-${ALDOR_MINGW_PREFIX}-g++}"
        ALDOR_AR="${ALDOR_AR:-${ALDOR_MINGW_PREFIX}-ar}"
        ALDOR_RANLIB="${ALDOR_RANLIB:-${ALDOR_MINGW_PREFIX}-ranlib}"
        ALDOR_OBJEXT="${ALDOR_OBJEXT:-o}"
        ALDOR_LIBEXT="${ALDOR_LIBEXT:-a}"
        ALDOR_EXEEXT="${ALDOR_EXEEXT:-.exe}"
        [ -n "${MACHINE:-}" ] || export MACHINE=win32gcc
        ;;
    msvc)
        ALDOR_CC="${ALDOR_CC:-cl}"
        ALDOR_CXX="${ALDOR_CXX:-cl}"
        ALDOR_AR="${ALDOR_AR:-lib}"
        ALDOR_RANLIB="${ALDOR_RANLIB:-:}"
        ALDOR_OBJEXT="${ALDOR_OBJEXT:-obj}"
        ALDOR_LIBEXT="${ALDOR_LIBEXT:-lib}"
        ALDOR_EXEEXT="${ALDOR_EXEEXT:-.exe}"
        [ -n "${MACHINE:-}" ] || export MACHINE=win32msvc
        export ALDOR_MSVC_EXPERIMENTAL=1
        ;;
    *)
        echo "Unknown ALDOR_TOOLCHAIN='$ALDOR_TOOLCHAIN'." >&2
        echo "Use auto, gcc/gnu/g++, clang/clang++, mingw, or msvc." >&2
        exit 2
        ;;
esac

export ALDOR_OBJEXT="${ALDOR_OBJEXT:-o}"
export ALDOR_LIBEXT="${ALDOR_LIBEXT:-a}"
case "$ALDOR_HOST_SYSTEM" in
    CYGWIN*|MINGW*|MSYS*) export ALDOR_EXEEXT="${ALDOR_EXEEXT:-.exe}" ;;
    *)                   export ALDOR_EXEEXT="${ALDOR_EXEEXT:-}" ;;
esac

# Backward compatibility: the rest of the distribution may still refer to
# HOST_* or conventional compiler variables.  Make all of them agree with the
# resolved Aldor toolchain.  In particular CC is intentionally exported so
# generated Aldor C uses the selected compiler too (-Ccc/CC semantics).
export ALDOR_CC ALDOR_CXX ALDOR_AR ALDOR_RANLIB ALDOR_LIBTOOL
export HOST_CC="$ALDOR_CC"
export HOST_CXX="$ALDOR_CXX"
export HOST_AR="$ALDOR_AR"
export HOST_RANLIB="$ALDOR_RANLIB"
export CC="$ALDOR_CC"
export CXX="$ALDOR_CXX"
export AR="$ALDOR_AR"
export RANLIB="$ALDOR_RANLIB"

# Native archive policy.  On Darwin, `doar` uses Apple's `/usr/bin/libtool
# -static` to construct the final native `.a` archive.  Apple `ar` remains the
# member-operation helper, and explicit ranlib refreshes retain capability-
# probed warning suppression.  `uniar` is reserved for Aldor `.al` archives.
# Explicit ALDOR_RANLIB_FLAGS (including empty) overrides the probe.
ALDOR_AR_FLAGS="${ALDOR_AR_FLAGS:-}"
if [ -z "${ALDOR_RANLIB_FLAGS+x}" ]; then
    ALDOR_RANLIB_FLAGS=""
    if [ "$ALDOR_HOST_SYSTEM" = Darwin ] && [ "$HOST_RANLIB" != : ]; then
        archive_probe_dir=$(mktemp -d "${TMPDIR:-/tmp}/aldor-archive-probe.XXXXXX" 2>/dev/null || true)
        if [ -n "$archive_probe_dir" ]; then
            : > "$archive_probe_dir/empty.s"
            if "$HOST_CC" -c "$archive_probe_dir/empty.s" \
                    -o "$archive_probe_dir/empty.o" >/dev/null 2>&1; then
                "$HOST_AR" rc "$archive_probe_dir/empty.a" \
                    "$archive_probe_dir/empty.o" >/dev/null 2>&1 || true
                if [ -f "$archive_probe_dir/empty.a" ]; then
                    ranlib_probe_out=$("$HOST_RANLIB" -no_warning_for_no_symbols \
                        "$archive_probe_dir/empty.a" 2>&1)
                    ranlib_probe_status=$?
                    if [ $ranlib_probe_status -eq 0 ] && [ -z "$ranlib_probe_out" ]; then
                        ALDOR_RANLIB_FLAGS="-no_warning_for_no_symbols"
                    fi
                fi
            fi
            rm -rf "$archive_probe_dir"
        fi
    fi
fi
export ALDOR_AR_FLAGS ALDOR_RANLIB_FLAGS

export GLOBAL_CFLAGS="-std=c99"
export GLOBAL_CXXFLAGS="-std=c++20"
ALDOR_MACOS_SDK_ARG=""
ALDOR_MACOS_CXX_COMPAT_FLAG="${ALDOR_MACOS_CXX_COMPAT_FLAG:-}"
if [ "$ALDOR_HOST_SYSTEM" = Darwin ] && \
   [ -n "$ALDOR_MACOS_CXX_COMPAT_FLAG" ]; then
    # This compatibility option is intentionally C++-only.  In particular it
    # must not affect GMP probing, generated Aldor C, or ordinary C sources.
    GLOBAL_CXXFLAGS="$GLOBAL_CXXFLAGS $ALDOR_MACOS_CXX_COMPAT_FLAG"
fi
if [ "$ALDOR_HOST_SYSTEM" = Darwin ] && \
   [ -n "${ALDOR_MACOS_SDK:-}" ]; then
    if [ ! -d "$ALDOR_MACOS_SDK" ]; then
        echo "Selected SDK for Aldor does not exist: $ALDOR_MACOS_SDK" >&2
        exit 2
    fi
    case "$ALDOR_MACOS_SDK" in
        *[[:space:]]*)
            echo "Selected SDK for Aldor contains shell whitespace:" >&2
            echo "  $ALDOR_MACOS_SDK" >&2
            echo "Use a canonical SDK installation path without whitespace." >&2
            exit 2
            ;;
    esac
    ALDOR_MACOS_SDK_ARG="-isysroot=$ALDOR_MACOS_SDK"
    export SDKROOT="$ALDOR_MACOS_SDK"
    export GLOBAL_CFLAGS="$GLOBAL_CFLAGS $ALDOR_MACOS_SDK_ARG"
    export GLOBAL_CXXFLAGS="$GLOBAL_CXXFLAGS $ALDOR_MACOS_SDK_ARG"
fi
case "$ALDOR_HOST_SYSTEM" in
Linux)
    # ISO C99 for C sources, plus explicitly requested POSIX.1-2008
    # interfaces and the legacy default-source namespace still used by Store.
    GLOBAL_CFLAGS="$GLOBAL_CFLAGS -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE"
    GLOBAL_CXXFLAGS="$GLOBAL_CXXFLAGS -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE"
    ;;
CYGWIN*)
    # Cygwin-hosted native builds need POSIX feature visibility.  A MinGW
    # cross compiler defines _WIN32 and uses the Windows OS layer instead, so
    # do not feed it Cygwin's POSIX feature-test selection.
    if [ "$ALDOR_TOOLCHAIN" != mingw ] && [ "$ALDOR_TOOLCHAIN" != mingw64 ]; then
        GLOBAL_CFLAGS="$GLOBAL_CFLAGS -D_POSIX_C_SOURCE=200809L"
        GLOBAL_CXXFLAGS="$GLOBAL_CXXFLAGS -D_POSIX_C_SOURCE=200809L"
    fi
    ;;
esac
# Generated C is sent directly to the selected C compiler.  The historical
# unicl configuration supplied target-system libraries (notably libm) after
# the Aldor runtime archive.  Preserve that ordering explicitly.  Users may
# override the comma-separated list when targeting a different C environment.
if [ -z "${ALDOR_SYSTEM_LIBS+x}" ]; then
    case "$ALDOR_HOST_SYSTEM:$ALDOR_TOOLCHAIN" in
        Darwin:*|*:msvc) ALDOR_SYSTEM_LIBS="" ;;
        *)              ALDOR_SYSTEM_LIBS="m" ;;
    esac
fi
export ALDOR_SYSTEM_LIBS

# GMP discovery.  GMP is part of the standard Aldor library build, but on
# macOS Homebrew keeps its headers/libraries under a package prefix rather
# than promising they are visible in the SDK search path.  Keep the location
# explicit and overridable; do not hard-code /usr/local or /opt/homebrew.
if [ -z "${ALDOR_GMP_INCLUDE_DIR+x}" ] || [ -z "${ALDOR_GMP_LIB_DIR+x}" ]; then
    _gmp_prefix=${ALDOR_GMP_PREFIX:-}
    if [ -z "$_gmp_prefix" ] && command -v pkg-config >/dev/null 2>&1 && \
       pkg-config --exists gmp >/dev/null 2>&1; then
        _gmp_i=$(pkg-config --cflags-only-I gmp 2>/dev/null | sed -n 's/.*-I\([^ ]*\).*/\1/p' | head -1)
        _gmp_l=$(pkg-config --libs-only-L  gmp 2>/dev/null | sed -n 's/.*-L\([^ ]*\).*/\1/p' | head -1)
        [ -n "${ALDOR_GMP_INCLUDE_DIR+x}" ] || ALDOR_GMP_INCLUDE_DIR=${_gmp_i:-}
        [ -n "${ALDOR_GMP_LIB_DIR+x}" ]     || ALDOR_GMP_LIB_DIR=${_gmp_l:-}
    elif [ -z "$_gmp_prefix" ] && [ "$ALDOR_HOST_SYSTEM" = Darwin ] && \
         command -v brew >/dev/null 2>&1; then
        _gmp_prefix=$(brew --prefix gmp 2>/dev/null || true)
    fi

    if [ -n "$_gmp_prefix" ]; then
        [ -n "${ALDOR_GMP_INCLUDE_DIR+x}" ] || ALDOR_GMP_INCLUDE_DIR="$_gmp_prefix/include"
        [ -n "${ALDOR_GMP_LIB_DIR+x}" ]     || ALDOR_GMP_LIB_DIR="$_gmp_prefix/lib"
    fi
    : "${ALDOR_GMP_INCLUDE_DIR:=}"
    : "${ALDOR_GMP_LIB_DIR:=}"
    unset _gmp_prefix _gmp_i _gmp_l
fi
export ALDOR_GMP_INCLUDE_DIR ALDOR_GMP_LIB_DIR

requireGmp() {
    local d src exe status
    local -a inc=() lib=()
    [ -z "$ALDOR_GMP_INCLUDE_DIR" ] || inc+=("-I$ALDOR_GMP_INCLUDE_DIR")
    [ -z "$ALDOR_GMP_LIB_DIR" ]     || lib+=("-L$ALDOR_GMP_LIB_DIR")

    d=$(mktemp -d "${TMPDIR:-/tmp}/aldor-gmp-probe.XXXXXX") || return 2
    src="$d/gmp-probe.c"
    exe="$d/gmp-probe"
    cat >"$src" <<'EOF_GMP_PROBE'
#include <gmp.h>
int main(void) { mpz_t z; mpz_init(z); mpz_clear(z); return 0; }
EOF_GMP_PROBE
    "$HOST_CC" $GLOBAL_CFLAGS "${inc[@]}" "$src" "${lib[@]}" -lgmp -o "$exe" \
        >/dev/null 2>&1
    status=$?
    rm -rf "$d"
    if [ $status -ne 0 ]; then
        echo "Cannot compile and link the required GMP C interface." >&2
        if [ "$ALDOR_HOST_SYSTEM" = Darwin ]; then
            echo "Install GMP (for Homebrew: brew install gmp), or set ALDOR_GMP_PREFIX." >&2
        else
            echo "Install the GMP development headers/library, or set ALDOR_GMP_PREFIX." >&2
        fi
        [ -z "$ALDOR_GMP_INCLUDE_DIR" ] || echo "GMP include directory: $ALDOR_GMP_INCLUDE_DIR" >&2
        [ -z "$ALDOR_GMP_LIB_DIR" ]     || echo "GMP library directory: $ALDOR_GMP_LIB_DIR" >&2
        return 2
    fi
}

# Keep release builds hermetic with respect to a caller's compiler-option
# environment, as the historical build did.  Toolchain/platform-specific
# generated-C options may be appended below after this reset.
export ALDORARGS=""

# Keep the few compatibility diagnostics required by generated C local to
# generated C.  Do not apply them to the host C/C++ compiler/runtime sources.
case "$ALDOR_TOOLCHAIN" in
    clang|clang++)
        # Probe generated-C warning controls independently.  Older Clang
        # releases (for example Clang 14 on Debian/RISC-V) understand
        # -Wno-unused-value but predate -Wno-deprecated-non-prototype.
        # One unsupported newer option must not prevent us from using an
        # older supported option.
        if printf 'int f(); int main(void) { return 0; }\n' | \
           "$HOST_CC" -x c -std=c99 -Werror \
           -Wno-deprecated-non-prototype -c -o /dev/null - \
           >/dev/null 2>&1; then
            export ALDORARGS="$ALDORARGS -Cargs=-Wno-deprecated-non-prototype"
        fi
        if printf 'int main(void) { 1; return 0; }\n' | \
           "$HOST_CC" -x c -std=c99 -Werror -Wno-unused-value \
           -c -o /dev/null - >/dev/null 2>&1; then
            export ALDORARGS="$ALDORARGS -Cargs=-Wno-unused-value"
        fi
        ;;
esac

# MACHINE is a legacy build-tool selector.  Establish a host default only
# for platforms whose spelling we know, and never override an explicit
# cross-build/toolchain choice (notably win32gcc/win32msvc).
if [ -z "${MACHINE:-}" ] ; then
    case "$ALDOR_HOST_SYSTEM" in
        Linux)  export MACHINE=linux ;;
        Darwin) export MACHINE=macosx ;;
    esac
fi

aldor_configure_macos_build_flags()
{
    # Use one deployment target for bootstrap/runtime/generated code.  If the
    # caller has not selected one, use the running macOS major.minor version.
    if [ -z "${MACOSX_DEPLOYMENT_TARGET:-}" ] && command -v sw_vers >/dev/null 2>&1 ; then
        export MACOSX_DEPLOYMENT_TARGET=$(sw_vers -productVersion | awk -F. '{print $1 "." $2}')
    fi

    # Keep warning selection tied to the selected compiler rather than to a
    # hard-coded clang executable.  This permits either clang or GCC on macOS.
    macCArgs="$ALDOR_MACOS_SDK_ARG"
    macCxxArgs="$ALDOR_MACOS_SDK_ARG"
    [ -z "$ALDOR_MACOS_CXX_COMPAT_FLAG" ] || \
        macCxxArgs="$macCxxArgs $ALDOR_MACOS_CXX_COMPAT_FLAG"
    macAldorCArgs=""

    macAddAldorCArg()
    {
        if [ -z "$macAldorCArgs" ] ; then
            macAldorCArgs="$1"
        else
            macAldorCArgs="$macAldorCArgs,$1"
        fi
    }
    [ -z "$ALDOR_MACOS_SDK_ARG" ] || macAddAldorCArg "$ALDOR_MACOS_SDK_ARG"

    macCompilerAccepts()
    {
        local compiler="$1" language="$2" standard="$3" probe="$4"
        local source baseline output

        # Test the exact option that will be used.  GCC deliberately stays
        # silent for an unknown -Wno-* option when there are no other
        # diagnostics, so a clean successful compile is not sufficient.
        # Generate one harmless baseline diagnostic: an unsupported negative
        # warning option then forces GCC to reveal itself with an additional
        # note.  A language-inappropriate option similarly adds a diagnostic.
        # The option is usable iff the compiler succeeds and the diagnostic
        # stream is otherwise unchanged.
        source='#warning ALDOR_WARNING_OPTION_PROBE
int main(void) { return 0; }'
        baseline=$(printf '%s\n' "$source" | \
            "$compiler" -x "$language" "$standard" $ALDOR_MACOS_SDK_ARG \
            -c -o /dev/null - 2>&1) || return 1
        output=$(printf '%s\n' "$source" | \
            "$compiler" -x "$language" "$standard" $ALDOR_MACOS_SDK_ARG "$probe" \
            -c -o /dev/null - 2>&1) || return 1
        [ "$output" = "$baseline" ]
    }

    # Warning namespaces are language-specific.  Probe C and C++ separately:
    # GNU GCC, for example, accepts -Wno-int-conversion for C but warns if the
    # same option is passed to C++.  Generated Aldor code receives only the C
    # controls.
    for w in \
        -Wno-int-conversion \
        -Wno-unused-command-line-argument \
        -Wno-deprecated-declarations \
        -Wno-builtin-requires-header \
        -Wno-incompatible-library-redeclaration
    do
        if macCompilerAccepts "$HOST_CC" c -std=c99 "$w" ; then
            macCArgs="$macCArgs $w"
            macAddAldorCArg "$w"
        fi
        if macCompilerAccepts "$HOST_CXX" c++ -std=c++20 "$w" ; then
            macCxxArgs="$macCxxArgs $w"
        fi
    done

    if [ -d /opt/local/include ] ; then
        macCArgs="$macCArgs -I/opt/local/include"
        macCxxArgs="$macCxxArgs -I/opt/local/include"
        macAddAldorCArg "-I/opt/local/include"
    fi
    if [ -d /opt/local/lib ] ; then
        macCArgs="$macCArgs -L/opt/local/lib"
        macCxxArgs="$macCxxArgs -L/opt/local/lib"
        macAddAldorCArg "-L/opt/local/lib"
    fi

    export GLOBAL_CFLAGS="-std=c99 $macCArgs"
    export GLOBAL_CXXFLAGS="-std=c++20 $macCxxArgs"
    if [ -n "$macAldorCArgs" ]; then
        # Generated C may now be compiled directly by the selected CC rather
        # than through unicl, so pass ordinary compiler options rather than
        # unicl's private -Wopts syntax.
        oldIFS=$IFS; IFS=,
        for arg in $macAldorCArgs; do
            [ -n "$arg" ] && ALDORARGS="$ALDORARGS -Cargs=$arg"
        done
        IFS=$oldIFS
        export ALDORARGS
    fi

}

aldor_configure_platform_build_flags()
{
    case "$ALDOR_HOST_SYSTEM" in
        Darwin) aldor_configure_macos_build_flags ;;
    esac
}

aldor_configure_platform_build_flags

# When GMP lives outside the platform linker's default search path (notably a
# Homebrew keg), generated C links that request -Clib=gmp need the same native
# library directory used by the early GMP probe.
if [ -n "${ALDOR_GMP_LIB_DIR:-}" ]; then
    ALDORARGS="$ALDORARGS -Cargs=-L$ALDOR_GMP_LIB_DIR"
    export ALDORARGS
fi

showAldorToolchain()
{
    echo "Toolchain:     $ALDOR_TOOLCHAIN"
    echo "C compiler:    $HOST_CC"
    echo "C++ compiler:  $HOST_CXX"
    if [ "$ALDOR_HOST_SYSTEM" = Darwin ] && \
       [ "$ALDOR_TOOLCHAIN" = gcc ]; then
        if [ -n "${ALDOR_MACOS_SDK:-}" ]; then
            echo "Selected SDK for Aldor: $ALDOR_MACOS_SDK"
        else
            echo "Selected SDK for Aldor: compiler default"
        fi
        echo "SDK selection: ${ALDOR_MACOS_SDK_SELECTION:-unknown}"
        echo "C++ compatibility: ${ALDOR_MACOS_CXX_COMPAT_MODE:-none}"
        [ -z "${MACOSX_DEPLOYMENT_TARGET:-}" ] || \
            echo "Deployment target: $MACOSX_DEPLOYMENT_TARGET"
    fi
    echo "Archiver:      $HOST_AR"
    if [ "$ALDOR_HOST_SYSTEM" = Darwin ] && [ -n "${ALDOR_LIBTOOL:-}" ]; then
        echo "Archive writer: $ALDOR_LIBTOOL -static ${ALDOR_LIBTOOL_FLAGS:-}"
    fi
    echo "Ranlib:        $HOST_RANLIB"
    echo "Object suffix: .$ALDOR_OBJEXT"
    echo "Library suffix:.$ALDOR_LIBEXT"
    echo "Executable suffix: ${ALDOR_EXEEXT:-<none>}"
    if [ "${ALDOR_MSVC_EXPERIMENTAL:-0}" = 1 ]; then
        echo "MSVC note: filename conventions are selected; GCC-style option" >&2
        echo "translation is not yet complete, so MSVC builds are experimental." >&2
    fi
}

# ALDORROOT is used by recursive "build" as the root of the distribution.
# Subdirectories include bin, toolbin, include, lib, share.
# There is some equivocation whether to use $ALDORROOT/lib or $LIBPATH, etc.

# A `junk` traversal is deliberately independent of the installation/build
# root.  All other commands retain the historical ALDORROOT requirement.
if [ "${1:-}" != junk ]; then
    if [ -z "$ALDORROOT" ] ; then
        echo "ALDORROOT not set.  Stop." >&2
        exit 1
    fi

    # ALDORTMP is used by recursive build and tests as a place for
    # intermediate files.
    if [ -z "$ALDORTMP" ] ; then
        export ALDORTMP="$ALDORROOT/tmp.$$"
        if [ ! -d "$ALDORTMP" ] ; then
            mkdir -p "$ALDORTMP"
        fi
    fi
fi


# ALDORDEPTH is used for the entry/exit messages.

if [ -z "$ALDORDEPTH" ] ; then
    export ALDORDEPTH=1
else
    export ALDORDEPTH=$(($ALDORDEPTH + 1))
fi


# ALDORDISTRO is used to abbreviate directory names relative to starting point.

if [ -z "$ALDORDISTRO" ] ; then
    export ALDORDISTRO="`pwd`"
fi

###############################################################################
#
# Main entry point
#
###############################################################################

# doMain calls the doStart, then, depending on the command, one of
# doBuild, doTest, doJunk, etc.
#
# If the do* functions are not previously defined, defaults are used.


#
# Main function.   Call as: doMain "$@"
#
doMain() {
    local cmd="$1" ; shift

    # Junk checking is a source-tree operation.  It must be usable on a fresh
    # package without ALDORROOT and must not create or initialize a build root.
    if [ "$cmd" = junk ]; then
        doJunk "$@"
        return $?
    fi

    initializeAldorRoot || return $?

    showEnter "$cmd"
    doStart   "$cmd" || return $?

    local rc=0
    case "$cmd" in
    "" | build) ( set -e ; doBuild "$@" ); rc=$? ;;
    junk)       : ;;
    test)       (         doTest  "$@" ); rc=$? ;;
    walk)       ( set -e ; doWalk  "$@" ); rc=$? ;;
    reset)      ( set -e ; doReset "$@" ); rc=$? ;;
    none)       ;;
    *)          echo "'$cmd' is not a recognized build command.  Syntax is"
                echo "    doMain [ |build|junk|test|walk|reset|none] [arg ... ]"
                exit 1
                ;;
    esac
    showExit "$cmd"
    return $rc
}


###############################################################################
#
# Trace build script directory entry/exit
#
###############################################################################

showEnter() {
    if [ "$1"Z != junkZ ] ; then
        doNTimes $ALDORDEPTH echo -n ">>>"
        echo " Entering `abbrevCwd`"
    fi
}
showExit()  {
    if [ "$1"Z != junkZ ] ; then
        doNTimes $ALDORDEPTH echo -n "<<<"
        echo " Exiting `abbrevCwd`"
    fi
}


###############################################################################
#
# do* Actions
#
###############################################################################

#
# If "doStart" has not been defined, then give a default.
#
if ! declare -F doStart >/dev/null ; then
    doStart() { : ; }
fi


#
# reset-all
#
if ! declare -F doReset >/dev/null ; then
    doReset() {
        echo "There is no default 'reset' action."
    }
fi

#
# If "doBuild" has not been defined, then give a default.
#
if ! declare -F doBuild >/dev/null ; then
    doBuild() {
        local d
        for d in * ; do
            if [ -d "$d" -a -f "$d/build.sh" ] ; then
                ( cd "$d" ; bash build.sh build "$@" ) || return $?
            fi
        done
    }
fi

#
# If "doTest" has not been defined, then give a default.
#
if ! declare -F doTest >/dev/null ; then
    doTest() {
        local d
        for d in * ; do
            if [ -d "$d" -a -f "$d/build.sh" ] ; then
                ( cd "$d" ; bash build.sh test "$@" ) || return $?
            fi
        done
    }
fi

#
# Walk the directory tree, doing nothing.
#
if ! declare -F doWalk >/dev/null ; then
    doWalk() {
        local d
        for d in * ; do
            if [ -d "$d" -a -f "$d/build.sh" ] ; then
                ( cd "$d" ; bash build.sh walk "$@" )
            fi
        done
    }
fi

#
# List files not given in OkFiles file.
#
doLocalJunk() {
    if [ -f OkFiles ] ; then
        local OKFILES="`sed -e 's/#.*//' OkFiles` OkFiles"

    elif [ -f OkFiles.sh ] ; then
        local OKFILES=`bash  OkFiles.sh`
    fi
    if [ -z "$OKFILES" ] ; then return ; fi

    local DUPS=`ls -d $OKFILES * 2>/dev/null | sort | uniq -u`
    local ff
    for ff in $DUPS ; do
        echo `abbrevCwd`/$ff
    done
}

if ! declare -F doJunk >/dev/null ; then
    doJunk() {
        doLocalJunk

        local dd
        for dd in * ; do
            if [ -d "$dd" -a -f "$dd/build.sh" ] ; then
                # Junk checking needs only the source-tree structure and each
                # directory's OkFiles list.  Recurse in-process so it never
                # depends on an installed $ALDORROOT/toolbin/build-fns.sh.
                ( cd "$dd" ; doJunk )
            fi
        done
    }
fi

if ! declare -F doCleanup >/dev/null ; then
    doCleanup() {
        echo "Terminated by break." 1>&2
    }
fi

trap doCleanup 1 2 15

###############################################################################
#
# High-level worker functions, used by the above
#
###############################################################################

initializeAldorRoot() {
    if [ -z "$ALDORROOT" ] ; then
        echo "ALDORROOT is not defined.  Cannot initialize."
        return 1;
    fi

    # Make skeleton, if needed.
    local d
    for d in bin toolbin lib include share testout ; do
        if ! [ -d "$ALDORROOT/$d" ] ; then
            mkdir -p "$ALDORROOT/$d"
            chmod 755 "$ALDORROOT/$d"
        fi
    done
    for d in bin toolbin ; do
        if ! [[ ":$PATH:" =~ ":$ALDORROOT/$d:" ]] ; then
            export PATH="$ALDORROOT/$d:$PATH"
        fi
    done

    # Install findable copy of build functions.
    if ! [ -f "$ALDORROOT/toolbin/build-fns.sh" ] ; then
        if [ -f build-fns.sh ] ; then
            cpCmd build-fns.sh "$ALDORROOT/toolbin"
        fi
    fi

    # For building and installing aldor programs
    #FIXME These should be defined elsewhere, or not used at all.
    if [ -z "$INCPATH" ] ;    then export INCPATH="${ALDORROOT}/include" ; fi
    if [ -z "$LIBPATH" ] ;    then export LIBPATH="${ALDORROOT}/lib" ; fi

    if [ -z "$ALGEBRAROOT" ] ; then export ALGEBRAROOT="$ALDORROOT" ; fi
    if [ -z "$ALGEBRA" ] ; then export ALGEBRA="${LIBPATH}/libalgebra.al" ; fi

}

#
# Standardize references to build locations so logs can be compared.
# varname = $1,  filename = $2
#
abbrevInFile() {
    cp "$2"  "$2".tmp
    sed -e "s:${!1}:\$$1:g" < "$2".tmp > "$2"
    rm -f "$2".tmp
}

relativizeFileContents() {
    if [ -n "$ALDORROOT" ]   ; then abbrevInFile ALDORROOT   "$1" ; fi
    if [ -n "$ALDORDISTRO" ] ; then abbrevInFile ALDORDISTRO "$1" ; fi
}

#
# For building and installing shell scripts and C programs  in $ALDORROOT
#
announce () {
    echo Building $1
}
install () {
    local dir="$1"
    local src="$2"
    local base
    base=`basename "$src"`
    announce "$base"
    mkdir -p "$ALDORROOT/$dir"
    cp "$src" "$ALDORROOT/$dir/$base"
    chmod 755 "$ALDORROOT/$dir/$base"
}
installc () {
    dir="$1" ;       shift
    targetsrc="$1" ; shift

    targetbase=`basename "$targetsrc"`
    target="$ALDORROOT/$dir/$targetbase$ALDOR_EXEEXT"

    announce "$targetbase$ALDOR_EXEEXT"
    if [ ! -d "$ALDORROOT/$dir" ] ; then mkdir -p "$ALDORROOT/$dir" ; fi

    "$HOST_CC" $GLOBAL_CFLAGS "$targetsrc.c" "$@" -o "$target" || return $?
    ensureExecutable  "$target"
}
installcpp () {
    dir="$1" ;       shift
    targetsrc="$1" ; shift

    targetbase=`basename "$targetsrc"`
    target="$ALDORROOT/$dir/$targetbase$ALDOR_EXEEXT"

    announce "$targetbase$ALDOR_EXEEXT"
    if [ ! -d "$ALDORROOT/$dir" ] ; then mkdir -p "$ALDORROOT/$dir" ; fi

    "$HOST_CXX" $GLOBAL_CXXFLAGS "$targetsrc.cpp" "$@" -o "$target" || return $?
    ensureExecutable  "$target"
}

#
# Compile an aldor file into a library.
#   $1   base name of the file to compile.
#   $2   base name the library.
#   rest arguments to compile command.

compileAldorIntoLib() { (
    export SrcSuffix=as
    local CompileCmd=(
        aldor -Mno-mactext -M2
        "-I$ALDORROOT/include" "-I`pwd`" "-Y$ALDORROOT/lib"
        -Fo -Fao
    )
    compileIntoLib "$@"
) }

#
# Compile a C file into a library.
#   $1   base name of the file to compile.
#   $2   base name the library.
#   rest arguments to compile command.

compileCIntoLib() { (
    export SrcSuffix=c
    local CompileCmd=(
        "$HOST_CC" $GLOBAL_CFLAGS $CFLAGS
        "-I$ALDORROOT/include" "-I`pwd`" -c
    )
    compileIntoLib "$@"
) }

#
# Compile an ordinary C++20 implementation file into a library.
#
compileCppIntoLib() { (
    export SrcSuffix=cpp
    local CompileCmd=(
        "$HOST_CXX" $GLOBAL_CXXFLAGS $CFLAGS
        "-I$ALDORROOT/include" "-I`pwd`" -c
    )
    compileIntoLib "$@"
) }

#
# Compile a deliberately shared/generated .c source as C++20 for the compiler.
# The same file may be compiled as C in the runtime or utility builds.
#
compileCAsCppIntoLib() { (
    export SrcSuffix=c
    local CompileCmd=(
        "$HOST_CXX" $GLOBAL_CXXFLAGS $CFLAGS -x c++
        "-I$ALDORROOT/include" "-I`pwd`" -c
    )
    compileIntoLib "$@"
) }

#
# Compile a source file into a library without writing to src directory.
#
# Arguments:
#   $1   base name of the file to compile.
#   $2   base name the library.
#   rest arguments to compile command.
#
# Requires environment variables:
#   CompileCmd  -- e.g. gcc or aldor
#   SrcSuffix   -- e.g. c or as

compileIntoLib() { (
    # Set directories with defaults, if needed.
    if [ -z "$LIBPATH" ] ; then LIBPATH="$ALDORROOT/lib" ; fi

    # Digest arguments and set variables.
    local SrcBase=$1 ; shift
    local LibBase=$1 ; shift
    local SrcDir=`pwd`
    local SrcFile="$SrcBase.$SrcSuffix"

    # Prepend original source directory if file name not absolute.
    if ! [[ "$SrcFile" =~ ^/ ]] ; then SrcFile="$SrcDir/$SrcFile" ; fi

    # Remove directory part now that we have made SrcFile.
    SrcBase=`basename "$SrcBase"`

    # Compile in the library directory.
    echo "* Compiling $SrcBase.$SrcSuffix"
    cd "$LIBPATH"
    "${CompileCmd[@]}" "$@" "$SrcFile" || return $?

    # Add to appropriate libraries.
    local carg aoNeeded=no  oNeeded=no
    for carg in "$@" ; do
        if [ "$carg" = -Fao ] ; then aoNeeded=yes ; fi
        if [ "$carg" = -Fo ]  ; then oNeeded=yes  ; fi
    done
    if [ -f "$SrcBase.ao" ] ; then
        arDataCmd "$LibBase.al" "$SrcBase.ao"
        rmCmd "$SrcBase.ao"
    elif [ $aoNeeded = yes ] ; then
        echo "Error: compileIntoLib needed .ao file, but none was generated." >&2
        return 1
    fi
    local objFile="$SrcBase.$ALDOR_OBJEXT"
    local nativeLib="$LibBase.$ALDOR_LIBEXT"
    if [ -f "$objFile" ] ; then
        arReplaceCmd "$nativeLib" "$objFile" || return $?
        rmCmd "$objFile"
    elif [ $oNeeded = yes ] ; then
        echo "Error: compileIntoLib needed .$ALDOR_OBJEXT file, but none was generated." >&2
        return 1
    fi

    # No need to cd back or unset variables since we are in a subshell.
) }

# Compile the .ao files in a .al library and add native objects to the corresponding native archive
#
# E.g. doBUildObjecLib libfun [compiler options]
# compiles the .ao members of $ALDORROOT/lib/libfun.al  and
# and adds the native object files to $ALDORROOT/lib/libfun.$ALDOR_LIBEXT.

doBuildObjectLib() {
    local libnoex="$1" ; shift

    cd "$ALDORROOT/lib"

    # Compile each .ao member into the selected native archive.  Archive
    # member names are values, not shell words: preserve them as one argument.
    local i ofile
    while IFS= read -r i ; do
        [ -n "$i" ] || continue
        uniar x "$libnoex.al" "$i"
        aldor -Fo "$@" "$i"
        ofile="$(basename "$i" .ao).$ALDOR_OBJEXT"
        arReplaceCmd "$libnoex.$ALDOR_LIBEXT" "$ofile"
        rm -f "$i" "$ofile"
    done < <(uniar t "$libnoex.al")
    ranlibCmd "$libnoex.$ALDOR_LIBEXT"
}

###############################################################################
#
# Functions to support testAldor on test suites.
#
###############################################################################

# Wrap the testaldor commands so they know about the correct directories.
doTestAldor() { (
    if [ -z "$TestOut" ] ; then echo "ERROR: TestOut is not set" ; return ; fi
    if [ -z "$TestDir" ] ; then echo "ERROR: TestDir is not set" ; return ; fi

    local debug=no
    local testOpts=()
    while [ $# -gt 0 ] ; do
        case "$1" in
        -nodiff) testOpts+=( -nodiff )            ; shift ;;
        -debug)  testOpts+=( -debug ) ; debug=yes ; shift ;;
        -only)   if [ "$2" != all ] ; then
                     testOpts+=( -only "test$2" )
                 fi
                 shift 2 ;;
        *)       break ;;
        esac
    done

    local args=("${testOpts[@]}" -keep -outdir "$TestOut" \
        -refdir "$TestDir/../testout")

    local base
    for base in "$@" ; do
        args+=( "$TestDir/$base" )
    done

    if [ $debug = yes ] ; then
        echo "In directory `pwd`."
        echo "Running:" testaldor "${args[@]}"
    fi
    testaldor "${args[@]}"
) }

countTestLines() {
    # First argument is pattern
    egrep -e "$@" \
    | egrep -v ">> Failed tests:" | egrep -v ">> No tests failed" \
    | wc -l
}

analyzeLog() {
    local total=`countTestLines  '^>> '             "$@"`
    local nOk=`countTestLines    '^>> .*OK$'        "$@"`
    local nDiff=`countTestLines  '^>> .*DIFFERENT' "$@"`
    local nErr=`countTestLines   '^>> .*ERROR$'     "$@"`

    printf "%3d OK + %3d diff + %3d err = %3d total\n" $nOk $nDiff $nErr $total
}

###############################################################################
#
# Adjust std commands to do what we need
#
###############################################################################

mvCmd() {
    if [ $# != 2 ] ; then
        echo "Error: Want 2 arguments.  Got $#: $*.  Refusing."
        exit 1
    elif [ -d "$2" ] ; then
        echo "Error: Want desination to be a regular file. Got $2. Refusing"
        exit 1
    elif isCygwin ; then
        rm -f "$2"
        mv "$1" "$2"
        # chmod ugo+r "$2" 2> /dev/null
    else
        mv "$1" "$2"
    fi
}
cpCmd() {
    if [ "$#" -ne 2 ] ; then
        echo "Error: Need two arguments. Got $#: $*. Refusing."
        exit 1
    fi
    local base=`basename "$1"`
    local dest
    if [ -d "$2" ] ; then
        dest="$2/$base"
    else
        dest="$2"
    fi
    cp "$1" "$dest"
    chmod u+w "$dest"
}

rmCmd()   { rm    -f "$@"    ; }
ccCmd()   { "$HOST_CC"  $GLOBAL_CFLAGS   "$@" ; }
cxxCmd()  { "$HOST_CXX" $GLOBAL_CXXFLAGS "$@" ; }
createEmptyArchive() {
    rm -f "$1"
    case "$1" in
        *.al) uniar r "$1" ;;
        *)
            if [ "${ALDOR_TOOLCHAIN:-auto}" = msvc ]; then
                echo "MSVC empty-library creation is not implemented yet: $1" >&2
                return 2
            fi
            printf '!<arch>\n' > "$1"
            ;;
    esac
}
# Native object archive operations.  GNU/LLVM/MinGW use ar/ranlib syntax.
# MSVC naming is represented now; full lib.exe command-line translation is a
# later toolchain step, so fail explicitly rather than silently corrupting an
# archive if an MSVC build reaches this operation.
arReplaceCmd() {
    if [ "${ALDOR_TOOLCHAIN:-auto}" = msvc ]; then
        echo "MSVC archive command translation is not implemented yet: $1" >&2
        return 2
    fi
    doar r "$@"
}
arCmd()      { arReplaceCmd "$@" ; }
ranlibCmd()  {
    if [ "$HOST_RANLIB" = : ]; then return 0; fi
    doranlib "$@"
}
arDataCmd() { uniar r "$@"; }
yaccCmd() {
    yacc  "$@"
}
lexCmd()  {
    flex "$@"
}


###############################################################################
#
# Low-level Utility functions
#
###############################################################################

# Hostname, with the possibility of override (e.g. with multiple interfaces)
stableHostname() {
    if [ -e "$HOME/.hostname" ] ; then
        cat "$HOME/.hostname"
    else
        hostname -s
    fi
}

# Repeat a command n times.

doNTimes() {
    local i n
    n="$1" ; shift

    for ((i = 0 ; i < $n ; i++)) ; do
        "$@"
    done
}


# Used by testing return code
isCygwin() { uname | grep -i cygwin >/dev/null ; }
isDarwin() { [ `uname` = Darwin ] ; }

replaceprefix() {
    if [ -z "$1" ] ; then
        echo "$3"
    else
        echo "$3" | sed -e "s:$1:$2:"
    fi
}


# Print current working dir with build location  replaced by "..."
# and escaped so that whole thing does not need to be quoted.
abbrevCwd() {
    local d=`pwd`
    printf '%q' `replaceprefix "$ALDORDISTRO" ... "$d"`
}


ensureExecutable() {
    chmod ugo+x "$1" || return $?
    if isDarwin ; then unquarantine "$1" || return $? ; fi
    return 0
}


#
# Deal with MacOS paranoia
unquarantine() {
    if ! isDarwin ; then
        # echo "Not Darwin -- no quarantine."
        return
    fi

    local DarwinVersion=`uname -r | cut -d. -f1`

    if [ "$DarwinVersion" -lt 20 ] ; then
        # echo "Darwin version < 20 -- no quarantine."
        return
    fi

    # Source archives downloaded by a browser can carry the quarantine
    # attribute into copied files.  Removing it is harmless when absent.
    if command -v xattr >/dev/null 2>&1 ; then
        xattr -d com.apple.quarantine "$1" 2>/dev/null || true
    fi

    # Locally-built command-line executables are signed on Darwin without
    # requiring a user-specific certificate or entitlements file.
    echo "Code signing..."
    codesign --force --sign - "$1" >/dev/null 2>&1 || {
        echo "Unable to code sign $1" >&2
        return 1
    }
}

checkForErrors() {
    local matches
    matches=$(egrep -i \
        '(^|[[:space:]:])(fatal error|error:|undefined reference|linker failed|make(\[[0-9]+\])?: \*\*\*|cannot (open|find|execute)|not found)([[:space:]:]|$)' \
        "$@" | egrep -vi 'terror\.(c|cpp)' || true)
    if [ -n "$matches" ] ; then
        echo "$matches"
        return 1
    fi
    return 0
}
