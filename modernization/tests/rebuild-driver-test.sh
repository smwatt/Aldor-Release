#!/bin/bash
# Copyright (C) 2026 Stephen M. Watt.
# See legal/LICENSE in the Aldor distribution for licensing terms.
set -euo pipefail

here=$(cd -P -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)
package_top=$(cd -P -- "$here/../../.." && pwd -P)
source_rebuild="$package_top/distro/rebuild.sh"
source_build_fns="$package_top/distro/build-fns.sh"

# The macOS SDK helper section must remain compatible with macOS /bin/bash (3.2).
# Check only the early platform-helper section; ordinary build functions later
# in build-fns.sh may legitimately use Bash arrays.
platform_helpers=$(sed -n \
    '1,/^if \[ "${ALDOR_BUILD_FNS_PLATFORM_ONLY:-0}" = 1 \]; then/p' \
    "$source_build_fns")
if grep -Eq \
    'local[[:space:]]+-a|declare[[:space:]]+-a|sysroot\[@\]' \
    <<<"$platform_helpers"
then
    echo "macOS SDK helper section has an array pattern unsafe under Bash 3.2" >&2
    exit 1
fi

# Toolchain viability must exercise normal SDK/libc and Mach interfaces.
# Do not use `printf ... | grep` here.  This script runs with pipefail, and
# on macOS grep may exit after finding an early match while printf is still
# writing the relatively large helper section.  The resulting broken pipe
# makes a successful content check fail spuriously.
for header in \
    '#include <stdio.h>' \
    '#include <unistd.h>' \
    '#include <mach/mach.h>'
do
    if ! grep -F "$header" <<<"$platform_helpers" >/dev/null; then
        echo "macOS SDK helper section is missing: $header" >&2
        exit 1
    fi
done

work=$(mktemp -d "${TMPDIR:-/tmp}/aldor-rebuild-test.XXXXXX")
# macOS commonly supplies TMPDIR with a trailing slash.  mktemp preserves
# the resulting doubled separator, while rebuild.sh canonicalizes its paths.
# Canonicalize the test work root too so path-valued assertions compare
# equivalent pathnames textually as well as semantically.
work=$(cd -P -- "$work" && pwd -P)
trap 'rm -rf "$work"' EXIT HUP INT TERM

make_case() {
    local name=$1 case_top="$work/$1/aldor-v9.9.9test"
    mkdir -p "$case_top/fakebin" "$case_top/distro/aldor/src"
    cp "$source_rebuild" "$case_top/distro/rebuild.sh"
    cp "$source_build_fns" "$case_top/distro/build-fns.sh"
    cat > "$case_top/distro/aldor/src/version.h" <<'VER'
#define ALDOR_VERSION_STRING "9.9.9-test"
VER
    cat > "$case_top/distro/build.sh" <<'STUB'
#!/bin/bash
printf 'TC=%s CC=%s CXX=%s SDK=%s COMPAT=%s ROOT=%s ARGS=' \
    "$ALDOR_TOOLCHAIN" "$ALDOR_CC" "$ALDOR_CXX" \
    "${ALDOR_MACOS_SDK:-default}" "${ALDOR_MACOS_CXX_COMPAT_FLAG:-none}" \
    "$ALDOR_BUILD_ROOT" >> "$TRACE"
printf '<%s>' "$@" >> "$TRACE"
printf '\n' >> "$TRACE"
mkdir -p "$ALDOR_BUILD_ROOT"
: > "$ALDOR_BUILD_ROOT/build.log"
: > "$ALDOR_BUILD_ROOT/test.log"
[ "${FAIL_TC:-}" != "$ALDOR_TOOLCHAIN" ]
STUB
    chmod +x "$case_top/distro/build.sh"
    printf '%s\n' "$case_top"
}

fake_uname() {
    local dir=$1 system=$2 arch=${3:-}
    if [ -z "$arch" ]; then
        case "$system" in
            Darwin) arch=arm64 ;;
            CYGWIN*) arch=x86_64 ;;
            *) arch=x86_64 ;;
        esac
    fi
    cat > "$dir/uname" <<EOF_UNAME
#!/bin/sh
case "\${1:-}" in
    -m) echo '$arch' ;;
    *)  echo '$system' ;;
esac
EOF_UNAME
    chmod +x "$dir/uname"
}

