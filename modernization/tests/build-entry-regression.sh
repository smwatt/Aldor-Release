#!/usr/bin/env bash
# Copyright (C) 2026 Stephen M. Watt.
# See legal/LICENSE in the Aldor distribution for licensing terms.
set -euo pipefail

# Regression checks for the top-level distro build entry point.  These checks
# deliberately do not build Aldor; they exercise root validation and cwd
# recovery with the doMain "none" action.
script_ref=${BASH_SOURCE[0]}
case "$script_ref" in
  */*) script_parent=${script_ref%/*} ;;
  *) script_parent=. ;;
esac
here=$(cd -P -- "$script_parent" && pwd -P)
distro=$(cd -P -- "$here/../.." && pwd -P)
package=$(cd -P -- "$distro/.." && pwd -P)
work=$(mktemp -d "${TMPDIR:-/tmp}/aldor-build-entry.XXXXXX")
trap 'rm -rf "$work"' EXIT

fail() {
  echo "FAIL: $*" >&2
  exit 1
}

# The production build driver must source the helper from this distro by
# absolute path, never by PATH lookup.
grep -Fq 'source "$ALDORDISTRO_PHYS/build-fns.sh"' "$distro/build.sh" || \
  fail "build.sh does not bind its own build-fns.sh explicitly"

# Build/test logs are archival evidence.  Their configuration headers must
# retain real source/build paths rather than post-processing them to symbolic
# $ALDORROOT/$ALDORDISTRO placeholders.
if grep -q 'relativizeFileContents "\$log"' "$distro/build.sh"; then
  fail "build.sh still relativizes archival build/test logs"
fi

# A root containing the source distro must be rejected before any removal.
if ALDORROOT="$package" ALDORTMP="$work/unsafe-tmp" \
     bash "$distro/build.sh" none >"$work/unsafe.out" 2>&1; then
  fail "unsafe ALDORROOT ancestor was accepted"
fi
grep -q 'Refusing unsafe ALDORROOT' "$work/unsafe.out" || \
  fail "unsafe ALDORROOT was rejected without the expected diagnostic"
[ -f "$distro/build.sh" ] || fail "unsafe-root check damaged the distro"

# Absolute invocation must work from a directory other than the distro.
mkdir -p "$work/outside" "$work/root-outside"
(
  cd "$work/outside"
  ALDORROOT="$work/root-outside" bash "$distro/build.sh" none
) >"$work/outside.out" 2>&1 || fail "absolute build.sh invocation from outside distro failed"

# On Linux an absolute pathname can recover even if Bash inherits a cwd whose
# parent hierarchy has just been removed.  Bash itself may print a shell-init
# getcwd warning before our script starts; successful doMain completion is the
# property being tested here.
if [ "$(uname -s 2>/dev/null)" = Linux ]; then
  mkdir -p "$work/stale-parent/a/b" "$work/root-stale"
  (
    cd "$work/stale-parent/a/b"
    rm -rf "$work/stale-parent"
    ALDORROOT="$work/root-stale" bash "$distro/build.sh" none
  ) >"$work/stale.out" 2>&1 || \
    fail "absolute build.sh did not recover from stale cwd"

  grep -q 'Entering' "$work/stale.out" || \
    fail "stale-cwd check did not reach doMain"
fi

# A root not built by this exact distro must not be accepted for testing when
# the caller explicitly disables package-root fallback.
mkdir -p "$work/root-old/toolbin"
printf '%s\n' '/some/other/aldor/distro' > "$work/root-old/.aldor-distro-source"
: > "$work/root-old/toolbin/testaldor"
chmod +x "$work/root-old/toolbin/testaldor"
if ALDORROOT="$work/root-old" ALDOR_NO_PACKAGE_ROOT_FALLBACK=1 \
     bash "$distro/build.sh" test >"$work/stale-root.out" 2>&1; then
  fail "stale/mismatched ALDORROOT was accepted for testing"
fi
grep -q 'Refusing to test a stale or mismatched ALDORROOT' "$work/stale-root.out" || \
  fail "stale test root was rejected without the expected diagnostic"

# A completed package-local root must win over an unrelated inherited
# ALDORROOT for a plain `build.sh test`.  Use a tiny synthetic package so this
# checks selection logic without running the real compiler suite.
fakepkg="$work/fallback-package"
fakedistro="$fakepkg/distro"
case "$(uname -s 2>/dev/null)" in
  Darwin) fakeos=macos ;;
  CYGWIN*) fakeos=cygwin ;;
  Linux)
    fakeos=$(sed -n 's/^ID=//p' /etc/os-release 2>/dev/null | head -1 | tr -d '"')
    [ -n "$fakeos" ] || fakeos=linux
    ;;
  *) fakeos=$(uname -s 2>/dev/null | tr '[:upper:]' '[:lower:]') ;;
esac
fakearch=$(printf '%s' "$(uname -m 2>/dev/null || printf unknown)" | tr '[:upper:]' '[:lower:]' | tr -c 'a-z0-9._-' '_')
defs=$(${CC:-cc} -dM -E -x c - </dev/null 2>/dev/null || true)
if printf '%s\n' "$defs" | grep -q '^#define __clang__ '; then
  faketc=clang
elif printf '%s\n' "$defs" | grep -q '^#define __GNUC__ '; then
  faketc=gcc
else
  faketc=unknown
fi
fakeroot="$fakepkg/build/aldor-9.9.9-test-${fakeos}-${fakearch}-${faketc}"
mkdir -p "$fakedistro/aldor/src" "$fakedistro/lib" "$fakedistro/modernization" "$fakeroot/toolbin"
printf '#define ALDOR_VERSION_STRING "9.9.9-test"\n' > "$fakedistro/aldor/src/version.h"
cp "$distro/build.sh" "$distro/build-fns.sh" "$fakedistro/"
printf '#!/bin/sh\nexit 0\n' > "$fakedistro/aldor/build.sh"
printf '#!/bin/sh\nexit 0\n' > "$fakedistro/lib/build.sh"
printf '#!/bin/sh\nexit 0\n' > "$fakedistro/modernization/validate.sh"
printf '#!/bin/sh\nexit 0\n' > "$fakedistro/modernization/make-test-report.sh"
printf '#!/bin/sh\nexit 0\n' > "$fakedistro/modernization/runtime-smoke.sh"
printf '#!/bin/sh\nexit 0\n' > "$fakedistro/modernization/classify-results.sh"
chmod +x "$fakedistro/aldor/build.sh" "$fakedistro/lib/build.sh" \
  "$fakedistro/modernization/validate.sh" "$fakedistro/modernization/make-test-report.sh" \
  "$fakedistro/modernization/runtime-smoke.sh" "$fakedistro/modernization/classify-results.sh"
: > "$fakeroot/toolbin/testaldor"
chmod +x "$fakeroot/toolbin/testaldor"
printf '%s\n' "$(cd -P "$fakedistro" && pwd -P)" > "$fakeroot/.aldor-distro-source"
if ! env -u ALDOR_BUILD_ROOT ALDORROOT="$work/root-old" \
     ALDOR_TOOLCHAIN="$faketc" \
     bash "$fakedistro/build.sh" test >"$work/fallback-root.out" 2>&1; then
  fail "package-local completed root was not selected for test"
fi
grep -q 'Ignoring inherited ALDORROOT for test' "$work/fallback-root.out" || \
  fail "package-root fallback did not diagnose the inherited stale root"
grep -q "Using this package's completed test root" "$work/fallback-root.out" || \
  fail "package-root fallback did not announce the selected root"

# build.log and test.log are the only top-level logs in a configuration root.
# Each log must begin with self-contained configuration metadata and end with
# an explicit elapsed-time/status footer suitable for archival comparison.
logpkg="$work/log-package"
logdistro="$logpkg/distro"
logroot="$logpkg/build/aldor-9.9.9-test-testos-testarch-testtool"
mkdir -p "$logdistro/aldor/src" "$logdistro/lib" \
  "$logdistro/modernization/benchmark"
cp "$distro/build.sh" "$logdistro/build.sh"
cat > "$logdistro/aldor/src/version.h" <<'EOF'
#define ALDOR_VERSION_STRING "9.9.9-test"
EOF
cat > "$logdistro/build-fns.sh" <<'EOF'
initializeAldorRoot() {
  mkdir -p "$ALDORROOT/toolbin" "$ALDORROOT/testout"
  : > "$ALDORROOT/toolbin/testaldor"
  chmod +x "$ALDORROOT/toolbin/testaldor"
}
requireGmp() { return 0; }
showAldorToolchain() { echo 'stub toolchain'; }
relativizeFileContents() { :; }
checkForErrors() { return 0; }
doMain() {
  case "${1:-build}" in
    build) doBuild ;;
    test)  doTest ;;
    reset) doReset ;;
    none)  echo 'Entering none';;
    *) return 0 ;;
  esac
}
EOF
for d in aldor lib; do
  cat > "$logdistro/$d/build.sh" <<'EOF'
#!/bin/sh
printf 'stub phase: %s\n' "${1:-build}"
exit 0
EOF
  chmod +x "$logdistro/$d/build.sh"
done
for f in validate.sh make-test-report.sh runtime-smoke.sh classify-results.sh; do
  cat > "$logdistro/modernization/$f" <<'EOF'
#!/bin/sh
exit 0
EOF
  chmod +x "$logdistro/modernization/$f"
done
ALDOR_BUILD_ROOT="$logroot" ALDORROOT="$logroot" ALDORTMP="$logroot/tmp" \
  ALDOR_TOOLCHAIN=testtool \
  bash "$logdistro/build.sh" build > "$work/log-build.out" 2>&1 || \
  fail "synthetic build log run failed"
ALDOR_BUILD_ROOT="$logroot" ALDORROOT="$logroot" ALDORTMP="$logroot/tmp" \
  ALDOR_TOOLCHAIN=testtool \
  bash "$logdistro/build.sh" test > "$work/log-test.out" 2>&1 || \
  fail "synthetic test log run failed"
[ "$(find "$logroot" -maxdepth 1 -type f -name '*.log' | wc -l | tr -d ' ')" = 2 ] || \
  fail "configuration root does not contain exactly build.log and test.log"
[ "$(head -1 "$logroot/build.log")" = 'Aldor build log' ] || fail "build.log header missing"
[ "$(head -1 "$logroot/test.log")" = 'Aldor test log' ] || fail "test.log header missing"
for f in "$logroot/build.log" "$logroot/test.log"; do
  grep -q '^Version:        9.9.9-test$' "$f" || fail "version missing from $f header"
  grep -q '^Host:           ' "$f" || fail "host missing from $f header"
  grep -q '^Date:           ' "$f" || fail "date missing from $f header"
  grep -q '^Architecture:   ' "$f" || fail "architecture missing from $f header"
  grep -q '^Toolchain:      testtool$' "$f" || fail "toolchain missing from $f header"
  grep -q '^Build dir:      ' "$f" || fail "build directory missing from $f header"
  tail -8 "$f" | grep -q '^Elapsed:         ' || fail "elapsed footer missing from $f"
  tail -8 "$f" | grep -q '^Elapsed seconds: ' || fail "elapsed seconds missing from $f"
  [ "$(tail -1 "$f")" = 'Exit status:     0' ] || fail "status is not final line of $f"
done

# Official full packages contain .git.  Log metadata must recover the exact
# commit even when the git executable itself is unavailable.
mkdir -p "$logpkg/.git/refs/heads"
printf 'ref: refs/heads/release-test\n' > "$logpkg/.git/HEAD"
printf '0123456789abcdef0123456789abcdef01234567\n' > \
  "$logpkg/.git/refs/heads/release-test"
rm -f "$logroot/build.log" "$logroot/test.log"
PATH_NO_GIT="$work/no-git-bin"
mkdir -p "$PATH_NO_GIT"
cat > "$PATH_NO_GIT/git" <<'EOF'
#!/bin/sh
exit 127
EOF
chmod +x "$PATH_NO_GIT/git"
PATH="$PATH_NO_GIT:$PATH" ALDOR_BUILD_ROOT="$logroot" ALDORROOT="$logroot" \
  ALDORTMP="$logroot/tmp" ALDOR_TOOLCHAIN=testtool \
  bash "$logdistro/build.sh" build > "$work/log-build-nogit.out" 2>&1 || \
  fail "synthetic no-git build log run failed"
grep -q '^Git HEAD:       0123456789abcdef0123456789abcdef01234567$' \
  "$logroot/build.log" || fail "build.log did not recover embedded Git HEAD"

# Darwin must default to Apple's native archive tools rather than whichever
# GNU binutils happen to occur first in PATH.  Xcode 26 requires 64-bit Mach-O
# archive members to have Apple's 8-byte member alignment.  ALDOR_AR and
# ALDOR_RANLIB remain explicit overrides (tested by the next block).
mkdir -p "$work/fake-darwin-default-bin" "$work/archive-default-root"
cat > "$work/fake-darwin-default-bin/uname" <<'EOF'
#!/bin/sh
echo Darwin
EOF
chmod +x "$work/fake-darwin-default-bin/uname"
(
  set +u
  export PATH="$work/fake-darwin-default-bin:$PATH"
  export ALDORROOT="$work/archive-default-root"
  export ALDOR_TOOLCHAIN=gcc
  export ALDOR_CC="${CC:-cc}"
  export ALDOR_CXX="${CXX:-c++}"
  export ALDOR_AR_FLAGS="" ALDOR_RANLIB_FLAGS=""
  export ALDOR_GMP_INCLUDE_DIR="" ALDOR_GMP_LIB_DIR=""
  unset ALDOR_AR ALDOR_RANLIB ALDOR_LIBTOOL HOST_AR HOST_RANLIB AR RANLIB
  source "$distro/build-fns.sh"
  [ ! -x /usr/bin/ar ] || [ "$ALDOR_AR" = /usr/bin/ar ]
  [ ! -x /usr/bin/ranlib ] || [ "$ALDOR_RANLIB" = /usr/bin/ranlib ]
  [ ! -x /usr/bin/libtool ] || [ "$ALDOR_LIBTOOL" = /usr/bin/libtool ]
) || fail "Darwin did not select the native system archive tools by default"

# Darwin native archive policy must construct the final `.a` with Apple
# libtool -static.  Apple ar remains the member-operation helper and explicit
# ranlib refreshes retain the quiet no-symbol option.  Exercise the policy
# with fake uname/ar/libtool/ranlib so the test is host-independent.
mkdir -p "$work/fake-darwin-bin" "$work/archive-policy-root"
cat > "$work/fake-darwin-bin/uname" <<'EOF'
#!/bin/sh
echo Darwin
EOF
cat > "$work/fake-ar" <<'EOF'
#!/bin/sh
printf '%s\n' "$*" >> "$AR_TRACE"
exec "$REAL_AR" "$@"
EOF
cat > "$work/fake-libtool" <<'EOF'
#!/bin/sh
printf '%s\n' "$*" >> "$LIBTOOL_TRACE"
[ "$1" = -static ] || exit 2
shift
while [ "$#" -gt 0 ] && [ "$1" != -o ]; do
    shift
done
[ "$1" = -o ] || exit 2
out=$2
shift 2
rm -f "$out"
# A portable stand-in for Apple libtool sufficient for this regression:
# flatten ordinary object inputs and archive inputs into a fresh archive.
tmp="${out}.fake-libtool.$$"
mkdir "$tmp" || exit 1
trap 'rm -rf "$tmp"' 0 1 2 3 15
for inarg in "$@"; do
    case "$inarg" in
        *.a)
            (cd "$tmp" && "$REAL_AR" x "$inarg") || exit 1
            ;;
        *) cp "$inarg" "$tmp/${inarg##*/}" || exit 1 ;;
    esac
