#!/bin/bash
# Copyright (C) 2026 Stephen M. Watt.
# See legal/LICENSE in the Aldor distribution for licensing terms.
set -euo pipefail

SRCROOT="${1:-$(cd "$(dirname "$0")/../.." && pwd)}"
CC="${HOST_CC:-cc}"
TMP=$(mktemp -d "${TMPDIR:-/tmp}/aldor-portability.XXXXXX")
trap 'rm -rf "$TMP"' EXIT
trap 'rc=$?; printf "FAIL: portability regression line %s: %s (status %s)\n" \
    "$LINENO" "$BASH_COMMAND" "$rc" >&2; exit "$rc"' ERR

# Simulate Cygwin only inside Aldor's platform/configuration layer.  Do not
# expose fake x86-64/Cygwin predefined macros to the host C library headers:
# that is not a valid cross-environment test and breaks, for example, native
# RISC-V glibc headers.
cat >"$TMP/cygwin-platform.c" <<'CEOF'
#include "platform.h"
#include "cconfig.h"
#ifndef OS_WINDOWS
# error Cygwin must select the Aldor Windows OS layer
#endif
#ifndef ENV_CYGWIN
# error Cygwin identity must be preserved by platform.h
#endif
#if !defined(CC_GCC) && !defined(CC_CLANG)
# error simulated Cygwin must select a supported GCC/Clang compiler family
#endif
#ifndef HW_X86_64
# error x86_64 Cygwin hardware was not detected
#endif
#if defined(CC_MISSING_STRCASECMP)
# error Cygwin provides POSIX strcasecmp
#endif
#if defined(CC_ISATTY_IS__ISATTY)
# error Cygwin provides POSIX isatty
#endif
#if defined(CC_FILENO_IS__FILENO)
# error Cygwin provides POSIX fileno
#endif
#ifndef CC_HAS_POSIX_PUTENV
# error Cygwin POSIX putenv capability must be recorded in cconfig.h
#endif
#ifndef CC_SUPPLY_DECL_SBRK
# error Cygwin must receive the intptr_t sbrk declaration from cport.h
#endif
static const char actual[] = CONFIGSYS;
static const char expected[] = "win64cygwin";
int main(void)
{
    unsigned i = 0;
    for (;;) {
        if (actual[i] != expected[i]) return 1;
        if (actual[i] == 0) return 0;
        ++i;
    }
}
CEOF

"$CC" -std=c99 -D__CYGWIN__ -D__x86_64__ \
    -U__linux__ -U__APPLE__ -U__MACH__ \
    -U__MINGW32__ -U__MINGW64__ -U_WIN32 -U_WIN64 \
    -I"$SRCROOT/aldor/src" "$TMP/cygwin-platform.c" \
    -o "$TMP/cygwin-platform"
"$TMP/cygwin-platform"

# The Cygwin configuration uses the Aldor Windows OS implementation, but its
# C library remains POSIX.  The integration with the real POSIX/Cygwin headers
# can only be checked meaningfully on a Cygwin host; foreign-host macro
# simulation must not rewrite the target identity seen by native libc headers.
case "$(uname -s 2>/dev/null)" in
  CYGWIN*)
    cat >"$TMP/cygwin-posix-headers.c" <<'CEOF'
#include "cport.h"
#include <sys/stat.h>
int main(void)
{
    (void) strcasecmp("A", "a");
    (void) isatty(0);
    (void) fileno(stdin);
    (void) putenv((char *) "ALDOR_PORTABILITY_TEST=1");
    (void) sbrk(0);
    return 0;
}
CEOF
    "$CC" -std=c99 -D_POSIX_C_SOURCE=200809L \
        -I"$SRCROOT/aldor/src" -I"$SRCROOT/aldor/subcmd/testaldor" \
        "$TMP/cygwin-posix-headers.c" -c -o "$TMP/cygwin-posix-headers.o"
    ;;
esac

# macOS publishes its historical sbrk(int) prototype itself.  The portability
# layer must not overlay the Linux/Cygwin intptr_t declaration on Darwin.
"$CC" -x c -dM -E -D__APPLE__ -D__MACH__ -D__x86_64__ \
    -U__linux__ -U__CYGWIN__ -U__MINGW32__ -U__MINGW64__ -U_WIN32 -U_WIN64 \
    -I"$SRCROOT/aldor/src" -include cconfig.h /dev/null >"$TMP/macos-config.macros"
grep -q '^#define OS_MAC_OSX 1$' "$TMP/macos-config.macros"
if grep -q '^#define CC_SUPPLY_DECL_SBRK 1$' "$TMP/macos-config.macros"; then
    echo 'macOS incorrectly receives the intptr_t sbrk declaration' >&2
    exit 1
fi

# File.uniqueName must not redeclare the system mkstemp symbol using Aldor's
# word-level Foreign-C signature.  A distinct adapter name keeps the generated
# prototype compatible with hosts (notably macOS) that expose mkstemp from
# <stdlib.h>.
grep -q 'salMkstemp: Pointer -> MachineInteger' \
    "$SRCROOT/lib/aldor/src/util/sal_file.as"
if grep -Eq '^[[:space:]]*import[[:space:]]*\{[[:space:]]*mkstemp:' \
    "$SRCROOT/lib/aldor/src/util/sal_file.as"; then
    echo "sal_file.as directly imports libc mkstemp" >&2
    exit 1
fi

cat >"$TMP/mkstemp-prototype.c" <<'CEOF'
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
typedef unsigned long FiWord;
extern FiWord salMkstemp(FiWord);
int main(void) { return 0; }
CEOF
"$CC" -std=c99 -pedantic-errors "$TMP/mkstemp-prototype.c" \
    -c -o "$TMP/mkstemp-prototype.o"

"$CC" -std=c99 -pedantic-errors -D_POSIX_C_SOURCE=200809L \
    -D_DEFAULT_SOURCE -I"$SRCROOT/aldor/src" \
    -c "$SRCROOT/lib/aldor/src/util/sal_util.c" -o "$TMP/sal_util.o"

cat >"$TMP/mkstemp-adapter.c" <<'CEOF'
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
extern unsigned long salMkstemp(unsigned long);
int main(void)
{
    char name[] = "/tmp/aldor-mkstemp-XXXXXX";
    long fd = (long) salMkstemp((unsigned long) (uintptr_t) name);
    if (fd < 0) return 1;
    if (access(name, F_OK) != 0) return 2;
    if (unlink(name) != 0) return 3;
    return 0;
}
CEOF
"$CC" -std=c99 -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE \
    "$TMP/mkstemp-adapter.c" "$TMP/sal_util.o" \
    -o "$TMP/mkstemp-adapter"
"$TMP/mkstemp-adapter"



# The C++ foreign-interface generator writes Aldor source through string
# arguments, not through a printf format string.  Percent therefore must not
# be doubled: "%%" would survive into generated Aldor source and make the
# -Fc++ bridge uncompilable (forn9/forn10).
grep -q 'return strCopy("%");' "$SRCROOT/aldor/src/gencpp.cpp"
if grep -q 'return strCopy("%%")' "$SRCROOT/aldor/src/gencpp.cpp"; then
    echo "gencpp.cpp still doubles Aldor percent in generated source" >&2
    exit 1
fi

# The flattened AxlLib harness has several command-line tests which share the
# historical basic test0.as fixture.  It must be present in a clean package.
test -f "$SRCROOT/lib/axllib/test/test0.as"
grep -q 'f(n: Integer): Integer == (n + 1) \* n' \
    "$SRCROOT/lib/axllib/test/test0.as"

