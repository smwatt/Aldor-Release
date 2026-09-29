/*****************************************************************************
 *
 * ccode.h: Structures for manipulating C programs.
 *
 * This file is part of Aldor.
 *
 * Aldor is licensed under the Apache License, Version 2.0.
 *
 *
 * See legal/LICENSE in the Aldor distribution for details.
 *
 * Copyright (C) 1990-2026 Stephen M. Watt.
 */

#ifndef _CCODE_H_
#define _CCODE_H_

#include "axlport.h"

#include "srcpos.h"
#include "strops.h"
#include "symbol.h"

/******************************************************************************
 *
 * :: Grammar
 *
 ******************************************************************************
 *
 * This grammar is adapted from K+R 2nd Edition.
 * It shows how the ccoXxxx macros are used to represent C source code.
 *
 *  TranslationUnit
 *  : seq(ExternalDeclaration)          { $$ = ccoUnit($1); }
 *  ;
 *
 *  ExternalDeclaration
 *  : FunctionDefinition
 *  | Declaration
 *  | PreprocessorLine                  {"Not part of K+R grammar";}
 *  ;
 *
 *  FunctionDefinition
 *  : optseq(DeclarationSpecifier)
 *    Declarator optseq(Declaration) CompoundStatement
 *                                      { $$ = ccoFDef($1, A, B, $4); }
 *  ;
 *
 *  Declaration
 *  : seq(DeclarationSpecifier) optlist(InitDeclarator) ';'
 *                                      { $$ = ccoDecl($1,$2); }
 *  ;
 *
 *  DeclarationSpecifier
 *  : StorageClassSpecifier
 *  | TypeSpecifier
 *  | TypeQualifier
 *  ;
 *
 *  StorageClassSpecifier
 *  : 'auto'                            { $$ = ccoAuto(); }
 *  | 'register'                        { $$ = ccoRegister(); }
 *  | 'static'                          { $$ = ccoStatic(); }
 *  | 'extern'                          { $$ = ccoExtern(); }
 *  | 'typedef'                         { $$ = ccoTypedef(); }
 *  ;
 *
 *  TypeQualifier
 *  : 'const'                           { $$ = ccoConst(); }
 *  | 'volatile'                        { $$ = ccoVolatile(); }
 *  ;
 *
 *  TypeSpecifier
 *  : 'void'                            { $$ = ccoVoid(); }
 *  | 'char'                            { $$ = ccoChar(); }
 *  | 'short'                           { $$ = ccoShort(); }
 *  | 'int'                             { $$ = ccoInt(); }
 *  | 'long'                            { $$ = ccoLong(); }
 *  | 'float'                           { $$ = ccoFloat(); }
 *  | 'double'                          { $$ = ccoDouble(); }
 *  | 'signed'                          { $$ = ccoSigned(); }
 *  | 'unsigned'                        { $$ = ccoUnsigned(); }
 *  | 'struct' opt(Name) '{' seq(StructDeclaration) '}'
 *                                      { $$ = ccoStructDef($2,$4); }
 *  | 'union'  opt(Name) '{' seq(StructDeclaration) '}'
 *                                      { $$ = ccoUnionDef($2,$4); }
 *  | 'enum'   opt(Name) '{' list(Enumerator) '}'
 *                                      { $$ = ccoEnumDef($2,$4); }
 *  | 'struct' Name                     { $$ = ccoStructRef($2); }
 *  | 'union'  Name                     { $$ = ccoUnionRef($2); }
 *  | 'enum'   Name                     { $$ = ccoEnumRef($2); }
 *
 *  | TYPEDEF_NAME                      { $$ = ccoTypedefId(ccoIdOf(yylval)); }
 *  ;
 *
 *  InitDeclarator
 *  : Declarator
 *  | Declarator '=' Initializer        { $$ = ccoAsst($1,$3); }
 *  ;
 *
 *  StructDeclaration
 *  : seq(SpecifierQualifier) list(StructDeclarator) ';'
 *                                      { $$ = ccoDecl($1, $2); }
 *  ;
 *
 *  SpecifierQualifier
 *  : TypeSpecifier
 *  | TypeQualifier
 *  ;
 *
 *  StructDeclarator
 *  : Declarator
 *  | Declarator ':' Expression         { $$ = ccoBitField($1, $3); }
 *  ;
 *
 *  Enumerator
 *  : IDENTIFIER
 *  | IDENTIFIER '=' Expression         { $$ = ccoAsst($1, $3); }
 *  ;
 *
 *  Declarator
 *  : DirectDeclarator
 *  | Pointer(DirectDeclarator)
 *  ;
 *
 *  DirectDeclarator
 *  : IDENTIFIER
 *  | '(' Declarator ')'                { $$ = $2; }
 *  | DirectDeclarator '[' opt(Expression)     ']'
 *                                      { $$ = ccoARef($1, $3); }
 *  | DirectDeclarator '(' ParameterTypeList   ')'
 *                                      { $$ = ccoFCall($1, $3); }
 *  | DirectDeclarator '(' optlist(IDENTIFIER) ')'
 *                                      { $$ = ccoFCall($1, $3); }
 *  ;
 *
 *  Pointer(A)
 *  : '*' OptQual(A)                    { $$ = ccoPreStar($2); }
 *  | '*' OptQual(Pointer(A))           { $$ = ccoPreStar($2); }
 *  ;
 *
 *  OptQual(A)
 *  : A
 *  | TypeSpecifier OptQual(A)          { $$ = ccoQual($1, $2); }
 *  ;
 *
 *  ParameterTypeList
 *  : list(ParameterDeclaration)
 *  ;
 *
 *  ParameterDeclaration
 *  : seq(DeclarationSpecifier) Declarator
 *                                      { $$ = ccoParam(extractID($2),$1,$2); }
 *  | seq(DeclarationSpecifier) opt(AbstractDeclarator)
 *                                      { $$ = ccoParam(extractID($2),$1,$2); }
 *  | '...'
 *                                      { $$ = ccoVAParam(); }
 *  ;
 *
 *  Initializer
 *  : AssignmentExpression
 *  | '{' list(Initializer) '}'         { $$ = ccoInit($2); }
 *  | '{' list(Initializer) ',' '}'     { $$ = ccoInit($2); }
 *  ;
 *
 *  TypeName
 *  : SpecifierQualifier opt(AbstractDeclarator)
 *                                      { $$ = ccoQual($1, $2); }
 *  | SpecifierQualifier TypeName       { $$ = ccoQual($1, $2); }
 *  ;
 *
 *  AbstractDeclarator
 *  : Pointer(Nothing)
 *  | Pointer(DirectAbstractDeclarator)
 *  |         DirectAbstractDeclarator
 *  ;
 *
 *  DirectAbstractDeclarator
 *  :    '(' AbstractDeclarator ')'     { $$ = $2; }
 *  |    '[' opt(Expression)        ']' { $$ = ccoARef(0, $2); }
 *  |    '(' opt(ParameterTypeList) ')' { $$ = ccoFCall(0, $2); }
 *  | DirectAbstractDeclarator '[' opt(Expression) ']'
 *                                      { $$ = ccoARef($1, $3); }
 *  | DirectAbstractDeclarator '('  opt(ParameterTypeList)  ')'
 *                                      { $$ = ccoFCall($1, $3); }
 *  ;
 *
 *  Statement
 *  : CompoundStatement
 *  | opt(Expression) ';'               { $$ = ccoStat($1); }
 *  | IDENTIFIER ':' Statement          { $$ = ccoLabel($1, $3); }
 *  | 'case' Expression ':' Statement   { $$ = ccoCase($2, $4); }
 *  | 'default' ':' Statement           { $$ = ccoDefault($3); }
 *  | 'switch' '(' Expression ')' Statement
 *                                      { $$ = ccoSwitch($3, $5); }
 *  | 'if' '(' Expression ')' Statement { $$ = ccoIf($3, $5, 0); }
 *  | 'if' '(' Expression ')' Statement 'else' Statement
 *                                      { $$ = ccoIf($3, $5, $7); }
 *  | 'while' '(' Expression ')' Statement
 *                                      { $$ = ccoWhile($3, $5); }
 *  | 'do' Statement 'while' '(' Expression ')' ';'
 *                                      { $$ = ccoDo($2, $5); }
 *  | 'for' '('opt(Expression)';'opt(Expression)';'opt(Expression)')' Statement
 *                                      { $$ = ccoFor($3, $5, $7, $9); }
 *  | 'goto' IDENTIFIER ';'             { $$ = ccoGoto($2); }
 *  | 'continue' ';'                    { $$ = ccoContinue(); }
 *  | 'break' ';'                       { $$ = ccoBreak(); }
 *  | 'return' opt(Expression) ';'      { $$ = ccoReturn($2); }
 *  ;
 *
 *  CompoundStatement
 *  : '{' optseq(DeclarationOrStatement) '}'
 *                                      { $$ = ccoCompound($2); }
 *  ;
 *
 *  DeclarationOrStatement
 *  : Declaration
 *  | Statement
 *  | Comment                           {"Not part of K+R grammar";}
 *  ;
 *
 *  Expression
 *  : AssignmentExpression
 *  | Expression ',' AssignmentExpression
 *                                     { $$ = ccoComma($1,$3); }
 *  ;
 *
 *  AssignmentExpression:   E;
 *
 *  E
 *  : CastExpression
 *  | E '=' E                           { $$ = ccoAsst       ($1,$3); }
 *  | E '*='  E                         { $$ = ccoStarAsst   ($1,$3); }
 *  | E '/='   E                        { $$ = ccoDivAsst    ($1,$3); }
 *  | E '%='   E                        { $$ = ccoModAsst    ($1,$3); }
 *  | E '+='  E                         { $$ = ccoPlusAsst   ($1,$3); }
 *  | E '-=' E                          { $$ = ccoMinusAsst  ($1,$3); }
 *  | E '<<='   E                       { $$ = ccoUShAsst    ($1,$3); }
 *  | E '>>='   E                       { $$ = ccoDShAsst    ($1,$3); }
 *  | E '&='   E                        { $$ = ccoAndAsst    ($1,$3); }
 *  | E '^='   E                        { $$ = ccoXorAsst    ($1,$3); }
 *  | E '|='    E                       { $$ = ccoOrAsst     ($1,$3); }
 *  | E '?' Expression ':' E            { $$ = ccoQuest      ($1,$3,$5); }
 *  | E '||'  E                         { $$ = ccoLOr        ($1,$3); }
 *  | E '&&' E                          { $$ = ccoLAnd       ($1,$3); }
 *  | E '|'   E                         { $$ = ccoOr         ($1,$3); }
 *  | E '^'  E                          { $$ = ccoXor        ($1,$3); }
 *  | E '&'  E                          { $$ = ccoAnd        ($1,$3); }
 *  | E '==' E                          { $$ = ccoEQ         ($1,$3); }
 *  | E '!=' E                          { $$ = ccoNE         ($1,$3); }
 *  | E '<' E                           { $$ = ccoLT         ($1,$3); }
 *  | E '<=' E                          { $$ = ccoLE         ($1,$3); }
 *  | E '>' E                           { $$ = ccoGT         ($1,$3); }
 *  | E '>=' E                          { $$ = ccoGE         ($1,$3); }
 *  | E '<<' E                          { $$ = ccoUSh        ($1,$3); }
 *  | E '>>' E                          { $$ = ccoDSh        ($1,$3); }
 *  | E '+' E                           { $$ = ccoPlus       ($1,$3); }
 *  | E '-' E                           { $$ = ccoMinus      ($1,$3); }
 *  | E '*' E                           { $$ = ccoStar       ($1,$3); }
 *  | E '/'  E                          { $$ = ccoDiv        ($1,$3); }
 *  | E '%'  E                          { $$ = ccoMod        ($1,$3); }
 *  ;
 *
 *  CastExpression
 *  : UnaryExpression
 *  | '(' TypeName ')' CastExpression   { $$ = ccoCast($2, $4); }
 *  ;
 *
 *  UnaryExpression
 *  : PostfixExpression
 *  | '++' UnaryExpression              { $$ = ccoPreInc  ($2); }
 *  | '--' UnaryExpression              { $$ = ccoPreDec  ($2); }
 *  | '&' CastExpression                { $$ = ccoPreAnd  ($2); }
 *  | '*' CastExpression                { $$ = ccoPreStar ($2); }
 *  | '+' CastExpression                { $$ = ccoPrePlus ($2); }
 *  | '-' CastExpression                { $$ = ccoPreMinus($2); }
 *  | '~' CastExpression                { $$ = ccoNot     ($2); }
 *  | '!' CastExpression                { $$ = ccoLNot    ($2); }
 *  | 'sizeof' UnaryExpression          { $$ = ccoSizeof  ($2); }
 *  | 'sizeof' '(' TypeName ')'         { $$ = ccoSizeof  ($3); }
 *  ;
 *
 *  PostfixExpression
 *  : PrimaryExpression
 *  | PostfixExpression '[' Expression ']'
 *                                      { $$ = ccoARef    ($1,$3); }
 *  | PostfixExpression '(' optlist(AssignmentExpression) ')'
 *                                      { $$ = ccoFCall   ($1,$3); }
 *  | PostfixExpression '.'  IDENTIFIER { $$ = ccoDot     ($1,$3); }
 *  | PostfixExpression '->' IDENTIFIER { $$ = ccoPointsTo($1,$3); }
 *  | PostfixExpression '++'            { $$ = ccoPostInc ($1); }
 *  | PostfixExpression '--'            { $$ = ccoPostDec ($1); }
 *  | PostfixExpression '*'             { $$ = ccoPostStar($1); }
 *  ;
 *
 *  PrimaryExpression
 *  : IDENTIFIER
 *  | STRING
 *  | Constant
 *  | '(' Expression ')'                { $$ = $2; }
 *  ;
 *
 *  Constant
 *  : INTEGER_CONSTANT
 *  | CHARACTER_CONSTANT
 *  | FLOATING_CONSTANT
 *  | ENUMERATION_CONSTANT
 *  ;
 *
 *  Name
 *  : IDENTIFIER
 *  | ENUMERATION_CONSTANT
 *  | TYPEDEF_NAME
 *  ;
 *
 *  PreprocessorLine
 *  : PREPROCESSOR_LINE
 *  ;
 *
 *  Comment
 *  : COMMENT
 *  ;
 *
 *  Nothing         : { $$ = 0; } ;
 *  opt(E)          : Nothing | E ;
 *  optlist(E)      : Nothing | list(E) ;
 *  optseq(E)       : Nothing | seq(E) ;
 *
 *  list(E)         : E | list(E) ',' E  { $$ = ccoMany_MakeFlat($1,$3); };
 *  seq(E)          : E | seq(E)      E  { $$ = ccoMany_MakeFlat($1,$2); };
 */