fake_macos_sdks() {
    local dir=$1 root=$2 active=$3
    mkdir -p "$root/SDKs/$active" "$root/SDKs/MacOSX15.4.sdk"
    cat > "$dir/xcode-select" <<EOF_XCODE
#!/bin/sh
[ "\${1:-}" = -p ] && { echo '$root'; exit 0; }
exit 1
EOF_XCODE
    cat > "$dir/xcrun" <<EOF_XCRUN
#!/bin/sh
case "\$*" in
  *"--sdk macosx --show-sdk-path"*) echo '$root/SDKs/$active'; exit 0 ;;
esac
exit 1
EOF_XCRUN
    cat > "$dir/sw_vers" <<'EOF_SWVERS'
#!/bin/sh
[ "${1:-}" = -productVersion ] && { echo 26.6.2; exit 0; }
exit 1
EOF_SWVERS
    chmod +x "$dir/xcode-select" "$dir/xcrun" "$dir/sw_vers"
}

# family is gcc or clang.  For clang we deliberately also expose __GNUC__ to
# model the real Clang compatibility macros; detection must prefer __clang__.
fake_compiler() {
    local dir=$1 name=$2 family=$3 release=$4 target=$5 banner=$6
    local viable=${7:-yes}
    local major minor patch
    major=${release%%.*}
    minor=${release#*.}; minor=${minor%%.*}
    patch=${release##*.}
    if [ "$family" = clang ]; then
        cat > "$dir/$name" <<EOF_CC
#!/bin/sh
case " \$* " in
  *" -dumpmachine "*) echo '$target'; exit 0 ;;
  *" -dM "*)
    echo '#define __clang__ 1'
    echo '#define __clang_major__ $major'
    echo '#define __clang_minor__ $minor'
    echo '#define __clang_patchlevel__ $patch'
    echo '#define __GNUC__ 4'
    echo '#define __GNUC_MINOR__ 2'
    echo '#define __GNUC_PATCHLEVEL__ 1'
    exit 0 ;;
  *" --version "*) echo '$banner'; exit 0 ;;
  *"probe.cpp"*|*"probe.c"*)
    case '$viable' in
      yes) ;;
      no) exit 1 ;;
      compat)
        case " \$* " in
          *"probe.c "*) ;;
          *"probe.cpp "*)
            case " \$* " in
              *" -D_Static_assert=static_assert "*) ;;
              *) exit 1 ;;
            esac ;;
        esac
        ;;
      sdk:*)
        good=${viable#sdk:}
        case " \$* " in
          *" -isysroot \$good "*|*" -isysroot=\$good "*) ;;
          *) exit 1 ;;
        esac
        ;;
    esac ;;
esac
exit 0
EOF_CC
    else
        cat > "$dir/$name" <<EOF_CC
#!/bin/sh
case " \$* " in
  *" -dumpmachine "*) echo '$target'; exit 0 ;;
  *" -dM "*)
    echo '#define __GNUC__ $major'
    echo '#define __GNUC_MINOR__ $minor'
    echo '#define __GNUC_PATCHLEVEL__ $patch'
    exit 0 ;;
  *" --version "*) echo '$banner'; exit 0 ;;
  *"probe.cpp"*|*"probe.c"*)
    case '$viable' in
      yes) ;;
      no) exit 1 ;;
      compat)
        case " \$* " in
          *"probe.c "*) ;;
          *"probe.cpp "*)
            case " \$* " in
              *" -D_Static_assert=static_assert "*) ;;
              *) exit 1 ;;
            esac ;;
        esac
        ;;
      sdk:*)
        good=${viable#sdk:}
        case " \$* " in
          *" -isysroot \$good "*|*" -isysroot=\$good "*) ;;
          *) exit 1 ;;
        esac
        ;;
    esac ;;
esac
exit 0
EOF_CC
    fi
    chmod +x "$dir/$name"
}

assert_lines() {
    local wanted=$1 file=$2 got
    got=$(wc -l < "$file" | tr -d ' ')
    [ "$got" = "$wanted" ] || {
        echo "Expected $wanted lines in $file, got $got" >&2
        cat "$file" >&2
        exit 1
    }
}

run_clean_env() {
    CC= CXX= ALDOR_CC= ALDOR_CXX= "$@"
}

# Compiler-discovery cases are synthetic and must not depend on compilers
# installed in the caller's private PATH.
test_system_path=/usr/bin:/bin:/usr/sbin:/sbin