# Replacement-archive tests must use their private temporary library rather
# than modifying $ALDORROOT/lib and contaminating later tests.
for f in arrepl0.sh arrepl1.sh; do
    grep -q '\$DOALDOR -l "\$LIB"' "$SRCROOT/lib/axllib/test/$f"
done

# These Unix test paths must select the installed aldor executable rather than
# unconditionally invoking the native-Windows wrapper aldor.sh.
for f in emit1.sh forn8.sh supcat.sh; do
    grep -q 'ALDOR=aldor' "$SRCROOT/lib/axllib/test/$f"
done

# emit0's first compiler wrapper must return so it can replace itself with the
# deliberately failing second-stage compiler.  `exec` makes that mutation
# unreachable and turns the expected cleanup warning into a successful run.
if grep -Fq 'exec "%s" "$@"' "$SRCROOT/lib/axllib/test/emit0.sh"; then
    echo "emit0.sh broken-compiler wrapper still uses exec" >&2
    exit 1
fi

# C++ foreign-interface tests must be warning-clean without suppressing the
# diagnostic for converting string literals to mutable char*.  Aldor String
# is mutable, so callers which pass literal text must provide writable arrays.
for f in forn9.sh forn10.sh; do
    if grep -q -- '-Wno-write-strings' "$SRCROOT/lib/axllib/test/$f"; then
        echo "$f suppresses mutable-string diagnostics" >&2
        exit 1
    fi
done
if grep -Eq 'ItemPrettyComplex::bracket\([^;]*"' \
    "$SRCROOT/lib/axllib/test/appli1.C"; then
    echo "appli1.C passes a C++ string literal to mutable Aldor String" >&2
    exit 1
fi
if grep -Eq 'MyString::bracket\("' "$SRCROOT/lib/axllib/test/appli2.C"; then
    echo "appli2.C passes a C++ string literal to mutable Aldor String" >&2
    exit 1
fi


# The historical Enum(foo) portability macro is unnecessary with the current
# C99/C++20 compiler requirements and collides with COM methods named Enum in
# modern Windows SDK headers (seen on Cygwin).  Keep the global macro gone.
if grep -Eq '^[[:space:]]*#[[:space:]]*define[[:space:]]+Enum\(' \
    "$SRCROOT/aldor/src/cport.h"; then
    echo "cport.h still defines the global Enum macro" >&2
    exit 1
fi
if grep -R -E -n --include='*.h' --include='*.cpp' --include='*.c0' \
    '(^|[^A-Za-z0-9_])Enum\(' "$SRCROOT/aldor/src" | \
    grep -v 'cport.h:' >/dev/null; then
    echo "compiler sources still use the historical Enum(...) macro" >&2
    exit 1
fi
cat >"$TMP/enum-sdk-collision.cpp" <<'CEOF'
#include "cport.h"
struct ComLike { virtual int Enum(int, int) = 0; };
int main() { return 0; }
CEOF
c++ -std=c++20 -I"$SRCROOT/aldor/src" \
    "$TMP/enum-sdk-collision.cpp" -c -o "$TMP/enum-sdk-collision.o"

# Mutable C-string declarations in the Windows OS layer must point to writable
# storage rather than C++ string literals.  Do not silence -Wwrite-strings.
for f in "$SRCROOT/aldor/src/os_windows.c0" "$SRCROOT/aldor/src/opsys.cpp"; do
    if grep -Eq 'MutString[[:space:]]+[A-Za-z0-9_]+[[:space:]]*=[[:space:]]*"' "$f"; then
        echo "$f initializes MutString directly from a string literal" >&2
        exit 1
    fi
done


# Cygwin's Windows OS implementation must agree exactly with the modern C++
# declarations.  The raw store entry points sit inside ALDOR_C_BEGIN/END so
# this C-source fragment also has the correct C linkage when included by C++.
grep -q 'osGetEnv(String envar)' "$SRCROOT/aldor/src/os_windows.c0"
grep -q 'ALDOR_C_BEGIN' "$SRCROOT/aldor/src/os_windows.c0"
grep -q 'extern MostAlignedType \*__stoAlloc(unsigned code, ULong size);' \
    "$SRCROOT/aldor/src/os_windows.c0"
grep -q 'extern void[[:space:]]*__stoFree(void \*p);' \
    "$SRCROOT/aldor/src/os_windows.c0"
if grep -q 'MostAlignedType \*stoAlloc(unsigned, ULong);' \
    "$SRCROOT/aldor/src/os_windows.c0"; then
    echo "os_windows.c0 still has a block-local store declaration" >&2
    exit 1
fi

# Cygwin stays on the normal B-tree Store.  The CP6 malloc-store experiment
# changed compiler behaviour and was deliberately reverted.
if grep -q 'defined(__CYGWIN__) && !defined(FOAM_RTS)' \
    "$SRCROOT/aldor/src/store.cpp"; then
    echo "store.cpp still contains the rejected Cygwin malloc-store fallback" >&2
    exit 1
fi

# Modern 64-bit Windows/Cygwin processes can contain far more writable VM
# regions than the old Win32-era fixed map allowed.  Keep the map spacious,
# bounds-checked, and pointer-width clean.
grep -q '#define MAX_HEAP_SECTIONS[[:space:]]*65536' \
    "$SRCROOT/aldor/src/os_windows.c0"
grep -q 'Aldor memory map exceeds' "$SRCROOT/aldor/src/os_windows.c0"
grep -q '(UAInt) ptr < (UAInt) osHighTide' "$SRCROOT/aldor/src/os_windows.c0"

# glibc/RISC-V does not implement the GNU trap-enable fenv extensions.
# It must take the ISO-C fallback instead of emitting link-time warnings and
# returning failure from fegetexcept/feenableexcept/fedisableexcept.
grep -q 'defined(CC_FENV_GLIBC_TRAPS)' \
    "$SRCROOT/aldor/src/fenvport.h"

# macOS/modern-Clang portability fixes should be expressed in the source, not
# hidden by diagnostics suppression.
if grep -q '<malloc.h>' "$SRCROOT/lib/axllib/test/cpxadd.c"; then
    echo "cpxadd.c still uses obsolete malloc.h" >&2
    exit 1
fi
grep -q 'extern FiWord fact(FiWord);' "$SRCROOT/lib/axllib/test/expfact.c"
grep -q 'extern void quotient(FiWord, FiWord, FiWord \*, FiWord \*);' \
    "$SRCROOT/lib/axllib/test/expquo.c"
grep -q 'int INIT__0_rtexns(void) { return 0; }' \
    "$SRCROOT/lib/axllib/test/eager0.sh"

# Compile the Darwin-only C++ unicl post-link branch on a real Darwin host.
# Do not fake OS_MAC_OSX on an unrelated architecture: platform.h correctly
# rejects, for example, a synthetic RISC-V macOS target.  The real macOS
# release matrix supplies the required compiler coverage.
if [ "$(uname -s 2>/dev/null || :)" = Darwin ]; then
    "${CXX:-c++}" -std=c++20 -fsyntax-only -DOS_MAC_OSX \
        -I"$SRCROOT/aldor/src" \
        "$SRCROOT/aldor/subcmd/unitools/unicl.cpp"
fi

grep -q 'snprintf(path, pathSize, uclDefaultPath' \
    "$SRCROOT/aldor/subcmd/unitools/unicl.cpp"
if grep -q 'sprintf(path, uclDefaultPath' \
    "$SRCROOT/aldor/subcmd/unitools/unicl.cpp"; then
    echo "unicl still uses deprecated sprintf for its configuration path" >&2
    exit 1
fi