/******************************************************************************
 *
 * :: CCode Node Tags
 *
 *****************************************************************************/

typedef enum ccodeTag {
    CCO_START,
        CCO_Unit = CCO_START,           /* file          */

        /* Types and declaration words */
        CCO_Auto,                       /* auto           */
        CCO_Register,                   /* register       */
        CCO_Static,                     /* static         */
        CCO_Extern,                     /* extern         */
        CCO_Typedef,                    /* typedef        */

        CCO_Const,                      /* const          */
        CCO_Volatile,                   /* volatile       */

        CCO_Void,                       /* void           */
        CCO_Char,                       /* char           */
        CCO_Short,                      /* short          */
        CCO_Int,                        /* int            */
        CCO_Long,                       /* long           */
        CCO_Float,                      /* float          */
        CCO_Double,                     /* double         */
        CCO_Signed,                     /* signed         */
        CCO_Unsigned,                   /* unsigned       */
        CCO_TypedefId,                  /* id             */

        CCO_StructRef,                  /* struct a       */
        CCO_StructDef,                  /* struct {}      */
        CCO_UnionRef,                   /* union  a       */
        CCO_UnionDef,                   /* union  {}      */
        CCO_EnumRef,                    /* enum   a       */
        CCO_EnumDef,                    /* enum   {}      */

        CCO_Param,                      /* [a] int a      */
        CCO_VAParam,                    /* ...            */

        CCO_Decl,                       /* extern int a   */

        CCO_FDef,                       /* void f() {}    */
        CCO_Type,                       /* int (*)()      */
        CCO_BitField,                   /* a : b          */
        CCO_Init,                       /* {i}            */
        CCO_Qual,                       /* * const **     */

        /* Label, Case and Default must occur within a Compound. */
        CCO_Label,                      /* i:             */
        CCO_Case,                       /* case e:        */
        CCO_Default,                    /* default:       */

        /* Executable statements */
        CCO_Compound,                   /* {a b c ...}    */
        CCO_Stat,                       /* e;             */
        CCO_Goto,                       /* goto id;       */
        CCO_Continue,                   /* continue;      */
        CCO_Break,                      /* break;         */
        CCO_Return,                     /* return e;      */
        CCO_If,                         /* if(b) t else e;*/
        CCO_Switch,                     /* switch(e) s    */
        CCO_While,                      /* while (b) s    */
        CCO_Do,                         /* do s while(e); */
        CCO_For,                        /* for(a;b;c) s   */

        /* Expression formers */
        CCO_Comma,                      /* a, b           */
        CCO_Asst,                       /* a  = b         */
        CCO_StarAsst,                   /* a *= b         */
        CCO_DivAsst,                    /* a /= b         */
        CCO_ModAsst,                    /* a %= b         */
        CCO_PlusAsst,                   /* a += b         */
        CCO_MinusAsst,                  /* a -= b         */
        CCO_UShAsst,                    /* a <<= b        */
        CCO_DShAsst,                    /* a >>= b        */
        CCO_AndAsst,                    /* a &= b         */
        CCO_XorAsst,                    /* a ^= b         */
        CCO_OrAsst,                     /* a |= b         */
        CCO_Quest,                      /* a ? b : c      */
        CCO_LOr,                        /* a || b         */
        CCO_LAnd,                       /* a && b         */
        CCO_Or,                         /* a | b          */
        CCO_Xor,                        /* a ^ b          */
        CCO_And,                        /* a & b          */
        CCO_EQ,                         /* a == b         */
        CCO_NE,                         /* a != b         */
        CCO_LT,                         /* a <  b         */
        CCO_LE,                         /* a <= b         */
        CCO_GT,                         /* a >  b         */
        CCO_GE,                         /* a >= b         */
        CCO_USh,                        /* a << b         */
        CCO_DSh,                        /* a >> b         */
        CCO_Plus,                       /* a + b          */
        CCO_Minus,                      /* a - b          */
        CCO_Star,                       /* a * b          */
        CCO_Div,                        /* a / b          */
        CCO_Mod,                        /* a % b          */
        CCO_Cast,                       /* (a) b          */
        CCO_Sizeof,                     /* sizeof a       */
        CCO_Not,                        /* ~a             */
        CCO_LNot,                       /* !a             */
        CCO_PreAnd,                     /* &a             */
        CCO_PreStar,                    /* *a             */
        CCO_PrePlus,                    /* +a             */
        CCO_PreMinus,                   /* -a             */
        CCO_PreInc,                     /* ++a            */
        CCO_PreDec,                     /* --a            */
        CCO_PostInc,                    /* a++            */
        CCO_PostDec,                    /* a--            */
        CCO_PostStar,                   /* a*             */
        CCO_ARef,                       /* a[b]           */
        CCO_FCall,                      /* a(b)           */
        CCO_Dot,                        /* a . b          */
        CCO_PointsTo,                   /* a->b           */
        CCO_Paren,                      /* (a)            */

        CCO_StringVal,                  /* "abcdefg"      */
        CCO_CharVal,                    /* 'c'            */
        CCO_IntVal,                     /* 8              */
        CCO_FloatVal,                   /* 8.9            */
        CCO_EnumId,                     /* id             */
        CCO_Id,                         /* id             */

        /* Sytlized lexical items */
        CCO_CppLine,                    /* # define xyzzy */
        CCO_Comment,                    /* (* xyzzy *)    */

        /* Meta-nodes */
        CCO_Many,                       /* many args, determined by context */
    CCO_LIMIT
} CCodeTag;


