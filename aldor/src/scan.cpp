///////////////////////////////////////////////////////////////////////////////
//
// scan.cpp: Lexical analyzer.
//
///////////////////////////////////////////////////////////////////////////////

// This file is part of Aldor.
//
// Aldor is licensed under the Apache License, Version 2.0.
//
//
// See legal/LICENSE in the Aldor distribution for details.
//
// Copyright (C) 1990-2026 Stephen M. Watt.

# include "axlphase.h"
# include "foam_c.h"


/******************************************************************************
 *
 * :: External line continuation test
 *
 *****************************************************************************/

/*
 * This function can be used outside the scanner to test whether
 * a line is complete by itself, or whether it needs to be continued.
 *
 * !! As you can see, it is not yet really here.
 */

#define SCAN_IsCont_WasContinued        (1 << 1)
#define SCAN_IsCont_InString            (1 << 2)

bool
scanIsContinued(MutString line)
{
  /* Internal state */
  static        int     unmatchedBraces = 0;
  static  bool  isDefining = false;
  static  bool  topLine = true;
  static  bool  inStringLiteral = false;
  static  bool  sawEscape = false;

  bool  foundSemicolon = false;
  bool  doubleEqualIsLast = false;
  Length        len, i;

  if (!line) return (unmatchedBraces > 0);
  len = strLength(line);

  if (line[0] == '#' && unmatchedBraces == 0) return false;

  if (line[0] == '\n') return true;

  if (line[0] != ' ' && line[0] != '\n' && line[0] != '\t' && line[0] != '\r')
    isDefining = false;

  for (i = 0; i < len; i++)
    if (sawEscape) {
      sawEscape = false;
    }
    else if (inStringLiteral) {
      /* hunt for end of string */
      switch (line[i]) {
      case '_':
        sawEscape = true;
        break;
      case '"':
        if (! sawEscape) inStringLiteral = false;
        break;
      default:
        break;
      }
    }
    else {
      switch (line[i]) {
      case '_':
        sawEscape = true;
        break;
      case '"':
        inStringLiteral = true;
        doubleEqualIsLast = false;
        break;
      case '(':
      case '{':
        unmatchedBraces++;
        break;
      case ')':
      case '}':
        unmatchedBraces--;
        break;
      case ';':
        foundSemicolon = true;
        break;
      case '=':
        if (line[i+1] == '=')
          doubleEqualIsLast = true;
        break;
      case ' ':
        break;
      case '\n':
        break;
      default:
        doubleEqualIsLast = false;
        break;
      }
    }

  /*
   * A prompt is necessary on some platforms to read properly. We
   * ought to check doubleEqualIsLast and topLine to see if we need
   * a prompt but this would confuse the user: they won't type in
   * the leading whitespace needed to indent their code. Since we
   * can recognise such a situation here we ought to do something
   * so that the indentation check made before this function was
   * invoked realises the user is piling.
   */
  if ((unmatchedBraces > 0  ||inStringLiteral) &&
      fintMode == FINT_LOOP &&
      osIsInteractive(osStdin))
    {
      fprintf(osStdout, "...     ");
      fflush(osStdout);
    }

  /* Detect now this kind of syntax error */
  if (unmatchedBraces < 0) {
    unmatchedBraces = 0;
    return false;
  }

  if (topLine && doubleEqualIsLast) isDefining = true;

  if (isDefining) return true;

  if (unmatchedBraces > 0 || inStringLiteral) return true;


  if (foundSemicolon) return false;

  if (isDefining) {
    isDefining = false;
    return false;
  }

  topLine = true;
  return false;
}


/******************************************************************************
 *
 * :: Scanner's state and movement
 *
 *****************************************************************************/

# define ESC_CHAR      '_'              /* Changes meaning of next char. */
# define tokCoError(p,e,m)              Token::error(p, e, comsgString(m))

typedef enum {
        AnyFloat,                       /* Any float can be scanned. */
        NoPreDotFloat,                  /* Only floats without leading dot. */
        NoDotFloat                      /* Only floats without dot. */
} FloatState;