# Cygwin test-driver portability: do not use a pointer-valued variadic list
# terminated by an untyped integer 0, and do not ask Win32 CreateProcess to
# execute the dounicl shell wrapper.  Cygwin PATH lists use ':' even though
# the remainder of the OS implementation follows Windows conventions.
grep -q 'fframeAlloc.*srcExt, int setc' \
    "$SRCROOT/aldor/subcmd/testaldor/testaldor.c"
if grep -Eq 'fframeAlloc\([^;]*,[[:space:]]*0\)' \
    "$SRCROOT/aldor/subcmd/testaldor/testaldor.c"; then
    echo "testaldor still uses a zero sentinel for fframeAlloc" >&2
    exit 1
fi
grep -q 'MutString CC[[:space:]]*=[[:space:]]*"unicl"' \
    "$SRCROOT/aldor/subcmd/testaldor/testaldor.c"
grep -q '#define FPATHSEP[[:space:]]*'"'"':'"'" \
    "$SRCROOT/aldor/src/os_windows.c0"
grep -q 'n = i + 1;' "$SRCROOT/aldor/src/opsys_t.c"
grep -q 'if (!rep_) return;' "$SRCROOT/aldor/src/fname.cpp"

# Portability-layer discipline. Raw compiler/OS predefined macros belong in
# platform.h (or generated parser/scanner sources), not in ordinary sources.
# cconfig.h derives capabilities/defects; cport.h repairs the ordinary C/POSIX
# interface. Included .c0 implementation fragments follow the same rule.
raw_env_re='__CYGWIN__|__MINGW32__|__MINGW64__|__linux__|__APPLE__|__MACH__|__GNUC__|__clang__|__GLIBC__|__arch64__|__sun|__SVR4|_WIN32|_WIN64|_MSC_VER|__BORLANDC__|__STDC__|__MSDOS__|__hpux|__OpenBSD__'
if grep -RInE --include='*.h' --include='*.c' --include='*.cpp' --include='*.c0' \
    "$raw_env_re" \
    "$SRCROOT/aldor/tools/unix" "$SRCROOT/aldor/subcmd" "$SRCROOT/aldor/src" \
    | grep -v '/aldor/src/platform.h:' \
    | grep -v '/aldor/src/axl_y.c:' \
    | grep -v '/aldor/tools/unix/preserve-lex.yy.c:' \
    | grep -v '/aldor/tools/unix/preserve-y.tab.c:'; then
    echo 'raw compilation-environment macro leaked outside platform.h/generated sources' >&2
    exit 1
fi

# The old per-header insulation layer is gone. Ordinary header repairs live
# centrally in cport.h; specialized fenv support is a named platform API.
if find "$SRCROOT/aldor/src" "$SRCROOT/aldor/subcmd" -name '*.h0' -print | grep .; then
    echo 'obsolete .h0 portability wrapper remains' >&2
    exit 1
fi
grep -q 'CC_HAS_POSIX_PUTENV' "$SRCROOT/aldor/src/cconfig.h"
grep -q 'CC_SUPPLY_DECL_SBRK' "$SRCROOT/aldor/src/cconfig.h"
grep -q 'ALDOR_C int putenv(char \*);' "$SRCROOT/aldor/src/cport.h"
grep -q 'ALDOR_C void \*sbrk(intptr_t);' "$SRCROOT/aldor/src/cport.h"
grep -q 'CC_SUPPLY_DECL_SBRK' "$SRCROOT/aldor/src/cport.h"
test -f "$SRCROOT/aldor/src/fenvport.h"
if grep -RInE --include='*.c' --include='*.cpp' --include='*.c0' \
    'extern[[:space:]].*(putenv|sbrk)[[:space:]]*\(' \
    "$SRCROOT/aldor/tools/unix" "$SRCROOT/aldor/subcmd" "$SRCROOT/aldor/src"; then
    echo 'system-interface repair leaked into ordinary source' >&2
    exit 1
fi

# The stdc.cpp -> cport.cpp consolidation must be complete in active runtime
# build recipes, not merely in the main compiler source list.  RC16 initially
# missed the secondary libfoam/GMP recipes, so make this a release invariant.
grep -q 'cport.cpp opsys.cpp' "$SRCROOT/aldor/lib/libfoam/build.sh"
if grep -q 'stdc.cpp' "$SRCROOT/aldor/lib/libfoam/build.sh"; then
    echo 'libfoam build still references removed stdc.cpp' >&2
    exit 1
fi
grep -q 'cport opsys btree' "$SRCROOT/aldor/contrib/gmp/build.sh"
if grep -Eq '(^|[[:space:]])stdc([[:space:]\\]|$)' \
    "$SRCROOT/aldor/contrib/gmp/build.sh"; then
    echo 'GMP runtime build still references removed stdc source stem' >&2
    exit 1
fi

# Supported source is prototype C.  The Cygwin/Windows test-driver fragment
# must not retain the five historical K&R definitions that Clang diagnoses.
if grep -Eq '^(osRun\(cmd\)|osRunOutput\(cmd, fout\)|osRunScript\(cmd, fnout\)|osMakeDir\(fn\)|osRemoveDir\(fn\))$' \
    "$SRCROOT/aldor/subcmd/testaldor/tx_windows.c0"; then
    echo 'K&R function definition remains in tx_windows.c0' >&2
    exit 1
fi

# Generated references must not capture the absolute checkpoint location.
if grep -R -E -n '/(home|Users)/.*/aldor-v0\.18' \
    "$SRCROOT/lib/axllib/testout" >/dev/null; then
    echo "AxlLib reference output contains a host-specific absolute path" >&2
    exit 1
fi

# The synchronized `all` run keeps complete test coverage and, by default,
# preserves full inline old/new diffs.  -nodiff remains available explicitly
# for a deliberately abbreviated run.
grep -q 'export ALDOR_TEST_BRIEF_DIFFS=0' "$SRCROOT/build.sh"
grep -q 'bash build.sh test -nodiff' "$SRCROOT/build.sh"
grep -q 'bash build.sh test "\$@"' "$SRCROOT/lib/axllib/build.sh"


# RC3 Foreign-C/runtime separation.  Foreign C identifiers must never be
# rewritten by name.  Runtime stdio adapters are explicit FOAM ABI functions
# and are called explicitly by the runtime/basic Aldor source.
if grep -q '#define fputc fiFputc' "$SRCROOT/aldor/src/ccode.cpp"; then
    echo "ccode still rewrites user Foreign C identifiers" >&2
    exit 1
fi
# Generated-C ABI header must stay independent of the implementation portability
# umbrella.  Stdio/POSIX adapters are declarations here and implementations in
# foam_c.cpp, so user Foreign C names are not predeclared accidentally.
if grep -q '#include "cport.h"' \
    "$SRCROOT/aldor/src/foam_c.h" \
    "$SRCROOT/aldor/src/foam_c0.h" \
    "$SRCROOT/aldor/src/foamopt.h" \
    "$SRCROOT/aldor/src/optcfg.h"; then
    echo 'generated-C ABI header transitively imports cport.h' >&2
    exit 1
fi
for name in fiFopen fiFclose fiFgetc fiFgets fiFputc fiFputs fiFseek fiFtell; do
    grep -q "extern FiWord $name" "$SRCROOT/aldor/src/foam_c.h"
    grep -q "^$name(" "$SRCROOT/aldor/src/foam_c.cpp"
done
grep -q 'extern void   fiFflush ' "$SRCROOT/aldor/src/foam_c.h"
grep -q '^fiFflush(FiWord stream)' "$SRCROOT/aldor/src/foam_c.cpp"
grep -q '^#define FiWord_UByte' "$SRCROOT/aldor/src/foam_c.cpp"

