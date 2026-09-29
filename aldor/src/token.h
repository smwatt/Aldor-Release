/*****************************************************************************
 *
 * token.h: Lexical elements.
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

#ifndef _TOKEN_H_
#define _TOKEN_H_

# include "axlobs.h"

/*
 * The enumeration of Token Tags.
 * These must be in the same order as the tokInfo array.
 */
enum tokenTag {
    TK_START = 1,

        TK_GEN_START = TK_START,
        TK_Id = TK_GEN_START,
        TK_Blank,
        TK_Int,
        TK_Float,
        TK_String,
        TK_PreDoc,
        TK_PostDoc,
        TK_Comment,
        TK_SysCmd,
        TK_Error,

    TK_GEN_LIMIT,
    KW_ALPHA_START = TK_GEN_LIMIT,

        KW_Add = KW_ALPHA_START,
        KW_And,
        KW_Always,
        KW_Assert,
        KW_Break,
        KW_But,
        KW_By,
        KW_Case,
        KW_Catch,
        KW_Default,
        KW_Define,
        KW_Delay,
        KW_Do,
        KW_Else,
        KW_Except,
        KW_Export,
        KW_Extend,
        KW_Finally,
        KW_Fix,
        KW_For,
        KW_Fluid,
        KW_Free,
        KW_From,
        KW_Generate,
        KW_Goto,
        KW_Has,
        KW_If,
        KW_Import,
        KW_In,
        KW_Inline,
        KW_Is,
        KW_Isnt,
        KW_Iterate,
        KW_Let,
        KW_Local,
        KW_Macro,
        KW_Mod,
        KW_Never,
        KW_Not,
        KW_Of,
        KW_Or,
        KW_Pretend,
        KW_Quo,
        KW_Reference,
        KW_Rem,
        KW_Repeat,
        KW_Return,
        KW_Rule,
        KW_Select,
        KW_Then,
        KW_Throw,
        KW_To,
        KW_Try,
        KW_Where,
        KW_While,
        KW_With,
        KW_Yield,

    KW_ALPHA_LIMIT,
    KW_SYMBOL_START = KW_ALPHA_LIMIT,

        KW_Quote = KW_SYMBOL_START,
        KW_Grave,
        KW_Ampersand,
        KW_Comma,
        KW_Semicolon,
        KW_Dollar,
        KW_Sharp,
        KW_At,

        KW_Assign,
        KW_Colon,
        KW_ColonStar,
        KW_2Colon,

        KW_Star,
        KW_2Star,

        KW_Dot,
        KW_2Dot,

        KW_EQ,
        KW_2EQ,
        KW_MArrow,
        KW_Implies,

        KW_GT,
        KW_2GT,
        KW_GE,

        KW_LT,
        KW_2LT,
        KW_LE,
        KW_LArrow,

        KW_Hat,
        KW_HatE,

        KW_Tilde,
        KW_TildeE,

        KW_Plus,
        KW_PlusMinus,
        KW_MapsTo,
        KW_MapsToStar,

        KW_Minus,
        KW_RArrow,
        KW_MapStar,

        KW_Slash,
        KW_Wedge,

        KW_Backslash,
        KW_Vee,

        KW_OBrack,
        KW_OBBrack,
        KW_OCurly,
        KW_OBCurly,
        KW_OParen,
        KW_OBParen,

        KW_CBrack,
        KW_CCurly,
        KW_CParen,

        KW_Bar,
        KW_CBBrack,
        KW_CBCurly,
        KW_CBParen,
        KW_2Bar,

    KW_SYMBOL_LIMIT,
    KW_INTERNAL_START = KW_SYMBOL_LIMIT,

        KW_NewLine = KW_INTERNAL_START,

        KW_StartPile,    /* #pile    */
        KW_EndPile,      /* #endpile */

        KW_SetTab,
        KW_BackSet,
        KW_BackTab,

        KW_Juxtapose,    /* pseudo tag for pretty printing */

    KW_INTERNAL_LIMIT,
    TK_LIMIT = KW_INTERNAL_LIMIT
};

typedef enum tokenTag  TokenTag;


#ifdef __cplusplus
/*
 * Token abstraction.
 */
# include <cstddef>
# include <type_traits>

class Token {
public:
        Token() = default;
        constexpr Token(std::nullptr_t) noexcept : rep_(nullptr) {}

        static Token id       (SrcPos, SrcPos, Symbol);
        static Token blank    (SrcPos, SrcPos, Symbol);
        static Token integer  (SrcPos, SrcPos, String);
        static Token floating (SrcPos, SrcPos, String);
        static Token string   (SrcPos, SrcPos, String);
        static Token preDoc   (SrcPos, SrcPos, String);
        static Token postDoc  (SrcPos, SrcPos, String);
        static Token comment  (SrcPos, SrcPos, String);
        static Token sysCmd   (SrcPos, SrcPos, String);
        static Token error    (SrcPos, SrcPos, String);
        static Token keyword  (SrcPos, SrcPos, TokenTag);
        static Token fromPointer(void * p) noexcept
                { return Token(reinterpret_cast<Rep *>(p)); }
        static const Token nil;