static List<SrcLine>      scSrcLines;     /* Current and remaining src lines. */
static MutString           scLine;         /* Current src text. 0 if at EOF. */
static int              scLineIndex;    /* Index into scLine. */
static int              scLineChar;     /* Position = index if no tabs */
static SrcPos           scLinePos;      /* Source position of line. */
static bool             scIsSysCmd;     /* Is this line a system command? */
static int              scIndentation;  /* Amount line was indented */
static FloatState       scFloatState;   /* What floats can be scanned now? */
static bool             scIsInComment;  /* Middle of a comment? */
static bool             scIsEscaped;    /* Is the peeked chacter escaped? */

# define scEndChar      0
# define scPeekChar()   ((!scLine) ? scEndChar : scLine[scLineIndex])
# define scNextChar()   ((!scLine) ? scEndChar : scLine[scLineIndex+1])
# define scPeekString() (scLine+scLineIndex)

/*
 * Forward declarations.
 */
local void              scStart         (List<SrcLine>);
local void              scEnd           (void);
local void              scStartLine     (void);

local Token             scanToken       (void);
local Token             scanSysCommand  (void);
local Token             scanTokenCases  (void);
local Token             scanWord        (void);
local Token             scanNumber      (void);
local Token             scanString      (void);
local Token             scanDoc         (void);
local Token             scanComment     (void);
local Token             scanSpecial     (void);
local Token             scanError       (void);
local Token             scanNewLine     (void);

local FloatState        floatCanFollow  (Token);


/******************************************************************************
 *
 * :: Top-level of scanner
 *
 *****************************************************************************/

/*
 * "scan" takes a linked list of string lines and returns a linked
 * list of token lines.
 * There is always at least one token on each line, and the first is
 * either a system command or an indentation token.
 */

List<Token>
scan(List<SrcLine> sll)
{
        Token     t;
        List<Token> tl;

        scStart(sll);

        tl = 0;

        while ((t = scanToken()) != 0)
                tl = listCons<Token>(t, tl);

        tl = listNReverse<Token>(tl);

        scEnd();
        return tl;
}


/*
 * Certain lexical contexts restrict the form of floats allowed.
 * E.g.   sin 1.2   vs   m.1.2
 */
local FloatState
floatCanFollow(Token tk)
{
        if (!tk) return AnyFloat;

        switch (tk.tag()) {
        case KW_Dot:
                return NoDotFloat;
        case TK_Id:
        case TK_Int:
        case TK_Float:
        case TK_String:
                return NoPreDotFloat;
        default:
                return tk.isCloser() ? NoPreDotFloat : AnyFloat;
        }
}

/******************************************************************************
 *
 * :: Arbitrary-length token collection
 *
 *****************************************************************************/

#define scTokPos()      (scLinePos).offset(scLineChar-(scIsEscaped?1:0))
#define scTokText()     scTokBuf.chars()

#define scStartTok()    (BUF_START(scTokBuf),   scTokPos())
#define scEndTok()      (BUF_ADD1(scTokBuf, 0), scTokPos())

static Buffer   scTokBuf;
local void
scStart(List<SrcLine> sll)
{
        scSrcLines  = sll;
        scTokBuf    = Buffer::create();
        scStartLine();
}


local void
scEnd(void)
{
        scTokBuf.free();
}

local void
scStartLine(void)
{
        while (scSrcLines
                && car(scSrcLines).isSysCmd()
                && car(scSrcLines).sysCmdHandled())
        {
                scSrcLines = cdr(scSrcLines);
        }
        if (!scSrcLines) {
                scLine = 0;
                DO_DEBUG(fprintf(dbOut, "Scan ended.\n"));
        }
        else {
                SrcLine sl;
                sl = car(scSrcLines);
                scLine        = sl.text();
                scLineIndex   = 0;
                scLineChar    = sl.indentation();

                scLinePos     = sl.position();
                scIsSysCmd    = sl.isSysCmd();
                scIndentation = sl.indentation();
                scFloatState  = AnyFloat;
                scIsInComment = false;
                scIsEscaped   = false;
                DO_DEBUG(fprintf(dbOut, "Scanning line:%s", scLine));
        }
}

# define scAddChar(c) BUF_ADD1(scTokBuf, (c))