# The portability layer is below FOAM in the dependency graph.  No FOAM
# type/name or FOAM ABI include may occur there.
portability_files=(
    "$SRCROOT/aldor/src/platform.h"
    "$SRCROOT/aldor/src/cconfig.h"
    "$SRCROOT/aldor/src/cport.h"
    "$SRCROOT/aldor/src/cport.cpp"
    "$SRCROOT/aldor/src/fenvport.h"
)
if grep -En '(^|[^[:alnum:]_])(fi[A-Z]|Fi[A-Z])' \
    "${portability_files[@]}" >/dev/null; then
    echo 'portability layer contains a FOAM fi*/Fi* name' >&2
    exit 1
fi
if grep -n 'foam_c\.h' "${portability_files[@]}" >/dev/null; then
    echo 'portability layer imports the FOAM generated-C ABI header' >&2
    exit 1
fi
grep -q 'fiFputc:' "$SRCROOT/aldor/lib/libfoamlib/basic.as"
grep -q 'fiFputs:' "$SRCROOT/aldor/lib/libfoamlib/basic.as"
grep -q 'fiFputc:' "$SRCROOT/lib/axllib/src/basic.as"
grep -q 'fiFputs:' "$SRCROOT/lib/axllib/src/basic.as"
if grep -R -F -n 'Foreign C ""' "$SRCROOT/aldor/lib" "$SRCROOT/lib/axllib/src" >/dev/null; then
    echo "shipped library still contains Foreign C with an empty header" >&2
    exit 1
fi
grep -q 'Empty header name' "$SRCROOT/aldor/src/abnorm.cpp"

# RC4 generated-C prototypes.  Both headerless `Foreign C' and plain
# `Foreign' carry the Aldor function signature through FOAM so genc can emit
# a real C prototype.  The optimizer must preserve/remap that signature.
grep -q 'forg->protocol == FOAM_Proto_Other' \
    "$SRCROOT/aldor/src/genfoam.cpp"
grep -q 'p == FOAM_Proto_C || p == FOAM_Proto_Other' \
    "$SRCROOT/aldor/src/genc.cpp"
grep -q 'case FOAM_Proto_Other:' "$SRCROOT/aldor/src/of_deadv.cpp"
grep -q 'case FOAM_Proto_Other:' "$SRCROOT/aldor/src/of_inlin.cpp"
grep -q 'ccoTypeIdOf("void")' "$SRCROOT/aldor/src/genc.cpp"

# Shipped code that needs the word-sized FOAM/libc bridge names those
# adapters explicitly.  There must be no return to spelling-based C-name
# rewriting in the compiler.
if grep -Eq '#define[[:space:]]+(fopen|fclose|fflush|fgetc|fgets|fputc|fputs|fseek|ftell|powf|system)[[:space:]]+fi' \
    "$SRCROOT/aldor/src/ccode.cpp"; then
    echo "ccode again rewrites C library identifiers to FOAM adapters" >&2
    exit 1
fi
grep -q 'fiFopen:'  "$SRCROOT/aldor/lib/libfoamlib/file.as"
grep -q 'fiFgetc:'  "$SRCROOT/lib/aldor/src/base/sal_tstream.as"
grep -q 'fiFflush:' "$SRCROOT/lib/aldor/src/base/sal_bstream.as"
grep -q 'fiPowf:'   "$SRCROOT/lib/aldor/src/arith/sal_sfloat.as"
grep -q 'fiSystemVoid:' \
    "$SRCROOT/lib/algebra/src/extree/parser/sit_maple.as"
bad_flush_name='fiFflush''Void'
if grep -RIn --exclude-dir=Version-1.5-work --exclude='*.md' \
    "$bad_flush_name" \
    "$SRCROOT/aldor/src" "$SRCROOT/aldor/lib" "$SRCROOT/lib" >/dev/null; then
    echo 'alternate FOAM flush adapter remains in active source' >&2
    exit 1
fi

# RC2 Darwin -grun repair: C++ unicl itself owns post-link executable
# preparation; this must not depend on the obsolete dounicl wrapper.
grep -q 'uclPrepareLinkedOutput' \
    "$SRCROOT/aldor/subcmd/unitools/unicl.cpp"
grep -q 'codesign --force --sign - ' \
    "$SRCROOT/aldor/subcmd/unitools/unicl.cpp"

# RC7: unicl's private -Wn/-Wv/etc. options are exact spellings.  Native
# compiler warning options such as -Wno-unused-value must pass through and
# must never be mistaken for -Wn (no execute).
grep -q 'isArgument(argv\[i\], "Wn")' \
    "$SRCROOT/aldor/subcmd/unitools/unicl.cpp"
grep -q 'hasArgumentPrefix(argv\[i\], "Wopts=")' \
    "$SRCROOT/aldor/subcmd/unitools/unicl.cpp"
grep -q 'uclStartOptions = listNConc1' \
    "$SRCROOT/aldor/subcmd/unitools/unicl.cpp"
if grep -q 'checkArgument(argv\[i\], "Wn")' \
    "$SRCROOT/aldor/subcmd/unitools/unicl.cpp"; then
    echo "unicl still prefix-matches -Wn" >&2
    exit 1
fi

# RC6: explicit runtime adapter names must remain executable in the FOAM
# interpreter.  The aliases are interpreter implementation details only;
# they do not alter Foreign C name resolution or generated C spelling.
grep -q 'DECL_FOREIGN_ALIAS("fiFputc", fputc)' \
    "$SRCROOT/aldor/src/fint.cpp"
grep -q 'DECL_FOREIGN_ALIAS("fiFputs", fputs)' \
    "$SRCROOT/aldor/src/fint.cpp"
grep -q 'DECL_FOREIGN_ALIAS("fiFopen", fopen)' \
    "$SRCROOT/aldor/src/fint.cpp"
grep -q 'DECL_FOREIGN_ALIAS("fiFclose", fclose)' \
    "$SRCROOT/aldor/src/fint.cpp"
grep -q 'DECL_FOREIGN(fiFflush)' \
    "$SRCROOT/aldor/src/fint.cpp"
grep -q 'case FINT_FOREIGN_fiFflush:' \
    "$SRCROOT/aldor/src/fint.cpp"
if grep -q "$bad_flush_name" "$SRCROOT/aldor/src/fint.cpp"; then
    echo 'alternate FOAM flush interpreter entry remains' >&2
    exit 1
fi
grep -q 'DECL_FOREIGN_ALIAS("fiFgetc", fgetc)' \
    "$SRCROOT/aldor/src/fint.cpp"
grep -q 'DECL_FOREIGN_ALIAS("fiFgets", fgets)' \
    "$SRCROOT/aldor/src/fint.cpp"
grep -q 'DECL_FOREIGN_ALIAS("fiFseek", fseek)' \
    "$SRCROOT/aldor/src/fint.cpp"
grep -q 'DECL_FOREIGN_ALIAS("fiFtell", ftell)' \
    "$SRCROOT/aldor/src/fint.cpp"
grep -q 'retDataObj->fiWord = fiPowf(expr1.fiWord, expr2.fiWord);' \
    "$SRCROOT/aldor/src/fint.cpp"
grep -q 'FINT_FOREIGN_fiSystemVoid' "$SRCROOT/aldor/src/fint.cpp"

# RC6: declaration buffering is retained solely to preserve stable generated-C
# formatting.  No C identifier may be rewritten while that buffer is active.
grep -q 'BufferOutput\[100000\]' "$SRCROOT/aldor/src/ccode.cpp"
grep -q 'ccoPutsFileOnly(BufferOutput)' "$SRCROOT/aldor/src/ccode.cpp"
if grep -Eq '#define[[:space:]]+(fopen|fclose|fflush|fgetc|fgets|fputc|fputs|fseek|ftell|powf|system)[[:space:]]+fi' \
    "$SRCROOT/aldor/src/ccode.cpp"; then
    echo "RC6 declaration buffer again rewrites C identifiers" >&2
    exit 1