# Grice/macOS-26 shape: plain gcc/g++ are genuine MacPorts GCC while
# clang/clang++ are Apple Clang, but the installed GCC cannot compile the Mach
# headers from the current SDK.  Automatic mode must still run GCC so the
# actual build result is recorded rather than silently losing matrix coverage.
case_top=$(make_case mac-grice)
bin="$case_top/fakebin"
fake_uname "$bin" Darwin
macdev="$work/mac-grice/developer"
fake_macos_sdks "$bin" "$macdev" MacOSX26.sdk
fake_compiler "$bin" gcc gcc 14.3.0 arm64-apple-darwin25 \
    'gcc (MacPorts gcc14 14.3.0_0+stdlib_flag) 14.3.0' \
    compat
fake_compiler "$bin" g++ gcc 14.3.0 arm64-apple-darwin25 \
    'g++ (MacPorts gcc14 14.3.0_0+stdlib_flag) 14.3.0' \
    compat
fake_compiler "$bin" clang clang 21.0.0 arm64-apple-darwin25 \
    'Apple clang version 21.0.0'
fake_compiler "$bin" clang++ clang 21.0.0 arm64-apple-darwin25 \
    'Apple clang version 21.0.0'
TRACE="$work/mac-grice.trace" run_clean_env env PATH="$bin:$test_system_path" \
    TRACE="$work/mac-grice.trace" \
    bash "$case_top/distro/rebuild.sh" > "$work/mac-grice.out" 2> "$work/mac-grice.err"
assert_lines 2 "$work/mac-grice.trace"
grep -F 'TC=gcc CC=gcc CXX=g++ SDK=default COMPAT=-D_Static_assert=static_assert' \
    "$work/mac-grice.trace" >/dev/null
grep -F 'TC=clang CC=clang CXX=clang++ SDK=default COMPAT=none' "$work/mac-grice.trace" >/dev/null
grep -F 'build/aldor-9.9.9-test-macos-arm64-gcc' "$work/mac-grice.trace" >/dev/null
grep -F 'build/aldor-9.9.9-test-macos-arm64-clang' "$work/mac-grice.trace" >/dev/null
grep -F 'Host preflight:    pass' "$work/mac-grice.out" >/dev/null
grep -F 'Selected SDK for Aldor: compiler default' "$work/mac-grice.out" >/dev/null
grep -F 'SDK selection:     compiler-default' "$work/mac-grice.out" >/dev/null
grep -F 'C++ compatibility: xnu-static-assert' "$work/mac-grice.out" >/dev/null
grep -F 'Verified release:  clang 21.0.0' "$work/mac-grice.out" >/dev/null

# Explicit selection is a force/diagnostic path: identification is still
# checked, but viability does not silently suppress the requested toolchain.
TRACE="$work/mac-grice-force.trace" PATH="$bin:$test_system_path" \
    ALDOR_TOOLCHAIN=gcc ALDOR_CC=gcc ALDOR_CXX=g++ \
    bash "$case_top/distro/rebuild.sh" > "$work/mac-grice-force.out"
assert_lines 1 "$work/mac-grice-force.trace"
grep -F 'TC=gcc CC=gcc CXX=g++ SDK=default COMPAT=-D_Static_assert=static_assert' \
    "$work/mac-grice-force.trace" >/dev/null

# An explicit SDK choice is authoritative: never fall back behind the user's
# back, even when a different installed SDK would pass.
set +e
TRACE="$work/mac-grice-explicit.trace" PATH="$bin:$test_system_path" \
    ALDOR_TOOLCHAIN=gcc ALDOR_CC=gcc ALDOR_CXX=g++ \
    ALDOR_MACOS_SDK="$macdev/SDKs/MacOSX26.sdk" \
    bash "$case_top/distro/rebuild.sh" > "$work/mac-grice-explicit.out" 2>&1
status=$?
set -e
[ "$status" -eq 0 ]
assert_lines 1 "$work/mac-grice-explicit.trace"
grep -F "TC=gcc CC=gcc CXX=g++ SDK=$macdev/SDKs/MacOSX26.sdk COMPAT=-D_Static_assert=static_assert" \
    "$work/mac-grice-explicit.trace" >/dev/null
grep -F 'Host preflight:    pass' "$work/mac-grice-explicit.out" >/dev/null
grep -F 'SDK selection:     explicit' "$work/mac-grice-explicit.out" >/dev/null
grep -F 'C++ compatibility: xnu-static-assert' "$work/mac-grice-explicit.out" >/dev/null