# define scAdvance0()   {                                               \
        if (!scLine) {                                                  \
                /* skip */                                              \
        }                                                               \
        else if (!scLine[++scLineIndex]) {                              \
                if (scSrcLines) scSrcLines = cdr(scSrcLines);           \
                scStartLine();                                          \
        }                                                               \
        else {                                                          \
                scIsSysCmd = false;                                     \
                if (scLine[scLineIndex] == '\t') {                      \
                        if ((scLineChar+1) % TABSTOP == 0)              \
                          scLineChar += TABSTOP;                        \
                        else                                            \
                          scLineChar = ROUND_UP(scLineChar+1,TABSTOP)-1;\
                }                                                       \
                else                                                    \
                        ++scLineChar;                                   \
        }                                                               \
}

/* Having this as a macro speeds up the scanner 5-10% but adds 5K. */

/* local void */
# define scAdvance()   {                                                \
        scAdvance0();                                                   \
        if (!scLine || scPeekChar() != ESC_CHAR || scIsInComment)       \
                scIsEscaped = false;                                    \
        else                                                            \
                scAdvance1();                                           \
}

local void
scAdvance1(void)
{
  restart:
        if (!scLine || scPeekChar() != ESC_CHAR || scIsInComment)
                scIsEscaped = false;
        else {
                scAdvance0();
                if (isspace(scPeekChar())) {
                        while (isspace(scPeekChar())) scAdvance0();
                        goto restart;
                }
                scIsEscaped = true;
        }
}

/* local void */
# define scSkipSpace()                                                  \
{                                                                       \
        int     _c;                                                     \
                                                                        \
        while (scLine) {                                                \
                _c = scLine[scLineIndex];                               \
                if (_c!=' ' && _c!='\t' &&_c!='\r')                             \
                        break;                                          \
                scAdvance();                                            \
        }                                                               \
}


/******************************************************************************
 *
 * :: Scanners for different token classes
 *
 *****************************************************************************/

local Token
scanToken(void)
{
        Token tk     = scanTokenCases();
        scFloatState = floatCanFollow(tk);

        DO_DEBUG({
                fprintf(dbOut, "Scanned: ");
                tk.print(dbOut);
                fnewline(dbOut);
        });
        return tk;
}

local Token
scanTokenCases(void)
{
        unsigned char   c, cn;

        if (scIsSysCmd)                 return scanSysCommand();

        scSkipSpace();
        c = scPeekChar();

        if (c == scEndChar)             return 0;
        if (c == '\n')                  return scanNewLine();

        if (isalpha(c) || c == '%' || c == '?' || scIsEscaped)
                                        return scanWord();
        if (isdigit(c))                 return scanNumber();
        if (c == '"')                   return scanString();

        cn = scNextChar();

        if (c == '.' && isdigit(cn) && scFloatState == AnyFloat)
                                        return scanNumber();
        if (c == '-' && cn == '-')      return scanComment();
        if (c == '+' && cn == '+')      return scanDoc();
        if (isprint(c))                 return scanSpecial();

        return scanError();
}


local Token
scanWord(void)
{
        int             c0, c, i;
        bool            escaped, normal;
        SrcPos          spos, epos;
        TokenTag        kno;
        Token           tok;
        String     s;

        spos    = scStartTok();
        escaped = scIsEscaped;

        c0 = scPeekChar();

        for (i = 0; ; i++) {
                c = scPeekChar();
                normal = isalnum(c) || c == '%' || c == '!' || c == '?';
                if (!normal && !scIsEscaped)
                        break;
                if (scIsEscaped && normal && i > 0)
                        comsgWarnPos(scTokPos(), ALDOR_W_FunnyEscape);
                scAddChar(c);
                scAdvance();
        }
        epos = scEndTok();
        s    = scTokText();
        kno  = keyTag(s);

        normal = isalnum(c0) || c0 == '%' || c0 == '!' || c0 == '?';
        if (kno == TK_LIMIT && escaped && normal)
                comsgWarnPos(spos, ALDOR_W_FunnyEscape);

        if (!escaped && c0 == '?')
                tok = Token::blank(spos, epos, Symbol::intern(s));
        else if (!escaped && kno != TK_LIMIT)
                tok = Token::keyword(spos, epos, kno);
        else
                tok = Token::id(spos, epos, Symbol::intern(s));

        return tok;
}