fi

# RC6: link file options are rebased after the link-directory chdir, and a
# successful linker return is not accepted unless the requested output exists.
grep -q 'if (ccIsFileOption(o->tag))' "$SRCROOT/aldor/src/ccomp.cpp"
grep -q 'linker returned success but output' "$SRCROOT/aldor/src/ccomp.cpp"
grep -q 'executable is missing immediately before run' "$SRCROOT/aldor/src/ccomp.cpp"
grep -q 'linked executable disappeared during cleanup' "$SRCROOT/aldor/src/emit.cpp"
grep -q '!files\[ix\].equals(execfn)' "$SRCROOT/aldor/src/emit.cpp"

# Release identity has numeric fields, a suffix, and a canonical version
# string.  The version string must be exactly the numeric base followed by the
# suffix, so punctuation such as the leading hyphen in "-alpha" is checked.
python3 - "$SRCROOT/aldor/src/version.h" <<'PYVER'
import re, sys
text=open(sys.argv[1], encoding='utf-8').read()
def one(pattern):
    m=re.search(pattern, text)
    if not m:
        raise SystemExit(f'missing version field: {pattern}')
    return m.group(1)
major=one(r'#define ALDOR_VERSION_MAJOR\s+(\d+)')
minor=one(r'#define ALDOR_VERSION_MINOR\s+(\d+)')
micro=one(r'#define ALDOR_VERSION_MICRO\s+(-?\d+)')
suffix=one(r'#define ALDOR_VERSION_SUFFIX\s+"([^"]*)"')
version=one(r'#define ALDOR_VERSION_STRING\s+"([^"]*)"')
expected=f'{major}.{minor}.{micro}{suffix}'
if version != expected:
    raise SystemExit(
        f'inconsistent Aldor version identity: {version} != {expected}')
PYVER

# RC26: the host-interface probe remains diagnostic/preferential, but a
# coherent compiler family may not be silently dropped when every candidate
# fails it.  The full build must be attempted so its normal log records the
# result.
grep -q '#include <stdio.h>' "$SRCROOT/rebuild.sh"
grep -q '#include <unistd.h>' "$SRCROOT/rebuild.sh"
grep -q 'building %s/%s anyway so the result is logged' \
    "$SRCROOT/rebuild.sh"

# RC20.1/RC20.4: a bare variable reference in a FOAM_Seq has no effect and
# should be removed early, but expression lowering can also synthesize a
# temporary identifier after it has already emitted the real C operation.
# Keep the narrow FOAM cleanup, do not clear phase-shared inliner metadata,
# and centrally reject CCO_Stat(CCO_Id) in GenC.
grep -q 'foamTag(foam) == FOAM_Loc' "$SRCROOT/aldor/src/of_deadv.cpp"
grep -q 'foamTag(foam) == FOAM_Glo' "$SRCROOT/aldor/src/of_deadv.cpp"
grep -A30 'dvRemoveSeqNops(Foam seq)' "$SRCROOT/aldor/src/of_deadv.cpp" | \
    grep -q 'foamPure(foam)'
if grep -q 'usedefChainsFreeFrFlog(inlProg->flog);' \
    "$SRCROOT/aldor/src/of_inlin.cpp" "$SRCROOT/aldor/src/inlutil.cpp"; then
    echo 'inliner finalization still clears phase-shared FOAM info' >&2
    exit 1
fi

grep -A25 'gc0AddTopLevelStmt(Cstmts stmts, CCode stmt)' \
    "$SRCROOT/aldor/src/genc.cpp" | grep -q 'ccoTag(stmt) == CCO_Stat'
grep -A25 'gc0AddTopLevelStmt(Cstmts stmts, CCode stmt)' \
    "$SRCROOT/aldor/src/genc.cpp" | grep -q 'ccoTag(ccoArgv(stmt)\[0\]) == CCO_Id'

# RC19.6: native .a archive policy is centralized.  Darwin final archive
# construction uses Apple libtool -static, while Apple ar remains the helper
# for archive member operations.  `uniar` remains exclusively for `.al`.
if grep -q 'ALDOR_AR_FLAGS="S"' "$SRCROOT/build-fns.sh"; then
    echo 'Darwin native archives still request the alignment-breaking S modifier' >&2
    exit 1
fi
grep -q 'ALDOR_DEFAULT_LIBTOOL=/usr/bin/libtool' "$SRCROOT/build-fns.sh"
grep -q 'export ALDOR_CC ALDOR_CXX ALDOR_AR ALDOR_RANLIB ALDOR_LIBTOOL' \
    "$SRCROOT/build-fns.sh"
grep -q 'ALDOR_LIBTOOL_FLAGS="-no_warning_for_no_symbols"' \
    "$SRCROOT/build-fns.sh"
grep -q 'install  toolbin doar' "$SRCROOT/aldor/tools/unix/build.sh"
grep -q 'libtool' "$SRCROOT/aldor/tools/unix/doar"
grep -q -- '-static .*ALDOR_LIBTOOL_FLAGS.* -o' "$SRCROOT/aldor/tools/unix/doar"
grep -q 'doar r "\$@"' "$SRCROOT/build-fns.sh"
grep -q 'ALDOR_RANLIB_FLAGS' "$SRCROOT/aldor/tools/unix/doranlib"
grep -q 'doranlib "\$@"' "$SRCROOT/build-fns.sh"

# Active shell build/test paths must not bypass the native archive wrappers.
if grep -RInE '^[[:space:]]*(ar|ranlib)[[:space:]]' \
    "$SRCROOT/lib" "$SRCROOT/aldor" \
    --include='*.sh' --include='cmds.compile' \
    | grep -v '/modernization/tests/' \
    | grep -v '/history/' \
    | grep -v '/misc/errors' \
    | grep -v '/aldor/tools/unix/doar:' \
    | grep -v '/aldor/tools/unix/doranlib:'; then
    echo 'active build/test path bypasses doar/doranlib' >&2
    exit 1
fi

# RC15: GMP discovery must be prefix-based and the ABI shim must receive the
# discovered include directory; generated-C links receive its library path.
grep -q 'brew --prefix gmp' "$SRCROOT/build-fns.sh"
grep -q 'ALDOR_GMP_PREFIX' "$SRCROOT/build-fns.sh"
grep -q 'requireGmp' "$SRCROOT/build.sh"
grep -q 'ALDOR_GMP_INCLUDE_DIR' "$SRCROOT/lib/aldor/src/gmp/build.sh"
grep -q 'ALDOR_GMP_INCLUDE_DIR' "$SRCROOT/aldor/contrib/gmp/build.sh"
grep -q 'Cargs=-L\$ALDOR_GMP_LIB_DIR' "$SRCROOT/build-fns.sh"

# RC18: generated-C ABI namespace must remain clean even transitively.
cat >"$TMP/foam-namespace.c" <<'EOF'
#include "foam_c.h"
extern FiWord unlink(FiWord);
extern FiWord fopen(FiWord, FiWord);
int main(void) { return 0; }
EOF
"${CC:-cc}" -std=c99 -fno-builtin -I"$SRCROOT/aldor/src" \
    -c "$TMP/foam-namespace.c" -o "$TMP/foam-namespace.o"

# All supported targets use C float for Aldor single precision.  The obsolete
# representation switch must not silently survive as an undefined #if.
if grep -RIn 'CPORT_SF_IS_DOUBLE' \
    "$SRCROOT/aldor/src" >/dev/null; then
    echo 'obsolete CPORT_SF_IS_DOUBLE branch remains' >&2
    exit 1