/******************************************************************************
 *
 * :: Constructor Macros
 *
 *****************************************************************************/

#define ccoUnit(m)       ccoNewNodeT(CCO_Unit,1,m)

#define ccoAuto()        ccoNewNodeT(CCO_Auto,0)
#define ccoRegister()    ccoNewNodeT(CCO_Register,0)
#define ccoStatic()      ccoNewNodeT(CCO_Static,0)
#define ccoExtern()      ccoNewNodeT(CCO_Extern,0)
#define ccoTypedef()     ccoNewNodeT(CCO_Typedef,0)

#define ccoConst()       ccoNewNodeT(CCO_Const,0)
#define ccoVolatile()    ccoNewNodeT(CCO_Volatile,0)

#define ccoVoid()        ccoNewNodeT(CCO_Void,0)
#define ccoChar()        ccoNewNodeT(CCO_Char,0)
#define ccoShort()       ccoNewNodeT(CCO_Short,0)
#define ccoInt()         ccoNewNodeT(CCO_Int,0)
#define ccoLong()        ccoNewNodeT(CCO_Long,0)
#define ccoFloat()       ccoNewNodeT(CCO_Float,0)
#define ccoDouble()      ccoNewNodeT(CCO_Double,0)
#define ccoSigned()      ccoNewNodeT(CCO_Signed,0)
#define ccoUnsigned()    ccoNewNodeT(CCO_Unsigned,0)
#define ccoTypedefId(id) ccoNewNodeT(CCO_TypedefId,1,id)