        bool      isNull() const noexcept { return rep_ == nullptr; }
        explicit  operator bool() const noexcept { return !isNull(); }
        TokenTag  tag() const noexcept;
        UByte     extra() const noexcept;
        void      setExtra(UByte) noexcept;
        SrcPos    position() const noexcept;
        SrcPos    end() const noexcept;
        MutString stringValue() const noexcept;
        Symbol    symbolValue() const noexcept;
        bool      is(TokenTag) const noexcept;
        bool      hasString() const noexcept;
        bool      isNoncomment() const noexcept;
        bool      isOpener() const noexcept;
        bool      isCloser() const noexcept;
        bool      isFollower() const noexcept;
        Token     copy() const;
        void      free();
        int       print(FILE *) const;

        friend bool operator==(Token a, Token b) noexcept { return a.rep_ == b.rep_; }
        friend bool operator!=(Token a, Token b) noexcept { return !(a == b); }
        friend bool operator==(Token a, std::nullptr_t) noexcept { return a.isNull(); }
        friend bool operator!=(Token a, std::nullptr_t) noexcept { return !a.isNull(); }
private:
        struct Rep;
        explicit constexpr Token(Rep *rep) noexcept : rep_(rep) {}
        static Token fromSymbol(SrcPos, SrcPos, TokenTag, Symbol);
        static Token fromString(SrcPos, SrcPos, TokenTag, String);
        static Token fromKeyword(SrcPos, SrcPos, TokenTag);
        Rep *rep_;
};
static_assert(sizeof(Token) == sizeof(void *));
static_assert(std::is_trivially_copyable_v<Token>);
extern int toklistPrint(FILE *, List<Token>);

#else
/*
 * Token definition
 */
struct token {
        BPack(TokenTag)         tag;
        BPack(UByte)            extra;
        SrcPos                  pos, end;       /* Begin and end positions. */
        union {
                MutString          str;
                Symbol          sym;
        } val;
};


/*
 * Token Creators and Accessors
 */
# define        tokTag(tok)             ((TokenTag) (tok)->tag)

# define        tokId(pos,end,sym)      tokNew(pos, end, TK_Id,     sym)
# define        tokBlank(pos,end,sym)   tokNew(pos, end, TK_Blank,  sym)
# define        tokInt(pos,end,str)     tokNew(pos, end, TK_Int,    str)
# define        tokFloat(pos,end,str)   tokNew(pos, end, TK_Float,  str)
# define        tokString(pos,end,str)  tokNew(pos, end, TK_String, str)
# define        tokPreDoc(pos,end,str)  tokNew(pos, end, TK_PreDoc, str)
# define        tokPostDoc(pos,end,str) tokNew(pos, end, TK_PostDoc,str)
# define        tokComment(pos,end,str) tokNew(pos, end, TK_Comment,str)
# define        tokSysCmd(pos,end,str)  tokNew(pos, end, TK_SysCmd, str)
# define        tokError(pos,end,msg)   tokNew(pos, end, TK_Error,  msg)
# define        tokKeyword(pos,end,key) tokNew(pos, end, key)

# define        tokIs(tok, ktag)        ((tok)->tag == (ktag))
# define        tokHasString(tok)       (tokInfo((tok)->tag).hasString)

# define        tokIsNoncomment(tok)    (!tokInfo((tok)->tag).isComment)
# define        tokIsOpener(tok)        (tokInfo((tok)->tag).isOpener)
# define        tokIsCloser(tok)        (tokInfo((tok)->tag).isCloser)
# define        tokIsFollower(tok)      (tokInfo((tok)->tag).isFollower)

extern Token    tokNew                  (SrcPos pos,SrcPos end,TokenTag t,...);
extern void     tokFree                 (Token);
extern Token    tokCopy                 (Token);
extern int      tokPrint                (FILE *, Token);
extern int      toklistPrint            (FILE *, TokenList);

#endif

/*
 * Keyword search structures.
 */
extern void     keyInit                 (void);
extern String keyString             (TokenTag t);
extern TokenTag keyTag                  (String);
extern TokenTag keyLongest              (String);
extern bool     keyIsDisabled           (TokenTag);
extern void     keySetDisabled          (TokenTag, bool);

/*
 * Data Structure for the omniscient information array for Tokens.
 */
struct tok_info {
        TokenTag        tag;
        Symbol          sym;
        String     str;
        UByte           hasString;      /* Value union has string (vs symbol) */
        UByte           isComment;
        UByte           isOpener;
        UByte           isCloser;
        UByte           isFollower;
        UByte           isLangword;
        UByte           isLeftAssoc;    /* associates left to right     */
        UByte           isMaybeInfix;   /* +, * are but 'add' might be */
        UByte           precedence;     /* 0 is lowest or don't care    */
        UByte           isDisabled;     /* Non-zero if keyword is disabled */
};

extern struct tok_info  tokInfoTable[];
#define tokInfo(tag)    (tokInfoTable[(tag)-TK_START])

#endif /* !_TOKEN_H_ */