# Apple gcc spelling: even with a deliberately misleading --version banner,
# predefined macros say Clang.  It must not be counted as GCC; a real versioned
# GCC is found separately.
case_top=$(make_case mac-apple-gcc-alias)
bin="$case_top/fakebin"
fake_uname "$bin" Darwin
fake_compiler "$bin" gcc clang 21.0.0 arm64-apple-darwin25 \
    'gcc (GCC) misleading-banner'
fake_compiler "$bin" g++ clang 21.0.0 arm64-apple-darwin25 \
    'g++ (GCC) misleading-banner'
fake_compiler "$bin" clang clang 21.0.0 arm64-apple-darwin25 \
    'Apple clang version 21.0.0'
fake_compiler "$bin" clang++ clang 21.0.0 arm64-apple-darwin25 \
    'Apple clang version 21.0.0'
fake_compiler "$bin" gcc-15 gcc 15.1.0 arm64-apple-darwin25 \
    'gcc-15 (GCC) 15.1.0'
fake_compiler "$bin" g++-15 gcc 15.1.0 arm64-apple-darwin25 \
    'g++-15 (GCC) 15.1.0'
run_clean_env env PATH="$bin:$test_system_path" TRACE="$work/mac-alias.trace" \
    bash "$case_top/distro/rebuild.sh" > "$work/mac-alias.out"
assert_lines 2 "$work/mac-alias.trace"
grep -F 'TC=gcc CC=gcc-15 CXX=g++-15' "$work/mac-alias.trace" >/dev/null
grep -F 'TC=clang CC=clang CXX=clang++' "$work/mac-alias.trace" >/dev/null

# If the first genuine GCC pair is coherent but cannot compile required host
# interfaces, discovery must continue to a later versioned GCC pair.
case_top=$(make_case fallback-viable-gcc)
bin="$case_top/fakebin"
fake_uname "$bin" Darwin
fake_compiler "$bin" gcc gcc 14.3.0 arm64-apple-darwin25 'gcc 14.3.0' no
fake_compiler "$bin" g++ gcc 14.3.0 arm64-apple-darwin25 'g++ 14.3.0' no
fake_compiler "$bin" gcc-15 gcc 15.1.0 arm64-apple-darwin25 'gcc-15 15.1.0' yes
fake_compiler "$bin" g++-15 gcc 15.1.0 arm64-apple-darwin25 'g++-15 15.1.0' yes
fake_compiler "$bin" clang clang 21.0.0 arm64-apple-darwin25 'clang 21.0.0'
fake_compiler "$bin" clang++ clang 21.0.0 arm64-apple-darwin25 'clang++ 21.0.0'
run_clean_env env PATH="$bin:$test_system_path" TRACE="$work/fallback-gcc.trace" \
    bash "$case_top/distro/rebuild.sh" > "$work/fallback-gcc.out" 2> "$work/fallback-gcc.err"
assert_lines 2 "$work/fallback-gcc.trace"
grep -F 'TC=gcc CC=gcc-15 CXX=g++-15' "$work/fallback-gcc.trace" >/dev/null
grep -F 'Host-interface preflight failed for gcc candidate gcc/g++' "$work/fallback-gcc.err" >/dev/null

# A same-family but mismatched plain gcc/g++ release pair is not accepted.
# Discovery must continue to a coherent versioned pair.
case_top=$(make_case mismatched-release)
bin="$case_top/fakebin"
fake_uname "$bin" Darwin
fake_compiler "$bin" gcc gcc 14.3.0 arm64-apple-darwin25 'gcc 14.3.0'
fake_compiler "$bin" g++ gcc 13.4.0 arm64-apple-darwin25 'g++ 13.4.0'
fake_compiler "$bin" gcc-15 gcc 15.1.0 arm64-apple-darwin25 'gcc-15 15.1.0'
fake_compiler "$bin" g++-15 gcc 15.1.0 arm64-apple-darwin25 'g++-15 15.1.0'
fake_compiler "$bin" clang clang 21.0.0 arm64-apple-darwin25 'clang 21.0.0'
fake_compiler "$bin" clang++ clang 21.0.0 arm64-apple-darwin25 'clang++ 21.0.0'
run_clean_env env PATH="$bin:$test_system_path" TRACE="$work/mismatch-release.trace" \
    bash "$case_top/distro/rebuild.sh" > "$work/mismatch-release.out"
