/*****************************************************************************
 *
 * cport.h: Present a single sane C99/C++20 environment to Aldor sources.
 *
 ****************************************************************************/

#ifndef _CPORT_H_
#define _CPORT_H_

#include "platform.h"
#include "cconfig.h"

/* Baseline C library used throughout the compiler/runtime. */
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifndef __cplusplus
# include <stdbool.h>
#endif

#if defined(CC_HAS_STRINGS_H)
# include <strings.h>
#endif

#if defined(CC_UNISTD_IS_IO_H)
# include <io.h>
# include <direct.h>
# include <process.h>
#else
# include <unistd.h>
#endif

/* C ABI for headers shared by generated C and the C++ runtime. */
#ifdef __cplusplus
# define ALDOR_C_BEGIN extern "C" {
# define ALDOR_C_END   }
# define ALDOR_C       extern "C"
#else
# define ALDOR_C_BEGIN
# define ALDOR_C_END
# define ALDOR_C
#endif

/* Repair declarations/names hidden or renamed by otherwise conforming host
 * libraries.  Ordinary Aldor source uses only the standard/POSIX spellings. */
#if defined(CC_HAS_POSIX_PUTENV)
ALDOR_C int putenv(char *);
#endif
#if defined(CC_HAS_POSIX_MKSTEMP)
ALDOR_C int mkstemp(char *);
#endif
#if defined(CC_SUPPLY_DECL_SBRK)
ALDOR_C void *sbrk(intptr_t);
#endif
#if defined(CC_MISSING_STRCASECMP)
# define strcasecmp   _stricmp
# define strncasecmp  _strnicmp
#endif
#if defined(CC_ISATTY_IS__ISATTY)
# define isatty _isatty
#endif
#if defined(CC_FILENO_IS__FILENO)
# define fileno _fileno
#endif
#if defined(CC_PUTENV_IS__PUTENV)
# define putenv _putenv
#endif
#if defined(CC_GETCWD_IS__GETCWD)
# define getcwd _getcwd
#endif
#if defined(CC_CHDIR_IS__CHDIR)
# define chdir _chdir
#endif
#if defined(CC_UNLINK_IS__UNLINK)
# define unlink _unlink
#endif
#if defined(CC_GETPID_IS__GETPID)
# define getpid _getpid
#endif

/* Aldor's assertion facility intentionally has stable behavior independent of
 * vendor <assert.h> implementations. */