fi

# libc/POSIX names with a non-C ABI must use named Aldor adapters.  These are
# the shipped cases which previously relied on headerless Foreign C.
if grep -RInE 'import[[:space:]]*\{[[:space:]]*unlink:' \
    "$SRCROOT/lib/aldor" >/dev/null; then
    echo 'raw unlink Foreign C import remains' >&2
    exit 1
fi
grep -q 'salUnlink: Pointer -> MachineInteger' "$SRCROOT/lib/aldor/src/util/sal_file.as"
grep -q 'pow: (DFlo, DFlo) -> DFlo } from Foreign C "<math.h>"' \
    "$SRCROOT/lib/aldor/src/arith/sal_dfloat.as"
if grep -q 'import { EOF: MachineInteger} from Foreign C' \
    "$SRCROOT/lib/aldor/src/base/sal_base.as"; then
    echo 'unused raw EOF Foreign C import remains' >&2
    exit 1
fi
# Character literals 0 and 9 require MachineInteger literal syntax in this
# implementation scope.  Keep the local Z import that makes null/tab portable.
grep -A3 '^} == add {' "$SRCROOT/lib/aldor/src/base/sal_base.as" >/dev/null || true
grep -q '^[[:space:]]*import from Z;' "$SRCROOT/lib/aldor/src/base/sal_base.as"

# Secondary runtime source manifests must name real source files.  Check the
# explicit libfoam list and the stem list used by the GMP runtime build.
python3 - "$SRCROOT" <<'PY2'
import pathlib,re,sys
root=pathlib.Path(sys.argv[1])
text=(root/'aldor/lib/libfoam/build.sh').read_text()
m=re.search(r'local CppSrcFiles="([^"]+)"', text, re.S)
if m:
    for tok in m.group(1).split():
        p=root/'aldor/src'/tok
        if not p.exists():
            raise SystemExit(f'missing libfoam source: {tok}')
text=(root/'aldor/contrib/gmp/build.sh').read_text()
m=re.search(r'for f in \\\n(.*?)\n    do', text, re.S)
if m:
    for tok in re.findall(r'[A-Za-z_][A-Za-z0-9_]*', m.group(1)):
        p=root/'aldor/src'/(tok+'.cpp')
        if not p.exists():
            raise SystemExit(f'missing GMP runtime source: {tok}.cpp')
PY2

# Build scripts must preserve argument boundaries when source/build paths
# contain whitespace.  In particular, Bash arrays used for compiler options
# must be expanded with "${array[@]}", not as scalar $array, and path-valued
# variables passed to compile*IntoLib must be quoted.
python3 - "$SRCROOT" <<'PYSHELLARGS'
import pathlib
import re
import sys

root = pathlib.Path(sys.argv[1])
problems = []
array_decl = re.compile(
    r'(?m)^\s*(?:local\s+)?([A-Za-z_][A-Za-z0-9_]*)\s*=\s*\('
)

for path in root.rglob("*.sh"):
    try:
        text = path.read_text()
    except UnicodeDecodeError:
        continue

    for name in set(array_decl.findall(text)):
        scalar = re.compile(
            rf'(?<![A-Za-z0-9_])\${re.escape(name)}(?![A-Za-z0-9_])'
        )
        braced = re.compile(rf'\$\{{{re.escape(name)}\}}')
        for lineno, line in enumerate(text.splitlines(), 1):
            if scalar.search(line) or braced.search(line):
                problems.append(
                    f"{path.relative_to(root)}:{lineno}: "
                    f"array {name} expanded as a scalar"
                )

    for lineno, line in enumerate(text.splitlines(), 1):
        if not re.search(
            r'\bcompile(?:Aldor|C|Cpp|CAsCpp)IntoLib\b', line
        ):
            continue
        outside_quotes = re.sub(r'"(?:[^"\\]|\\.)*"', '', line)
        if re.search(
            r'\$[A-Za-z_][A-Za-z0-9_]*(?:Dir|ROOT|PATH)\b',
            outside_quotes
        ):
            problems.append(
                f"{path.relative_to(root)}:{lineno}: "
                "unquoted path variable in compile*IntoLib call"
            )

if problems:
    raise SystemExit(
        "shell argument-boundary audit failed:\n  " + "\n  ".join(problems)
    )
PYSHELLARGS

# RC12: float2 is a numerical-property test, not an exact rendering of the
# last IEEE-754 bits.  Keep the stable reference platform-independent.
grep -q 'Result within tolerance' "$SRCROOT/lib/axllib/test/float2.as"
grep -q 'Quadrature converged' "$SRCROOT/lib/axllib/testout/float2.out"
if grep -q '^Result = ' "$SRCROOT/lib/axllib/testout/float2.out"; then
    echo 'float2 reference still encodes exact machine floating-point digits' >&2
    exit 1
fi

# RC13: generated-C Clang warning probes are independent.  Emulate an older
# Clang that rejects -Wno-deprecated-non-prototype but accepts
# -Wno-unused-value, then source build-fns and inspect the generated-C args.
cat >"$TMP/fake-clang14" <<'SH'
#!/bin/sh
for arg in "$@"; do
    case "$arg" in
        -Wno-deprecated-non-prototype) exit 1 ;;
    esac
done
exit 0
SH
chmod +x "$TMP/fake-clang14"
mkdir -p "$TMP/fake-root"
ALDORROOT="$TMP/fake-root" \
ALDOR_TOOLCHAIN=clang \
ALDOR_CC="$TMP/fake-clang14" \
ALDOR_CXX="$TMP/fake-clang14" \
ALDOR_RANLIB_FLAGS="" \
ALDOR_GMP_INCLUDE_DIR="" ALDOR_GMP_LIB_DIR="" \
bash -c 'source "$1"; printf "%s\n" "$ALDORARGS"' \
    _ "$SRCROOT/build-fns.sh" >"$TMP/fake-clang14.args"
grep -q -- '-Cargs=-Wno-unused-value' "$TMP/fake-clang14.args"
if grep -q -- '-Cargs=-Wno-deprecated-non-prototype' "$TMP/fake-clang14.args"; then
    echo 'unsupported generated-C warning option leaked into ALDORARGS' >&2
    exit 1
fi

# RC14: GCC can return success for a C-only negative warning option in C++
# while still printing a command-line diagnostic.  Emulate GCC 16 on macOS:
# -Wno-int-conversion is clean in C but noisy in C++.  The exact-option probe
# must keep it out of GLOBAL_CXXFLAGS while retaining it for generated C.
cat >"$TMP/fake-gcc16" <<'SH'
#!/bin/sh
lang=c
read_stdin=no
for arg in "$@"; do
    [ "$arg" = c++ ] && lang=c++
    [ "$arg" = - ] && read_stdin=yes
done
input=
[ "$read_stdin" = no ] || input=$(cat)
for arg in "$@"; do
    if [ "$arg" = -Wno-int-conversion ] && [ "$lang" = c++ ]; then
        echo "cc1plus: warning: command-line option '-Wno-int-conversion' is valid for C/ObjC but not for C++" >&2
    fi
    case "$arg" in
        -Wno-unused-command-line-argument|-Wno-builtin-requires-header|-Wno-incompatible-library-redeclaration)
            # GCC normally stays silent for an unknown -Wno-* option until
            # some other diagnostic is emitted.  The new probe deliberately
            # supplies #warning so the unsupported option becomes visible.
            case "$input" in
                *ALDOR_WARNING_OPTION_PROBE*)
                    echo "cc1: note: unrecognized command-line option '$arg' may have been intended to silence earlier diagnostics" >&2
                    ;;
            esac
            ;;
    esac