#define ccoStructRef(i)  ccoNewNodeT(CCO_StructRef,1,i)
#define ccoStructDef(i,b)ccoNewNodeT(CCO_StructDef,2,i,b)
#define ccoUnionRef(i)   ccoNewNodeT(CCO_UnionRef,1,i)
#define ccoUnionDef(i,b) ccoNewNodeT(CCO_UnionDef,2,i,b)
#define ccoEnumRef(i)    ccoNewNodeT(CCO_EnumRef,1,i)
#define ccoEnumDef(i,b)  ccoNewNodeT(CCO_EnumDef,2,i,b)

#define ccoParam(i,w,v)  ccoNewNodeT(CCO_Param,3,i,w,v)
#define ccoVAParam()     ccoNewNodeT(CCO_VAParam,0)
#define ccoDecl(w,v)     ccoNewNodeT(CCO_Decl,2,w,v)
#define ccoFDef(s,h,d,b) ccoNewNodeT(CCO_FDef,4,s,h,d,b)
#define ccoType(r,d)     ccoNewNodeT(CCO_Type,2,r,d)
#define ccoBitField(a,b) ccoNewNodeT(CCO_BitField,2,a,b)
#define ccoInit(i)       ccoNewNodeT(CCO_Init,1,i)
#define ccoQual(m,t)     ccoNewNodeT(CCO_Qual,2,m,t)

