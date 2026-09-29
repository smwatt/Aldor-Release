/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 7 "axl_y.yt"

# include "axlphase.h"

# define yyerror(s)     yyerrorfn(s)

#ifdef NDEBUG
#  undef  YYDEBUG
#else
#  define YYDEBUG 1
#endif
int yydebug;

# define TPOS(t)((t).position())
# define TEND(t)((t).end().offset(-1))
# define APOS(a) abPos(a)
# define TEST(a) abNewTest(abPos(a),(a))
# define abZip   abNewNothing(SrcPos::none)

#line 89 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    _YY_TK_Id = 1,
    _YY_TK_Blank = 2,
    _YY_TK_Int = 3,
    _YY_TK_Float = 4,
    _YY_TK_String = 5,
    _YY_TK_PreDoc = 6,
    _YY_TK_PostDoc = 7,
    _YY_TK_Comment = 8,
    _YY_TK_SysCmd = 9,
    _YY_TK_Error = 10,
    _YY_KW_Add = 11,
    _YY_KW_And = 12,
    _YY_KW_Always = 13,
    _YY_KW_Assert = 14,
    _YY_KW_Break = 15,
    _YY_KW_But = 16,
    _YY_KW_By = 17,
    _YY_KW_Case = 18,
    _YY_KW_Catch = 19,
    _YY_KW_Default = 20,
    _YY_KW_Define = 21,
    _YY_KW_Delay = 22,
    _YY_KW_Do = 23,
    _YY_KW_Else = 24,
    _YY_KW_Except = 25,
    _YY_KW_Export = 26,
    _YY_KW_Extend = 27,
    _YY_KW_Finally = 28,
    _YY_KW_Fix = 29,
    _YY_KW_For = 30,
    _YY_KW_Fluid = 31,
    _YY_KW_Free = 32,
    _YY_KW_From = 33,
    _YY_KW_Generate = 34,
    _YY_KW_Goto = 35,
    _YY_KW_Has = 36,
    _YY_KW_If = 37,
    _YY_KW_Import = 38,
    _YY_KW_In = 39,
    _YY_KW_Inline = 40,
    _YY_KW_Is = 41,
    _YY_KW_Isnt = 42,
    _YY_KW_Iterate = 43,
    _YY_KW_Let = 44,
    _YY_KW_Local = 45,
    _YY_KW_Macro = 46,
    _YY_KW_Mod = 47,
    _YY_KW_Never = 48,
    _YY_KW_Not = 49,
    _YY_KW_Of = 50,
    _YY_KW_Or = 51,
    _YY_KW_Pretend = 52,
    _YY_KW_Quo = 53,
    _YY_KW_Reference = 54,
    _YY_KW_Rem = 55,
    _YY_KW_Repeat = 56,
    _YY_KW_Return = 57,
    _YY_KW_Rule = 58,
    _YY_KW_Select = 59,
    _YY_KW_Then = 60,
    _YY_KW_Throw = 61,
    _YY_KW_To = 62,
    _YY_KW_Try = 63,
    _YY_KW_Where = 64,
    _YY_KW_While = 65,
    _YY_KW_With = 66,
    _YY_KW_Yield = 67,
    _YY_KW_Quote = 68,
    _YY_KW_Grave = 69,
    _YY_KW_Ampersand = 70,
    _YY_KW_Comma = 71,
    _YY_KW_Semicolon = 72,
    _YY_KW_Dollar = 73,
    _YY_KW_Sharp = 74,
    _YY_KW_At = 75,
    _YY_KW_Assign = 76,
    _YY_KW_Colon = 77,
    _YY_KW_ColonStar = 78,
    _YY_KW_2Colon = 79,
    _YY_KW_Star = 80,
    _YY_KW_2Star = 81,
    _YY_KW_Dot = 82,
    _YY_KW_2Dot = 83,
    _YY_KW_EQ = 84,
    _YY_KW_2EQ = 85,
    _YY_KW_MArrow = 86,
    _YY_KW_Implies = 87,
    _YY_KW_GT = 88,
    _YY_KW_2GT = 89,
    _YY_KW_GE = 90,
    _YY_KW_LT = 91,
    _YY_KW_2LT = 92,
    _YY_KW_LE = 93,
    _YY_KW_LArrow = 94,
    _YY_KW_Hat = 95,
    _YY_KW_HatE = 96,
    _YY_KW_Tilde = 97,
    _YY_KW_TildeE = 98,
    _YY_KW_Plus = 99,
    _YY_KW_PlusMinus = 100,
    _YY_KW_MapsTo = 101,
    _YY_KW_MapsToStar = 102,
    _YY_KW_Minus = 103,
    _YY_KW_RArrow = 104,
    _YY_KW_MapStar = 105,
    _YY_KW_Slash = 106,
    _YY_KW_Wedge = 107,
    _YY_KW_Backslash = 108,
    _YY_KW_Vee = 109,
    _YY_KW_OBrack = 110,
    _YY_KW_OBBrack = 111,
    _YY_KW_OCurly = 112,
    _YY_KW_OBCurly = 113,
    _YY_KW_OParen = 114,
    _YY_KW_OBParen = 115,
    _YY_KW_CBrack = 116,
    _YY_KW_CCurly = 117,
    _YY_KW_CParen = 118,
    _YY_KW_Bar = 119,
    _YY_KW_CBBrack = 120,
    _YY_KW_CBCurly = 121,
    _YY_KW_CBParen = 122,
    _YY_KW_2Bar = 123,
    _YY_KW_NewLine = 124,
    _YY_KW_StartPile = 125,
    _YY_KW_EndPile = 126,
    _YY_KW_SetTab = 127,
    _YY_KW_BackSet = 128,
    _YY_KW_BackTab = 129,
    _YY_KW_Juxtapose = 130
  };
#endif
/* Tokens.  */
#define _YY_TK_Id 1
#define _YY_TK_Blank 2
#define _YY_TK_Int 3
#define _YY_TK_Float 4
#define _YY_TK_String 5
#define _YY_TK_PreDoc 6
#define _YY_TK_PostDoc 7
#define _YY_TK_Comment 8
#define _YY_TK_SysCmd 9
#define _YY_TK_Error 10
#define _YY_KW_Add 11
#define _YY_KW_And 12
#define _YY_KW_Always 13
#define _YY_KW_Assert 14
#define _YY_KW_Break 15
#define _YY_KW_But 16
#define _YY_KW_By 17
#define _YY_KW_Case 18
#define _YY_KW_Catch 19
#define _YY_KW_Default 20
#define _YY_KW_Define 21
#define _YY_KW_Delay 22
#define _YY_KW_Do 23
#define _YY_KW_Else 24
#define _YY_KW_Except 25
#define _YY_KW_Export 26
#define _YY_KW_Extend 27
#define _YY_KW_Finally 28
#define _YY_KW_Fix 29
#define _YY_KW_For 30
#define _YY_KW_Fluid 31
#define _YY_KW_Free 32
#define _YY_KW_From 33
#define _YY_KW_Generate 34
#define _YY_KW_Goto 35
#define _YY_KW_Has 36
#define _YY_KW_If 37
#define _YY_KW_Import 38
#define _YY_KW_In 39
#define _YY_KW_Inline 40
#define _YY_KW_Is 41
#define _YY_KW_Isnt 42
#define _YY_KW_Iterate 43
#define _YY_KW_Let 44
#define _YY_KW_Local 45
#define _YY_KW_Macro 46
#define _YY_KW_Mod 47
#define _YY_KW_Never 48
#define _YY_KW_Not 49
#define _YY_KW_Of 50
#define _YY_KW_Or 51
#define _YY_KW_Pretend 52
#define _YY_KW_Quo 53
#define _YY_KW_Reference 54
#define _YY_KW_Rem 55
#define _YY_KW_Repeat 56
#define _YY_KW_Return 57
#define _YY_KW_Rule 58
#define _YY_KW_Select 59
#define _YY_KW_Then 60
#define _YY_KW_Throw 61
#define _YY_KW_To 62
#define _YY_KW_Try 63
#define _YY_KW_Where 64
#define _YY_KW_While 65
#define _YY_KW_With 66
#define _YY_KW_Yield 67
#define _YY_KW_Quote 68
#define _YY_KW_Grave 69
#define _YY_KW_Ampersand 70
#define _YY_KW_Comma 71
#define _YY_KW_Semicolon 72
#define _YY_KW_Dollar 73
#define _YY_KW_Sharp 74
#define _YY_KW_At 75
#define _YY_KW_Assign 76
#define _YY_KW_Colon 77
#define _YY_KW_ColonStar 78
#define _YY_KW_2Colon 79
#define _YY_KW_Star 80
#define _YY_KW_2Star 81
#define _YY_KW_Dot 82
#define _YY_KW_2Dot 83
#define _YY_KW_EQ 84
#define _YY_KW_2EQ 85
#define _YY_KW_MArrow 86
#define _YY_KW_Implies 87
#define _YY_KW_GT 88
#define _YY_KW_2GT 89
#define _YY_KW_GE 90
#define _YY_KW_LT 91
#define _YY_KW_2LT 92
#define _YY_KW_LE 93
#define _YY_KW_LArrow 94
#define _YY_KW_Hat 95
#define _YY_KW_HatE 96
#define _YY_KW_Tilde 97
#define _YY_KW_TildeE 98
#define _YY_KW_Plus 99
#define _YY_KW_PlusMinus 100
#define _YY_KW_MapsTo 101
#define _YY_KW_MapsToStar 102
#define _YY_KW_Minus 103
#define _YY_KW_RArrow 104
#define _YY_KW_MapStar 105
#define _YY_KW_Slash 106
#define _YY_KW_Wedge 107
#define _YY_KW_Backslash 108
#define _YY_KW_Vee 109
#define _YY_KW_OBrack 110
#define _YY_KW_OBBrack 111
#define _YY_KW_OCurly 112
#define _YY_KW_OBCurly 113
#define _YY_KW_OParen 114
#define _YY_KW_OBParen 115
#define _YY_KW_CBrack 116
#define _YY_KW_CCurly 117
#define _YY_KW_CParen 118
#define _YY_KW_Bar 119
#define _YY_KW_CBBrack 120
#define _YY_KW_CBCurly 121
#define _YY_KW_CBParen 122
#define _YY_KW_2Bar 123
#define _YY_KW_NewLine 124
#define _YY_KW_StartPile 125
#define _YY_KW_EndPile 126
#define _YY_KW_SetTab 127
#define _YY_KW_BackSet 128
#define _YY_KW_BackTab 129
#define _YY_KW_Juxtapose 130

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 165 "axl_y.yt"

        Token           tok ;
        TokenList       toklist ;
        AbSyn           ab ;
        AbSynList       ablist ;

#line 409 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);





#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2514

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  133
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  143
/* YYNRULES -- Number of rules.  */
#define YYNRULES  358
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  547