done
case "$input" in
    *ALDOR_WARNING_OPTION_PROBE*)
        echo '<stdin>:1:2: warning: #warning ALDOR_WARNING_OPTION_PROBE [-Wcpp]' >&2
        ;;
esac
exit 0
SH
chmod +x "$TMP/fake-gcc16"
mkdir -p "$TMP/fake-root-gcc16" "$TMP/fake-macos-bin"
cat >"$TMP/fake-macos-bin/uname" <<'SH'
#!/bin/sh
echo Darwin
SH
cat >"$TMP/fake-macos-bin/sw_vers" <<'SH'
#!/bin/sh
[ "$1" = -productVersion ] && echo 26.6.2
SH
chmod +x "$TMP/fake-macos-bin/uname" "$TMP/fake-macos-bin/sw_vers"
PATH="$TMP/fake-macos-bin:$PATH" \
ALDORROOT="$TMP/fake-root-gcc16" \
ALDOR_TOOLCHAIN=gcc \
ALDOR_CC="$TMP/fake-gcc16" \
ALDOR_CXX="$TMP/fake-gcc16" \
ALDOR_RANLIB_FLAGS="" \
ALDOR_GMP_INCLUDE_DIR="" ALDOR_GMP_LIB_DIR="" \
bash -c 'source "$1"; printf "CXX=%s\nALDOR=%s\n" "$GLOBAL_CXXFLAGS" "$ALDORARGS"' \
    _ "$SRCROOT/build-fns.sh" >"$TMP/fake-gcc16.args"
if grep '^CXX=' "$TMP/fake-gcc16.args" | grep -q -- '-Wno-int-conversion'; then
    echo 'C-only GCC warning option leaked into GLOBAL_CXXFLAGS' >&2
    exit 1
fi
grep '^ALDOR=' "$TMP/fake-gcc16.args" | grep -q -- '-Cargs=-Wno-int-conversion'

# An explicitly selected SDK for Darwin GCC must reach implementation C, C++,
# and generated Aldor C consistently.  The environment also retains SDKROOT
# for direct host-compiler calls made by validation scripts.
fake_sdk="$TMP/MacOSX15.4.sdk"
mkdir -p "$fake_sdk" "$TMP/fake-root-gcc-sdk"
PATH="$TMP/fake-macos-bin:$PATH" \
ALDORROOT="$TMP/fake-root-gcc-sdk" \
ALDOR_TOOLCHAIN=gcc \
ALDOR_CC="$TMP/fake-gcc16" \
ALDOR_CXX="$TMP/fake-gcc16" \
ALDOR_MACOS_SDK="$fake_sdk" \
ALDOR_MACOS_SDK_SELECTION=explicit \
ALDOR_RANLIB_FLAGS="" \
ALDOR_GMP_INCLUDE_DIR="" ALDOR_GMP_LIB_DIR="" \
bash -c 'source "$1"; printf "C=%s\nCXX=%s\nALDOR=%s\nSDKROOT=%s\n" \
    "$GLOBAL_CFLAGS" "$GLOBAL_CXXFLAGS" "$ALDORARGS" "$SDKROOT"' \
    _ "$SRCROOT/build-fns.sh" >"$TMP/fake-gcc-sdk.args"
grep '^C=' "$TMP/fake-gcc-sdk.args" | grep -F -- "-isysroot=$fake_sdk" >/dev/null
grep '^CXX=' "$TMP/fake-gcc-sdk.args" | grep -F -- "-isysroot=$fake_sdk" >/dev/null
grep '^ALDOR=' "$TMP/fake-gcc-sdk.args" | grep -F -- \
    "-Cargs=-isysroot=$fake_sdk" >/dev/null
grep -F "SDKROOT=$fake_sdk" "$TMP/fake-gcc-sdk.args" >/dev/null

# RC29: the macOS-26 GNU-GCC header compatibility definition is host-C++
# only.  It must not perturb ordinary C, generated Aldor C, or GMP probing.
mkdir -p "$TMP/fake-root-gcc-compat"
PATH="$TMP/fake-macos-bin:$PATH" \
ALDORROOT="$TMP/fake-root-gcc-compat" \
ALDOR_TOOLCHAIN=gcc \
ALDOR_CC="$TMP/fake-gcc16" ALDOR_CXX="$TMP/fake-gcc16" \
ALDOR_MACOS_CXX_COMPAT_FLAG='-D_Static_assert=static_assert' \
ALDOR_MACOS_CXX_COMPAT_MODE=xnu-static-assert \
ALDOR_RANLIB_FLAGS="" \
ALDOR_GMP_INCLUDE_DIR="" ALDOR_GMP_LIB_DIR="" \
bash -c 'source "$1"; printf "C=%s\nCXX=%s\nALDOR=%s\n" \
    "$GLOBAL_CFLAGS" "$GLOBAL_CXXFLAGS" "$ALDORARGS"' \
    _ "$SRCROOT/build-fns.sh" >"$TMP/fake-gcc-compat.args"
if grep '^C=' "$TMP/fake-gcc-compat.args" | grep -q -- '-D_Static_assert=static_assert'; then
    echo 'macOS GCC C++ compatibility definition leaked into C flags' >&2
    exit 1
fi
grep '^CXX=' "$TMP/fake-gcc-compat.args" | \
    grep -F -- '-D_Static_assert=static_assert' >/dev/null
if grep '^ALDOR=' "$TMP/fake-gcc-compat.args" | grep -q -- '-D_Static_assert=static_assert'; then
    echo 'macOS GCC C++ compatibility definition leaked into generated C' >&2
    exit 1
fi

for w in unused-command-line-argument builtin-requires-header incompatible-library-redeclaration; do
    if grep -q -- "-Wno-$w" "$TMP/fake-gcc16.args"; then
        echo "unknown GCC negative warning option leaked into build flags: -Wno-$w" >&2
        exit 1
    fi
done

# Darwin low-level storage must preserve the BTree store's dense-address
# assumption.  Reserve one contiguous PROT_NONE arena, expose only its used
# prefix, and keep freed pieces reusable rather than deallocating holes from
# the reservation.  Also use the actual host VM page size on Apple Silicon.
MACVM="$SRCROOT/aldor/src/os_macosx_vm.c0"
grep -q 'mmap(0, want, PROT_NONE' "$MACVM"
grep -q 'mprotect(osArenaCommit, ncommit' "$MACVM"
grep -q 'sysconf(_SC_PAGESIZE)' "$MACVM"
grep -q 'if (ptrEQ(ff, NULL)) return;' "$MACVM"
if grep -q '#define OS_PAGE_SIZE 4096' "$MACVM"; then
    echo 'Darwin allocator still hard-codes a 4K VM page size' >&2
    exit 1
fi
python3 - "$MACVM" <<'PYMACVM'
from pathlib import Path
import sys
s = Path(sys.argv[1]).read_text()
body = s.split('\nvoid\nosFree(void *p)\n', 1)[1]
body = body.split('\n/*\n * Place a link', 1)[0]
if 'vm_deallocate' in body:
    raise SystemExit('Darwin osFree punches a hole in the reserved arena')
if 'ff->use = OS_STORE_FREE;' not in body:
    raise SystemExit('Darwin osFree does not make the piece reusable')
PYMACVM

# Matrix/comparison harnesses are release portability gates and must run under
# Apple's system Bash without assuming GNU coreutils.  Keep Bash-4 mapfile
# out, provide native macOS SHA-256 fallbacks, and retain a no-coreutils
# timeout path for executable Algebra tests.
for f in algebra-matrix.sh generated-c-compare.sh; do
    script="$SRCROOT/modernization/$f"
    if grep -Eq '(^|[[:space:]])mapfile([[:space:]]|$)' "$script"; then
        echo "$f still requires Bash-4 mapfile" >&2
        exit 1
    fi
    grep -q 'command -v shasum' "$script"
    grep -q 'openssl dgst -sha256' "$script"