#define ccoLabel(i,s)    ccoNewNodeT(CCO_Label,2,i,s)
#define ccoCase(e,s)     ccoNewNodeT(CCO_Case,2,e,s)
#define ccoDefault(s)    ccoNewNodeT(CCO_Default,1,s)
#define ccoCompound(a)   ccoNewNodeT(CCO_Compound,1,a)
#define ccoStat(e)       ccoNewNodeT(CCO_Stat,1,e)
#define ccoGoto(i)       ccoNewNodeT(CCO_Goto,1,i)
#define ccoContinue()    ccoNewNodeT(CCO_Continue,0)
#define ccoBreak()       ccoNewNodeT(CCO_Break,0)
#define ccoReturn(e)     ccoNewNodeT(CCO_Return,1,e)
#define ccoIf(b,t,e)     ccoNewNodeT(CCO_If,3,b,t,e)
#define ccoSwitch(e,b)   ccoNewNodeT(CCO_Switch,2,e,b)
#define ccoWhile(b,s)    ccoNewNodeT(CCO_While,2,b,s)
#define ccoDo(s,e)       ccoNewNodeT(CCO_Do,2,s,e)
#define ccoFor(a,b,c,s)  ccoNewNodeT(CCO_For,4,a,b,c,s)

#define ccoComma(a,b)    ccoNewNodeT(CCO_Comma,2,a,b)
#define ccoAsst(a,b)     ccoNewNodeT(CCO_Asst,2,a,b)
#define ccoStarAsst(a,b) ccoNewNodeT(CCO_StarAsst,2,a,b)
#define ccoDivAsst(a,b)  ccoNewNodeT(CCO_DivAsst,2,a,b)
#define ccoModAsst(a,b)  ccoNewNodeT(CCO_ModAsst,2,a,b)
#define ccoPlusAsst(a,b) ccoNewNodeT(CCO_PlusAsst,2,a,b)
#define ccoMinusAsst(a,b)ccoNewNodeT(CCO_MinusAsst,2,a,b)
#define ccoUShAsst(a,b)  ccoNewNodeT(CCO_UShAsst,2,a,b)
#define ccoDShAsst(a,b)  ccoNewNodeT(CCO_DShAsst,2,a,b)
#define ccoAndAsst(a,b)  ccoNewNodeT(CCO_AndAsst,2,a,b)
#define ccoXorAsst(a,b)  ccoNewNodeT(CCO_XorAsst,2,a,b)
#define ccoOrAsst(a,b)   ccoNewNodeT(CCO_OrAsst,2,a,b)
#define ccoQuest(a,b,c)  ccoNewNodeT(CCO_Quest,3,a,b,c)
#define ccoLOr(a,b)      ccoNewNodeT(CCO_LOr,2,a,b)
#define ccoLAnd(a,b)     ccoNewNodeT(CCO_LAnd,2,a,b)
#define ccoOr(a,b)       ccoNewNodeT(CCO_Or,2,a,b)
#define ccoXor(a,b)      ccoNewNodeT(CCO_Xor,2,a,b)
#define ccoAnd(a,b)      ccoNewNodeT(CCO_And,2,a,b)
#define ccoEQ(a,b)       ccoNewNodeT(CCO_EQ,2,a,b)
#define ccoNE(a,b)       ccoNewNodeT(CCO_NE,2,a,b)
#define ccoLT(a,b)       ccoNewNodeT(CCO_LT,2,a,b)
#define ccoLE(a,b)       ccoNewNodeT(CCO_LE,2,a,b)
#define ccoGT(a,b)       ccoNewNodeT(CCO_GT,2,a,b)
#define ccoGE(a,b)       ccoNewNodeT(CCO_GE,2,a,b)
#define ccoUSh(a,b)      ccoNewNodeT(CCO_USh,2,a,b)
#define ccoDSh(a,b)      ccoNewNodeT(CCO_DSh,2,a,b)
#define ccoPlus(a,b)     ccoNewNodeT(CCO_Plus,2,a,b)
#define ccoMinus(a,b)    ccoNewNodeT(CCO_Minus,2,a,b)
#define ccoStar(a,b)     ccoNewNodeT(CCO_Star,2,a,b)
#define ccoDiv(a,b)      ccoNewNodeT(CCO_Div,2,a,b)
#define ccoMod(a,b)      ccoNewNodeT(CCO_Mod,2,a,b)
#define ccoCast(a,b)     ccoNewNodeT(CCO_Cast,2,a,b)
#define ccoSizeof(a)     ccoNewNodeT(CCO_Sizeof,1,a)
#define ccoNot(a)        ccoNewNodeT(CCO_Not,1,a)
#define ccoLNot(a)       ccoNewNodeT(CCO_LNot,1,a)
#define ccoPreAnd(a)     ccoNewNodeT(CCO_PreAnd,1,a)
#define ccoPreStar(a)    ccoNewNodeT(CCO_PreStar,1,a)
#define ccoPrePlus(a)    ccoNewNodeT(CCO_PrePlus,1,a)
#define ccoPreMinus(a)   ccoNewNodeT(CCO_PreMinus,1,a)
#define ccoPreInc(a)     ccoNewNodeT(CCO_PreInc,1,a)
#define ccoPreDec(a)     ccoNewNodeT(CCO_PreDec,1,a)
#define ccoPostInc(a)    ccoNewNodeT(CCO_PostInc,1,a)
#define ccoPostDec(a)    ccoNewNodeT(CCO_PostDec,1,a)
#define ccoPostStar(a)   ccoNewNodeT(CCO_PostStar,1,a)
#define ccoARef(a,b)     ccoNewNodeT(CCO_ARef,2,a,b)
#define ccoFCall(a,b)    ccoNewNodeT(CCO_FCall,2,a,b)
#define ccoDot(a,b)      ccoNewNodeT(CCO_Dot,2,a,b)
#define ccoPointsTo(a,b) ccoNewNodeT(CCO_PointsTo,2,a,b)
#define ccoParen(a)      ccoNewNodeT(CCO_Paren,1,a)