#define YYUNDEFTOK  2
#define YYMAXUTOK   257


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   324,   324,   329,   333,   334,   335,   340,   342,   344,
     346,   348,   350,   352,   354,   356,   358,   360,   364,   366,
     368,   373,   378,   383,   384,   385,   390,   391,   395,   397,
     403,   407,   408,   414,   418,   419,   424,   431,   434,   437,
     442,   443,   445,   447,   449,   451,   455,   456,   458,   460,
     462,   464,   468,   469,   471,   473,   475,   477,   483,   484,
     486,   488,   490,   492,   498,   500,   504,   509,   510,   512,
     514,   516,   518,   520,   522,   524,   526,   528,   530,   532,
     534,   536,   538,   540,   542,   544,   546,   548,   552,   553,
     555,   557,   559,   561,   563,   565,   567,   569,   571,   573,
     575,   577,   579,   581,   583,   585,   587,   589,   591,   596,
     597,   602,   604,   609,   614,   616,   618,   621,   623,   625,
     630,   631,   636,   641,   643,   648,   650,   655,   656,   658,
     660,   665,   672,   673,   675,   679,   680,   685,   686,   688,
     690,   695,   696,   698,   700,   705,   706,   708,   713,   714,
     716,   721,   722,   727,   728,   733,   734,   740,   741,   743,
     745,   749,   750,   752,   754,   759,   763,   764,   769,   770,
     775,   776,   781,   782,   787,   788,   790,   792,   794,   799,
     805,   806,   807,   808,   809,   810,   811,   812,   816,   817,
     818,   819,   820,   821,   822,   823,   827,   828,   829,   830,
     831,   832,   833,   834,   836,   836,   836,   837,   837,   838,
     838,   838,   839,   839,   839,   840,   840,   840,   841,   841,
     841,   842,   842,   843,   843,   843,   844,   844,   844,   845,
     845,   845,   846,   846,   851,   855,   859,   864,   865,   867,
     871,   872,   874,   880,   881,   883,   885,   889,   890,   892,
     894,   900,   901,   905,   906,   907,   911,   912,   916,   917,
     918,   922,   923,   927,   928,   932,   937,   942,   949,   959,
     966,   975,   980,   981,   985,   986,   990,   992,   994,  1000,
    1002,  1004,  1012,  1017,  1021,  1025,  1029,  1033,  1037,  1041,
    1045,  1051,  1053,  1057,  1059,  1063,  1065,  1069,  1071,  1075,
    1077,  1081,  1083,  1087,  1089,  1093,  1095,  1101,  1102,  1105,
    1106,  1109,  1110,  1113,  1114,  1117,  1118,  1121,  1122,  1125,
    1126,  1129,  1130,  1137,  1146,  1151,  1156,  1158,  1163,  1165,
    1172,  1177,  1182,  1187,  1196,  1198,  1202,  1204,  1208,  1210,
    1214,  1216,  1224,  1230,  1232,  1234,  1241,  1247,  1253,  1255,
    1257,  1263,  1269,  1270,  1276,  1278,  1284,  1286,  1292
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- MutString name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "_YY_TK_Id", "_YY_TK_Blank",
  "_YY_TK_Int", "_YY_TK_Float", "_YY_TK_String", "_YY_TK_PreDoc",
  "_YY_TK_PostDoc", "_YY_TK_Comment", "_YY_TK_SysCmd", "_YY_TK_Error",
  "_YY_KW_Add", "_YY_KW_And", "_YY_KW_Always", "_YY_KW_Assert",
  "_YY_KW_Break", "_YY_KW_But", "_YY_KW_By", "_YY_KW_Case", "_YY_KW_Catch",
  "_YY_KW_Default", "_YY_KW_Define", "_YY_KW_Delay", "_YY_KW_Do",
  "_YY_KW_Else", "_YY_KW_Except", "_YY_KW_Export", "_YY_KW_Extend",
  "_YY_KW_Finally", "_YY_KW_Fix", "_YY_KW_For", "_YY_KW_Fluid",
  "_YY_KW_Free", "_YY_KW_From", "_YY_KW_Generate", "_YY_KW_Goto",
  "_YY_KW_Has", "_YY_KW_If", "_YY_KW_Import", "_YY_KW_In", "_YY_KW_Inline",
  "_YY_KW_Is", "_YY_KW_Isnt", "_YY_KW_Iterate", "_YY_KW_Let",
  "_YY_KW_Local", "_YY_KW_Macro", "_YY_KW_Mod", "_YY_KW_Never",
  "_YY_KW_Not", "_YY_KW_Of", "_YY_KW_Or", "_YY_KW_Pretend", "_YY_KW_Quo",
  "_YY_KW_Reference", "_YY_KW_Rem", "_YY_KW_Repeat", "_YY_KW_Return",
  "_YY_KW_Rule", "_YY_KW_Select", "_YY_KW_Then", "_YY_KW_Throw",
  "_YY_KW_To", "_YY_KW_Try", "_YY_KW_Where", "_YY_KW_While", "_YY_KW_With",
  "_YY_KW_Yield", "_YY_KW_Quote", "_YY_KW_Grave", "_YY_KW_Ampersand",
  "_YY_KW_Comma", "_YY_KW_Semicolon", "_YY_KW_Dollar", "_YY_KW_Sharp",
  "_YY_KW_At", "_YY_KW_Assign", "_YY_KW_Colon", "_YY_KW_ColonStar",
  "_YY_KW_2Colon", "_YY_KW_Star", "_YY_KW_2Star", "_YY_KW_Dot",
  "_YY_KW_2Dot", "_YY_KW_EQ", "_YY_KW_2EQ", "_YY_KW_MArrow",
  "_YY_KW_Implies", "_YY_KW_GT", "_YY_KW_2GT", "_YY_KW_GE", "_YY_KW_LT",
  "_YY_KW_2LT", "_YY_KW_LE", "_YY_KW_LArrow", "_YY_KW_Hat", "_YY_KW_HatE",
  "_YY_KW_Tilde", "_YY_KW_TildeE", "_YY_KW_Plus", "_YY_KW_PlusMinus",
  "_YY_KW_MapsTo", "_YY_KW_MapsToStar", "_YY_KW_Minus", "_YY_KW_RArrow",
  "_YY_KW_MapStar", "_YY_KW_Slash", "_YY_KW_Wedge", "_YY_KW_Backslash",
  "_YY_KW_Vee", "_YY_KW_OBrack", "_YY_KW_OBBrack", "_YY_KW_OCurly",
  "_YY_KW_OBCurly", "_YY_KW_OParen", "_YY_KW_OBParen", "_YY_KW_CBrack",
  "_YY_KW_CCurly", "_YY_KW_CParen", "_YY_KW_Bar", "_YY_KW_CBBrack",
  "_YY_KW_CBCurly", "_YY_KW_CBParen", "_YY_KW_2Bar", "_YY_KW_NewLine",
  "_YY_KW_StartPile", "_YY_KW_EndPile", "_YY_KW_SetTab", "_YY_KW_BackSet",
  "_YY_KW_BackTab", "_YY_KW_Juxtapose", "$accept", "Goal", "Expression",
  "Labeled", "Declaration", "ExportDecl", "ToPart", "FromPart",
  "MacroBody", "Sig", "DeclPart", "Comma", "CommaItem", "DeclBinding",
  "InfixedExprsDecl", "InfixedExprs", "Binding_Collection_",
  "Binding_BalStatement_", "Binding_AnyStatement_",
  "BindingL_Infixed_Collection_", "BindingL_Infixed_BalStatement_",
  "BindingL_Infixed_AnyStatement_",
  "BindingR_InfixedExprsDecl_AnyStatement_", "AnyStatement",
  "BalStatement", "Flow_BalStatement_", "Flow_AnyStatement_", "GenBound",
  "ButExpr", "Cases", "AlwaysPart_BalStatement_",
  "AlwaysPart_AnyStatement_", "Collection", "Iterators", "Iterators1",
  "Iterator", "ForLhs", "SuchthatPart", "Infixed", "InfixedExpr", "E3",
  "E4", "E5", "E6", "E7", "E8", "E9", "E11_E12_", "E11_Op_", "Type", "E12",
  "E13", "QualTail", "OpQualTail", "E14", "E15", "Op", "NakedOp",
  "ArrowOp", "LatticeOp", "RelationOp", "SegOp", "PlusOp", "QuotientOp",
  "TimesOp", "PowerOp", "ArrowTok", "LatticeTok", "RelationTok", "SegTok",
  "PlusTok", "QuotientTok", "TimesTok", "PowerTok", "Application",
  "RightJuxtaposed", "LeftJuxtaposed", "Jright_Atom_", "Jright_Molecule_",
  "Jleft_Atom_", "Jleft_Molecule_", "Molecule", "Enclosure",
  "DeclMolecule", "BlockMolecule", "BlockEnclosure", "Block", "Parened",
  "Bracketed", "QuotedIds", "Names", "Atom", "Name", "Id", "Literal",
  "Nothing", "UnqualOp_PowerTok_", "UnqualOp_TimesTok_",
  "UnqualOp_QuotientTok_", "UnqualOp_PlusTok_", "UnqualOp_SegTok_",
  "UnqualOp_RelationTok_", "UnqualOp_LatticeTok_", "UnqualOp_ArrowTok_",
  "QualOp_PowerTok_", "QualOp_TimesTok_", "QualOp_QuotientTok_",
  "QualOp_PlusTok_", "QualOp_SegTok_", "QualOp_RelationTok_",
  "QualOp_LatticeTok_", "QualOp_ArrowTok_", "opt_Application_", "opt_E14_",
  "opt_SuchthatPart_", "opt_Collection_", "opt_Name_", "opt_FromPart_",
  "opt_Sig_", "opt_Labeled_", "Doc_Expression_", "PreDocument",
  "PostDocument", "PreDocumentList", "PostDocumentList",
  "enlist1_Name__YY_KW_Comma_AB_Comma_",
  "enlist1_InfixedExpr__YY_KW_Comma_AB_Comma_",
  "enlist1_CommaItem__YY_KW_Comma_AB_Comma_",
  "enlist1_Infixed__YY_KW_Comma_AB_Comma_", "enlister1_Name__YY_KW_Comma_",
  "enlister1_InfixedExpr__YY_KW_Comma_",
  "enlister1_CommaItem__YY_KW_Comma_", "enlister1_Infixed__YY_KW_Comma_",
  "enlist1a_Labeled__YY_KW_Semicolon_AB_Sequence_",
  "enlister1a_Labeled__YY_KW_Semicolon_", "Piled_Expression_",
  "Curly_Labeled_", "PileContents_Expression_", "CurlyContents_Labeled_",
  "CurlyContentsList_Labeled_", "CurlyContent1_Labeled_",
  "CurlyContentA_Labeled_", "CurlyContentB_Labeled_", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130
};
# endif