done
grep -q '^run_with_timeout()' "$SRCROOT/modernization/algebra-matrix.sh"
grep -q 'if command -v timeout' "$SRCROOT/modernization/algebra-matrix.sh"
grep -q 'command -v gtimeout' "$SRCROOT/modernization/algebra-matrix.sh"
grep -q 'run_with_timeout "$TIMEOUT_SECS"' \
    "$SRCROOT/modernization/algebra-matrix.sh"


# Checked-in generated parser sources must not be rewritten by ordinary
# builds according to filesystem mtimes.  Regeneration is explicit so that
# a release build does not depend on the host yacc/bison version.
python3 - "$SRCROOT" <<'PYGENERATED'
from pathlib import Path
import re
import sys

root = Path(sys.argv[1])
build = root / "aldor/src/build.sh"
text = build.read_text()

if "ALDOR_REGENERATE_GRAMMAR" not in text:
    raise SystemExit(
        "aldor/src/build.sh lacks the explicit grammar-regeneration gate"
    )
if re.search(r'\baxl\.y\s+-nt\s+axl_y\.c\b', text):
    raise SystemExit(
        "aldor/src/build.sh still regenerates axl_y.c according to mtimes"
    )

hits = []
for path in root.rglob("*.sh"):
    try:
        body = path.read_text()
    except UnicodeDecodeError:
        continue
    for n, line in enumerate(body.splitlines(), 1):
        if re.search(r'\[[^]]+\s-(?:nt|ot)\s+[^]]+\]', line):
            hits.append(f"{path.relative_to(root)}:{n}:{line.strip()}")

if hits:
    raise SystemExit(
        "timestamp-driven shell regeneration/checks remain:\n  "
        + "\n  ".join(hits)
    )
PYGENERATED


# Release builds and tests must survive source/build roots containing spaces.
python3 - "$SRCROOT" <<'PYPATHSPACE'
from pathlib import Path
import sys

root = Path(sys.argv[1])

ccomp = (root / "aldor/src/ccomp.cpp").read_text()
if 'cmdbuf.printf("\'%s\'", fn.unparseStaticWith());' not in ccomp:
    raise SystemExit(
        "ccGoProgram does not quote the executable pathname"
    )

unix = (root / "aldor/subcmd/testaldor/tx_unix.c0").read_text()
required_unix = [
    "#define OSFmtScript\t\"sh '%s'\"",
    "#define OSFmtDiff\t\"diff -b '%s' '%s'\"",
    "return system(strPrintf(\"rm -rf '%s'\", fn));",
]
for needle in required_unix:
    if needle not in unix:
        raise SystemExit(
            "testaldor Unix pathname quoting regression: " + needle
        )

windows = (root / "aldor/subcmd/testaldor/tx_windows.c0").read_text()
required_windows = [
    'bname = strPrintf("sh \\"%s\\"", fname);',
    '#define OSFmtDiff\t"fc \\"%s\\" \\"%s\\""',
    "#define OSFmtDiff   \"diff -b '%s' '%s'\"",
]
for needle in required_windows:
    if needle not in windows:
        raise SystemExit(
            "testaldor Windows/Cygwin pathname quoting regression: " + needle
        )
PYPATHSPACE

# The installed doaldor helper must also preserve a path containing blanks as
# one -R argument.  This is exercised dynamically because a string-built
# command can look reasonable in source while still being split by the shell.
doaldor_root="$TMP/doaldor root"
doaldor_lib="$doaldor_root/lib with spaces"
doaldor_trace="$TMP/doaldor.args"
mkdir -p "$doaldor_root/bin" "$doaldor_lib"
cat >"$doaldor_root/bin/aldor" <<'SH'
#!/bin/sh
: > "$DOALDOR_TRACE"
for arg in "$@"; do
    printf '<%s>\n' "$arg" >> "$DOALDOR_TRACE"
done
exit 0
SH
for tool in uniar ar; do
    cat >"$doaldor_root/bin/$tool" <<'SH'
#!/bin/sh
exit 0
SH
done
chmod +x "$doaldor_root/bin/aldor" \
    "$doaldor_root/bin/uniar" "$doaldor_root/bin/ar"
DOALDOR_TRACE="$doaldor_trace" \
DOALDOR_EXPECTED_LIB="$doaldor_lib" \
ALDORROOT="$doaldor_root" MACHINE=unix \
sh "$SRCROOT/aldor/tools/unix/doaldor" \
    -l "$doaldor_lib" foo repl -Mno-warnings -D AddExport >/dev/null
if ! grep -Fqx "<-R$doaldor_lib>" "$doaldor_trace"; then
    echo "doaldor split a whitespace-containing -R pathname" >&2
    cat "$doaldor_trace" >&2
    exit 1
fi

# Path-valued shell variables in release tests must remain single argv values.
# The release is deliberately exercised from source/build roots containing
# blanks, so this is a portability requirement rather than a style rule.
python3 - "$SRCROOT" <<'PYPATHS'
from pathlib import Path
import re
import sys

root = Path(sys.argv[1])
paths = [
    root / "aldor/test",
    root / "lib/axllib/test",
    root / "lib/aldor/test",
    root / "lib/algebra/test",
]
path_vars = {
    "TMPDIR", "ALDORTMP", "SRC", "CURDIR", "ALDORROOT",
    "TestDir", "TestOut", "AXIOMXL", "SrcDir", "ALGEBRAROOT",
}
names = "|".join(sorted(path_vars, key=len, reverse=True))
var_re = re.compile(r"\$(?:\{(" + names + r")[^}]*\}|(" + names + r"))")

bad = []
for directory in paths:
    for path in sorted(directory.glob("*.sh")):
        for lineno, line in enumerate(
                path.read_text(errors="replace").splitlines(), 1):
            stripped = line.lstrip()
            if not stripped or stripped.startswith("#"):
                continue
            # Scalar assignment RHS is not subject to ordinary shell word
            # splitting, but indexed-array elements are.  For example,
            # opts=(-y${ALDORROOT}/lib) splits a path containing blanks.
            if re.match(r"^[A-Za-z_][A-Za-z0-9_]*=", stripped) and not \
               re.match(r"^[A-Za-z_][A-Za-z0-9_]*=\s*\(", stripped):
                continue
            for match in var_re.finditer(line):
                before = line[:match.start()]
                if before.count('"') % 2 or before.count("'") % 2:
                    continue
                bad.append(
                    f"{path.relative_to(root)}:{lineno}: {line}"
                )

if bad:
    print("unquoted path-valued shell expansions:", file=sys.stderr)
    print("\n".join(bad), file=sys.stderr)
    raise SystemExit(1)
PYPATHS

for f in \
    "$SRCROOT"/aldor/test/*.sh \
    "$SRCROOT"/lib/axllib/test/*.sh \
    "$SRCROOT"/lib/aldor/test/*.sh \
    "$SRCROOT"/lib/algebra/test/*.sh
do
    bash -n "$f"
done

# Embedded-library test discovery must preserve each pathname as one argument.
for f in lib/aldor/test/build.sh lib/algebra/test/build.sh; do
    if grep -Eq \
        'xargs[[:space:]]+grep|for[[:space:]]+f[[:space:]]+in[[:space:]]+`find' \
        "$SRCROOT/$f"; then
        echo "$f still word-splits discovered source pathnames" >&2
        exit 1
    fi
done

echo "portability regression: PASS"