#define ccoStringVal(s)    ccoNewToken(CCO_StringVal,s)
#define ccoCharVal(c)    ccoNewToken(CCO_CharVal,c)
#define ccoIntVal(i)    ccoNewToken(CCO_IntVal,i)
#define ccoFloatVal(f)    ccoNewToken(CCO_FloatVal,f)
#define ccoEnumId(id)   ccoNewToken(CCO_EnumId,id)
#define ccoId(id)   ccoNewToken(CCO_Id,id)

#define ccoCppLine(s,t)  ccoNewNodeT(CCO_CppLine,2,s,t)
#define ccoComment(s)    ccoNewNodeT(CCO_Comment,1,s)

#define ccoMany0()                      ccoNewNodeT(CCO_Many,0)
#define ccoMany1(a)                     ccoNewNodeT(CCO_Many,1,a)
#define ccoMany2(a,b)                   ccoNewNodeT(CCO_Many,2,a,b)
#define ccoMany3(a,b,c)                 ccoNewNodeT(CCO_Many,3,a,b,c)
#define ccoMany4(a,b,c,d)               ccoNewNodeT(CCO_Many,4,a,b,c,d)
#define ccoMany5(a,b,c,d,e)             ccoNewNodeT(CCO_Many,5,a,b,c,d,e)
#define ccoMany6(a,b,c,d,e,f)           ccoNewNodeT(CCO_Many,6,a,b,c,d,e,f)
#define ccoMany7(a,b,c,d,e,f,g)         ccoNewNodeT(CCO_Many,7,a,b,c,d,e,f,g)
#define ccoMany8(a,b,c,d,e,f,g,h)       ccoNewNodeT(CCO_Many,8,a,b,c,d,e,f,g,h)
#define ccoMany9(a,b,c,d,e,f,g,h,i)     ccoNewNodeT(CCO_Many,9,a,b,c,d,e,f,g,h,i)