done
set -- "$tmp"/*
[ -e "$1" ] || { printf '!<arch>\n' > "$out"; exit 0; }
"$REAL_AR" rc "$out" "$@"
EOF
cat > "$work/fake-ranlib" <<'EOF'
#!/bin/sh
printf '%s\n' "$*" >> "$RANLIB_TRACE"
case " $* " in
  *" -no_warning_for_no_symbols "*) exit 0 ;;
  *) exit 2 ;;
esac
EOF
chmod +x "$work/fake-darwin-bin/uname" "$work/fake-ar" \
    "$work/fake-libtool" "$work/fake-ranlib"
(
  set +u
  export PATH="$work/fake-darwin-bin:$distro/aldor/tools/unix:$PATH"
  export ALDORROOT="$work/archive-policy-root"
  export ALDOR_TOOLCHAIN=clang
  export ALDOR_CC="${CC:-cc}"
  export ALDOR_CXX="${CXX:-c++}"
  export REAL_AR="$(command -v ar)"
  export ALDOR_AR="$work/fake-ar"
  export ALDOR_LIBTOOL="$work/fake-libtool"
  export ALDOR_RANLIB="$work/fake-ranlib"
  export AR_TRACE="$work/ar.trace"
  export LIBTOOL_TRACE="$work/libtool.trace"
  export RANLIB_TRACE="$work/ranlib.trace"
  export ALDOR_GMP_INCLUDE_DIR="" ALDOR_GMP_LIB_DIR=""
  unset ALDOR_AR_FLAGS ALDOR_RANLIB_FLAGS ALDOR_LIBTOOL_FLAGS
  source "$distro/build-fns.sh"
  [ -z "$ALDOR_AR_FLAGS" ]
  [ "$ALDOR_RANLIB_FLAGS" = "-no_warning_for_no_symbols" ]
  [ "$ALDOR_LIBTOOL_FLAGS" = "-no_warning_for_no_symbols" ]
  # Use real host object files.  In particular, Apple's ar/ranlib diagnose
  # arbitrary text files named .o and do not promise meaningful member/index
  # semantics for them.  The production path always receives real objects.
  cat > "$work/dummy.c" <<'EOF'
int aldor_archive_dummy(void) { return 0; }
EOF
  "$ALDOR_CC" -c "$work/dummy.c" -o "$work/dummy.o"
  arReplaceCmd "$work/dummy.a" "$work/dummy.o"
  ranlibCmd "$work/dummy.a"
  doar r "$work/dummy2.a" "$work/dummy.o"

  # Preserve ar-style replacement and deletion semantics while ensuring every
  # mutating operation is finalized by libtool -static.
  cat > "$work/a.c" <<'EOF'
int aldor_archive_a(void) { return 1; }
EOF
  cat > "$work/b.c" <<'EOF'
int aldor_archive_b(void) { return 3; }
EOF
  "$ALDOR_CC" -c "$work/a.c" -o "$work/a.o"
  "$ALDOR_CC" -c "$work/b.c" -o "$work/b.o"
  doar rv "$work/dummy3.a" "$work/a.o"
  cat > "$work/a.c" <<'EOF'
int aldor_archive_a(void) { return 2; }
EOF
  "$ALDOR_CC" -c "$work/a.c" -o "$work/a.o"
  doar r "$work/dummy3.a" "$work/a.o" "$work/b.o"
  [ "$("$REAL_AR" t "$work/dummy3.a" | grep -c '^a\.o$')" -eq 1 ]
  [ "$("$REAL_AR" t "$work/dummy3.a" | grep -c '^b\.o$')" -eq 1 ]
  doar d "$work/dummy3.a" a.o
  ! "$REAL_AR" t "$work/dummy3.a" | grep '^a\.o$' >/dev/null
  "$REAL_AR" t "$work/dummy3.a" | grep '^b\.o$' >/dev/null
) || fail "Darwin native archive libtool policy failed"
grep -q -- '-static -no_warning_for_no_symbols -o .*dummy.a' "$work/libtool.trace" || \
  fail "arReplaceCmd did not construct the Darwin archive with libtool -static"
grep -q -- '-static -no_warning_for_no_symbols -o .*dummy2.a' "$work/libtool.trace" || \
  fail "doar did not construct the Darwin archive with libtool -static"
grep -q -- '-no_warning_for_no_symbols.*dummy.a' "$work/ranlib.trace" || \
  fail "ranlibCmd did not propagate the probed Darwin no-symbol option"

# macOS GMP discovery must use Homebrew's package prefix when pkg-config does
# not provide GMP, without hard-coding Intel or Apple-Silicon Homebrew paths.
mkdir -p "$work/fake-gmp-bin" "$work/fake-gmp-prefix/include" "$work/fake-gmp-prefix/lib" "$work/gmp-root"
cat > "$work/fake-gmp-bin/uname" <<'EOF'
#!/bin/sh
echo Darwin
EOF
cat > "$work/fake-gmp-bin/pkg-config" <<'EOF'
#!/bin/sh
exit 1
EOF
cat > "$work/fake-gmp-bin/brew" <<EOF
#!/bin/sh
[ "\$1" = --prefix ] && [ "\$2" = gmp ] && { echo "$work/fake-gmp-prefix"; exit 0; }
exit 1
EOF
chmod +x "$work/fake-gmp-bin/uname" "$work/fake-gmp-bin/pkg-config" "$work/fake-gmp-bin/brew"
(
  set +u
  export PATH="$work/fake-gmp-bin:$PATH"
  export ALDORROOT="$work/gmp-root"
  export ALDOR_TOOLCHAIN=gcc
  export ALDOR_CC="${CC:-cc}"
  export ALDOR_CXX="${CXX:-c++}"
  export ALDOR_AR_FLAGS="" ALDOR_RANLIB_FLAGS=""
  unset ALDOR_GMP_PREFIX ALDOR_GMP_INCLUDE_DIR ALDOR_GMP_LIB_DIR
  source "$distro/build-fns.sh"
  [ "$ALDOR_GMP_INCLUDE_DIR" = "$work/fake-gmp-prefix/include" ]
  [ "$ALDOR_GMP_LIB_DIR" = "$work/fake-gmp-prefix/lib" ]
) || fail "Homebrew GMP prefix discovery failed"

# Empty archive creation must produce the standard eight-byte Unix archive
# header.  This is accepted by both GNU and Apple/BSD ar, unlike `ar cr` with
# zero members on macOS.
(
  set +u
  export ALDORROOT="$work/archive-root"
  mkdir -p "$ALDORROOT"
  source "$distro/build-fns.sh"
  createEmptyArchive "$work/empty.a"
  [ "$(wc -c < "$work/empty.a")" -eq 8 ]
  [ -z "$(ar t "$work/empty.a")" ]
) || fail "portable empty-archive initialization failed"

# The standalone distro build driver must not depend on an external dirname.
# Integrated validation prepends Aldor's toolbin, which contains its own
# historical dirname utility and therefore can shadow the host command.
mkdir -p "$work/no-dirname-bin" "$work/no-dirname-root"
cat >"$work/no-dirname-bin/dirname" <<'EOF'
#!/bin/sh
echo "unexpected external dirname invocation" >&2
exit 97
EOF
chmod +x "$work/no-dirname-bin/dirname"
PATH="$work/no-dirname-bin:$PATH" ALDORROOT="$work/no-dirname-root" \
  bash "$distro/build.sh" none >"$work/no-dirname.out" 2>&1 || \
  fail "distro build.sh depends on external dirname"

# Housekeeping through the standalone distro driver must require no ALDORROOT and must
# never print the detailed build context.  During integrated validation the
# source tree has just been used for a build, so this regression must not
# assume it is a pristine release extraction; release packaging separately
# requires `build.sh junk` to be completely silent on a clean package.
env -u ALDORROOT -u ALDORTMP -u INCPATH -u LIBPATH -u ALGEBRAROOT -u ALGEBRA \
  bash "$distro/build.sh" junk >"$work/junk-context.out" 2>&1 || \
  fail "distro junk action failed without ALDORROOT"
if grep -Eq '^(Aldor package build|Version:|Host:|OS:|OS version:|System:|Architecture:|Toolchain:|C compiler:|C version:|C\+\+ compiler:|C\+\+ version:|Git HEAD:|Started:|Log:|Command:)' \
     "$work/junk-context.out"; then
  fail "distro junk action printed build context"
fi

echo "PASS: build entry/root safety regression checks"