grep -F 'TC=gcc CC=gcc-15 CXX=g++-15' "$work/mismatch-release.trace" >/dev/null

# Explicit gcc selection must reject a C/C++ pair targeting different systems.
case_top=$(make_case mismatched-target)
bin="$case_top/fakebin"
fake_uname "$bin" Linux
fake_compiler "$bin" mycc gcc 14.3.0 x86_64-linux-gnu 'gcc 14.3.0'
fake_compiler "$bin" mycxx gcc 14.3.0 aarch64-linux-gnu 'g++ 14.3.0'
set +e
PATH="$bin:$test_system_path" ALDOR_TOOLCHAIN=gcc ALDOR_CC=mycc ALDOR_CXX=mycxx \
    bash "$case_top/distro/rebuild.sh" > "$work/mismatch-target.out" 2>&1
status=$?
set -e
[ "$status" -eq 2 ]
grep -F 'target triples differ' "$work/mismatch-target.out" >/dev/null

# Cygwin automatic mode exercises both GCC and Clang when both are present.
case_top=$(make_case cyg-default)
bin="$case_top/fakebin"
fake_uname "$bin" CYGWIN_NT-10.0-26100
fake_compiler "$bin" gcc gcc 14.2.0 x86_64-pc-cygwin 'gcc (GCC) 14.2.0'
fake_compiler "$bin" g++ gcc 14.2.0 x86_64-pc-cygwin 'g++ (GCC) 14.2.0'
fake_compiler "$bin" clang clang 20.0.0 x86_64-pc-cygwin 'clang version 20.0.0'
fake_compiler "$bin" clang++ clang 20.0.0 x86_64-pc-cygwin 'clang version 20.0.0'
run_clean_env env PATH="$bin:$test_system_path" TRACE="$work/cyg-default.trace" \
    bash "$case_top/distro/rebuild.sh" > "$work/cyg-default.out"
assert_lines 2 "$work/cyg-default.trace"
grep -F 'TC=gcc CC=gcc CXX=g++' "$work/cyg-default.trace" >/dev/null
grep -F 'TC=clang CC=clang CXX=clang++' "$work/cyg-default.trace" >/dev/null
grep -F 'build/aldor-9.9.9-test-cygwin-x86_64-gcc' "$work/cyg-default.trace" >/dev/null
grep -F 'build/aldor-9.9.9-test-cygwin-x86_64-clang' "$work/cyg-default.trace" >/dev/null

# MinGW/MSVC names are reserved future targets but must not be attempted yet.
case_top=$(make_case future-windows-disabled)
bin="$case_top/fakebin"
fake_uname "$bin" CYGWIN_NT-10.0-26100
for tc in mingw-gcc mingw-clang msvc; do
    set +e
    CC= CXX= ALDOR_CC= ALDOR_CXX= PATH="$bin:$test_system_path" TRACE="$work/future-$tc.trace" \
        ALDOR_TOOLCHAIN="$tc" bash "$case_top/distro/rebuild.sh" all \
        > "$work/future-$tc.out" 2>&1
    status=$?
    set -e
    [ "$status" -eq 2 ]
    test ! -s "$work/future-$tc.trace"
    grep -F 'recognized but not enabled for builds yet' "$work/future-$tc.out" >/dev/null
done

# A failed first toolchain must not prevent the second selected toolchain from
# being exercised; the aggregate command must nevertheless fail.
case_top=$(make_case continue-after-failure)
bin="$case_top/fakebin"
fake_uname "$bin" Darwin
fake_compiler "$bin" gcc gcc 15.1.0 arm64-apple-darwin25 'gcc 15.1.0'
fake_compiler "$bin" g++ gcc 15.1.0 arm64-apple-darwin25 'g++ 15.1.0'
fake_compiler "$bin" clang clang 20.0.0 arm64-apple-darwin25 'clang 20.0.0'
fake_compiler "$bin" clang++ clang 20.0.0 arm64-apple-darwin25 'clang++ 20.0.0'
set +e
CC= CXX= ALDOR_CC= ALDOR_CXX= PATH="$bin:$test_system_path" TRACE="$work/failure.trace" FAIL_TC=gcc \
    bash "$case_top/distro/rebuild.sh" all > "$work/failure.out" 2>&1
status=$?
set -e
[ "$status" -ne 0 ]
assert_lines 2 "$work/failure.trace"
grep -F 'TC=clang ' "$work/failure.trace" >/dev/null
grep -F 'failed: gcc:' "$work/failure.out" >/dev/null