/******************************************************************************
 *
 * :: Structure Declarations
 *
 *****************************************************************************/

union ccode;

#ifdef __cplusplus
# include <cstddef>
# include <type_traits>

class CCode {
public:
        constexpr CCode() noexcept : rep_(nullptr) {}
        constexpr CCode(std::nullptr_t) noexcept : rep_(nullptr) {}
        static CCode fromPointer(void *p) noexcept { return CCode(p); }
        void *asPointer() const noexcept { return rep_; }

        ccode *operator->() const noexcept { return rep_; }
        bool isNull() const noexcept { return rep_ == nullptr; }
        CCodeTag tag() const noexcept;
        SrcPos &position() const noexcept;
        UShort &argc() const noexcept;
        CCode *argv() const noexcept;
        explicit operator bool() const noexcept { return !isNull(); }

        friend bool operator==(CCode a, CCode b) noexcept
                { return a.rep_ == b.rep_; }
        friend bool operator!=(CCode a, CCode b) noexcept
                { return !(a == b); }
        friend bool operator==(CCode a, std::nullptr_t) noexcept
                { return a.isNull(); }
        friend bool operator!=(CCode a, std::nullptr_t) noexcept
                { return !a.isNull(); }

private:
        explicit CCode(void *p) noexcept
                : rep_(reinterpret_cast<ccode *>(p)) {}
        ccode *rep_;
};

static_assert(sizeof(CCode) == sizeof(void *));
static_assert(alignof(CCode) == alignof(void *));
static_assert(std::is_trivially_copyable_v<CCode>);
static_assert(std::is_standard_layout_v<CCode>);

#else
typedef union ccode *CCode;
#endif