/*
 * Most general form of number:
 *   RRrWW.FFeSXX
 *
 * RR  is the radix part.
 * 'r' is the radix character.
 * WW  is the whole part.
 * '.' is the point character.
 * FF  is the fractional part.
 * 'e' is the exponent character.
 * S   is the sign character.
 * XX  is the exponent part.
 */

#define isRadixChar(c)  ((c) == 'r')
#define isPointChar(c)  ((c) == '.')
#define isExponChar(c)  ((c) == 'e' || (c) == 'E')
#define isSignChar(c)   ((c) == '+' || (c) == '-')



local Token
scanNumber(void)
{
        int     c, nd, rpos = 0;
        bool    hasradix, haspoint, hasexpon;
        SrcPos  spos, epos = SrcPos::none;
        MutString  s;
        ULong   rad;

        c  = scPeekChar();

        assert ( isdigit(c) || isPointChar(c) );

        spos = scStartTok();

        for (nd = 0; isdigit(c = scPeekChar()); nd++) {
                scAddChar(c);
                scAdvance();
        }

        /* Have the first group of numbers. */
        hasradix = isRadixChar(c);

        if (hasradix) {
                /*
                 * Note the position of the first digit of the
                 * whole part of the number: later we need to check
                 * if this is the special identifier 0 or 1.
                 */
                rpos = nd + 1;


                /*
                 * Validate the radix: it must be a decimal integer
                 * between 2 and 36 inclusive. We assume that scTokText
                 * does NOT reset the token buffer. We know that
                 * (nd > 0) otherwise we wouldn't have been called.
                 */
                s = scTokText();
                rad = fiScanSmallIntFrString(s, nd, 10);
                if ((rad < 2) || (rad > 36))
                        return tokCoError(spos, epos, ALDOR_E_ScanBadRadix);


                /* Okay - carry on scanning */
                scAddChar(c);
                scAdvance();

                if (!isdigit(c=scPeekChar()) && !isupper(c) && !isPointChar(c))
                {
                        epos = scEndTok();
                        return tokCoError(spos, epos, ALDOR_E_ScanBadAftRad);
                }

                /* That was the radix part, still need whole part. */
                for (nd = 0; isdigit(c = scPeekChar()) || isupper(c); nd++) {
                        /* Ought to check that digit < rad */
                        scAddChar(c);
                        scAdvance();
                }
        }

        /* Have the whole part, if any. */
        haspoint = isPointChar(c);

        if (haspoint && scFloatState == NoDotFloat) {
                epos = scEndTok();
                s    = scTokText();
                c    = s[rpos];
                if (nd == 0) return tokCoError(spos, epos, ALDOR_E_ScanNoDigits);


                /* 0 and 1 are special identifiers; radix is ignored */
                if (nd == 1)
                {
                        if (c == '0')
                                return Token::id(spos, epos, Symbol::intern("0"));
                        else if (c == '1')
                                return Token::id(spos, epos, Symbol::intern("1"));
                }
                return Token::integer(spos, epos, s);
        }

        /* Do not consume . in 9.. (could merge with previous test) */
        if (haspoint && isPointChar(scNextChar())) {
                epos = scEndTok();
                s    = scTokText();
                c    = s[rpos];
                if (nd == 0) return tokCoError(spos, epos, ALDOR_E_ScanNoDigits);

                /* 0 and 1 are special identifiers; radix is ignored */
                if (nd == 1)
                {
                        if (c == '0')
                                return Token::id(spos, epos, Symbol::intern("0"));
                        else if (c == '1')
                                return Token::id(spos, epos, Symbol::intern("1"));
                }
                return Token::integer(spos, epos, s);
        }

        if (haspoint) {
                scAddChar(c);
                scAdvance();

                for (   ;
                        isdigit(c = scPeekChar()) || (hasradix&&isupper(c));
                        nd++ )
                {
                        scAddChar(c);
                        scAdvance();
                }
        }

        /* Have the fraction part, if any. */
        if (nd == 0) {
                epos = scEndTok();
                return tokCoError(spos, epos, ALDOR_E_ScanNoDigits);
        }

        hasexpon = isExponChar(c);

        if (hasexpon)
        {
                scAddChar(c);
                scAdvance();

                if ( isSignChar(c = scPeekChar()) ) {
                        scAddChar(c);
                        scAdvance();
                }

                if (!isdigit(scPeekChar())) {
                        epos = scEndTok();
                        return tokCoError(spos, epos, ALDOR_E_ScanBadExpon);
                }

                while ( isdigit(c = scPeekChar()) ) {
                        scAddChar(c);
                        scAdvance();
                }
        }

        epos = scEndTok();
        s    = scTokText();
        c    = s[rpos];

        if (haspoint || hasexpon)
                return Token::floating(spos, epos, s);
        else
        {
                /* 0 and 1 are special identifiers; radix is ignored */
                if (nd == 1)
                {
                        if (c == '0')
                                return Token::id(spos, epos, Symbol::intern("0"));
                        else if (c == '1')
                                return Token::id(spos, epos, Symbol::intern("1"));
                }
                return Token::integer(spos, epos, s);
        }
}


