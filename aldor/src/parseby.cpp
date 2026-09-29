///////////////////////////////////////////////////////////////////////////////
//
// parseby.cpp: Various support functions for the parser.
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

/*
 * The main job is done by axl.y.
 */

# include "axlphase.h"

static List<Token>        yytoklist;      /* The unseen tokens */
static int              yyerrcount;
AbSyn                   yypval;

AbSyn
parse(List<Token> *prest)
{
        extern int      yydebug;
        extern int      yyparse(void);
        AbSyn           absyn;

        yydebug = 0;
        DO_DEBUG(yydebug = 1);

        yyerrcount = 0;
        yytoklist  = *prest;

        yyparse();

        absyn     = yypval;
        *prest    = yytoklist;
        yypval    = 0;
        yytoklist = 0;

        /* 0 is understood by compilerCmd to indicate error. */
        if (yyerrcount)
                return 0;
        else
                return absyn;
}

union YYSTYPE {
        Token           tok;
        List<Token>       toklist;
        AbSyn           ab;
        List<AbSyn>       ablist;
};
extern union YYSTYPE yylval;

int
yylex(void)
{
        do {
                if (!yytoklist)
                        return 0;

                yylval.tok = car(yytoklist);
                yytoklist  = cdr(yytoklist);

        } while (yylval.tok.is(TK_Comment));

        return yylval.tok.tag();
}

local bool
hasAnyRealTokens(List<Token> tl)
{
        for ( ; tl; tl = cdr(tl))
                if (car(tl).tag() != KW_BackTab) return true;
        return false;
}

#define YaccOverflow      "yacc stack overflow"
#define YaccError         "syntax error"
#define YaccErrorNoBackup "syntax error - cannot backup"

int
yyerrorfn(String s)
{
        SrcPos  pos = yylval.tok.position();

        if (strEqual(s, YaccOverflow))
                comsgFatal(abNewNothing(pos), ALDOR_F_SyntaxOverflow, int0);

        else if (!hasAnyRealTokens(yytoklist))
                comsgError(abNewNothing(pos), ALDOR_E_SyntaxNoRecovery);

        else if (strEqual(s, YaccError) || strEqual(s, YaccErrorNoBackup))
        {
                bool    e = yylval.tok.is(TK_Error);
                MutString  msg = yylval.tok.stringValue();
                AbSyn   nowt = abNewNothing(pos);


                /* Give full error message if possible */
                if (e)  comsgError(nowt, ALDOR_E_SyntaxFullError, msg);
                else    comsgError(nowt, ALDOR_E_SyntaxError);
        }
        else
                comsgError(abNewNothing(pos), ALDOR_E_SyntaxErrorHuh, s);

        return ++yyerrcount;
}

AbSyn
parseMakeJuxtapose(AbSyn aa, AbSyn ab)
{
        SrcPos  pa = abPos(aa), pb = abPos(ab);

        if ((pa).line() != (pb).line())
                comsgWarning(ab, ALDOR_W_FunnyJuxta);

        return abNewPrefix(pa, aa, ab);
}


/*
 * Warn about deprecated syntax and then return the AbSyn
 * unchanged. Use a generic message if tok not recognised.
 */
AbSyn
parseDeprecated(TokenTag tok, AbSyn ab)
{
        switch (tok)
        {
           case KW_Always:
                comsgWarning(ab, ALDOR_W_OldSyntaxAlways);
                break;
           case KW_But:
                comsgWarning(ab, ALDOR_W_OldSyntaxCatch);
                break;
           case KW_Except:
                comsgWarning(ab, ALDOR_W_OldSyntaxThrow);
                break;
           default:
                comsgWarning(ab, ALDOR_W_OldSyntaxUnknown);
                break;
        }

        return ab;
}


