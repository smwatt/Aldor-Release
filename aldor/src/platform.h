/*****************************************************************************
 *
 * platform.h: Identify the supported Aldor compilation environment.
 *
 ****************************************************************************/

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

/*
 * Supported hosts:
 *   Linux:   GCC or Clang; x86-64, AArch64, RISC-V64
 *   macOS:   GCC or Clang; x86-64, AArch64
 *   Windows: Cygwin GCC/Clang; MinGW GCC/Clang; MSVC
 *            (Windows targets are presently x86-64 only.)
 *
 * This is the only hand-written Aldor header which interprets compiler
 * predefined environment macros.  Later layers use OS_*, ENV_*, CC_* and
 * HW_* properties defined here.
 */

/* ---------- operating-system / runtime environment ---------- */
#if defined(__CYGWIN__)
# define OS_WINDOWS 1
# define ENV_CYGWIN 1
# define CONFIG "Win64 Cygwin"
# define CONFIGSYS "win64cygwin"
#elif defined(__MINGW32__) || defined(__MINGW64__)
# define OS_WINDOWS 1
# define ENV_MINGW 1
# define CONFIG "Win64 MinGW"
# define CONFIGSYS "win64mingw"
#elif defined(_WIN32)
# define OS_WINDOWS 1
# define ENV_MSVC 1
# define CONFIG "Win64 MSVC ABI"
# define CONFIGSYS "win64msvc"
#elif defined(__APPLE__) && defined(__MACH__)
# define OS_UNIX 1
# define OS_MAC_OSX 1
# define ENV_POSIX 1
# define CONFIG "macOS"
# define CONFIGSYS "macOSX"
#elif defined(__linux__)
# define OS_UNIX 1
# define OS_LINUX 1
# define ENV_POSIX 1
# define CONFIG "LINUX"
# define CONFIGSYS "linux"
#else
# error "Unsupported Aldor operating-system environment"
#endif

/* ---------- compiler family ---------- */
#if defined(__clang__)
# define CC_CLANG 1
# define CC_VERSION_MAJOR __clang_major__
# define CC_VERSION_MINOR __clang_minor__
# define CC_VERSION_PATCH __clang_patchlevel__
# if defined(__apple_build_version__)
#  define CC_APPLE_CLANG 1
#  define CC_APPLE_BUILD __apple_build_version__
# endif
#elif defined(__GNUC__)
# define CC_GCC 1
# define CC_VERSION_MAJOR __GNUC__
# define CC_VERSION_MINOR __GNUC_MINOR__
# define CC_VERSION_PATCH __GNUC_PATCHLEVEL__
#elif defined(_MSC_VER)
# define CC_MSVC 1
# define CC_VERSION_MAJOR (_MSC_VER / 100)
# define CC_VERSION_MINOR (_MSC_VER % 100)
# define CC_VERSION_PATCH 0
#else
# error "Unsupported Aldor C/C++ compiler"
#endif

#define CC_VERSION_ENCODE(MAJ, MIN, PAT) \
        ((MAJ) * 1000000 + (MIN) * 1000 + (PAT))
#define CC_VERSION_NUMBER \
        CC_VERSION_ENCODE(CC_VERSION_MAJOR, CC_VERSION_MINOR, CC_VERSION_PATCH)
#define CC_VERSION_AT_LEAST(MAJ, MIN, PAT) \
        (CC_VERSION_NUMBER >= CC_VERSION_ENCODE((MAJ), (MIN), (PAT)))

/* ---------- architecture ---------- */
#if defined(OS_WINDOWS)
# if defined(__x86_64__) || defined(_M_X64)
#  define HW_X86_64 1
# else
#  error "Unsupported Aldor Windows architecture (x86-64 required)"
# endif
#elif defined(OS_MAC_OSX)
# if defined(__x86_64__)
#  define HW_X86_64 1
# elif defined(__aarch64__) || defined(__arm64__)
#  define HW_ARM64 1
# else
#  error "Unsupported Aldor macOS architecture"
# endif
#elif defined(OS_LINUX)
# if defined(__x86_64__)
#  define HW_X86_64 1
# elif defined(__aarch64__)
#  define HW_ARM64 1
# elif defined(__riscv) && defined(__SIZEOF_POINTER__) && (__SIZEOF_POINTER__ == 8)
#  define HW_RISCV64 1
# else
#  error "Unsupported Aldor Linux architecture"
# endif
#endif

/* All currently supported targets are little-endian. */
#define HW_LITTLE_ENDIAN 1

#if defined(BOMB)
# define OS_Has_License BOMB
#endif

#endif /* !_PLATFORM_H_ */