local Token
scanString(void)
{
        int     c;
        SrcPos  spos, epos;

        spos = scStartTok();
        scAdvance(); /* " */
        for (;;) {
                c = scPeekChar();
                if (!scIsEscaped && c == '"')
                        break;
                if (!scIsEscaped && (c == '\n' || c == scEndChar)) {
                        epos = scEndTok();
                        return tokCoError(spos, epos, ALDOR_E_ScanOpenString);
                }
                scAddChar(c);
                scAdvance();
        }
        scAdvance(); /* " */
        epos = scEndTok();

        return Token::string(spos, epos, scTokText());
}

local Token
scanSysCommand(void)
{
        SrcPos  spos, epos;
        int     c;
        MutString  s;

        DO_DEBUG(fprintf(dbOut, "Scanning system command.\n"));

        spos = scStartTok();
        while ((c = scPeekChar()) != '\n' && c != scEndChar) {
                scAddChar(c);
                scAdvance();
        }
        epos = scEndTok();
        s    = scTokText();
        scAdvance();    /* Eat the \n */

        return Token::sysCmd(spos, epos, s);
}

local Token
scanComment(void)
{
        int     c;
        SrcPos  spos, epos;

        scIsInComment = true;
        spos = scStartTok();

        scAdvance(); scAdvance(); /* -- */
        while ((c = scPeekChar()) != '\n' && c != scEndChar) {
                scAddChar(c);
                scAdvance();
        }
        scIsInComment = false;
        epos = scEndTok();

        return Token::comment(spos, epos, scTokText());
}


local Token
scanDoc(void)
{
        SrcPos  spos, epos;
        bool    isPreDoc;
        int     c;
        MutString  s;

        scIsInComment = true;
        spos = scStartTok();

        scAdvance(); scAdvance(); /* ++ */

        isPreDoc = scPeekChar() == '+';
        if (isPreDoc) scAdvance();

        while ((c = scPeekChar()) != '\n' && c != scEndChar) {
                scAddChar(c);
                scAdvance();
        }
        scIsInComment = false;
        epos = scEndTok();
        s    = scTokText();

        return isPreDoc ? Token::preDoc(spos, epos, s) : Token::postDoc(spos, epos, s);
}

local Token
scanError(void)
{
        SrcPos  spos, epos;

        spos = scStartTok();
        scAdvance();
        epos = scEndTok();

        return tokCoError(spos, epos, ALDOR_E_ScanBadChar);
}


local Token
scanSpecial(void)
{
        SrcPos          spos, epos;
        TokenTag        kno;
        String     s;
        int             n;

        spos = scStartTok();
        kno  = keyLongest(scPeekString());

        if (kno == TK_LIMIT)   return scanError();
        s = keyString(kno);
        for(n = strlen(s); n > 0; n--)  scAdvance();

        epos = scEndTok();
        return Token::keyword(spos, epos, kno);
}


local Token
scanNewLine(void)
{
        SrcPos  spos, epos;

        spos = scStartTok();
        scAdvance();            /* \n */
        epos = scEndTok();

        return Token::keyword(spos, epos, KW_NewLine);
}
