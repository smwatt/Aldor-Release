/*****************************************************************************
 *
 * cconfig.h: Capabilities and defects of supported host C implementations.
 *
 ****************************************************************************/

#ifndef _CCONFIG_H_
#define _CCONFIG_H_

#include "platform.h"

/* Compiler capabilities shared by GCC and Clang. */
#if defined(CC_GCC) || defined(CC_CLANG)
# define CC_HAS_GNU_ATTRIBUTES 1
#endif

/* Native Windows C libraries expose several POSIX facilities under alternate
 * names.  Cygwin presents the POSIX interface directly. */
#if defined(ENV_MINGW) || defined(ENV_MSVC)
# define CC_NATIVE_WINDOWS_CRT 1
# define CC_MISSING_STRCASECMP 1
# define CC_ISATTY_IS__ISATTY 1
# define CC_FILENO_IS__FILENO 1
# define CC_UNISTD_IS_IO_H 1
# define CC_PUTENV_IS__PUTENV 1
# define CC_GETCWD_IS__GETCWD 1
# define CC_CHDIR_IS__CHDIR 1
# define CC_UNLINK_IS__UNLINK 1
# define CC_GETPID_IS__GETPID 1
#endif

/* POSIX facilities used by the compiler/utility OS layer.  putenv has the
 * same canonical declaration on all supported POSIX hosts, so cport.h may
 * supply it when feature-test settings hide the system declaration.
 *
 * sbrk is different: macOS exposes the historical sbrk(int) declaration,
 * while Linux and Cygwin use the intptr_t form (which may be hidden by
 * feature-test settings).  Do not redeclare the macOS interface. */
#if defined(OS_UNIX) || defined(ENV_CYGWIN)
# define CC_HAS_POSIX_PUTENV 1
# define CC_HAS_POSIX_MKSTEMP 1
# define CC_HAS_STRINGS_H 1
#endif
#if defined(OS_LINUX) || defined(ENV_CYGWIN)
# define CC_SUPPLY_DECL_SBRK 1
#endif

/* Floating-point trap-enable facilities used by foam_cfp.cpp. */
#if defined(OS_LINUX) && !defined(HW_RISCV64)
# define CC_FENV_GLIBC_TRAPS 1
#elif defined(OS_MAC_OSX) && defined(HW_X86_64)
# define CC_FENV_DARWIN_X86 1
#elif defined(OS_MAC_OSX) && defined(HW_ARM64)
# define CC_FENV_DARWIN_ARM 1
#endif

/* The Windows storage implementation historically relies on byte-aligned
 * pointer values in a few low-level tests. */
#if defined(OS_WINDOWS)
# define CC_ONE_BYTE_ALIGNED_POINTERS 1
#endif

#endif /* !_CCONFIG_H_ */