ALDOR_C_BEGIN
void _do_assert(const char *str, const char *file, int line);
extern int _dont_assert;
ALDOR_C_END
#if defined(QASSERT)
# define _a_msg(x) ((char *) 0)
#else
# define _a_msg(x) (x)
#endif
#ifdef assert
# undef assert
#endif
#if defined(NDEBUG)
# define assert(c) ((void) 0)
#else
# define assert(c) \
        ((_dont_assert || (c)) ? (void) 0 : \
         _do_assert(_a_msg(#c), __FILE__, __LINE__))
#endif

/* Normalize optional signal names used by the historical OS layer. */
#define SIGFAKE (-2)
#ifndef SIGABRT
# ifdef SIGIOT
#  define SIGABRT SIGIOT
# else
#  define SIGABRT SIGFAKE
# endif
#endif
#ifndef SIGALRM
# define SIGALRM SIGFAKE
#endif
#ifndef SIGBREAK
# define SIGBREAK SIGFAKE
#endif
#ifndef SIGBUS
# define SIGBUS SIGFAKE
#endif
#ifndef SIGEMT
# define SIGEMT SIGFAKE
#endif
#ifndef SIGFPE
# define SIGFPE SIGFAKE
#endif
#ifndef SIGHUP
# define SIGHUP SIGFAKE
#endif
#ifndef SIGILL
# define SIGILL SIGFAKE
#endif
#ifndef SIGINT
# define SIGINT SIGFAKE
#endif
#ifndef SIGKILL
# define SIGKILL SIGFAKE
#endif
#ifndef SIGPIPE
# define SIGPIPE SIGFAKE
#endif
#ifndef SIGQUIT
# define SIGQUIT SIGFAKE
#endif
#ifndef SIGSEGV
# define SIGSEGV SIGFAKE
#endif
#ifndef SIGSYS
# define SIGSYS SIGFAKE
#endif
#ifndef SIGTERM
# define SIGTERM SIGFAKE
#endif
#ifndef SIGTRAP
# define SIGTRAP SIGFAKE
#endif
#ifndef SIGXCPU
# define SIGXCPU SIGFAKE
#endif
#ifndef SIGXFSZ
# define SIGXFSZ SIGFAKE
#endif
#ifndef SIGDANGER
# define SIGDANGER SIGFAKE
#endif


/****************************************************************************
 *
 * C extensions and insulations.
 *
 * Here we describe the macros which insulate from things which require
 * different syntax in different C compilers or which require extra
 * care for portability.
 *
 *
 * 1. Constant declarations:
 *      ANSI C permits names to be declared as constants.  Sometimes
 *      it is not just permitted, but *necessary*.  I.e. when redefining
 *      a system function (printf) the argument declarations must match
 *      those in the system header file (stdio.h).  Since "const"
 *      In older C compilers in which "const" is not legal, we provide a
 *      macro "const" which expands to nothing.
 *
 * 2. Types:
 *
 *      Bool
 *      Hash
 *      Length
 *      Offset
 *      Millisec
 *              Integers for specific uses.
 *
 *      AInt
 *              Signed integer type with same size as "Pointer".
 *
 *      UByte
 *      UShort
 *      ULong
 *      ULong64
 *      UAInt
 *              Unsigned integers of specific sizes.
 *
 *      Pointer
 *      ConstPointer
 *              These are used as generic pointers to data.
 *              Arithmetic and comparison are not defined on "pointer"s
 *              (unless they are produced by ptrNormalize).
 *              ConstPointer is a pointer to constant data.
 *
 *      MutString
 *              Null-terminated string.
 *
 *      IOMode
 *              Strings for specific uses.
 *
 *      SFloat
 *      DFloat
 *              Floating-point numbers of specific sizes.
 *
 *      LongDouble
 *              long double, if supported, otherwise double.
 *
 *      MostAlignedType
 *              This is the type with the strongest alignment restrictions.
 *      UNotAsLong
 *              This is the largest int smaller than long (used in bigint)
 *      U16
 *              Unsigned 16 bits.
 *      U16sPerUNotAsLong
 *              Number of U16 in each UNotAsLong
 *
 *
 * 3. Bit sizes of types:
 *
 *      bitsizeof(t) returns the number of bits in type t.  E.g.
 *              struct {
 *                      int     x : K;
 *                      int     n : bitsizeof(int) - K;
 *              }
 *
 * 4. Using variable number of function arguments:
 *      We follow the ANSI <stdarg.h> conventions.
 *      If we have to, we implement it ourselves.
 *
 * 5. Token formation:
 *      The "Abut" macro is used to concatenate tokens.
 *
 *      Abut(a,b)
 *
 *      In ANSI C, this expands to a##b and in other C environments
 *      the old comment trick is used.
 *      Use Abut(a,b) rather than Abut(a, b) since spaces are significant
 *      in K&R C.
 *
 *      The "Enstring" macro is used to string-ize a token.
 *
 *      Enstring(a)
 *
 *      In ANSI C, this expands to #a and in other C environments
 *      the fact that cpp substitutes into strings is used.
 *
 *
 * 6. Trailing arrays:
 *      The "NARY" macro is used for trailing variable sized arrays.
 *
 *      The "fullsizeof" macro is used for the size of a structure with
 *      a given number of trailing components.  The arguments of must be
 *      suitable for "sizeof".
 *
 *              struct mything {
 *                      int             code;
 *                      struct jazz     jazz;
 *                      double          numbers[NARY];
 *              } *p;
 *
 *              p = (struct mything *) malloc(fullsizeof(*p, argc, double));
 *              int printf(char const *, fmt) { ...
 *
 * 7. Structure field offset manipulations.
 *      These are useful when one wants to iterate over various fields
 *      in a fixed array of structs.
 *      E.g.
 *              ...
 *                      phStats(" Time %6ld ms", fieldOffset(phInfo, time ));
 *                      phStats(" Store%6ld B ", fieldOffset(phInfo, store));
 *              }
 *
 *              phStats(char *fmt,  int offset)
 *              {
 *                      long    tot;
 *                      for (i = 0; i < PH_MAX; i++)
 *                              tot += * (long *) fieldAddr(phInfo+i, offset);
 *              ...
 *              }
 *
 * 8. Type alignment.
 *      "alignof" gives alignment of a type in the same units as "sizeof".
 *      E.g.    alignof(double) is sometimes 8 and sometimes 4.
 *
 *      isPtrMinAligned(p)  determines whether p is aligned enough
 *                          to point possibly to a pointer.
 *      isDataMaxAligned(p) determines whether p is aligned enough
 *                          to point to anything.
 *
 * 9. Integer compaction
 *      "Pack" and "BPack" allow code to compactly store values in structures
 *      while keeping the logical type explicit.
 *
 *      Pack(enum ee, short) expands to "short".
 *      BPack(xyzzy)         is equivalent to Pack(xyzzy, UByte).
 *
 * 10. Function scope
 *      The "local" declaration is used to declare a function local to a file.
 *      This allows more accurate profiling information, or to check for name
 *      collision if files are to be joined.
 *
 *      The code
 *              local char * f();
 *
 *      becomes
 *              char * f();             -- ifdef NLOCAL
 *              static char * f();      -- otherwise
 *
 *      (Of course one still uses "static" declarations for data,
 *       where appropriate.)
 *
 * 11. Enumerations
 *      Historical Aldor sources used an Enum(foo) portability macro for
 *      pre-ANSI compilers which could not forward-reference enumerations.
 *      Modern C99/C++20 builds use `enum foo` directly.
 *
 * 12. Empty files
 *      Not all compilers can compile files without definitions
 *      (e.g. Waterloo C on CMS).
 *      Include the following line at the end of such C files:
 *
 *              ThatsAll
 *
 * 13. Special values:
 *              true
 *              false
 *              long0           -- long integer 0
 *              int0            -- integer 0
 *              char0           -- character 0
 *              string0         -- character pointer 0
 *              TAB_STOP
 *
 * 14. Macro tools
 *      Nothing
 *         Different compilers complain about different sorts of empty
 *         expressions so the macro Nothing is provided to use as a
 *         low-grief, side-effect free expression.
 *         E.g.
 *              #ifdef NDEBUG
 *              # define DO_DEBUG(x)       Nothing
 *              #endif
 *
 *      STATEMENT(s)
 *         This macro has the same effect as "s" but can be used safely
 *         in any statement context.
 *
 *         E.g.  Given
 *
 *              #define DEBUG_GOOD(s)  STATEMENT(if (_debug_) {s;})
 *              #define DEBUG_BAD(s)   if (_debug_) {s;}
 *
 *              if (foo) DEBUG_GOOD(bar); else baz;
 *              if (foo) DEBUG_BAD(bar);  else baz;
 *
 *         then the "else baz" would pair with the "if (foo)" for DEBUG_GOOD
 *         but with the "if (_debug_)" for DEBUG_BAD.
 *
 *  15. NotReached
 *      This macro has two uses:
 *      (1) to trap bugs where supposedly  impossible situations arise, and
 *      (2) to gag compilers which incorrectly deduce that a value is missing.
 *
 *      E.g.  Some compilers complain about r not having a value in f(r) below:
 *              int a, r;
 *              switch (a % 3) {
 *              case 0:  case 1:  case 2: r = 0; break;
 *              }
 *              f(r)
 *
 *         So use
 *              int a, r;
 *              switch (a % 3) {
 *              case 0:  case 1:  case 2: r = 0; break;
 *              default: NotReached(r = 0);
 *              }
 *              f(r)
 *
 *  16. Pointer conversion
 *      C does not allow subtraction of pointers into different objects, nor
 *      are the results of conversion to integers standard.
 *      These macros allow low-level manipulation of pointers in a portable way:
 *
 *      Pointer ptrCanon  (Pointer);
 *              Produce a pointer in canonical form.
 *              The results can be compared (e.g. using ==, <, >).
 *
 *      Pointer ptrOff    (const char *p, long offset);
 *              Produce a canonical pointer offset a given distance from p.
 *
 *      Bool    ptrEqual  (Pointer, Pointer);
 *              Compare two possible non-canonical pointers.
 *
 *      long    ptrDiff   (const char *a, const char *b);
 *              Compute the difference between possibly non-canonical pointers.
 *
 *      long    ptrToLong (Pointer);
 *              Produce an integer value equal to an offset from 0 in a flat
 *              address space.
 *
 *      Pointer ptrFrLong (long);
 *              Produce a pointer for an offset from 0 in a flat address space.
 *              This may involve splitting the integer into a segment/offset
 *              representation.
 *
 *      Bool    ptrGT(SomePointerType a, SomePointerType b)
 *      Bool    ptrGE(SomePointerType a, SomePointerType b)
 *      Bool    ptrLT(SomePointerType a, SomePointerType b)
 *      Bool    ptrLE(SomePointerType a, SomePointerType b)
 *      Bool    ptrEQ(SomePointerType a, SomePointerType b)
 *      Bool    ptrNE(SomePointerType a, SomePointerType b)
 *              These macros compare pointers according to their
 *              canonical values.
 *
 *  17. Integer byte-ordering
 *      BYTEn and UNBYTEn provide a portable way to access the bytes of
 *      integers of different sizes.
 *
 *  18. Character set conversion
 *      charToAscii and charFrAscii provide a portable translation to and from
 *      the Ascii character set for use in Aldor Library files.
 *
 *  19. Floating-point characteristics
 *
 *      {SF,DF}_HasNANs      -- Does it have NANs + INFs?
 *      {SF,DF}_HasNorm1     -- Does it normally have an implicit 1 bit?
 *      {SF,DF}_LgLgBase     -- Log base 2 of Log base 2 of fraction radix.
 *      {SF,DF}_Excess       -- Exponent excess.
 *      {SF,DF}_FracOff      -- Big-endian bit offset of fraction.
 *      SF_no_denorms        -- denormed small floats are bad?
 *
 *      {SF,DF}_UByte(px,i)  -- The i-th byte in big-endian order.
 *      {SF,DF}_UShort(px,i) -- The i-th short byte in big-endian order.
 *
 *  20. Special declaration modifiers
 *
 *      SignalModifier  modifies pointers to signal handler functions.
 *
 *  21. For side-effect only -- ignore result
 *
 *      IgnoreResult(x)
 *
 ****************************************************************************/

/*****************************************************************************
 *
 * :: 2. Types
 *
 ****************************************************************************/

#ifndef CPortBasicTypedefs
#define CPortBasicTypedefs
  typedef int           CBool;
  typedef double        MostAlignedType;
#endif /* CPortBasicTypedefs */


typedef unsigned char   UByte;
typedef unsigned short  UShort;
typedef unsigned long   ULong;
typedef uint64_t        ULong64;

typedef intptr_t       AInt;
typedef uintptr_t       UAInt;

typedef unsigned short  UNotAsLong;
#define U16sPerUNotAsLong 1

typedef unsigned short  U16;
typedef UAInt           Hash;
typedef size_t          Length;
typedef ULong           Offset;
typedef ULong           Millisec;

typedef const void      *ConstPointer;

typedef char            *MutString;
typedef const char      *String;

typedef const char      *IOMode;

typedef float           SFloat;
typedef double          DFloat;

typedef long double     LongDouble;


#define boolToString(b) ((b) ? "true" : "false")

/*****************************************************************************
 *
 * :: 3. Bit sizes
 *
 ****************************************************************************/

#define bitsizeof(X)    (CHAR_BIT * sizeof(X))

/*****************************************************************************
 *
 * :: 4. Using variable number of function arguments
 *
 ****************************************************************************/

/*
 * Handled by <stdarg.h>
 */

/*****************************************************************************
 *
 * :: 5. Token Formation
 *
 ****************************************************************************/

# define Abut(a,b)      a##b
# define Enstring(a)    #a

/*****************************************************************************
 *
 * :: 6. Trailing arrays
 *
 ****************************************************************************/

# define NARY                   10 /* Enough to quiet bounds checking CC's */
# define fullsizeof(hty,n,aty)  (sizeof(hty) + (n) * sizeof(aty) - NARY * sizeof(aty))

/*****************************************************************************
 *
 * :: 7. Structure field offset manipulations
 *
 ****************************************************************************/

# define fieldAddr(p, offset)   ( (char *)(p) + (offset) )
# define fieldOffset(p, field)  ( (char *)(&((p)->field)) - (char *)(p) )

/*****************************************************************************
 *
 * :: 8. Type alignment
 *
 ****************************************************************************/

# ifdef __cplusplus
#  define cportAlignof(t)       alignof(t)
# else
/* C99 has no standard alignment operator.  offsetof on a char-prefixed
 * struct gives the implementation's required member alignment. */
#  define cportAlignof(t)       offsetof(struct { char c; t x; }, x)
# endif
# define isPtrMinAligned(p)     (((uintptr_t)(p)) % cportAlignof(long *) == 0)
# define isDataMaxAligned(p)    (((uintptr_t)(p)) % cportAlignof(MostAlignedType) == 0)

/*****************************************************************************
 *
 * :: 9. Integer compaction
 *
 ****************************************************************************/

/* Define UnBPack to allow unpacked integers for debugging purposes. */

# define BPack(unpacked)        Pack(unpacked, UByte)
#ifdef UnBPack
# define Pack(unpacked, packed) unpacked
#else
# define Pack(unpacked, packed) packed
#endif

/*****************************************************************************
 *
 * :: 10. Function scope
 *
 ****************************************************************************/

#ifdef NLOCAL
# define local /* extern */
#else
# define local static
#endif

/*****************************************************************************
 *
 * :: 12. Empty files
 *
 ****************************************************************************/

# define ThatsAll(n)

/*****************************************************************************
 *
 * :: 13. Special values
 *
 ****************************************************************************/

# define long0          ((long) 0)
# define int0           ((int)  0)
# define char0          ((char) 0)
# define string0        ((char *) 0)

# define TABSTOP        8   /* tabs expand to every TABSTOP chars */

/*****************************************************************************
 *
 * :: 14. Macro tools
 *
 ****************************************************************************/

# define Nothing
# define Statement(stat){ stat; }       /* do { stat; } while(0) */

/*****************************************************************************
 *
 * :: 15. Not reached
 *
 ****************************************************************************/

# define NotReached(stat)       \
  {(void)printf("Not supposed to reach line %d in file: %s\n",__LINE__, __FILE__); \
   stat;}

/*****************************************************************************
 *
 * :: 16. Pointer conversion
 *
 ****************************************************************************/

# define        ptrCanon(p)     ((void *)(p))
# define        ptrOff(p,n)     ((void *)((char *)(p) + (n)))
# define        ptrEqual(p,q)   ((void *)(p) == (void *)(q))
# define        ptrDiff(p1,p2)  ((long)((char *)(p1) - (char *)(p2)))
# define        ptrToLong(p)    ((long)   (p))
# define        ptrFrLong(l)    ((void *)(l))

#define ptrEQ(a,b)      ( ptrEqual((void *)(a), (void *)(b)))
#define ptrNE(a,b)      (!ptrEqual((void *)(a), (void *)(b)))
/*-----------------------------------------------------------------------------
 * The following definitions may require work to be acceptable across
 * platforms.  However, any replacement MUST use effectively UNSIGNED
 * comparisons if there is any possibility that they are to be used on
 * a platform where Pointers are as long as Long's.
 *
 * Most C compilers ought to be able to Do The Right Thing with the
 * definitions below, as the corresponding C++ compiler is obliged to
 * support std::less<Pointer>() & co,  which is obliged to be a consistent
 * total order on the Pointer type regardless of where the pointers
 *  are found.
 *-----------------------------------------------------------------------------
 */

#define ptrGT(a,b)      (ptrCanon(a) >  ptrCanon(b))
#define ptrGE(a,b)      (ptrCanon(a) >= ptrCanon(b))
#define ptrLT(a,b)      (ptrCanon(a) <  ptrCanon(b))
#define ptrLE(a,b)      (ptrCanon(a) <= ptrCanon(b))

/*****************************************************************************
 *
 * :: 17. Integer byte-ordering
 *
 * The following macros are used to ensure portable data formats for Aldor
 * library files across different platforms.
 *
 * The size macros involved represent numbers of bits/bytes written
 * to library files, and not sizes of data structures in memory.
 * As a result, they must not use 'sizeof' to create their value.
 *
 ****************************************************************************/

# define BYTE_BYTES             1       /* Not sizeof(...)!  See above. */
# define HINT_BYTES             2       /* Not sizeof(...)!  See above. */
# define SINT_BYTES             4       /* Not sizeof(...)!  See above. */

# define BYTE_BITS              8       /* Not sizeof(...)!  See above. */
# define BYTE_MASK              ((1<<BYTE_BITS)-1)

# define MAX_BYTE               ((1<<(1*BYTE_BITS))-1)
# define MAX_HINT               UNBYTE2(MAX_BYTE,MAX_BYTE)
# define MAX_SINT               UNBYTE4(MAX_BYTE,MAX_BYTE,MAX_BYTE,MAX_BYTE)

# define UBYTE0(b)              BYTE0(b)

# define HBYTE0(b)              BYTE0(b)
# define HBYTE1(b)              BYTE1(b)

# define BYTE0(b)               (((ULong) b)&BYTE_MASK)
# define BYTE1(b)               BYTE0((b)>>(1*BYTE_BITS))
# define BYTE2(b)               BYTE0((b)>>(2*BYTE_BITS))
# define BYTE3(b)               BYTE0((b)>>(3*BYTE_BITS))

# define UNBYTE1(b0)            BYTE0(b0)
# define UNBYTE2(b0,b1)         (BYTE0(b0) | (BYTE0(b1)<<BYTE_BITS))
# define UNBYTE4(b0,b1,b2,b3)   (UNBYTE2(b0,b1)|(UNBYTE2(b2,b3)<<(2*BYTE_BITS)))

/*****************************************************************************
 *
 * :: 18. Character set conversion
 *
 * The following macros are used to ensure portable data formats for Aldor
 * library files across different platforms.
 *
 ****************************************************************************/

# define charToAscii(e)         (e)
# define charFrAscii(a)         (a)

# define strToAscii(e,a,cc)     (e)
# define strFrAscii(a,e,cc)     (a)

# define strnToAscii(e,cc)      (e)
# define strnFrAscii(a,cc)      (a)

/*****************************************************************************
 *
 * :: 19. Floating-point characteristics
 *
 ****************************************************************************/



/* IEEE-754 formats used by every supported target. */
#define SF_HasNANs     1
#define SF_HasNorm1    1
#define SF_LgLgBase    0
#define SF_Excess      0x7f
#define SF_FracOff     9

#define DF_HasNANs     1
#define DF_HasNorm1    1
#define DF_LgLgBase    0
#define DF_Excess      0x3ff
#define DF_FracOff     12


#if defined(HW_LITTLE_ENDIAN)
# define TixPart(W,P,i) (sizeof(W)/sizeof(P) - (i) - 1)
#else
# define TixPart(W,P,i) (i)
#endif

#define SF_UByte(px,i)  (((UByte  *) (px))[TixPart(float, UByte, i)])
#define DF_UByte(px,i)  (((UByte  *) (px))[TixPart(double,UByte, i)])

#define SF_UShort(px,i) (((UShort *) (px))[TixPart(float, UShort,i)])
#define DF_UShort(px,i) (((UShort *) (px))[TixPart(double,UShort,i)])
/*****************************************************************************
 *
 * :: 20. Special declaration modifiers
 *
 * The following modifiers control calling or linkage conventions
 * for particular platforms.
 *
 ****************************************************************************/

# define SignalModifier

/*****************************************************************************
 *
 * :: 21. For side-effect only -- ignore result
 *
 * Suppress warnings on functions with __attribute__(__warn_unused_result__)).
 *
 ****************************************************************************/

#define IgnoreResult(x) ((void)!(x))

#endif /* !_CPORT_H_ */
