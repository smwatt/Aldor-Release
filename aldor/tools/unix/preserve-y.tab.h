/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
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
    TK_Id = 258,
    TK_String = 259,
    TK_Num = 260,
    TK_Char = 261,
    TK_Comment = 262,
    TK_Space = 263,
    TK_Colon = 264,
    TK_Semicolon = 265,
    TK_Comma = 266,
    TK_VBar = 267,
    TK_OPren = 268,
    TK_CPren = 269,
    TK_OBrace = 270,
    TK_CBrace = 271,
    TK_OAngle = 272,
    TK_CAngle = 273,
    TK_OPct = 274,
    TK_CPct = 275,
    TK_PctPct = 276,
    TK_PctId = 277,
    TK_PctTokenType = 278,
    TK_PctRuleType = 279,
    TK_PctIncludeEnum = 280,
    TK_CTok = 281,
    TK_Other = 282,
    TK_NewLine = 283
  };
#endif
/* Tokens.  */
#define TK_Id 258
#define TK_String 259
#define TK_Num 260
#define TK_Char 261
#define TK_Comment 262
#define TK_Space 263
#define TK_Colon 264
#define TK_Semicolon 265
#define TK_Comma 266
#define TK_VBar 267
#define TK_OPren 268
#define TK_CPren 269
#define TK_OBrace 270
#define TK_CBrace 271
#define TK_OAngle 272
#define TK_CAngle 273
#define TK_OPct 274
#define TK_CPct 275
#define TK_PctPct 276
#define TK_PctId 277
#define TK_PctTokenType 278
#define TK_PctRuleType 279
#define TK_PctIncludeEnum 280
#define TK_CTok 281
#define TK_Other 282
#define TK_NewLine 283

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
#line 6 "zaccgram.y"
union yystype
{
#line 6 "zaccgram.y"

	char	*str;

#line 118 "y.tab.h"

};
#line 6 "zaccgram.y"
typedef union yystype YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