#define YYPACT_NINF (-407)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-353)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -407,    59,  -407,  -407,  -407,   734,  -407,    44,    77,  -407,
    1377,  -407,  -407,    60,   143,  -407,  -407,  -407,  -407,  -407,
    1491,  2403,  -407,  -407,  1941,  1941,  1491,  1491,  1491,  1941,
    1941,  1941,  1717,  1941,  1941,   100,    64,  1491,  1941,  1941,
    -407,  -407,  2403,  1941,  1829,  -407,  -407,  2058,  -407,  1491,
    -407,  1491,  1941,  1491,  1491,  1491,  1941,  1491,  2334,  -407,
     170,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  1147,  -407,  1261,   914,   143,
    -407,  -407,  -407,  -407,   115,  -407,  -407,  -407,   109,   142,
      72,  -407,   128,    11,    42,  2198,    61,   134,   117,    83,
    -407,   239,   136,  -407,     5,   224,  -407,  -407,  -407,  -407,
     525,  -407,   443,  -407,  -407,  -407,   130,   139,   149,   151,
     153,   155,   179,   181,  -407,  -407,  -407,  1946,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,    51,  -407,   172,  -407,
    -407,   143,   143,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
     156,    11,  -407,  -407,  -407,  -407,   185,  -407,  -407,  -407,
    -407,  -407,    68,    23,  -407,  -407,  1941,  1941,  1941,   240,
    -407,  -407,  -407,  1491,  1491,  -407,  -407,   221,   176,   176,
    -407,  -407,  1941,  -407,  -407,  -407,  2058,  -407,  2003,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,    72,    68,  -407,   248,
    -407,   123,  -407,  -407,  -407,   227,  -407,  -407,   218,  1377,
    -407,   180,  -407,  -407,   229,   190,  -407,   194,   182,  -407,
    1377,   122,  -407,  1491,  1491,  1491,  -407,  1491,  1491,  1491,
    1491,  1491,  -407,  1903,  1903,  -407,  2274,  2274,  2274,  1791,
    1791,   525,  1903,  1903,  1903,  1903,  1903,  1903,  1903,  1903,
    1903,  1903,  1903,  1903,  1903,  2115,    61,  1903,   117,   802,
     802,   802,   802,   802,   802,   802,   802,  1775,  -407,  -407,
    2078,  2078,  1491,  -407,  -407,  1491,  1491,  1491,  1491,  1491,
    -407,  2162,  1941,  1941,  -407,  -407,  -407,  -407,  -407,  1941,
     263,  -407,  1605,  -407,  -407,  -407,  -407,   176,  -407,  -407,
    1775,  -407,  -407,  1941,  2162,  2162,  -407,  2403,  -407,    68,
    -407,  -407,  1033,  -407,  -407,    77,   143,    77,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,   148,  -407,  -407,
    2198,  1791,  2198,  2198,    61,    61,   134,   117,    83,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
     126,  -407,   242,   566,  -407,   243,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,   246,  -407,   207,  -407,  1605,  2403,  1605,  1605,
    1605,   100,    64,  1491,  2403,   305,  1605,  1605,  1941,  1491,
    1605,  1491,  1605,   307,  -407,  -407,  -407,  -407,    31,   289,
     302,  -407,  -407,  -407,  -407,  -407,  -407,   780,   309,    78,
      68,    78,  -407,  -407,  -407,  -407,  -407,  2115,   802,  1941,
    1941,  -407,  -407,  -407,  -407,   322,  -407,  -407,  -407,  1605,
     325,   290,   328,  -407,  -407,   338,   324,  -407,   138,  -407,
    1491,  1605,  1605,  1605,  1605,  1605,  1605,  1605,  1941,  1941,
    1941,  1941,  1941,  1941,  1491,  1491,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  1605,  1941,  2162,  2162,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,   341,   342,   120,   120,  1605,  1605,
    1605,  -407,   344,  -407,  -407,  -407,  -407
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     282,     0,   354,     2,   351,     0,     1,     0,   282,   326,
     282,   324,   355,   353,   282,   327,   276,   279,   280,   281,
     282,   282,   222,   220,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   282,   282,     0,   282,   282,   282,
     218,   219,   282,   282,   282,   226,   108,     0,   227,   282,
     228,   282,   282,   282,   282,   282,   282,   282,     0,   277,
       0,   229,   232,   221,   209,   213,   214,   212,   216,   217,
     215,   205,   233,   211,   278,   210,   223,   225,   224,   204,
     206,   230,   208,   231,   207,   282,   282,   282,     0,   282,
       5,    17,     4,   338,    31,    39,    52,    65,    88,     0,
     122,   123,   120,   132,   136,   137,   141,   145,   148,   151,
     153,   155,   135,   157,   166,   168,   174,   161,   180,   181,
     182,   183,   184,   185,   186,   187,   305,   303,   301,   299,
     297,   295,   293,   291,   179,   234,   235,   240,   247,   252,
     134,   253,   254,   255,   251,   272,   273,   309,   203,   202,
     201,   200,   199,   198,   197,   196,     0,    30,   332,   263,
     264,   282,   282,   328,   357,   325,   100,   275,   290,   289,
     288,   287,   286,   285,   284,   283,   316,   274,   315,   195,
     194,   193,   192,   191,   190,   189,   188,   102,    12,    26,
      58,    34,    33,   336,    27,    36,   331,    13,    97,    96,
     105,   320,   319,    18,     8,    14,   282,   282,   282,     0,
     127,    11,    10,   282,   282,   109,   107,     0,   282,   282,
     101,     9,   282,    24,     7,    23,     0,   242,   237,   261,
     248,   262,   243,    98,    92,   314,   120,   313,   103,     0,
     106,     0,   126,   104,   269,     0,   334,   271,   330,   282,
     267,     0,   343,     3,   342,     0,   265,     0,     0,   348,
     282,     0,   358,   282,   282,   282,   124,   282,   282,   282,
     282,   282,   121,   282,   282,   133,   282,   282,   282,   282,
     282,   146,   282,   282,   282,   282,   282,   282,   282,   282,
     282,   282,   282,     0,     0,     0,   144,   282,   150,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   241,   249,
     282,   282,   282,   356,   329,   282,   282,   282,   282,   282,
      35,   282,   282,   282,    19,    20,   130,   128,   129,   282,
       0,    99,   282,   318,   317,    16,    15,   282,   239,   244,
       0,   238,   245,   282,   282,   282,   270,     0,   322,   321,
       6,   268,   345,   347,   266,   282,   282,   282,   346,    32,
      90,    91,    53,    54,    55,    56,    57,   165,    28,    29,
     138,   282,   139,   140,   142,   143,   147,   149,   152,   154,
     160,   159,   158,   156,   164,   163,   162,   167,   177,   178,
       0,   169,   170,   236,   306,   172,   304,   302,   300,   298,
     296,   294,   292,   259,   250,   260,   258,   308,   176,   257,
     307,   256,   175,   339,    59,    60,    61,    62,    63,   337,
     340,    22,   333,    21,   282,   110,   282,   282,   282,   282,
     282,   282,     0,   282,   282,   108,   282,   282,   282,   282,
     282,   282,   282,     0,    64,    38,    46,    66,    88,     0,
     120,    25,   246,   113,    37,    95,    40,   120,     0,   282,
     111,   282,   335,   344,   350,   323,   349,     0,     0,   282,
     282,   312,   311,   125,    79,   102,    76,    75,    84,   282,
     107,     0,   101,    77,    71,   103,     0,    85,     0,    83,
     282,   282,   282,   282,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   282,   282,    93,   119,    94,   171,
     173,   341,   131,    78,   282,   282,   282,   282,    89,    69,
      70,    47,    48,    49,    50,    51,    41,    42,    43,    44,
      45,   112,   117,   118,     0,    95,   282,   282,   282,   282,
     282,    72,   119,    73,    68,   114,   115
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -407,  -407,   -33,     1,  -407,   330,  -407,   169,  -407,   124,
     195,  -407,   -30,  -294,  -407,  -407,  -407,  1664,    43,  -175,
     -31,  -154,  -407,  -407,  -407,  -407,  -407,   -46,  -313,  -394,
    -161,  -406,   160,   192,  -407,   291,  -407,  -407,    70,    99,
    -407,   -98,  -110,   114,  -100,   113,  -223,   -26,  -407,   127,
      92,  -407,   -70,   -38,  -407,   -23,  -407,  -407,   284,   296,
     -68,   -89,   -92,   -77,   -90,   293,   -17,   -15,   -12,    -9,
       2,    24,    26,    41,   -36,  -407,  -407,  -101,  -407,  -407,
    -247,  -227,   -27,    96,    71,  -121,   678,  -407,  -407,  -407,
    -407,   -34,   -57,   -18,  -407,     0,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,    -6,   -40,  -166,   257,  -407,
    -210,   403,   -55,   413,   261,  -407,  -407,  -407,  -407,  -407,
    -407,  -407,  -407,  -407,  -407,  -407,  -407,  -407,   340,  -407,
    -407,  -407,  -407
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,   251,   252,    90,    91,   324,   333,   224,   201,
     275,    92,    93,   189,   190,   191,   453,   443,    94,   454,
     445,    95,   192,    96,   446,   447,    97,   214,   459,   455,
     541,   506,    98,    99,   100,   101,   209,   471,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   368,
     113,   114,   391,   394,   115,   116,   117,   167,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   392,   227,   136,   228,
     137,   138,   139,   408,   404,   230,   140,   141,   142,   143,
     245,   144,   176,   145,   146,   147,   179,   180,   181,   182,
     183,   184,   185,   186,   148,   149,   150,   151,   152,   153,
     154,   155,   411,   156,   473,   238,   187,   335,   203,   350,
     259,   260,   164,    11,   165,   247,   195,   157,   421,   248,
     196,   158,   422,   253,   254,   159,   160,   261,     3,     4,
       5,    12,    13
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
       2,   246,   220,   177,   168,     9,   169,   217,     9,   170,
     296,    89,   171,   232,   163,   282,   309,   281,   216,   284,
     229,   178,   298,   172,   177,   168,   249,   169,   297,   202,
     170,   283,   461,   171,   262,   215,   308,   280,   202,   202,
     177,   168,   178,   169,   172,   173,   170,   174,   393,   171,
     458,   458,   237,   336,   257,   508,   276,   -67,   322,     6,
     172,   379,   175,   166,   310,   383,   173,    16,   174,   198,
     199,   200,   395,   395,   395,   395,   395,   395,   395,   395,
      22,  -309,   173,   175,   174,     8,     2,   323,     9,   163,
     273,   274,   233,   504,   234,   277,   239,   240,   241,   175,
     243,    71,   210,   232,    32,   339,   313,   342,   505,   531,
     229,    79,    80,   362,   363,   364,   365,   366,    14,   311,
     491,   535,   236,   193,   193,   338,   242,   341,   193,   193,
     193,   508,   193,   193,   161,   539,  -309,   193,   193,    56,
      59,   344,   193,   193,   345,   464,    63,   466,   188,   197,
     540,    82,   162,    84,   204,   205,   516,   211,   212,   517,
      32,   163,   163,    74,   213,    61,    45,   221,   225,   374,
     375,   451,    48,    16,    50,    17,    18,    19,   370,   372,
     373,   263,   377,   330,   297,   297,   297,   297,   297,   297,
     289,    81,   232,    83,   232,    56,    58,   298,   264,   229,
     265,   229,   285,   536,   537,   299,   267,   281,   371,   371,
     371,   322,   235,   290,   300,   268,   269,   291,   334,   334,
     393,   283,   458,   458,   301,   286,   302,   356,   303,   287,
     304,   270,   271,   359,   315,    76,    77,  -310,    85,    78,
      86,   395,    87,   316,   317,   312,    59,   367,   367,   349,
     348,   293,   357,   358,   305,    88,   306,   331,   321,   318,
     319,   296,   396,   397,   398,   399,   400,   401,   402,    74,
     388,   389,   309,   406,   407,   407,   326,   327,   328,   297,
     403,   329,   413,   332,   282,   281,   281,   294,   284,   343,
     462,   347,  -310,   285,   272,   218,   219,   346,   351,   295,
     283,   465,   280,   352,   280,   280,   406,   360,   361,   353,
     410,   410,   355,   403,   354,   425,   286,   467,   468,   469,
     287,   193,    62,   526,   527,   528,   529,   530,   470,   177,
     168,   -87,   169,   490,    32,   170,    72,   334,   171,   362,
     363,   364,   365,   366,   460,   460,   337,   492,   -81,   172,
     503,   -86,   514,   463,   -80,     9,   163,     9,   414,   415,
     416,   417,   418,   229,   -82,   515,   229,   538,   -74,    56,
    -116,   173,   325,   174,   223,   444,   543,   380,   381,   382,
     493,   384,   385,   386,   387,   479,   320,   475,   175,   494,
     495,   266,   420,   423,   482,   376,   378,   509,   292,   424,
     278,   369,   450,   481,   288,   496,   497,   412,    10,   177,
     168,   452,   169,   457,   480,   170,   177,   168,   171,   169,
     419,    15,   170,   314,   472,   171,   255,   178,   272,   172,
     510,   215,   485,     0,   178,     0,   172,     0,   237,     0,
       0,     0,     0,   193,   193,     0,    16,     0,    17,    18,
      19,   173,     0,   174,     0,     0,  -282,     0,   173,   507,
     174,   507,   521,   522,   523,   524,   525,     0,   175,   166,
       0,   198,   199,   200,     0,   175,     0,     0,     0,   233,
     234,     0,   486,   240,   488,   243,     0,     0,     0,     0,
       0,     0,   448,     0,    47,     0,   450,     0,   450,   450,
     450,     0,     0,   456,     0,     0,   450,   450,   236,     0,
     450,  -282,   450,    58,     0,     0,   460,   460,     0,    59,
       0,     0,   331,     0,   449,     0,     0,     0,    16,     0,
      17,    18,    19,   518,   360,   361,   542,   542,  -282,   511,
     512,     0,    74,     0,     0,     0,     0,   532,   533,   450,
       0,     0,     0,     0,     0,    85,     0,   444,     0,    87,
       0,   450,   450,   450,   450,   450,   450,   450,   457,   457,
     457,   457,   457,   457,     0,     0,    47,     0,     0,     0,
       0,   518,   532,   533,   450,   457,   448,     0,   448,   448,
     448,     0,     0,  -282,     0,    58,   448,   448,   235,     0,
     448,    59,   448,     0,     0,     0,     0,     0,   450,   450,
     450,     0,     0,     0,     0,   193,   193,     0,   449,     0,
     449,   449,   449,     0,    74,     0,    76,    77,   449,   449,
      78,     0,   449,     0,   449,     0,    58,    85,     0,   448,
       0,    87,   272,     0,     0,     0,     0,     0,     0,   272,
     307,   448,   448,   448,   448,   448,   448,   448,   456,   456,
     456,   456,   456,   456,     0,     0,     0,     0,     0,     0,
       0,   449,     0,     0,   448,   456,     0,     0,    85,     0,
      86,     0,    87,   449,   449,   449,   449,   449,   449,   449,
       0,     0,     0,     0,     0,    88,     0,     0,   448,   448,
     448,     0,   194,   194,     0,     0,   449,   194,   194,   194,
       0,   194,   194,     0,     0,     0,   194,   194,     0,     0,
       0,   194,   194,     0,     0,   231,     0,     0,     0,     0,
     449,   449,   449,     0,  -352,     7,     0,  -282,     0,  -282,
    -282,  -282,     8,     0,     0,     0,     0,  -282,     0,     0,
    -282,  -282,     0,  -282,  -282,     0,  -282,  -282,  -282,  -282,
       0,  -282,  -282,  -282,     0,  -282,  -282,  -282,  -282,     0,
    -282,  -282,     0,  -282,  -282,     0,  -282,  -282,  -282,  -282,
       0,  -282,  -282,  -282,  -282,  -282,     0,     0,     0,  -282,
    -282,  -282,  -282,  -282,     0,  -282,     0,  -282,     0,  -282,
       0,  -282,  -282,  -282,  -282,    16,     0,    17,    18,    19,
    -282,  -282,    32,     0,     0,   231,  -282,  -282,     0,  -282,
    -282,     0,     0,     0,  -282,  -282,  -282,  -282,  -282,  -282,
    -282,  -282,  -282,  -282,  -282,  -282,  -282,     0,     0,  -282,
    -282,  -282,  -282,  -282,  -282,  -282,  -282,    56,  -282,     0,
    -282,     0,     0,  -352,     0,     0,     0,     0,   498,     0,
       0,     0,     0,  -282,     0,     0,     0,   499,   500,     0,
       0,     0,    58,     0,     0,     0,     0,     0,    59,     0,
       0,     0,     0,   501,   502,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     194,    74,     0,     0,   231,     0,   231,     0,     0,     0,
       0,     0,     0,     0,    85,   258,     0,  -282,    87,  -282,
    -282,  -282,     8,     0,     0,     0,     0,  -282,     0,     0,
    -282,  -282,     0,  -282,  -282,     0,  -282,  -282,  -282,  -282,
       0,  -282,  -282,  -282,     0,  -282,  -282,  -282,  -282,     0,
    -282,  -282,     0,  -282,  -282,     0,  -282,  -282,  -282,  -282,
       0,  -282,  -282,  -282,  -282,  -282,     0,     0,     0,  -282,
    -282,  -282,  -282,  -282,     0,  -282,     0,  -282,     0,  -282,
       0,  -282,  -282,  -282,  -282,   405,     0,     0,   409,   409,
    -282,  -282,     0,     0,     0,     0,  -282,  -282,     0,  -282,
    -282,     0,     0,     0,  -282,  -282,  -282,  -282,  -282,  -282,
    -282,  -282,  -282,  -282,  -282,  -282,  -282,     0,   405,  -282,
    -282,  -282,  -282,  -282,  -282,  -282,  -282,     0,  -282,     0,
    -282,     0,     0,     0,     0,     0,    16,     0,    17,    18,
      19,     0,     0,  -282,     0,     0,  -282,     0,     0,    20,
      21,     0,    22,    23,     0,    24,    25,    26,    27,     0,
      28,    29,    30,     0,    31,    32,    33,    34,   231,    35,
      36,   231,    37,    38,     0,    39,    40,    41,    42,     0,
      43,    44,    45,    46,    47,     0,     0,     0,    48,    49,
      50,    51,    52,     0,    53,     0,    54,     0,    55,     0,
      56,  -282,    57,    58,     0,     0,     0,     0,     0,    59,
      60,     0,     0,     0,     0,    61,    62,     0,    63,    64,
       0,     0,     0,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,     0,     0,    78,    79,
      80,    81,    82,    83,    84,    85,     0,    86,     0,    87,
      16,     0,    17,    18,    19,     0,     0,     0,     0,     0,
       0,     0,    88,    20,    21,     0,    22,    23,     0,    24,
      25,    26,    27,     0,    28,    29,    30,     0,    31,    32,
      33,    34,     0,    35,    36,     0,    37,    38,     0,    39,
      40,    41,    42,     0,    43,    44,    45,    46,    47,     0,
       0,     0,    48,    49,    50,    51,    52,     0,    53,     0,
      54,     0,    55,     0,    56,     0,    57,    58,     0,     0,
       0,     0,     0,    59,    60,     0,     0,     0,     0,    61,
      62,     0,    63,    64,     0,     0,     0,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
       0,     0,    78,    79,    80,    81,    82,    83,    84,    85,
       0,    86,     0,    87,    16,   250,    17,    18,    19,     0,
       0,     0,     0,     0,     0,     0,    88,    20,    21,     0,
      22,    23,     0,    24,    25,    26,    27,     0,    28,    29,
      30,     0,    31,    32,    33,    34,     0,    35,    36,     0,
      37,    38,     0,    39,    40,    41,    42,     0,    43,    44,
      45,    46,    47,     0,     0,     0,    48,    49,    50,    51,
      52,     0,    53,     0,    54,     0,    55,     0,    56,     0,
      57,    58,     0,     0,     0,     0,     0,    59,    60,     0,
       0,     0,     0,    61,    62,     0,    63,    64,     0,     0,
       0,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,     0,     0,    78,    79,    80,    81,
      82,    83,    84,    85,     0,    86,     0,    87,     0,     0,
      16,   256,    17,    18,    19,     0,     0,     0,     0,     0,
      88,     0,     0,    20,    21,     0,    22,    23,     0,    24,
      25,    26,    27,     0,    28,    29,    30,     0,    31,    32,
      33,    34,     0,    35,    36,     0,    37,    38,     0,    39,
      40,    41,    42,     0,    43,    44,    45,    46,    47,     0,
       0,     0,    48,    49,    50,    51,    52,     0,    53,     0,
      54,     0,    55,     0,    56,     0,    57,    58,     0,     0,
       0,     0,     0,    59,    60,     0,     0,     0,     0,    61,
      62,     0,    63,    64,     0,     0,     0,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
       0,     0,    78,    79,    80,    81,    82,    83,    84,    85,
       0,    86,     0,    87,    16,     0,    17,    18,    19,     0,
       0,     0,     0,     0,     0,     0,    88,    20,    21,     0,
      22,    23,     0,     0,     0,    26,    27,     0,    28,     0,
       0,     0,     0,    32,     0,     0,     0,    35,    36,     0,
      37,     0,     0,     0,    40,    41,    42,     0,     0,     0,
      45,    46,    47,     0,     0,     0,    48,    49,    50,    51,
      52,     0,    53,     0,    54,     0,    55,     0,    56,     0,
      57,    58,     0,     0,     0,     0,     0,    59,     0,     0,
       0,     0,     0,    61,    62,     0,    63,    64,     0,     0,
       0,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,     0,     0,    78,    79,    80,    81,
      82,    83,    84,    85,     0,    86,     0,    87,    16,     0,
      17,    18,    19,     0,     0,     0,     0,     0,     0,     0,
      88,   426,   427,     0,    22,    23,     0,     0,     0,   428,
     429,     0,   430,     0,     0,     0,     0,    32,     0,     0,
       0,   431,   432,     0,   433,     0,     0,     0,    40,    41,
     434,     0,     0,     0,    45,   435,    47,     0,     0,     0,
      48,   436,    50,   437,   438,     0,   439,     0,   440,     0,
     441,     0,    56,     0,   442,    58,     0,     0,     0,     0,
       0,    59,     0,     0,     0,     0,     0,    61,    62,     0,
      63,    64,     0,     0,     0,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,     0,     0,
      78,    79,    80,    81,    82,    83,    84,    85,     0,    86,
      16,    87,    17,    18,    19,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    88,     0,    22,    23,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     206,   207,     0,     0,     0,     0,     0,     0,     0,     0,
      40,    41,     0,     0,   208,     0,    45,     0,    47,     0,
       0,     0,    48,     0,    50,     0,     0,     0,    16,     0,
      17,    18,    19,     0,     0,     0,     0,    58,     0,     0,
       0,     0,     0,    59,    16,     0,    17,    18,    19,    61,
      62,     0,    63,    64,     0,     0,     0,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
       0,     0,    78,    79,    80,    81,    82,    83,    84,    85,
       0,    86,    16,    87,    17,    18,    19,     0,     0,     0,
       0,     0,    47,     0,     0,    58,    88,     0,    22,    23,
       0,    59,     0,     0,     0,     0,     0,    29,     0,     0,
       0,    58,     0,     0,     0,     0,     0,    59,     0,   222,
       0,     0,    40,    41,    74,     0,     0,     0,    45,     0,
      47,     0,     0,     0,    48,     0,    50,    85,     0,    86,
      74,    87,    76,    77,     0,     0,    78,     0,     0,    58,
       0,     0,     0,    85,    88,    59,    16,    87,    17,    18,
      19,    61,    62,     0,    63,    64,     0,     0,     0,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,     0,     0,    78,    79,    80,    81,    82,    83,
      84,    85,     0,    86,    16,    87,    17,    18,    19,    16,
       0,    17,    18,    19,    47,     0,     0,     0,    88,     0,
      22,    23,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    58,     0,     0,     0,     0,     0,    59,
       0,     0,     0,     0,    40,    41,     0,     0,     0,     0,
      45,     0,    47,     0,     0,     0,    48,   226,    50,     0,
       0,     0,    74,     0,     0,     0,    16,     0,    17,    18,
      19,    58,     0,     0,     0,    85,    58,    59,     0,    87,
       0,     0,    59,    61,    62,     0,    63,    64,     0,     0,
     307,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,     0,    74,    78,    79,    80,    81,
      82,    83,    84,    85,   226,    86,     0,    87,    85,     0,
      86,    16,    87,    17,    18,    19,     0,     0,     0,     0,
      88,     0,     0,    58,     0,    88,     0,     0,     0,    59,
       0,    16,     0,    17,    18,    19,     0,   340,     0,     0,
     474,     0,   476,   477,   478,     0,     0,     0,     0,     0,
     483,   484,    74,     0,   487,     0,   489,     0,     0,   226,
       0,     0,     0,     0,     0,    85,     0,    86,    16,    87,
      17,    18,    19,     0,     0,     0,     0,     0,    58,    47,
       0,     0,    88,     0,    59,     0,     0,     0,     0,     0,
       0,     0,     0,   513,     0,     0,     0,     0,    58,     0,
       0,     0,     0,     0,    59,   519,   520,    74,     0,     0,
       0,     0,     0,     0,     0,    16,   390,    17,    18,    19,
      85,     0,    86,     0,    87,     0,     0,    74,   534,     0,
       0,    22,    23,     0,     0,    58,     0,    88,     0,     0,
      85,    59,    86,     0,    87,     0,     0,     0,     0,     0,
       0,     0,   544,   545,   546,    40,    41,    88,     0,     0,
       0,    45,     0,    47,    74,     0,     0,    48,    23,    50,
       0,     0,     0,     0,     0,     0,     0,    85,     0,     0,
       0,    87,    58,     0,     0,     0,   279,     0,    59,     0,
       0,    40,    41,     0,    61,    62,     0,    63,    64,     0,
       0,     0,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,     0,     0,    78,    79,    80,
      81,    82,    83,    84,    85,     0,     0,    16,    87,    17,
      18,    19,     0,     0,    64,     0,     0,     0,    65,    66,
      67,    68,    69,    70,    23,     0,    73,     0,    75,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    40,    41,     0,
       0,     0,     0,     0,     0,    47,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    16,     0,     0,
       0,     0,     0,     0,    58,     0,     0,     0,     0,     0,
      59,     0,     0,    22,    23,     0,     0,     0,     0,     0,
      64,     0,     0,     0,    65,    66,    67,    68,    69,    70,
       0,     0,    73,    74,    75,    76,    77,    40,    41,    78,
       0,     0,     0,    45,     0,     0,    85,     0,     0,    48,
      87,    50,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   244,     0,    16,     0,     0,     0,
      59,     0,     0,     0,     0,     0,    61,    62,     0,    63,
      64,     0,    22,    23,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,     0,     0,    78,
      79,    80,    81,    82,    83,    84,    40,    41,     0,     0,
       0,     0,    45,     0,     0,     0,     0,     0,    48,     0,
      50,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    59,
       0,     0,     0,     0,     0,    61,    62,     0,    63,    64,
       0,     0,     0,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,     0,     0,    78,    79,
      80,    81,    82,    83,    84
};