struct ccoHdr {
        BPack(CCodeTag) tag;
        SrcPos          pos;
};

struct ccoNode {
        struct ccoHdr   hdr;
        UShort          argc;
        CCode           argv[NARY];
};

struct ccoToken {
        struct ccoHdr   hdr;
        Symbol          symbol;
};


union ccode {
        struct ccoHdr   ccoHdr;
        struct ccoNode  ccoNode;
        struct ccoToken ccoToken;
};

#ifdef __cplusplus
inline CCodeTag CCode::tag() const noexcept
        { return static_cast<CCodeTag>(rep_->ccoHdr.tag); }
inline SrcPos &CCode::position() const noexcept
        { return rep_->ccoHdr.pos; }
inline UShort &CCode::argc() const noexcept
        { return rep_->ccoNode.argc; }
inline CCode *CCode::argv() const noexcept
        { return rep_->ccoNode.argv; }
#endif

/******************************************************************************
 *
 * :: ccoInfoTable
 *
 *****************************************************************************/

typedef enum {
        CCOK_Keywd,     /* auto, char, ... */
        CCOK_Infix,     /* +,  -,  ->, ... */
        CCOK_Prefix,    /* ++, --, &, ... */
        CCOK_Postfix,   /* ++, -- */
        CCOK_Token,     /* ids, strings, ... */
        CCOK_Misc       /* for, decls, ... */
} CCodeKind;

struct cco_info {
        BPack(CCodeTag)  tag;
        BPack(CCodeKind) kind;
        BPack(int)       precedence;
        BPack(bool)      isLeftToRight;

        String      str;
};

extern struct cco_info ccoInfoTable[];

# define ccoInfo(i)     (ccoInfoTable[i])

/******************************************************************************
 *
 * :: Code types
 *
 *****************************************************************************/

/* Note: StandardC and OldC are mutually exclusive. */
typedef enum {
        CCOM_StandardC = 1 << 0,
        CCOM_OldC      = 1 << 1,
        CCOM_LineNo    = 1 << 2
} CCodeMode;


typedef void (*CCodeSrcFn)(SrcPos, MutString *fname, Length *lno, Length *cno);

/******************************************************************************
 *
 * :: Operations
 *
 *****************************************************************************/

#ifdef __cplusplus
#define         ccoTag(cco)     ((cco).tag())
#define         ccoPos(cco)     ((cco).position())
#define         ccoArgc(cco)    ((cco).argc())
#define         ccoArgv(cco)    ((cco).argv())
#else
#define         ccoTag(cco)     ((CCodeTag) ((cco)->ccoHdr.tag))
#define         ccoPos(cco)     ((cco)->ccoHdr.pos)
#define         ccoArgc(cco)    ((cco)->ccoNode.argc)
#define         ccoArgv(cco)    ((cco)->ccoNode.argv)
#endif

#define         ccoIsToken(cco) (ccoInfo(ccoTag(cco)).kind == CCOK_Token)
#define         ccoIsKeywd(cco) (ccoInfo(ccoTag(cco)).kind == CCOK_Keywd)
#define         ccoIsExpr(cco)  (ccoInfo(ccoTag(cco)).precedence > 0)

extern CCode    ccoNewToken     (CCodeTag, Symbol);
extern CCode    ccoNewNode      (CCodeTag, int argc);

#ifdef __cplusplus
template <typename... Args>
inline CCode
ccoNewNodeT(CCodeTag tag, int argc, Args... args)
{
        static_assert((std::is_same_v<std::decay_t<Args>, CCode> && ...),
                      "CCode node arguments must have type CCode");
        assert(argc == (int) sizeof...(Args));
        CCode cco = ccoNewNode(tag, argc);
        int i = 0;
        ((ccoArgv(cco)[i++] = args), ...);
        return cco;
}
#endif

extern bool     ccoTypeEqual    (CCode, CCode);
extern CCode    ccoCopy         (CCode);
extern void     ccoFree         (CCode);
extern int      ccoPrint        (FILE *, CCode, CCodeMode);
extern int      ccoPrintDb      (CCode);

#define         ccoIdOf(str)    ccoId(Symbol::intern(str))
#define         ccoIntOf(num)   ccoIntVal(Symbol::intern(strPrintf("%ldL",(long)(num))))
#define         ccoFloatOf(buf,flo) \
                                ccoFloatVal(Symbol::intern(DFloatSprint(buf,flo)))
#define         ccoCharOf(chr)  ccoCharVal(Symbol::intern(strPrintf("%c",chr)))
#define         ccoEnumOf(str)  ccoEnumId(Symbol::intern(str))
#define         ccoStringOf(str)ccoStringVal(Symbol::intern(str))

#endif /* !_CCODE_H_ */
