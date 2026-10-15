#!/bin/bash

# Recompile some source files and rebuild the library
if [ -z "$ALDORROOT" ] ; then echo "ALDORROOT not defined" ; exit 1; fi

#
# Reuse the toolchain and deployment policy of the build which produced
# ALDORROOT.  Incremental recompilation must never guess from the caller's
# interactive shell.
#
BuildLog="$ALDORROOT/build.log"
if [ ! -r "$BuildLog" ]; then
    echo "Cannot read build log: $BuildLog" >&2
    exit 2
fi

logval()
{
    awk -v key="$1:" '
        index($0, key) == 1 {
            line = $0
            sub(/^[^:]*:[[:space:]]*/, "", line)
            value = line
        }
        END {
            if (value != "")
                print value
        }
    ' "$BuildLog"
}

Toolchain=$(logval 'Toolchain')
CCLog=$(logval 'C compiler')
CXXLog=$(logval 'C++ compiler')
ARLog=$(logval 'Archiver')
RanlibLog=$(logval 'Ranlib')
SDKLog=$(logval 'Selected SDK for Aldor')
DeploymentLine=$(logval 'Deployment target')
Deployment=${DeploymentLine%% *}

if [ -z "$Toolchain" ] || [ -z "$CCLog" ] || [ -z "$CXXLog" ]; then
    echo "Incomplete toolchain information in $BuildLog" >&2
    exit 2
fi

# Do not permit unrelated settings from the invoking shell to win over the
# build we are modifying.
unset \
    ALDOR_CC ALDOR_CXX ALDOR_AR ALDOR_RANLIB ALDOR_LIBTOOL \
    HOST_CC HOST_CXX HOST_AR HOST_RANLIB \
    ALDOR_MACOS_SDK SDKROOT \
    ALDOR_MACOS_DEPLOYMENT_ARG ALDOR_MACOS_DEPLOYMENT_SOURCE

export ALDOR_TOOLCHAIN="$Toolchain"

export ALDOR_CC="$CCLog"
export ALDOR_CXX="$CXXLog"
export CC="$CCLog"
export CXX="$CXXLog"

if [ -n "$ARLog" ]; then
    export ALDOR_AR="$ARLog"
fi

if [ -n "$RanlibLog" ]; then
    export ALDOR_RANLIB="$RanlibLog"
fi

if [ -n "$SDKLog" ] && [ "$SDKLog" != "compiler default" ]; then
    export ALDOR_MACOS_SDK="$SDKLog"
    export SDKROOT="$SDKLog"
fi

if [ -n "$Deployment" ]; then
    export MACOSX_DEPLOYMENT_TARGET="$Deployment"
else
    unset MACOSX_DEPLOYMENT_TARGET
fi

ScriptDir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
DistroDir=$(cd "$ScriptDir/../.." && pwd)

# The normal build establishes these before using the archive wrappers.
export PATH="$ALDORROOT/toolbin:$ALDORROOT/bin:$PATH"

source "$DistroDir/build-fns.sh" none || exit $?

CXX="$HOST_CXX"
CXXFLAGS="$GLOBAL_CXXFLAGS -g -DUNUSED_LABELS -DNDEBUG -DTEST_ALL"

OBJEXT=${ALDOR_OBJEXT:-o}
LIBEXT=${ALDOR_LIBEXT:-a}
EXEEXT=${ALDOR_EXEEXT:-}

if [ "${ALDOR_TOOLCHAIN:-auto}" = msvc ]; then
    echo "recomp.sh does not yet translate MSVC command-line options." >&2
    exit 2
fi

function buildlib () {
    Lib=$1; shift

    cp "$ALDORROOT/lib/$Lib" .

    for SrcFile in "$@" ; do
        echo "Compiling $SrcFile"
        ObjFile="${SrcFile%.*}.$OBJEXT"
        "$CXX" $CXXFLAGS -c "$SrcFile" -o "$ObjFile"
        arReplaceCmd "$Lib" "$ObjFile" || return $?
        rm -f "$ObjFile"
    done
    ranlibCmd "$Lib" || return $?
    mv "$Lib" "$ALDORROOT/lib/$Lib"
}

# Libraries
echo "--- library"
buildlib "libascomp.$LIBEXT" "$@"

# Main programs
echo "--- aldor_t"
"$CXX" $CXXFLAGS main_t.cpp -L"$ALDORROOT/lib" -lascomp -lm \
    -o "aldor_t$EXEEXT"
mv "aldor_t$EXEEXT" "$ALDORROOT/bin/aldor_t$EXEEXT"

echo "--- aldor"
"$CXX" $CXXFLAGS main.cpp -L"$ALDORROOT/lib" -lascomp -lm \
    -o "aldor$EXEEXT"
mv "aldor$EXEEXT" "$ALDORROOT/bin/aldor$EXEEXT"