# Both accepted help spellings are side-effect free and explain family-based
# identification plus the independent-root and Cygwin override conventions.
case_top=$(make_case help)
bin="$case_top/fakebin"
fake_uname "$bin" Linux
TRACE="$work/help.trace" PATH="$bin:$test_system_path" \
    bash "$case_top/distro/rebuild.sh" --help > "$work/help.out"
[ ! -e "$work/help.trace" ]
grep -F 'build/aldor-<version>-<os>-<arch>-<toolchain>' "$work/help.out" >/dev/null
grep -F 'mingw-gcc, mingw-clang, and msvc' "$work/help.out" >/dev/null
grep -F 'compiler predefined' "$work/help.out" >/dev/null
TRACE="$work/help2.trace" PATH="$bin:$test_system_path" \
    bash "$case_top/distro/rebuild.sh" help > "$work/help2.out"
[ ! -e "$work/help2.trace" ]

# The package-root build wrapper must route direct builds into build/ and must
# not recreate the retired top-level logs/ directory.  Macro-based compiler
# family detection still determines the toolchain component of the root name.
if [ -f "$package_top/build.sh" ]; then
    wrap_case="$work/build-root-family/aldor-v9.9.9-test"
    mkdir -p "$wrap_case/distro/aldor/src" "$wrap_case/fakebin"
    cp "$package_top/build.sh" "$wrap_case/build.sh"
    cp "$package_top/distro/build-fns.sh" "$wrap_case/distro/build-fns.sh"
    cat > "$wrap_case/distro/aldor/src/version.h" <<'VER'
#define ALDOR_VERSION_STRING "9.9.9-test"
VER
    cat > "$wrap_case/distro/build.sh" <<'WRAP_STUB'
#!/bin/sh
printf 'ROOT=%s BUILD_ROOT=%s\n' "$ALDORROOT" "$ALDOR_BUILD_ROOT" > "$TRACE"
exit 0
WRAP_STUB
    chmod +x "$wrap_case/distro/build.sh"
    fake_uname "$wrap_case/fakebin" Linux x86_64
    fake_compiler "$wrap_case/fakebin" gcc clang 21.0.0 x86_64-linux-gnu \
        'gcc (GCC) deliberately misleading banner'
    fake_compiler "$wrap_case/fakebin" g++ clang 21.0.0 x86_64-linux-gnu \
        'g++ (GCC) deliberately misleading banner'
    TRACE="$work/build-root-family.trace" PATH="$wrap_case/fakebin:$test_system_path" \
        CC=gcc CXX=g++ ALDOR_TOOLCHAIN=auto ALDOR_SKIP_HARNESS_SMOKE=1 \
        bash "$wrap_case/build.sh" all > "$work/build-root-family.out"
    grep -E '/build/aldor-9\.9\.9-test-[a-z0-9._-]+-x86_64-clang' \
        "$work/build-root-family.trace" >/dev/null
    [ ! -d "$wrap_case/logs" ]
fi

# The lower-level all path must carry the root chosen by rebuild.sh through
# reset, build and test.  Stub doMain so this test exercises only root routing.
root_case="$work/root-routing/pkg"
mkdir -p "$root_case/distro"
cp "$package_top/distro/build.sh" "$root_case/distro/build.sh"
cat > "$root_case/distro/build-fns.sh" <<'ROOT_STUB'
initializeAldorRoot() { :; }
relativizeFileContents() { :; }
checkForErrors() { :; }
showAldorToolchain() { :; }
doMain() {
    printf 'CMD=%s ROOT=%s BUILD_ROOT=%s\n' \
        "${1:-build}" "$ALDORROOT" "${ALDOR_BUILD_ROOT:-}" >> "$TRACE"
}
ROOT_STUB
custom_root="$root_case/build/aldor-9.9.9-test-testos-x86_64-gcc"
TRACE="$work/root-routing.trace" ALDOR_BUILD_ROOT="$custom_root" \
    bash "$root_case/distro/build.sh" all > "$work/root-routing.out"
assert_lines 3 "$work/root-routing.trace"
[ "$(grep -F "ROOT=$custom_root BUILD_ROOT=$custom_root" \
        "$work/root-routing.trace" | wc -l | tr -d ' ')" = 3 ]
grep -F "Build root: $custom_root" "$work/root-routing.out" >/dev/null

printf 'rebuild-driver-test: PASS\n'