static const yytype_int16 yycheck[] =
{
       0,    58,    42,    21,    21,     5,    21,    37,     8,    21,
     120,    10,    21,    47,    14,   107,   137,   106,    36,   109,
      47,    21,   122,    21,    42,    42,    60,    42,   120,    29,
      42,   108,   345,    42,    89,    35,   137,   105,    38,    39,
      58,    58,    42,    58,    42,    21,    58,    21,   295,    58,
     344,   345,    52,   219,    87,   461,    14,    26,    35,     0,
      58,   284,    21,    20,    13,   288,    42,     3,    42,    26,
      27,    28,   299,   300,   301,   302,   303,   304,   305,   306,
      19,    13,    58,    42,    58,     8,    86,    64,    88,    89,
      79,    80,    49,    15,    51,    53,    53,    54,    55,    58,
      57,    96,    32,   137,    32,   226,   161,   228,    30,   503,
     137,   106,   107,   267,   268,   269,   270,   271,    74,    68,
      89,   515,    52,    24,    25,   226,    56,   228,    29,    30,
      31,   537,    33,    34,    74,    15,    68,    38,    39,    67,
      76,    18,    43,    44,    21,   355,    85,   357,    24,    25,
      30,   109,     9,   111,    30,    31,    18,    33,    34,    21,
      32,   161,   162,    99,    64,    82,    49,    43,    44,   279,
     280,   337,    55,     3,    57,     5,     6,     7,   276,   277,
     278,    66,   282,   213,   276,   277,   278,   279,   280,   281,
      54,   108,   226,   110,   228,    67,    70,   297,    89,   226,
      58,   228,    54,   516,   517,    75,    78,   296,   276,   277,
     278,    35,    52,    77,    75,    87,    88,    81,   218,   219,
     467,   298,   516,   517,    75,    77,    75,   260,    75,    81,
      75,   103,   104,   263,    78,   101,   102,    13,   112,   105,
     114,   468,   116,    87,    88,    73,    76,   273,   274,   249,
     249,    27,   130,   131,    75,   129,    75,   214,    73,   103,
     104,   371,   300,   301,   302,   303,   304,   305,   306,    99,
     293,   294,   393,   307,   310,   311,   206,   207,   208,   371,
     307,    41,   312,    62,   376,   374,   375,    63,   378,    41,
     347,    73,    68,    54,   102,    38,    39,    70,   118,    75,
     377,   356,   370,    74,   372,   373,   340,   264,   265,   119,
     310,   311,   130,   340,   120,    52,    77,    75,    75,    73,
      81,   222,    83,   498,   499,   500,   501,   502,   121,   347,
     347,    26,   347,    26,    32,   347,    97,   337,   347,   493,
     494,   495,   496,   497,   344,   345,   222,    58,    26,   347,
      41,    26,    62,   352,    26,   355,   356,   357,   315,   316,
     317,   318,   319,   390,    26,    41,   393,    26,    26,    67,
      26,   347,   203,   347,    44,   332,   537,   285,   286,   287,
      78,   289,   290,   291,   292,   431,   191,   427,   347,    87,
      88,   100,   322,   323,   434,   281,   283,   467,   114,   329,
     104,   274,   332,   433,   111,   103,   104,   311,     5,   427,
     427,   340,   427,   343,   432,   427,   434,   434,   427,   434,
     321,     8,   434,   162,   424,   434,    86,   427,   236,   427,
     468,   431,   438,    -1,   434,    -1,   434,    -1,   438,    -1,
      -1,    -1,    -1,   344,   345,    -1,     3,    -1,     5,     6,
       7,   427,    -1,   427,    -1,    -1,    13,    -1,   434,   459,
     434,   461,   493,   494,   495,   496,   497,    -1,   427,   426,
      -1,   428,   429,   430,    -1,   434,    -1,    -1,    -1,   436,
     437,    -1,   439,   440,   441,   442,    -1,    -1,    -1,    -1,
      -1,    -1,   332,    -1,    51,    -1,   426,    -1,   428,   429,
     430,    -1,    -1,   343,    -1,    -1,   436,   437,   438,    -1,
     440,    68,   442,    70,    -1,    -1,   516,   517,    -1,    76,
      -1,    -1,   479,    -1,   332,    -1,    -1,    -1,     3,    -1,
       5,     6,     7,   490,   491,   492,   536,   537,    13,   469,
     470,    -1,    99,    -1,    -1,    -1,    -1,   504,   505,   479,
      -1,    -1,    -1,    -1,    -1,   112,    -1,   514,    -1,   116,
      -1,   491,   492,   493,   494,   495,   496,   497,   498,   499,
     500,   501,   502,   503,    -1,    -1,    51,    -1,    -1,    -1,
      -1,   538,   539,   540,   514,   515,   426,    -1,   428,   429,
     430,    -1,    -1,    68,    -1,    70,   436,   437,   438,    -1,
     440,    76,   442,    -1,    -1,    -1,    -1,    -1,   538,   539,
     540,    -1,    -1,    -1,    -1,   516,   517,    -1,   426,    -1,
     428,   429,   430,    -1,    99,    -1,   101,   102,   436,   437,
     105,    -1,   440,    -1,   442,    -1,    70,   112,    -1,   479,
      -1,   116,   450,    -1,    -1,    -1,    -1,    -1,    -1,   457,
      84,   491,   492,   493,   494,   495,   496,   497,   498,   499,
     500,   501,   502,   503,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   479,    -1,    -1,   514,   515,    -1,    -1,   112,    -1,
     114,    -1,   116,   491,   492,   493,   494,   495,   496,   497,
      -1,    -1,    -1,    -1,    -1,   129,    -1,    -1,   538,   539,
     540,    -1,    24,    25,    -1,    -1,   514,    29,    30,    31,
      -1,    33,    34,    -1,    -1,    -1,    38,    39,    -1,    -1,
      -1,    43,    44,    -1,    -1,    47,    -1,    -1,    -1,    -1,
     538,   539,   540,    -1,     0,     1,    -1,     3,    -1,     5,
       6,     7,     8,    -1,    -1,    -1,    -1,    13,    -1,    -1,
      16,    17,    -1,    19,    20,    -1,    22,    23,    24,    25,
      -1,    27,    28,    29,    -1,    31,    32,    33,    34,    -1,
      36,    37,    -1,    39,    40,    -1,    42,    43,    44,    45,
      -1,    47,    48,    49,    50,    51,    -1,    -1,    -1,    55,
      56,    57,    58,    59,    -1,    61,    -1,    63,    -1,    65,
      -1,    67,    68,    69,    70,     3,    -1,     5,     6,     7,
      76,    77,    32,    -1,    -1,   137,    82,    83,    -1,    85,
      86,    -1,    -1,    -1,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,    -1,    -1,   105,
     106,   107,   108,   109,   110,   111,   112,    67,   114,    -1,
     116,    -1,    -1,   119,    -1,    -1,    -1,    -1,    78,    -1,
      -1,    -1,    -1,   129,    -1,    -1,    -1,    87,    88,    -1,
      -1,    -1,    70,    -1,    -1,    -1,    -1,    -1,    76,    -1,
      -1,    -1,    -1,   103,   104,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     222,    99,    -1,    -1,   226,    -1,   228,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   112,     1,    -1,     3,   116,     5,
       6,     7,     8,    -1,    -1,    -1,    -1,    13,    -1,    -1,
      16,    17,    -1,    19,    20,    -1,    22,    23,    24,    25,
      -1,    27,    28,    29,    -1,    31,    32,    33,    34,    -1,
      36,    37,    -1,    39,    40,    -1,    42,    43,    44,    45,
      -1,    47,    48,    49,    50,    51,    -1,    -1,    -1,    55,
      56,    57,    58,    59,    -1,    61,    -1,    63,    -1,    65,
      -1,    67,    68,    69,    70,   307,    -1,    -1,   310,   311,
      76,    77,    -1,    -1,    -1,    -1,    82,    83,    -1,    85,
      86,    -1,    -1,    -1,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,    -1,   340,   105,
     106,   107,   108,   109,   110,   111,   112,    -1,   114,    -1,
     116,    -1,    -1,    -1,    -1,    -1,     3,    -1,     5,     6,
       7,    -1,    -1,   129,    -1,    -1,    13,    -1,    -1,    16,
      17,    -1,    19,    20,    -1,    22,    23,    24,    25,    -1,
      27,    28,    29,    -1,    31,    32,    33,    34,   390,    36,
      37,   393,    39,    40,    -1,    42,    43,    44,    45,    -1,
      47,    48,    49,    50,    51,    -1,    -1,    -1,    55,    56,
      57,    58,    59,    -1,    61,    -1,    63,    -1,    65,    -1,
      67,    68,    69,    70,    -1,    -1,    -1,    -1,    -1,    76,
      77,    -1,    -1,    -1,    -1,    82,    83,    -1,    85,    86,
      -1,    -1,    -1,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,    -1,    -1,   105,   106,
     107,   108,   109,   110,   111,   112,    -1,   114,    -1,   116,
       3,    -1,     5,     6,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   129,    16,    17,    -1,    19,    20,    -1,    22,
      23,    24,    25,    -1,    27,    28,    29,    -1,    31,    32,
      33,    34,    -1,    36,    37,    -1,    39,    40,    -1,    42,
      43,    44,    45,    -1,    47,    48,    49,    50,    51,    -1,
      -1,    -1,    55,    56,    57,    58,    59,    -1,    61,    -1,
      63,    -1,    65,    -1,    67,    -1,    69,    70,    -1,    -1,
      -1,    -1,    -1,    76,    77,    -1,    -1,    -1,    -1,    82,
      83,    -1,    85,    86,    -1,    -1,    -1,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
      -1,    -1,   105,   106,   107,   108,   109,   110,   111,   112,
      -1,   114,    -1,   116,     3,   118,     5,     6,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   129,    16,    17,    -1,
      19,    20,    -1,    22,    23,    24,    25,    -1,    27,    28,
      29,    -1,    31,    32,    33,    34,    -1,    36,    37,    -1,
      39,    40,    -1,    42,    43,    44,    45,    -1,    47,    48,
      49,    50,    51,    -1,    -1,    -1,    55,    56,    57,    58,
      59,    -1,    61,    -1,    63,    -1,    65,    -1,    67,    -1,
      69,    70,    -1,    -1,    -1,    -1,    -1,    76,    77,    -1,
      -1,    -1,    -1,    82,    83,    -1,    85,    86,    -1,    -1,
      -1,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,    -1,    -1,   105,   106,   107,   108,
     109,   110,   111,   112,    -1,   114,    -1,   116,    -1,    -1,
       3,   120,     5,     6,     7,    -1,    -1,    -1,    -1,    -1,
     129,    -1,    -1,    16,    17,    -1,    19,    20,    -1,    22,
      23,    24,    25,    -1,    27,    28,    29,    -1,    31,    32,
      33,    34,    -1,    36,    37,    -1,    39,    40,    -1,    42,
      43,    44,    45,    -1,    47,    48,    49,    50,    51,    -1,
      -1,    -1,    55,    56,    57,    58,    59,    -1,    61,    -1,
      63,    -1,    65,    -1,    67,    -1,    69,    70,    -1,    -1,
      -1,    -1,    -1,    76,    77,    -1,    -1,    -1,    -1,    82,
      83,    -1,    85,    86,    -1,    -1,    -1,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
      -1,    -1,   105,   106,   107,   108,   109,   110,   111,   112,
      -1,   114,    -1,   116,     3,    -1,     5,     6,     7,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   129,    16,    17,    -1,
      19,    20,    -1,    -1,    -1,    24,    25,    -1,    27,    -1,
      -1,    -1,    -1,    32,    -1,    -1,    -1,    36,    37,    -1,
      39,    -1,    -1,    -1,    43,    44,    45,    -1,    -1,    -1,
      49,    50,    51,    -1,    -1,    -1,    55,    56,    57,    58,
      59,    -1,    61,    -1,    63,    -1,    65,    -1,    67,    -1,
      69,    70,    -1,    -1,    -1,    -1,    -1,    76,    -1,    -1,
      -1,    -1,    -1,    82,    83,    -1,    85,    86,    -1,    -1,
      -1,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,    -1,    -1,   105,   106,   107,   108,
     109,   110,   111,   112,    -1,   114,    -1,   116,     3,    -1,
       5,     6,     7,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     129,    16,    17,    -1,    19,    20,    -1,    -1,    -1,    24,
      25,    -1,    27,    -1,    -1,    -1,    -1,    32,    -1,    -1,
      -1,    36,    37,    -1,    39,    -1,    -1,    -1,    43,    44,
      45,    -1,    -1,    -1,    49,    50,    51,    -1,    -1,    -1,
      55,    56,    57,    58,    59,    -1,    61,    -1,    63,    -1,
      65,    -1,    67,    -1,    69,    70,    -1,    -1,    -1,    -1,
      -1,    76,    -1,    -1,    -1,    -1,    -1,    82,    83,    -1,
      85,    86,    -1,    -1,    -1,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,    -1,    -1,
     105,   106,   107,   108,   109,   110,   111,   112,    -1,   114,
       3,   116,     5,     6,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   129,    -1,    19,    20,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      33,    34,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    44,    -1,    -1,    47,    -1,    49,    -1,    51,    -1,
      -1,    -1,    55,    -1,    57,    -1,    -1,    -1,     3,    -1,
       5,     6,     7,    -1,    -1,    -1,    -1,    70,    -1,    -1,
      -1,    -1,    -1,    76,     3,    -1,     5,     6,     7,    82,
      83,    -1,    85,    86,    -1,    -1,    -1,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
      -1,    -1,   105,   106,   107,   108,   109,   110,   111,   112,
      -1,   114,     3,   116,     5,     6,     7,    -1,    -1,    -1,
      -1,    -1,    51,    -1,    -1,    70,   129,    -1,    19,    20,
      -1,    76,    -1,    -1,    -1,    -1,    -1,    28,    -1,    -1,
      -1,    70,    -1,    -1,    -1,    -1,    -1,    76,    -1,    40,
      -1,    -1,    43,    44,    99,    -1,    -1,    -1,    49,    -1,
      51,    -1,    -1,    -1,    55,    -1,    57,   112,    -1,   114,
      99,   116,   101,   102,    -1,    -1,   105,    -1,    -1,    70,
      -1,    -1,    -1,   112,   129,    76,     3,   116,     5,     6,
       7,    82,    83,    -1,    85,    86,    -1,    -1,    -1,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,    -1,    -1,   105,   106,   107,   108,   109,   110,
     111,   112,    -1,   114,     3,   116,     5,     6,     7,     3,
      -1,     5,     6,     7,    51,    -1,    -1,    -1,   129,    -1,
      19,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    70,    -1,    -1,    -1,    -1,    -1,    76,
      -1,    -1,    -1,    -1,    43,    44,    -1,    -1,    -1,    -1,
      49,    -1,    51,    -1,    -1,    -1,    55,    51,    57,    -1,
      -1,    -1,    99,    -1,    -1,    -1,     3,    -1,     5,     6,
       7,    70,    -1,    -1,    -1,   112,    70,    76,    -1,   116,
      -1,    -1,    76,    82,    83,    -1,    85,    86,    -1,    -1,
      84,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,    -1,    99,   105,   106,   107,   108,
     109,   110,   111,   112,    51,   114,    -1,   116,   112,    -1,
     114,     3,   116,     5,     6,     7,    -1,    -1,    -1,    -1,
     129,    -1,    -1,    70,    -1,   129,    -1,    -1,    -1,    76,
      -1,     3,    -1,     5,     6,     7,    -1,    84,    -1,    -1,
     426,    -1,   428,   429,   430,    -1,    -1,    -1,    -1,    -1,
     436,   437,    99,    -1,   440,    -1,   442,    -1,    -1,    51,
      -1,    -1,    -1,    -1,    -1,   112,    -1,   114,     3,   116,
       5,     6,     7,    -1,    -1,    -1,    -1,    -1,    70,    51,
      -1,    -1,   129,    -1,    76,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   479,    -1,    -1,    -1,    -1,    70,    -1,
      -1,    -1,    -1,    -1,    76,   491,   492,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,    51,     5,     6,     7,
     112,    -1,   114,    -1,   116,    -1,    -1,    99,   514,    -1,
      -1,    19,    20,    -1,    -1,    70,    -1,   129,    -1,    -1,
     112,    76,   114,    -1,   116,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   538,   539,   540,    43,    44,   129,    -1,    -1,
      -1,    49,    -1,    51,    99,    -1,    -1,    55,    20,    57,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   112,    -1,    -1,
      -1,   116,    70,    -1,    -1,    -1,    38,    -1,    76,    -1,
      -1,    43,    44,    -1,    82,    83,    -1,    85,    86,    -1,
      -1,    -1,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,    -1,    -1,   105,   106,   107,
     108,   109,   110,   111,   112,    -1,    -1,     3,   116,     5,
       6,     7,    -1,    -1,    86,    -1,    -1,    -1,    90,    91,
      92,    93,    94,    95,    20,    -1,    98,    -1,   100,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,    -1,
      -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,    -1,    -1,
      -1,    -1,    -1,    -1,    70,    -1,    -1,    -1,    -1,    -1,
      76,    -1,    -1,    19,    20,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    -1,    90,    91,    92,    93,    94,    95,
      -1,    -1,    98,    99,   100,   101,   102,    43,    44,   105,
      -1,    -1,    -1,    49,    -1,    -1,   112,    -1,    -1,    55,
     116,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    70,    -1,     3,    -1,    -1,    -1,
      76,    -1,    -1,    -1,    -1,    -1,    82,    83,    -1,    85,
      86,    -1,    19,    20,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,    -1,    -1,   105,
     106,   107,   108,   109,   110,   111,    43,    44,    -1,    -1,
      -1,    -1,    49,    -1,    -1,    -1,    -1,    -1,    55,    -1,
      57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,
      -1,    -1,    -1,    -1,    -1,    82,    83,    -1,    85,    86,
      -1,    -1,    -1,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,    -1,    -1,   105,   106,
     107,   108,   109,   110,   111
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,   134,   228,   271,   272,   273,     0,     1,     8,   228,
     254,   256,   274,   275,    74,   256,     3,     5,     6,     7,
      16,    17,    19,    20,    22,    23,    24,    25,    27,    28,
      29,    31,    32,    33,    34,    36,    37,    39,    40,    42,
      43,    44,    45,    47,    48,    49,    50,    51,    55,    56,
      57,    58,    59,    61,    63,    65,    67,    69,    70,    76,
      77,    82,    83,    85,    86,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   105,   106,
     107,   108,   109,   110,   111,   112,   114,   116,   129,   136,
     137,   138,   144,   145,   151,   154,   156,   159,   165,   166,
     167,   168,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   183,   184,   187,   188,   189,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   211,   213,   214,   215,
     219,   220,   221,   222,   224,   226,   227,   228,   237,   238,
     239,   240,   241,   242,   243,   244,   246,   260,   264,   268,
     269,    74,     9,   228,   255,   257,   151,   190,   199,   200,
     201,   202,   203,   204,   205,   206,   225,   226,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   249,   142,   146,
     147,   148,   155,   172,   219,   259,   263,   142,   151,   151,
     151,   142,   228,   251,   142,   142,    33,    34,    47,   169,
     171,   142,   142,    64,   160,   228,   226,   145,   251,   251,
     249,   142,    40,   138,   141,   142,    51,   210,   212,   215,
     218,   219,   224,   151,   151,   165,   171,   228,   248,   151,
     151,   151,   171,   151,    70,   223,   225,   258,   262,   224,
     118,   135,   136,   266,   267,   271,   120,   135,     1,   253,
     254,   270,   255,    66,    89,    58,   168,    78,    87,    88,
     103,   104,   166,    79,    80,   143,    14,    53,   192,    38,
     193,   194,   195,   196,   197,    54,    77,    81,   198,    54,
      77,    81,   191,    27,    63,    75,   175,   195,   177,    75,
      75,    75,    75,    75,    75,    75,    75,    84,   210,   218,
      13,    68,    73,   255,   257,    78,    87,    88,   103,   104,
     143,    73,    35,    64,   139,   140,   171,   171,   171,    41,
     145,   151,    62,   140,   228,   250,   250,   142,   210,   218,
      84,   210,   218,    41,    18,    21,    70,    73,   136,   228,
     252,   118,    74,   119,   120,   130,   135,   130,   131,   145,
     151,   151,   154,   154,   154,   154,   154,   180,   182,   182,
     174,   193,   174,   174,   175,   175,   176,   177,   178,   179,
     183,   183,   183,   179,   183,   183,   183,   183,   188,   188,
      51,   185,   209,   213,   186,   214,   186,   186,   186,   186,
     186,   186,   186,   215,   217,   219,   224,   207,   216,   219,
     228,   245,   216,   145,   151,   151,   151,   151,   151,   172,
     171,   261,   265,   171,   171,    52,    16,    17,    24,    25,
      27,    36,    37,    39,    45,    50,    56,    58,    59,    61,
      63,    65,    69,   150,   151,   153,   157,   158,   165,   166,
     171,   250,   217,   149,   152,   162,   165,   171,   146,   161,
     228,   161,   225,   136,   253,   255,   253,    75,    75,    73,
     121,   170,   228,   247,   150,   249,   150,   150,   150,   160,
     226,   145,   249,   150,   150,   248,   151,   150,   151,   150,
      26,    89,    58,    78,    87,    88,   103,   104,    78,    87,
      88,   103,   104,    41,    15,    30,   164,   228,   164,   185,
     186,   171,   171,   150,    62,    41,    18,    21,   151,   150,
     150,   153,   153,   153,   153,   153,   152,   152,   152,   152,
     152,   162,   151,   151,   150,   162,   161,   161,    26,    15,
      30,   163,   228,   163,   150,   150,   150
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int16 yyr1[] =
{
       0,   133,   134,   135,   136,   136,   136,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   138,   138,
     138,   139,   140,   141,   141,   141,   142,   142,   143,   143,
     144,   145,   145,   146,   147,   147,   148,   149,   150,   151,
     152,   152,   152,   152,   152,   152,   153,   153,   153,   153,
     153,   153,   154,   154,   154,   154,   154,   154,   155,   155,
     155,   155,   155,   155,   156,   156,   157,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   160,
     160,   161,   161,   162,   163,   163,   163,   164,   164,   164,
     165,   165,   166,   167,   167,   168,   168,   169,   169,   169,
     169,   170,   171,   171,   171,   172,   172,   173,   173,   173,
     173,   174,   174,   174,   174,   175,   175,   175,   176,   176,
     176,   177,   177,   178,   178,   179,   179,   180,   180,   180,
     180,   181,   181,   181,   181,   182,   183,   183,   184,   184,
     185,   185,   186,   186,   187,   187,   187,   187,   187,   188,
     189,   189,   189,   189,   189,   189,   189,   189,   190,   190,
     190,   190,   190,   190,   190,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   199,   199,   200,   200,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   202,   202,   203,   203,   203,   204,   204,   204,   205,
     205,   205,   206,   206,   207,   208,   209,   210,   210,   210,
     211,   211,   211,   212,   212,   212,   212,   213,   213,   213,
     213,   214,   214,   215,   215,   215,   216,   216,   217,   217,
     217,   218,   218,   219,   219,   220,   220,   221,   221,   222,
     222,   223,   224,   224,   225,   225,   226,   226,   226,   227,
     227,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   237,   238,   238,   239,   239,   240,   240,   241,
     241,   242,   242,   243,   243,   244,   244,   245,   245,   246,
     246,   247,   247,   248,   248,   249,   249,   250,   250,   251,
     251,   252,   252,   253,   254,   255,   256,   256,   257,   257,
     258,   259,   260,   261,   262,   262,   263,   263,   264,   264,
     265,   265,   266,   267,   267,   267,   268,   269,   270,   270,
     270,   271,   272,   272,   273,   273,   274,   274,   275
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     3,     2,     2,     2,
       2,     2,     2,     2,     2,     3,     3,     1,     2,     3,
       3,     2,     2,     1,     1,     3,     1,     1,     2,     2,
       1,     1,     3,     1,     1,     2,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     1,     3,     3,     3,
       3,     3,     1,     3,     3,     3,     3,     3,     1,     3,
       3,     3,     3,     3,     4,     1,     1,     1,     6,     3,
       3,     2,     5,     5,     4,     2,     2,     2,     3,     2,
       2,     2,     2,     2,     2,     2,     2,     1,     1,     6,
       3,     3,     2,     5,     5,     4,     2,     2,     2,     3,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     1,
       3,     1,     3,     1,     2,     2,     1,     2,     2,     1,
       1,     2,     1,     1,     2,     5,     2,     1,     2,     2,
       2,     2,     1,     2,     1,     1,     1,     1,     3,     3,
       3,     1,     3,     3,     2,     1,     2,     3,     1,     3,
       2,     1,     3,     1,     3,     1,     3,     1,     3,     3,
       3,     1,     3,     3,     3,     1,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     3,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       1,     2,     2,     1,     2,     2,     3,     1,     2,     2,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     3,     2,     3,     2,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     1,     1,     2,     1,     2,
       1,     1,     1,     1,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     1,     3,     2,     3,     3,     1,     3,
       3,     1,     1,     2,     1,     2,     3,     3,     3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 325 "axl_y.yt"
        { yypval = (yyval.ab) = (yyvsp[0].ab); }
#line 2451 "y.tab.c"
    break;

  case 6:
#line 336 "axl_y.yt"
        { (yyval.ab) = abNewLabel(TPOS((yyvsp[-2].tok)),(yyvsp[-1].ab),(yyvsp[0].ab)); }
#line 2457 "y.tab.c"
    break;

  case 7:
#line 341 "axl_y.yt"
        { (yyval.ab) = abNewMacro(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2463 "y.tab.c"
    break;

  case 8:
#line 343 "axl_y.yt"
        { (yyval.ab) = abNewExtend(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2469 "y.tab.c"
    break;

  case 9:
#line 345 "axl_y.yt"
        { (yyval.ab) = abNewLocal(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2475 "y.tab.c"
    break;

  case 10:
#line 347 "axl_y.yt"
        { (yyval.ab) = abNewFree(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2481 "y.tab.c"
    break;

  case 11:
#line 349 "axl_y.yt"
        { (yyval.ab) = abNewFluid(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2487 "y.tab.c"
    break;

  case 12:
#line 351 "axl_y.yt"
        { (yyval.ab) = abNewDefault(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2493 "y.tab.c"
    break;

  case 13:
#line 353 "axl_y.yt"
        { (yyval.ab) = abNewDDefine(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2499 "y.tab.c"
    break;

  case 14:
#line 355 "axl_y.yt"
        { (yyval.ab) = abNewFix(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2505 "y.tab.c"
    break;

  case 15:
#line 357 "axl_y.yt"
        { (yyval.ab) = abNewInline(TPOS((yyvsp[-2].tok)),(yyvsp[-1].ab),(yyvsp[0].ab)); }
#line 2511 "y.tab.c"
    break;

  case 16:
#line 359 "axl_y.yt"
        { (yyval.ab) = abNewImport(TPOS((yyvsp[-2].tok)),(yyvsp[-1].ab),(yyvsp[0].ab)); }
#line 2517 "y.tab.c"
    break;

  case 18:
#line 365 "axl_y.yt"
        { (yyval.ab) = abNewExport(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab), abZip,abZip); }
#line 2523 "y.tab.c"
    break;

  case 19:
#line 367 "axl_y.yt"
        { (yyval.ab) = abNewExport(TPOS((yyvsp[-2].tok)),(yyvsp[-1].ab),abZip,(yyvsp[0].ab)); }
#line 2529 "y.tab.c"
    break;

  case 20:
#line 369 "axl_y.yt"
        { (yyval.ab) = abNewExport(TPOS((yyvsp[-2].tok)),(yyvsp[-1].ab),(yyvsp[0].ab),abZip); }
#line 2535 "y.tab.c"
    break;

  case 21:
#line 374 "axl_y.yt"
        { (yyval.ab) = (yyvsp[0].ab); }
#line 2541 "y.tab.c"
    break;

  case 22:
#line 379 "axl_y.yt"
        { (yyval.ab) = (yyvsp[0].ab); }
#line 2547 "y.tab.c"
    break;

  case 25:
#line 386 "axl_y.yt"
        { (yyval.ab) = abNewImport(TPOS((yyvsp[-2].tok)),(yyvsp[-1].ab),(yyvsp[0].ab)); }
#line 2553 "y.tab.c"
    break;

  case 28:
#line 396 "axl_y.yt"
        { (yyval.ab) = (yyvsp[0].ab); }
#line 2559 "y.tab.c"
    break;

  case 29:
#line 398 "axl_y.yt"
        { (yyval.ab) = abNewHide(TPOS((yyvsp[-1].tok)), (yyvsp[0].ab)); }
#line 2565 "y.tab.c"
    break;

  case 32:
#line 409 "axl_y.yt"
        { (yyval.ab) = abNewWhere(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab),(yyvsp[-2].ab)); }
#line 2571 "y.tab.c"
    break;

  case 35:
#line 420 "axl_y.yt"
        { (yyval.ab) = abNewDeclare(APOS((yyvsp[-1].ab)),(yyvsp[-1].ab),(yyvsp[0].ab)); }
#line 2577 "y.tab.c"
    break;

  case 41:
#line 444 "axl_y.yt"
        { (yyval.ab) = abNewAssign(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 2583 "y.tab.c"
    break;

  case 42:
#line 446 "axl_y.yt"
        { (yyval.ab) = abNewDefine(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 2589 "y.tab.c"
    break;

  case 43:
#line 448 "axl_y.yt"
        { (yyval.ab) = abNewMDefine(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 2595 "y.tab.c"
    break;

  case 44:
#line 450 "axl_y.yt"
        { (yyval.ab) = abNewLambda(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),abZip,(yyvsp[0].ab)); }
#line 2601 "y.tab.c"
    break;

  case 45:
#line 452 "axl_y.yt"
        { (yyval.ab) = abNewPLambda(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),abZip,(yyvsp[0].ab)); }
#line 2607 "y.tab.c"
    break;

  case 47:
#line 457 "axl_y.yt"
        { (yyval.ab) = abNewAssign(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 2613 "y.tab.c"
    break;

  case 48:
#line 459 "axl_y.yt"
        { (yyval.ab) = abNewDefine(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 2619 "y.tab.c"
    break;

  case 49:
#line 461 "axl_y.yt"
        { (yyval.ab) = abNewMDefine(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 2625 "y.tab.c"
    break;

  case 50:
#line 463 "axl_y.yt"
        { (yyval.ab) = abNewLambda(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),abZip,(yyvsp[0].ab)); }
#line 2631 "y.tab.c"
    break;

  case 51:
#line 465 "axl_y.yt"
        { (yyval.ab) = abNewPLambda(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),abZip,(yyvsp[0].ab)); }
#line 2637 "y.tab.c"
    break;

  case 53:
#line 470 "axl_y.yt"
        { (yyval.ab) = abNewAssign(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 2643 "y.tab.c"
    break;

  case 54:
#line 472 "axl_y.yt"
        { (yyval.ab) = abNewDefine(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 2649 "y.tab.c"
    break;

  case 55:
#line 474 "axl_y.yt"
        { (yyval.ab) = abNewMDefine(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 2655 "y.tab.c"
    break;

  case 56:
#line 476 "axl_y.yt"
        { (yyval.ab) = abNewLambda(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),abZip,(yyvsp[0].ab)); }
#line 2661 "y.tab.c"
    break;

  case 57:
#line 478 "axl_y.yt"
        { (yyval.ab) = abNewPLambda(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),abZip,(yyvsp[0].ab)); }
#line 2667 "y.tab.c"
    break;

  case 59:
#line 485 "axl_y.yt"
        { (yyval.ab) = abNewAssign(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 2673 "y.tab.c"
    break;

  case 60:
#line 487 "axl_y.yt"
        { (yyval.ab) = abNewDefine(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 2679 "y.tab.c"
    break;

  case 61:
#line 489 "axl_y.yt"
        { (yyval.ab) = abNewMDefine(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 2685 "y.tab.c"
    break;

  case 62:
#line 491 "axl_y.yt"
        { (yyval.ab) = abNewLambda(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),abZip,(yyvsp[0].ab)); }
#line 2691 "y.tab.c"
    break;

  case 63:
#line 493 "axl_y.yt"
        { (yyval.ab) = abNewPLambda(APOS((yyvsp[-2].ab)),(yyvsp[-2].ab),abZip,(yyvsp[0].ab)); }
#line 2697 "y.tab.c"
    break;

  case 64:
#line 499 "axl_y.yt"
        { (yyval.ab) = abNewIf(TPOS((yyvsp[-3].tok)), TEST((yyvsp[-2].ab)),(yyvsp[0].ab),abZip); }
#line 2703 "y.tab.c"
    break;

  case 68:
#line 511 "axl_y.yt"
        { (yyval.ab) = abNewIf(TPOS((yyvsp[-5].tok)), TEST((yyvsp[-4].ab)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 2709 "y.tab.c"
    break;

  case 69:
#line 513 "axl_y.yt"
        { (yyval.ab) = abNewExit(TPOS((yyvsp[-1].tok)), TEST((yyvsp[-2].ab)),(yyvsp[0].ab)); }
#line 2715 "y.tab.c"
    break;

  case 70:
#line 515 "axl_y.yt"
        { (yyval.ab) = abNewRepeatOL(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab),(yyvsp[-2].ablist)); listFree(AbSyn)((yyvsp[-2].ablist)); }
#line 2721 "y.tab.c"
    break;

  case 71:
#line 517 "axl_y.yt"
        { (yyval.ab) = abNewRepeat0(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2727 "y.tab.c"
    break;

  case 72:
#line 519 "axl_y.yt"
        { (void)parseDeprecated((TokenTag)_YY_KW_But, abNewNothing(TPOS((yyvsp[-2].tok)))); (yyval.ab) = abNewTry(TPOS((yyvsp[-4].tok)),(yyvsp[-3].ab),(yyvsp[-1].ab)->abSequence.argv[0], (yyvsp[-1].ab)->abSequence.argv[1],(yyvsp[0].ab)); }
#line 2733 "y.tab.c"
    break;

  case 73:
#line 521 "axl_y.yt"
        { (yyval.ab) = abNewTry(TPOS((yyvsp[-4].tok)),(yyvsp[-3].ab),(yyvsp[-1].ab)->abSequence.argv[0], (yyvsp[-1].ab)->abSequence.argv[1],(yyvsp[0].ab)); }
#line 2739 "y.tab.c"
    break;

  case 74:
#line 523 "axl_y.yt"
        { (yyval.ab) = abNewSelect(TPOS((yyvsp[-3].tok)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 2745 "y.tab.c"
    break;

  case 75:
#line 525 "axl_y.yt"
        { (yyval.ab) = abNewDo(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2751 "y.tab.c"
    break;

  case 76:
#line 527 "axl_y.yt"
        { (yyval.ab) = abNewDelay(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2757 "y.tab.c"
    break;

  case 77:
#line 529 "axl_y.yt"
        { (yyval.ab) = abNewReference(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2763 "y.tab.c"
    break;

  case 78:
#line 531 "axl_y.yt"
        { (yyval.ab) = abNewGenerate(TPOS((yyvsp[-2].tok)),(yyvsp[-1].ab),(yyvsp[0].ab)); }
#line 2769 "y.tab.c"
    break;

  case 79:
#line 533 "axl_y.yt"
        { (yyval.ab) = abNewAssert(TPOS((yyvsp[-1].tok)),TEST((yyvsp[0].ab))); }
#line 2775 "y.tab.c"
    break;

  case 80:
#line 535 "axl_y.yt"
        { (yyval.ab) = abNewIterate(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2781 "y.tab.c"
    break;

  case 81:
#line 537 "axl_y.yt"
        { (yyval.ab) = abNewBreak(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2787 "y.tab.c"
    break;

  case 82:
#line 539 "axl_y.yt"
        { (yyval.ab) = abNewReturn(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2793 "y.tab.c"
    break;

  case 83:
#line 541 "axl_y.yt"
        { (yyval.ab) = abNewYield(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2799 "y.tab.c"
    break;

  case 84:
#line 543 "axl_y.yt"
        { (yyval.ab) = parseDeprecated((TokenTag)_YY_KW_Except, abNewRaise(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab))); }
#line 2805 "y.tab.c"
    break;

  case 85:
#line 545 "axl_y.yt"
        { (yyval.ab) = abNewRaise(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2811 "y.tab.c"
    break;

  case 86:
#line 547 "axl_y.yt"
        { (yyval.ab) = abNewGoto(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2817 "y.tab.c"
    break;

  case 87:
#line 549 "axl_y.yt"
        { (yyval.ab) = abNewNever(TPOS((yyvsp[0].tok))); }
#line 2823 "y.tab.c"
    break;

  case 89:
#line 554 "axl_y.yt"
        { (yyval.ab) = abNewIf(TPOS((yyvsp[-5].tok)), TEST((yyvsp[-4].ab)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 2829 "y.tab.c"
    break;

  case 90:
#line 556 "axl_y.yt"
        { (yyval.ab) = abNewExit(TPOS((yyvsp[-1].tok)), TEST((yyvsp[-2].ab)),(yyvsp[0].ab)); }
#line 2835 "y.tab.c"
    break;

  case 91:
#line 558 "axl_y.yt"
        { (yyval.ab) = abNewRepeatOL(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab),(yyvsp[-2].ablist)); listFree(AbSyn)((yyvsp[-2].ablist)); }
#line 2841 "y.tab.c"
    break;

  case 92:
#line 560 "axl_y.yt"
        { (yyval.ab) = abNewRepeat0(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2847 "y.tab.c"
    break;

  case 93:
#line 562 "axl_y.yt"
        { (void)parseDeprecated((TokenTag)_YY_KW_But, abNewNothing(TPOS((yyvsp[-2].tok)))); (yyval.ab) = abNewTry(TPOS((yyvsp[-4].tok)),(yyvsp[-3].ab),(yyvsp[-1].ab)->abSequence.argv[0], (yyvsp[-1].ab)->abSequence.argv[1],(yyvsp[0].ab)); }
#line 2853 "y.tab.c"
    break;

  case 94:
#line 564 "axl_y.yt"
        { (yyval.ab) = abNewTry(TPOS((yyvsp[-4].tok)),(yyvsp[-3].ab),(yyvsp[-1].ab)->abSequence.argv[0], (yyvsp[-1].ab)->abSequence.argv[1],(yyvsp[0].ab)); }
#line 2859 "y.tab.c"
    break;

  case 95:
#line 566 "axl_y.yt"
        { (yyval.ab) = abNewSelect(TPOS((yyvsp[-3].tok)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 2865 "y.tab.c"
    break;

  case 96:
#line 568 "axl_y.yt"
        { (yyval.ab) = abNewDo(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2871 "y.tab.c"
    break;

  case 97:
#line 570 "axl_y.yt"
        { (yyval.ab) = abNewDelay(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2877 "y.tab.c"
    break;

  case 98:
#line 572 "axl_y.yt"
        { (yyval.ab) = abNewReference(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2883 "y.tab.c"
    break;

  case 99:
#line 574 "axl_y.yt"
        { (yyval.ab) = abNewGenerate(TPOS((yyvsp[-2].tok)),(yyvsp[-1].ab),(yyvsp[0].ab)); }
#line 2889 "y.tab.c"
    break;

  case 100:
#line 576 "axl_y.yt"
        { (yyval.ab) = abNewAssert(TPOS((yyvsp[-1].tok)),TEST((yyvsp[0].ab))); }
#line 2895 "y.tab.c"
    break;

  case 101:
#line 578 "axl_y.yt"
        { (yyval.ab) = abNewIterate(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2901 "y.tab.c"
    break;

  case 102:
#line 580 "axl_y.yt"
        { (yyval.ab) = abNewBreak(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2907 "y.tab.c"
    break;

  case 103:
#line 582 "axl_y.yt"
        { (yyval.ab) = abNewReturn(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2913 "y.tab.c"
    break;

  case 104:
#line 584 "axl_y.yt"
        { (yyval.ab) = abNewYield(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2919 "y.tab.c"
    break;

  case 105:
#line 586 "axl_y.yt"
        { (yyval.ab) = parseDeprecated((TokenTag)_YY_KW_Except, abNewRaise(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab))); }
#line 2925 "y.tab.c"
    break;

  case 106:
#line 588 "axl_y.yt"
        { (yyval.ab) = abNewRaise(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2931 "y.tab.c"
    break;

  case 107:
#line 590 "axl_y.yt"
        { (yyval.ab) = abNewGoto(TPOS((yyvsp[-1].tok)),(yyvsp[0].ab)); }
#line 2937 "y.tab.c"
    break;

  case 108:
#line 592 "axl_y.yt"
        { (yyval.ab) = abNewNever(TPOS((yyvsp[0].tok))); }
#line 2943 "y.tab.c"
    break;

  case 110:
#line 598 "axl_y.yt"
        { (yyval.ab) = (yyvsp[-1].ab); }
#line 2949 "y.tab.c"
    break;

  case 111:
#line 603 "axl_y.yt"
        { (yyval.ab) = abNew(AB_Sequence, APOS((yyvsp[0].ab)), 2, abZip, abZip); }
#line 2955 "y.tab.c"
    break;

  case 112:
#line 605 "axl_y.yt"
        { (yyval.ab) = abNew(AB_Sequence, APOS((yyvsp[-2].ab)), 2, (yyvsp[-2].ab), (yyvsp[0].ab)); }
#line 2961 "y.tab.c"
    break;

  case 114:
#line 615 "axl_y.yt"
        { (void)parseDeprecated((TokenTag)_YY_KW_Always, abNewNothing(TPOS((yyvsp[-1].tok)))); (yyval.ab) = (yyvsp[0].ab); }
#line 2967 "y.tab.c"
    break;

  case 115:
#line 617 "axl_y.yt"
        { (yyval.ab) = (yyvsp[0].ab); }
#line 2973 "y.tab.c"
    break;

  case 117:
#line 622 "axl_y.yt"
        { (void)parseDeprecated((TokenTag)_YY_KW_Always, abNewNothing(TPOS((yyvsp[-1].tok)))); (yyval.ab) = (yyvsp[0].ab); }
#line 2979 "y.tab.c"
    break;

  case 118:
#line 624 "axl_y.yt"
        { (yyval.ab) = (yyvsp[0].ab); }
#line 2985 "y.tab.c"
    break;

  case 121:
#line 632 "axl_y.yt"
        { (yyval.ab) = abNewCollectOL(APOS((yyvsp[-1].ab)),(yyvsp[-1].ab),(yyvsp[0].ablist)); listFree(AbSyn)((yyvsp[0].ablist)); }
#line 2991 "y.tab.c"
    break;

  case 122:
#line 637 "axl_y.yt"
        { (yyval.ablist) = listNReverse(AbSyn)((yyvsp[0].ablist)); }
#line 2997 "y.tab.c"
    break;

  case 123:
#line 642 "axl_y.yt"
        { (yyval.ablist) = listCons(AbSyn)((yyvsp[0].ab), NULL); }
#line 3003 "y.tab.c"
    break;

  case 124:
#line 644 "axl_y.yt"
        { (yyval.ablist) = listCons(AbSyn)((yyvsp[0].ab), (yyvsp[-1].ablist)); }
#line 3009 "y.tab.c"
    break;

  case 125:
#line 649 "axl_y.yt"
        { (yyval.ab) = abNewFor(TPOS((yyvsp[-4].tok)),(yyvsp[-3].ab),(yyvsp[-1].ab),(yyvsp[0].ab)); }
#line 3015 "y.tab.c"
    break;

  case 126:
#line 651 "axl_y.yt"
        { (yyval.ab) = abNewWhile(TPOS((yyvsp[-1].tok)),TEST((yyvsp[0].ab))); }
#line 3021 "y.tab.c"
    break;

  case 128:
#line 657 "axl_y.yt"
        { (yyval.ab) = abNewFree(TPOS((yyvsp[-1].tok)), (yyvsp[0].ab)); }
#line 3027 "y.tab.c"
    break;

  case 129:
#line 659 "axl_y.yt"
        { (yyval.ab) = abNewLocal(TPOS((yyvsp[-1].tok)), (yyvsp[0].ab)); }
#line 3033 "y.tab.c"
    break;

  case 130:
#line 661 "axl_y.yt"
        { (yyval.ab) = abNewFluid(TPOS((yyvsp[-1].tok)), (yyvsp[0].ab)); }
#line 3039 "y.tab.c"
    break;

  case 131:
#line 666 "axl_y.yt"
        { (yyval.ab) = TEST((yyvsp[0].ab)); }
#line 3045 "y.tab.c"
    break;

  case 133:
#line 674 "axl_y.yt"
        { (yyval.ab) = abNewDeclare(APOS((yyvsp[-1].ab)),(yyvsp[-1].ab),(yyvsp[0].ab)); }
#line 3051 "y.tab.c"
    break;

  case 138:
#line 687 "axl_y.yt"
        { (yyval.ab) = abNewAnd(TPOS((yyvsp[-1].tok)), TEST((yyvsp[-2].ab)),TEST((yyvsp[0].ab))); }
#line 3057 "y.tab.c"
    break;

  case 139:
#line 689 "axl_y.yt"
        { (yyval.ab) = abNewOr(TPOS((yyvsp[-1].tok)), TEST((yyvsp[-2].ab)),TEST((yyvsp[0].ab))); }
#line 3063 "y.tab.c"
    break;

  case 140:
#line 691 "axl_y.yt"
        { (yyval.ab) = abNewInfix(APOS((yyvsp[-1].ab)),(yyvsp[-1].ab),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3069 "y.tab.c"
    break;

  case 142:
#line 697 "axl_y.yt"
        { (yyval.ab) = abNewHas(TPOS((yyvsp[-1].tok)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3075 "y.tab.c"
    break;

  case 143:
#line 699 "axl_y.yt"
        { (yyval.ab) = abNewInfix(APOS((yyvsp[-1].ab)),(yyvsp[-1].ab),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3081 "y.tab.c"
    break;

  case 144:
#line 701 "axl_y.yt"
        { (yyval.ab) = abNewPrefix(APOS((yyvsp[-1].ab)),(yyvsp[-1].ab),(yyvsp[0].ab)); }
#line 3087 "y.tab.c"
    break;

  case 146:
#line 707 "axl_y.yt"
        { (yyval.ab) = abNewPostfix(APOS((yyvsp[0].ab)),(yyvsp[0].ab),(yyvsp[-1].ab)); }
#line 3093 "y.tab.c"
    break;

  case 147:
#line 709 "axl_y.yt"
        { (yyval.ab) = abNewInfix(APOS((yyvsp[-1].ab)),(yyvsp[-1].ab),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3099 "y.tab.c"
    break;

  case 149:
#line 715 "axl_y.yt"
        { (yyval.ab) = abNewInfix(APOS((yyvsp[-1].ab)),(yyvsp[-1].ab),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3105 "y.tab.c"
    break;

  case 150:
#line 717 "axl_y.yt"
        { (yyval.ab) = abNewPrefix(APOS((yyvsp[-1].ab)),(yyvsp[-1].ab),(yyvsp[0].ab)); }
#line 3111 "y.tab.c"
    break;

  case 152:
#line 723 "axl_y.yt"
        { (yyval.ab) = abNewInfix(APOS((yyvsp[-1].ab)),(yyvsp[-1].ab),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3117 "y.tab.c"
    break;

  case 154:
#line 729 "axl_y.yt"
        { (yyval.ab) = abNewInfix(APOS((yyvsp[-1].ab)),(yyvsp[-1].ab),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3123 "y.tab.c"
    break;

  case 156:
#line 735 "axl_y.yt"
        { (yyval.ab) = abNewInfix(APOS((yyvsp[-1].ab)),(yyvsp[-1].ab),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3129 "y.tab.c"
    break;

  case 158:
#line 742 "axl_y.yt"
        { (yyval.ab) = abNewCoerceTo(TPOS((yyvsp[-1].tok)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3135 "y.tab.c"
    break;

  case 159:
#line 744 "axl_y.yt"
        { (yyval.ab) = abNewRestrictTo(TPOS((yyvsp[-1].tok)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3141 "y.tab.c"
    break;

  case 160:
#line 746 "axl_y.yt"
        { (yyval.ab) = abNewPretendTo(TPOS((yyvsp[-1].tok)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3147 "y.tab.c"
    break;

  case 162:
#line 751 "axl_y.yt"
        { (yyval.ab) = abNewCoerceTo(TPOS((yyvsp[-1].tok)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3153 "y.tab.c"
    break;

  case 163:
#line 753 "axl_y.yt"
        { (yyval.ab) = abNewRestrictTo(TPOS((yyvsp[-1].tok)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3159 "y.tab.c"
    break;

  case 164:
#line 755 "axl_y.yt"
        { (yyval.ab) = abNewPretendTo(TPOS((yyvsp[-1].tok)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3165 "y.tab.c"
    break;

  case 167:
#line 765 "axl_y.yt"
        { (yyval.ab) = abNewInfix(APOS((yyvsp[-1].ab)),(yyvsp[-1].ab),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3171 "y.tab.c"
    break;

  case 169:
#line 771 "axl_y.yt"
        { (yyval.ab) = abNewQualify(TPOS((yyvsp[-1].tok)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3177 "y.tab.c"
    break;

  case 171:
#line 777 "axl_y.yt"
        { (yyval.ab) = abNewQualify(TPOS((yyvsp[-1].tok)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3183 "y.tab.c"
    break;

  case 173:
#line 783 "axl_y.yt"
        { (yyval.ab) = abNewQualify(TPOS((yyvsp[-1].tok)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3189 "y.tab.c"
    break;

  case 175:
#line 789 "axl_y.yt"
        { (yyval.ab) = abNewWith(TPOS((yyvsp[-1].tok)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3195 "y.tab.c"
    break;

  case 176:
#line 791 "axl_y.yt"
        { (yyval.ab) = abNewAdd(TPOS((yyvsp[-1].tok)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3201 "y.tab.c"
    break;

  case 177:
#line 793 "axl_y.yt"
        { (yyval.ab) = parseDeprecated((TokenTag)_YY_KW_Except, abNewExcept(TPOS((yyvsp[-1].tok)),(yyvsp[-2].ab),(yyvsp[0].ab))); }
#line 3207 "y.tab.c"
    break;

  case 178:
#line 795 "axl_y.yt"
        { (yyval.ab) = abNewExcept(TPOS((yyvsp[-1].tok)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3213 "y.tab.c"
    break;

  case 238:
#line 866 "axl_y.yt"
        { (yyval.ab) = parseMakeJuxtapose((yyvsp[-1].ab),(yyvsp[0].ab)); }
#line 3219 "y.tab.c"
    break;

  case 239:
#line 868 "axl_y.yt"
        { (yyval.ab) = abNewNot(TPOS((yyvsp[-1].tok)),TEST((yyvsp[0].ab))); }
#line 3225 "y.tab.c"
    break;

  case 241:
#line 873 "axl_y.yt"
        { (yyval.ab) = parseMakeJuxtapose((yyvsp[-1].ab),(yyvsp[0].ab)); }
#line 3231 "y.tab.c"
    break;

  case 242:
#line 875 "axl_y.yt"
        { (yyval.ab) = abNewNot(TPOS((yyvsp[-1].tok)),TEST((yyvsp[0].ab))); }
#line 3237 "y.tab.c"
    break;

  case 244:
#line 882 "axl_y.yt"
        { (yyval.ab) = abNewNot(TPOS((yyvsp[-1].tok)),TEST((yyvsp[0].ab))); }
#line 3243 "y.tab.c"
    break;

  case 245:
#line 884 "axl_y.yt"
        { (yyval.ab) = parseMakeJuxtapose((yyvsp[-1].ab),(yyvsp[0].ab)); }
#line 3249 "y.tab.c"
    break;

  case 246:
#line 886 "axl_y.yt"
        { (yyval.ab) = abNewPrefix(TPOS((yyvsp[-1].tok)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3255 "y.tab.c"
    break;

  case 248:
#line 891 "axl_y.yt"
        { (yyval.ab) = abNewNot(TPOS((yyvsp[-1].tok)),TEST((yyvsp[0].ab))); }
#line 3261 "y.tab.c"
    break;

  case 249:
#line 893 "axl_y.yt"
        { (yyval.ab) = parseMakeJuxtapose((yyvsp[-1].ab),(yyvsp[0].ab)); }
#line 3267 "y.tab.c"
    break;

  case 250:
#line 895 "axl_y.yt"
        { (yyval.ab) = abNewPrefix(TPOS((yyvsp[-1].tok)),(yyvsp[-2].ab),(yyvsp[0].ab)); }
#line 3273 "y.tab.c"
    break;

  case 265:
#line 933 "axl_y.yt"
        {
          (yyval.ab) = abNewParen(TPOS((yyvsp[-1].tok)), abNewComma0(TPOS((yyvsp[-1].tok))));
          abSetEnd((yyval.ab), TEND((yyvsp[0].tok)));
        }
#line 3282 "y.tab.c"
    break;

  case 266:
#line 938 "axl_y.yt"
        { (yyval.ab) = abNewParen(TPOS((yyvsp[-2].tok)), (yyvsp[-1].ab)); abSetEnd((yyval.ab), TEND((yyvsp[0].tok))); }
#line 3288 "y.tab.c"
    break;

  case 267:
#line 943 "axl_y.yt"
        {
          (yyval.ab) = abNewMatchfix(TPOS((yyvsp[-1].tok)),
                             abNewId(TPOS((yyvsp[-1].tok)),ssymBracket),
                             abNewComma0(TPOS((yyvsp[-1].tok))));
          abSetEnd((yyval.ab), TEND((yyvsp[0].tok)));
        }
#line 3299 "y.tab.c"
    break;

  case 268:
#line 950 "axl_y.yt"
        {
          (yyval.ab) = abNewMatchfix(TPOS((yyvsp[-2].tok)),
                             abNewId(TPOS((yyvsp[-2].tok)),ssymBracket),
                             (yyvsp[-1].ab));
          abSetEnd((yyval.ab), TEND((yyvsp[0].tok)));
        }
#line 3310 "y.tab.c"
    break;

  case 269:
#line 960 "axl_y.yt"
        {
          (yyval.ab) = abNewMatchfix(TPOS((yyvsp[-1].tok)),
                             abNewId(TPOS((yyvsp[-1].tok)),ssymEnum),
                             abNewComma0(TPOS((yyvsp[-1].tok))));
          abSetEnd((yyval.ab), TEND((yyvsp[0].tok)));
        }
#line 3321 "y.tab.c"
    break;

  case 270:
#line 967 "axl_y.yt"
        {
          (yyval.ab) = abNewMatchfix(TPOS((yyvsp[-2].tok)),
                             abNewId(TPOS((yyvsp[-2].tok)),ssymEnum),
                             (yyvsp[-1].ab));
          abSetEnd((yyval.ab), TEND((yyvsp[0].tok)));
        }
#line 3332 "y.tab.c"
    break;

  case 276:
#line 991 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id,    (yyvsp[0].tok)); }
#line 3338 "y.tab.c"
    break;

  case 277:
#line 993 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id,    (yyvsp[0].tok)); }
#line 3344 "y.tab.c"
    break;

  case 278:
#line 995 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id,    (yyvsp[0].tok)); }
#line 3350 "y.tab.c"
    break;

  case 279:
#line 1001 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_LitInteger, (yyvsp[0].tok)); }
#line 3356 "y.tab.c"
    break;

  case 280:
#line 1003 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_LitFloat,   (yyvsp[0].tok)); }
#line 3362 "y.tab.c"
    break;

  case 281:
#line 1005 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_LitString,  (yyvsp[0].tok)); }
#line 3368 "y.tab.c"
    break;

  case 282:
#line 1012 "axl_y.yt"
        { (yyval.ab) = abZip; }
#line 3374 "y.tab.c"
    break;

  case 283:
#line 1018 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[0].tok)); }
#line 3380 "y.tab.c"
    break;

  case 284:
#line 1022 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[0].tok)); }
#line 3386 "y.tab.c"
    break;

  case 285:
#line 1026 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[0].tok)); }
#line 3392 "y.tab.c"
    break;

  case 286:
#line 1030 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[0].tok)); }
#line 3398 "y.tab.c"
    break;

  case 287:
#line 1034 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[0].tok)); }
#line 3404 "y.tab.c"
    break;

  case 288:
#line 1038 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[0].tok)); }
#line 3410 "y.tab.c"
    break;

  case 289:
#line 1042 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[0].tok)); }
#line 3416 "y.tab.c"
    break;

  case 290:
#line 1046 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[0].tok)); }
#line 3422 "y.tab.c"
    break;

  case 291:
#line 1052 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[0].tok)); }
#line 3428 "y.tab.c"
    break;

  case 292:
#line 1054 "axl_y.yt"
        { (yyval.ab) = abNewQualify(TPOS((yyvsp[-2].tok)), abNewOfToken(AB_Id, (yyvsp[-2].tok)), (yyvsp[0].ab)); }
#line 3434 "y.tab.c"
    break;

  case 293:
#line 1058 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[0].tok)); }
#line 3440 "y.tab.c"
    break;

  case 294:
#line 1060 "axl_y.yt"
        { (yyval.ab) = abNewQualify(TPOS((yyvsp[-2].tok)), abNewOfToken(AB_Id, (yyvsp[-2].tok)), (yyvsp[0].ab)); }
#line 3446 "y.tab.c"
    break;

  case 295:
#line 1064 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[0].tok)); }
#line 3452 "y.tab.c"
    break;

  case 296:
#line 1066 "axl_y.yt"
        { (yyval.ab) = abNewQualify(TPOS((yyvsp[-2].tok)), abNewOfToken(AB_Id, (yyvsp[-2].tok)), (yyvsp[0].ab)); }
#line 3458 "y.tab.c"
    break;

  case 297:
#line 1070 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[0].tok)); }
#line 3464 "y.tab.c"
    break;

  case 298:
#line 1072 "axl_y.yt"
        { (yyval.ab) = abNewQualify(TPOS((yyvsp[-2].tok)), abNewOfToken(AB_Id, (yyvsp[-2].tok)), (yyvsp[0].ab)); }
#line 3470 "y.tab.c"
    break;

  case 299:
#line 1076 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[0].tok)); }
#line 3476 "y.tab.c"
    break;

  case 300:
#line 1078 "axl_y.yt"
        { (yyval.ab) = abNewQualify(TPOS((yyvsp[-2].tok)), abNewOfToken(AB_Id, (yyvsp[-2].tok)), (yyvsp[0].ab)); }
#line 3482 "y.tab.c"
    break;

  case 301:
#line 1082 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[0].tok)); }
#line 3488 "y.tab.c"
    break;

  case 302:
#line 1084 "axl_y.yt"
        { (yyval.ab) = abNewQualify(TPOS((yyvsp[-2].tok)), abNewOfToken(AB_Id, (yyvsp[-2].tok)), (yyvsp[0].ab)); }
#line 3494 "y.tab.c"
    break;

  case 303:
#line 1088 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[0].tok)); }
#line 3500 "y.tab.c"
    break;

  case 304:
#line 1090 "axl_y.yt"
        { (yyval.ab) = abNewQualify(TPOS((yyvsp[-2].tok)), abNewOfToken(AB_Id, (yyvsp[-2].tok)), (yyvsp[0].ab)); }
#line 3506 "y.tab.c"
    break;

  case 305:
#line 1094 "axl_y.yt"
        { (yyval.ab) = abNewOfToken(AB_Id, (yyvsp[0].tok)); }
#line 3512 "y.tab.c"
    break;

  case 306:
#line 1096 "axl_y.yt"
        { (yyval.ab) = abNewQualify(TPOS((yyvsp[-2].tok)), abNewOfToken(AB_Id, (yyvsp[-2].tok)), (yyvsp[0].ab)); }
#line 3518 "y.tab.c"
    break;

  case 323:
#line 1138 "axl_y.yt"
        {
          (yyval.ab) = (yyvsp[-1].ab);
          if((yyvsp[0].ab)) (yyval.ab) = abNewDocumented(APOS((yyvsp[0].ab)),(yyval.ab),(yyvsp[0].ab));
          if((yyvsp[-2].ab)) (yyval.ab) = abNewDocumented(APOS((yyval.ab)),(yyval.ab),(yyvsp[-2].ab));
        }
#line 3528 "y.tab.c"
    break;

  case 324:
#line 1147 "axl_y.yt"
        { (yyval.ab) = abNewDocTextOfList((yyvsp[0].toklist)); listFree(Token)((yyvsp[0].toklist)); }
#line 3534 "y.tab.c"
    break;

  case 325:
#line 1152 "axl_y.yt"
        { (yyval.ab) = abNewDocTextOfList((yyvsp[0].toklist)); listFree(Token)((yyvsp[0].toklist)); }
#line 3540 "y.tab.c"
    break;

  case 326:
#line 1157 "axl_y.yt"
        { (yyval.toklist) = listNil(Token); }
#line 3546 "y.tab.c"
    break;

  case 327:
#line 1159 "axl_y.yt"
        { (yyval.toklist) = listCons(Token)((yyvsp[-1].tok), (yyvsp[0].toklist)); }
#line 3552 "y.tab.c"
    break;

  case 328:
#line 1164 "axl_y.yt"
        { (yyval.toklist) = listNil(Token); }
#line 3558 "y.tab.c"
    break;

  case 329:
#line 1166 "axl_y.yt"
        { (yyval.toklist) = listCons(Token)((yyvsp[-1].tok), (yyvsp[0].toklist)); }
#line 3564 "y.tab.c"
    break;

  case 330:
#line 1174 "axl_y.yt"
        { (yyval.ab) = abOneOrNewOfList(AB_Comma, (yyvsp[0].ablist)); listFree(AbSyn)((yyvsp[0].ablist)); }
#line 3570 "y.tab.c"
    break;

  case 331:
#line 1179 "axl_y.yt"
        { (yyval.ab) = abOneOrNewOfList(AB_Comma, (yyvsp[0].ablist)); listFree(AbSyn)((yyvsp[0].ablist)); }
#line 3576 "y.tab.c"
    break;

  case 332:
#line 1184 "axl_y.yt"
        { (yyval.ab) = abOneOrNewOfList(AB_Comma, (yyvsp[0].ablist)); listFree(AbSyn)((yyvsp[0].ablist)); }
#line 3582 "y.tab.c"
    break;

  case 333:
#line 1189 "axl_y.yt"
        { (yyval.ab) = abOneOrNewOfList(AB_Comma, (yyvsp[0].ablist)); listFree(AbSyn)((yyvsp[0].ablist)); }
#line 3588 "y.tab.c"
    break;

  case 334:
#line 1197 "axl_y.yt"
        { (yyval.ablist) = listCons(AbSyn)((yyvsp[0].ab), listNil(AbSyn) ); }
#line 3594 "y.tab.c"
    break;

  case 335:
#line 1199 "axl_y.yt"
        { (yyval.ablist) = listCons(AbSyn)((yyvsp[0].ab), (yyvsp[-2].ablist)); }
#line 3600 "y.tab.c"
    break;

  case 336:
#line 1203 "axl_y.yt"
        { (yyval.ablist) = listCons(AbSyn)((yyvsp[0].ab), listNil(AbSyn) ); }
#line 3606 "y.tab.c"
    break;

  case 337:
#line 1205 "axl_y.yt"
        { (yyval.ablist) = listCons(AbSyn)((yyvsp[0].ab), (yyvsp[-2].ablist)); }
#line 3612 "y.tab.c"
    break;

  case 338:
#line 1209 "axl_y.yt"
        { (yyval.ablist) = listCons(AbSyn)((yyvsp[0].ab), listNil(AbSyn) ); }
#line 3618 "y.tab.c"
    break;

  case 339:
#line 1211 "axl_y.yt"
        { (yyval.ablist) = listCons(AbSyn)((yyvsp[0].ab), (yyvsp[-2].ablist)); }
#line 3624 "y.tab.c"
    break;

  case 340:
#line 1215 "axl_y.yt"
        { (yyval.ablist) = listCons(AbSyn)((yyvsp[0].ab), listNil(AbSyn) ); }
#line 3630 "y.tab.c"
    break;

  case 341:
#line 1217 "axl_y.yt"
        { (yyval.ablist) = listCons(AbSyn)((yyvsp[0].ab), (yyvsp[-2].ablist)); }
#line 3636 "y.tab.c"
    break;

  case 342:
#line 1225 "axl_y.yt"
        { (yyval.ab) = abOneOrNewOfList(AB_Sequence, (yyvsp[0].ablist)); listFree(AbSyn)((yyvsp[0].ablist)); }
#line 3642 "y.tab.c"
    break;

  case 343:
#line 1231 "axl_y.yt"
        { (yyval.ablist) = listCons(AbSyn)((yyvsp[0].ab), listNil(AbSyn) ); }
#line 3648 "y.tab.c"
    break;

  case 344:
#line 1233 "axl_y.yt"
        { (yyval.ablist) = listCons(AbSyn)((yyvsp[0].ab), (yyvsp[-2].ablist)); }
#line 3654 "y.tab.c"
    break;

  case 345:
#line 1235 "axl_y.yt"
        { (yyval.ablist) = (yyvsp[-1].ablist); }
#line 3660 "y.tab.c"
    break;

  case 346:
#line 1242 "axl_y.yt"
        { (yyval.ab) = abOneOrNewOfList(AB_Sequence,(yyvsp[-1].ablist)); listFree(AbSyn)((yyvsp[-1].ablist)); }
#line 3666 "y.tab.c"
    break;

  case 347:
#line 1248 "axl_y.yt"
        { (yyval.ab) = (yyvsp[-1].ab); abSetPos((yyval.ab), TPOS((yyvsp[-2].tok))); abSetEnd((yyval.ab), TEND((yyvsp[0].tok))); }
#line 3672 "y.tab.c"
    break;

  case 348:
#line 1254 "axl_y.yt"
        { (yyval.ablist) = listCons(AbSyn)((yyvsp[0].ab), listNil(AbSyn)); }
#line 3678 "y.tab.c"
    break;

  case 349:
#line 1256 "axl_y.yt"
        { (yyval.ablist) = listCons(AbSyn)((yyvsp[0].ab), (yyvsp[-2].ablist)); }
#line 3684 "y.tab.c"
    break;

  case 350:
#line 1258 "axl_y.yt"
        { yyerrok; (yyval.ablist) = listCons(AbSyn)((yyvsp[0].ab), listNil(AbSyn)); }
#line 3690 "y.tab.c"
    break;

  case 351:
#line 1264 "axl_y.yt"
        { (yyval.ab) = abOneOrNewOfList(AB_Sequence,(yyvsp[0].ablist)); listFree(AbSyn)((yyvsp[0].ablist)); }
#line 3696 "y.tab.c"
    break;

  case 353:
#line 1271 "axl_y.yt"
        { (yyval.ablist) = listCons(AbSyn)((yyvsp[0].ab), (yyvsp[-1].ablist)); }
#line 3702 "y.tab.c"
    break;

  case 354:
#line 1277 "axl_y.yt"
        { (yyval.ablist) = listNil(AbSyn); }
#line 3708 "y.tab.c"
    break;

  case 355:
#line 1279 "axl_y.yt"
        { (yyval.ablist) = listCons(AbSyn)((yyvsp[0].ab), (yyvsp[-1].ablist)); }
#line 3714 "y.tab.c"
    break;

  case 356:
#line 1285 "axl_y.yt"
        { (yyval.ab) = (yyvsp[-2].ab); if((yyvsp[0].ab)) (yyval.ab) = abNewDocumented(APOS((yyvsp[0].ab)),(yyval.ab),(yyvsp[0].ab)); }
#line 3720 "y.tab.c"
    break;

  case 357:
#line 1287 "axl_y.yt"
        { yyerrok; (yyval.ab) = abNewNothing(TPOS((yyvsp[-1].tok))); }
#line 3726 "y.tab.c"
    break;

  case 358:
#line 1293 "axl_y.yt"
        {
          (yyval.ab) = (yyvsp[-1].ab);
          if((yyvsp[-2].ab)) (yyval.ab) = abNewDocumented(APOS((yyvsp[-2].ab)),(yyval.ab),(yyvsp[-2].ab));
          if((yyvsp[0].ab)) (yyval.ab) = abNewDocumented(APOS((yyval.ab)),(yyval.ab),(yyvsp[0].ab));
        }
#line 3736 "y.tab.c"
    break;


#line 3740 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
#ifdef UNUSED_LABELS
yyerrlab:
#endif /* UNUSED_LABELS */
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 1299 "axl_y.yt"

