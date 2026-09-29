/*****************************************************************************
 *
 * token.cpp: Lexical elements.
 *
 ****************************************************************************/
# include "axlobs.h"

struct Token::Rep {
        BPack(TokenTag) tag;
        BPack(UByte) extra;
        SrcPos pos, end;
        union { MutString str; Symbol sym; } val;
};
const Token Token::nil{nullptr};

Token Token::fromSymbol(SrcPos p, SrcPos e, TokenTag t, Symbol s) {
        Rep *r=(Rep *)stoAlloc((unsigned)OB_Token,sizeof(*r));
        r->tag=t; r->extra=0; r->pos=p; r->end=e; r->val.sym=s; return Token(r);
}
Token Token::fromString(SrcPos p, SrcPos e, TokenTag t, String s) {
        Rep *r=(Rep *)stoAlloc((unsigned)OB_Token,sizeof(*r)+strlen(s)+1);
        r->tag=t; r->extra=0; r->pos=p; r->end=e; r->val.str=((char*)r)+sizeof(*r); strcpy(r->val.str,s); return Token(r);
}
Token Token::fromKeyword(SrcPos p, SrcPos e, TokenTag t) { return fromSymbol(p,e,t,tokInfo(t).sym); }
Token Token::id(SrcPos p,SrcPos e,Symbol s){return fromSymbol(p,e,TK_Id,s);}
Token Token::blank(SrcPos p,SrcPos e,Symbol s){return fromSymbol(p,e,TK_Blank,s);}
Token Token::integer(SrcPos p,SrcPos e,String s){return fromString(p,e,TK_Int,s);}
Token Token::floating(SrcPos p,SrcPos e,String s){return fromString(p,e,TK_Float,s);}
Token Token::string(SrcPos p,SrcPos e,String s){return fromString(p,e,TK_String,s);}
Token Token::preDoc(SrcPos p,SrcPos e,String s){return fromString(p,e,TK_PreDoc,s);}
Token Token::postDoc(SrcPos p,SrcPos e,String s){return fromString(p,e,TK_PostDoc,s);}
Token Token::comment(SrcPos p,SrcPos e,String s){return fromString(p,e,TK_Comment,s);}
Token Token::sysCmd(SrcPos p,SrcPos e,String s){return fromString(p,e,TK_SysCmd,s);}
Token Token::error(SrcPos p,SrcPos e,String s){return fromString(p,e,TK_Error,s);}
Token Token::keyword(SrcPos p,SrcPos e,TokenTag t){return fromKeyword(p,e,t);}
TokenTag Token::tag() const noexcept{return (TokenTag)rep_->tag;}
UByte Token::extra() const noexcept{return rep_->extra;}
void Token::setExtra(UByte v) noexcept{rep_->extra=v;}
SrcPos Token::position() const noexcept{return rep_->pos;}
SrcPos Token::end() const noexcept{return rep_->end;}
MutString Token::stringValue() const noexcept{return rep_->val.str;}
Symbol Token::symbolValue() const noexcept{return rep_->val.sym;}
bool Token::is(TokenTag t) const noexcept{return tag()==t;}
bool Token::hasString() const noexcept{return tokInfo(tag()).hasString;}
bool Token::isNoncomment() const noexcept{return !tokInfo(tag()).isComment;}
bool Token::isOpener() const noexcept{return tokInfo(tag()).isOpener;}
bool Token::isCloser() const noexcept{return tokInfo(tag()).isCloser;}
bool Token::isFollower() const noexcept{return tokInfo(tag()).isFollower;}
void Token::free(){stoFree((void *)rep_);}
Token Token::copy() const {
 switch(tag()) {
 case TK_Id: case TK_Blank: return fromSymbol(position(),end(),tag(),symbolValue());
 case TK_Int: case TK_Float: case TK_String: case TK_Comment: case TK_PreDoc: case TK_PostDoc: case TK_SysCmd: case TK_Error:
  return fromString(position(),end(),tag(),stringValue());
 default:return fromKeyword(position(),end(),tag());
 }
}
int Token::print(FILE *fout) const {
 if(!*this)return 0; int cc;
 switch(tag()) {
 case TK_Id: case TK_Blank: cc=fprintf(fout,"%s",(symbolValue()).string());break;
 case TK_Int: case TK_Float: cc=fprintf(fout,"%s",stringValue());break;
 case TK_String: cc=fprintf(fout,"\"%s\"",stringValue());break;
 case TK_PreDoc: cc=fprintf(fout,"+++%s",stringValue());break;
 case TK_PostDoc: cc=fprintf(fout,"++%s",stringValue());break;
 case TK_Comment: cc=fprintf(fout,"--%s",stringValue());break;
 case TK_SysCmd: cc=fprintf(fout,"%s\n",stringValue());break;
 case TK_Error: cc=fprintf(fout,"<Error: %s> ",stringValue());break;
 case KW_StartPile: cc=fprintf(fout,"<#pile>\n");break;
 case KW_EndPile: cc=fprintf(fout,"<#endpile>\n");break;
 case KW_NewLine: cc=fprintf(fout,"<NL>\n");break;
 case KW_SetTab: findent+=2; cc=fprintf(fout,"{"); cc+=fnewline(fout);break;
 case KW_BackSet: cc=fprintf(fout,";"); cc+=fnewline(fout);break;
 case KW_BackTab: findent-=2; cc=fnewline(fout); cc+=fprintf(fout,"}");break;
 default:{ String s=keyString(tag()); cc=fprintf(fout,"|%s|",s?s:"??"); break;}
 }
 return cc;
}
int toklistPrint(FILE *fout,List<Token> tl){return listPrint<Token>(fout,tl,[](FILE*f,Token t)->int{return t.print(f);});}

/*
 * Functions for token searching.
 *
 * keyIx is an array taking a character 'ch' as an index and giving
 *   the tag of the first token beginning with 'ch'.
 */

#define KeyNope         (-1)

static short    keyIx[CHAR_MAX+1];

void
keyInit(void)
{
        int     i, ch, lastch;

        for (ch = 0; ch < CHAR_MAX+1; ch++)
                keyIx[ch] = KeyNope;

        lastch = 0;
        for (i = KW_ALPHA_START; i < KW_ALPHA_LIMIT; i++) {
                ch = tokInfo(i).str[0];
                if (ch != lastch) {
                        keyIx[ch] = i;
                        lastch = ch;
                }
        }
        lastch = 0;
        for (i = KW_SYMBOL_START; i < KW_SYMBOL_LIMIT; i++) {
                ch = tokInfo(i).str[0];
                if (ch != lastch) {
                        keyIx[ch] = i;
                        lastch = ch;
                }
        }

        for (i = KW_ALPHA_START; i < KW_ALPHA_LIMIT; i++)
                tokInfo(i).sym = Symbol::internConst(tokInfo(i).str);

        for (i = KW_SYMBOL_START; i < KW_SYMBOL_LIMIT; i++)
                tokInfo(i).sym = Symbol::internConst(tokInfo(i).str);
}

String
keyString(TokenTag no)
{
        return tokInfo(no).str;
}

TokenTag
keyTag(String str)
{
        bool    okay;
        int     i, ch;
        String tokstr;

        /*
         * If we have something, do any tokens start with the
         * same character?
         */
        if (!str || (ch = str[0]) == 0 || keyIx[ch] == KeyNope)
                return TK_LIMIT;


        /*
         * Search the token table starting with the first token that
         * shares the same first character as the target.
         */
        for (i = keyIx[ch]; ; i++) {
                /* Get the name and state of the i'th token */
                tokstr = tokInfo(i).str;
                okay = !(tokInfo(i).isDisabled);


                /* Have we run out of tokens with the same start? */
                if (tokstr[0] != ch)
                        break; /* Yes */


                /*
                 * If the word matches the name of the token then we
                 * check to see if it has been disabled. If not then
                 * we return the ID of the token.
                 */
                if (!strcmp(tokstr, str))
                        return okay ? (TokenTag)i : TK_LIMIT;
        }


        /* Not a token */
        return TK_LIMIT;
}

TokenTag
keyLongest(String str)
{
        bool    okay, matched;
        String tokstr;
        int     i, ch, maxlen, matchlen, matchno;


        /*
         * If we have something, do any tokens start with the
         * same character?
         */
        if (!str || (ch = str[0]) == 0 || keyIx[ch] == KeyNope)
                return TK_LIMIT;


        /* Initially there are no matching tokens */
        maxlen  = 0;
        matchno = TK_LIMIT;


        /*
         * Search the token table starting with the first token that
         * shares the same first character as the target.
         */
        for (i = keyIx[ch]; ; i++) {
                /* Get the name and state of the i'th token */
                tokstr = tokInfo(i).str;
                okay = !(tokInfo(i).isDisabled);


                /* Have we run out of tokens with the same start? */
                if (tokstr[0] != ch)
                        break; /* Yes */


                /* Compute the length of the match */
                matchlen = strMatch(tokInfo(i).str, str);


                /*
                 * Perfect match? Note that if we stored the
                 * length of every token in a table then we
                 * could avoid calls to strlen. Mind you, the
                 * scanner is blindly fast already so we might
                 * not notice any speed difference.
                 */
                matched = (matchlen == strlen(tokInfo(i).str));


                /*
                 * If the word matches the name of the token and it
                 * hasn't been disabled then remember the token if
                 * it is the longest that we've found so far.
                 */
                if (okay && matched && (matchlen > maxlen)) {
                        maxlen = matchlen;
                        matchno= i;
                }
        }

        /* Return the longest token found */
        return (TokenTag) matchno;
}


bool
keyIsDisabled(TokenTag tok)
{
        return tokInfo(tok).isDisabled;
}


void
keySetDisabled(TokenTag tok, bool flag)
{
        tokInfo(tok).isDisabled = flag ? 1 : 0;
}


/*
 * The TokenTag enumeration must use this order.
 *
 * This table MUST be laid out so that all "symbol" tokens starting with
 * the same character are contiguous.
 *
 * Be extra careful with fields F, G, H and K: if you get them wrong then
 * the lineariser will delete end-of-statement tokens when it ought not
 * to. This can lead to odd compiler behaviour that is hard to track down
 * unless you know about this table (see bug 1279).
 */
struct tok_info tokInfoTable[] = {
        /*
         * [A] TokenTag tag
         * [B] Symbol   sym
         * [C] MutString   str
         * [D] Byte     hasString    i.e. use val.str (vs val.sym)
         * [E] Byte     isComment    i.e. ++ --
         * [F] Byte     isOpener     i.e. ( [ { etc
         * [G] Byte     isCloser     i.e. ) ] } etc
         * [H] Byte     isFollower   i.e. then else always in etc
         * [I] Byte     isLangword   i.e. if then etc
         * [J] Byte     isLeftAssoc  i.e., associates left to right
         * [K] Byte     isMaybeInfix i.e., add, with, +
         * [L] Byte     precedence   i.e., 0 is lowest, or don't know
         * [M] Byte     isDisabled   i.e., non-zero means disabled
         *
         * [A]          [B][C]          [D][E] [F] [G] [H] [I] [J] [K] [L] [M]
         */
        {TK_Id,         0,"TK_Id",       0, 0,  0,  0,  0,  0,  1,  0, 170, 0},
        {TK_Blank,      0,"TK_Blank",    0, 0,  0,  0,  0,  0,  1,  0, 170, 0},
        {TK_Int,        0,"TK_Int",      1, 0,  0,  0,  0,  0,  1,  0, 170, 0},
        {TK_Float,      0,"TK_Float",    1, 0,  0,  0,  0,  0,  1,  0, 170, 0},
        {TK_String,     0,"TK_String",   1, 0,  0,  0,  0,  0,  1,  0, 170, 0},
        {TK_PreDoc,     0,"TK_PreDoc",   1, 1,  0,  0,  0,  0,  1,  0, 170, 0},
        {TK_PostDoc,    0,"TK_PostDoc",  1, 1,  0,  0,  0,  0,  1,  0, 170, 0},
        {TK_Comment,    0,"TK_Comment",  1, 1,  0,  0,  0,  0,  1,  0, 170, 0},
        {TK_SysCmd,     0,"TK_SysCmd",   1, 1,  0,  0,  0,  0,  1,  0, 170, 0},
        {TK_Error,      0,"TK_Error",    1, 0,  0,  0,  0,  0,  1,  0,   0, 0},

        {KW_Add,        0,"add",         0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_And,        0,"and",         0, 0,  0,  0,  1,  1,  1,  1,  40, 0},
        {KW_Always,     0,"always",      0, 0,  0,  0,  1,  1,  1,  1,   0, 0},
        {KW_Assert,     0,"assert",      0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Break,      0,"break",       0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_But,        0,"but",         0, 0,  0,  0,  1,  1,  1,  1,   0, 0},
        {KW_By,         0,"by",          0, 0,  0,  0,  0,  0,  1,  1, 110, 0},
        {KW_Case,       0,"case",        0, 0,  0,  0,  0,  0,  1,  1,   0, 0},
        {KW_Catch,      0,"catch",       0, 0,  0,  0,  1,  1,  1,  1,   0, 0},
        {KW_Default,    0,"default",     0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Define,     0,"define",      0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Delay,      0,"delay",       0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Do,         0,"do",          0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Else,       0,"else",        0, 0,  0,  0,  1,  1,  1,  0,   0, 0},
#if AXL_EDIT_1_1_13_06
        {KW_Except,     0,"except",      0, 0,  0,  0,  0,  1,  1,  1,   0, 0},
#else
        {KW_Except,     0,"except",      0, 0,  0,  0,  1,  1,  1,  0,   0, 0},
#endif
        {KW_Export,     0,"export",      0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Extend,     0,"extend",      0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Finally,    0,"finally",     0, 0,  0,  0,  1,  1,  1,  1,   0, 0},
        {KW_Fix,        0,"fix",         0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_For,        0,"for",         0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Fluid,      0,"fluid",       0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Free,       0,"free",        0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_From,       0,"from",        0, 0,  0,  0,  1,  1,  1,  0,   0, 0},
        {KW_Generate,   0,"generate",    0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Goto,       0,"goto",        0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Has,        0,"has",         0, 0,  0,  0,  0,  0,  1,  1,  40, 0},
        {KW_If,         0,"if",          0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Import,     0,"import",      0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_In,         0,"in",          0, 0,  0,  0,  1,  1,  1,  1,   0, 0},
        {KW_Inline,     0,"inline",      0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Is,         0,"is",          0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Isnt,       0,"isnt",        0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Iterate,    0,"iterate",     0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Let,        0,"let",         0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Local,      0,"local",       0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Macro,      0,"macro",       0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Mod,        0,"mod",         0, 0,  0,  0,  0,  0,  1,  1, 125, 0},
        {KW_Never,      0,"never",       0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Not,        0,"not",         0, 0,  0,  0,  0,  1,  1,  1, 150, 0},
        {KW_Of,         0,"of",          0, 0,  0,  0,  1,  1,  1,  1,  40, 0},
        {KW_Or,         0,"or",          0, 0,  0,  0,  1,  1,  1,  1,  40, 0},
        {KW_Pretend,    0,"pretend",     0, 0,  0,  0,  1,  1,  1,  1, 150, 0},
        {KW_Quo,        0,"quo",         0, 0,  0,  0,  0,  0,  1,  1, 125, 0},
        {KW_Reference,  0,"ref",         0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Rem,        0,"rem",         0, 0,  0,  0,  0,  0,  1,  1, 125, 0},
        {KW_Repeat,     0,"repeat",      0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Return,     0,"return",      0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Rule,       0,"rule",        0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Select,     0,"select",      0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Then,       0,"then",        0, 0,  0,  0,  1,  1,  1,  0,   0, 0},
#if AXL_EDIT_1_1_13_06
        {KW_Throw,      0,"throw",       0, 0,  0,  0,  0,  1,  1,  1,   0, 0},
#else
        {KW_Throw,      0,"throw",       0, 0,  0,  0,  1,  1,  1,  0,   0, 0},
#endif
        {KW_To,         0,"to",          0, 0,  0,  0,  1,  1,  1,  0,   0, 0},
        {KW_Try,        0,"try",         0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Where,      0,"where",       0, 0,  0,  0,  1,  1,  1,  0,   0, 0},
        {KW_While,      0,"while",       0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_With,       0,"with",        0, 0,  0,  0,  0,  1,  1,  0,   0, 0},
        {KW_Yield,      0,"yield",       0, 0,  0,  0,  0,  1,  1,  0,   0, 0},

        {KW_Quote,      0,"'",           0, 0,  0,  0,  0,  0,  1,  0,   0, 0},
        {KW_Grave,      0,"`",           0, 0,  0,  0,  0,  0,  1,  0,   0, 0},
        {KW_Ampersand,  0,"&",           0, 0,  0,  0,  0,  0,  1,  0, 160, 0},
        {KW_Comma,      0,",",           0, 0,  0,  0,  1,  0,  1,  1,  11, 0},
        {KW_Semicolon,  0,";",           0, 0,  0,  0,  0,  0,  1,  0,  10, 0},
        {KW_Dollar,     0,"$",           0, 0,  0,  0,  1,  0,  0,  1,  70, 0},
        {KW_Sharp,      0,"#",           0, 0,  0,  0,  0,  0,  1,  0,   0, 0},
        {KW_At,         0,"@",           0, 0,  0,  0,  0,  0,  1,  1, 150, 0},

        {KW_Assign,     0,":=",          0, 0,  0,  0,  1,  0,  0,  1,  20, 0},
        {KW_Colon,      0,":",           0, 0,  0,  0,  1,  0,  1,  1,  36, 0},
        {KW_ColonStar,  0,":*",          0, 0,  0,  0,  1,  0,  1,  1,  36, 0},
        {KW_2Colon,     0,"::",          0, 0,  0,  0,  1,  0,  1,  1, 150, 0},

        {KW_Star,       0,"*",           0, 0,  0,  0,  0,  0,  1,  1, 130, 0},
        {KW_2Star,      0,"**",          0, 0,  0,  0,  0,  0,  1,  1, 140, 0},

        {KW_Dot,        0,".",           0, 0,  0,  0,  1,  0,  1,  1, 170, 0},
        {KW_2Dot,       0,"..",          0, 0,  0,  0,  0,  0,  1,  1, 110, 0},

        {KW_EQ,         0,"=",           0, 0,  0,  0,  0,  0,  1,  1, 100, 0},
        {KW_2EQ,        0,"==",          0, 0,  0,  0,  1,  0,  0,  1,  25, 0},
        {KW_MArrow,     0,"==>",         0, 0,  0,  0,  1,  0,  0,  1,  25, 0},
        {KW_Implies,    0,"=>",          0, 0,  0,  0,  0,  0,  1,  1,  35, 0},

        {KW_GT,         0,">",           0, 0,  0,  0,  0,  0,  1,  1, 100, 0},
        {KW_2GT,        0,">>",          0, 0,  0,  0,  0,  0,  1,  1, 100, 0},
        {KW_GE,         0,">=",          0, 0,  0,  0,  0,  0,  1,  1, 100, 0},

        {KW_LT,         0,"<",           0, 0,  0,  0,  0,  0,  1,  1, 100, 0},
        {KW_2LT,        0,"<<",          0, 0,  0,  0,  0,  0,  1,  1, 100, 0},
        {KW_LE,         0,"<=",          0, 0,  0,  0,  0,  0,  1,  1, 100, 0},
        {KW_LArrow,     0,"<-",          0, 0,  0,  0,  0,  0,  1,  1,   0, 0},

        {KW_Hat,        0,"^",           0, 0,  0,  0,  0,  0,  1,  1, 140, 0},
        {KW_HatE,       0,"^=",          0, 0,  0,  0,  0,  0,  1,  1,  90, 0},

        {KW_Tilde,      0,"~",           0, 0,  0,  0,  0,  0,  0,  1, 150, 0},
        {KW_TildeE,     0,"~=",          0, 0,  0,  0,  0,  0,  1,  1,  90, 0},

        {KW_Plus,       0,"+",           0, 0,  0,  0,  0,  0,  1,  1, 120, 0},
        {KW_PlusMinus,  0,"+-",          0, 0,  0,  0,  0,  0,  1,  1, 120, 0},
        {KW_MapsTo,     0,"+->",         0, 0,  0,  0,  1,  0,  1,  1,  30, 0},
        {KW_MapsToStar, 0,"+->*",        0, 0,  0,  0,  1,  0,  1,  1,  30, 0},

        {KW_Minus,      0,"-",           0, 0,  0,  0,  0,  0,  1,  1, 120, 0},
        {KW_RArrow,     0,"->",          0, 0,  0,  0,  0,  0,  0,  1,  80, 0},
        {KW_MapStar,    0,"->*",         0, 0,  0,  0,  0,  0,  0,  1,  80, 0},

        {KW_Slash,      0,"/",           0, 0,  0,  0,  0,  0,  1,  1, 130, 0},
        {KW_Wedge,      0,"/\\",         0, 0,  0,  0,  0,  0,  1,  1,  40, 0},

        {KW_Backslash,  0,"\\",          0, 0,  0,  0,  0,  0,  1,  1, 130, 0},
        {KW_Vee,        0,"\\/",         0, 0,  0,  0,  0,  0,  1,  1,  40, 0},

        {KW_OBrack,     0,"[",           0, 0,  1,  0,  0,  0,  1,  0,   0, 0},
        {KW_OBBrack,    0,"[|",          0, 0,  1,  0,  0,  0,  1,  0,   0, 0},
        {KW_OCurly,     0,"{",           0, 0,  1,  0,  0,  0,  1,  0,   0, 0},
        {KW_OBCurly,    0,"{|",          0, 0,  1,  0,  0,  0,  1,  0,   0, 0},
        {KW_OParen,     0,"(",           0, 0,  1,  0,  0,  0,  1,  0,   0, 0},
        {KW_OBParen,    0,"(|",          0, 0,  1,  0,  0,  0,  1,  0,   0, 0},

        {KW_CBrack,     0,"]",           0, 0,  0,  1,  0,  0,  1,  0,   0, 0},
        {KW_CCurly,     0,"}",           0, 0,  0,  1,  0,  0,  1,  0,   0, 0},
        {KW_CParen,     0,")",           0, 0,  0,  1,  0,  0,  1,  0,   0, 0},

        {KW_Bar,        0,"|",           0, 0,  0,  0,  0,  0,  1,  0,   0, 0},
        {KW_CBBrack,    0,"|]",          0, 0,  0,  1,  0,  0,  1,  0,   0, 0},
        {KW_CBCurly,    0,"|}",          0, 0,  0,  1,  0,  0,  1,  0,   0, 0},
        {KW_CBParen,    0,"|)",          0, 0,  0,  1,  0,  0,  1,  0,   0, 0},
        {KW_2Bar,       0,"||",          0, 0,  0,  0,  0,  1,  1,  0,   0, 0},

        {KW_NewLine,    0,"\n",          0, 0,  0,  0,  0,  0,  1,  0,   0, 0},
        {KW_StartPile,  0,"#pile",       0, 0,  0,  0,  0,  0,  1,  0,   0, 0},
        {KW_EndPile,    0,"#endpile",    0, 0,  0,  0,  0,  0,  1,  0,   0, 0},
        {KW_SetTab,     0,"KW_SetTab",   0, 0,  0,  0,  0,  0,  1,  0,   0, 0},
        {KW_BackSet,    0,"KW_BackSet",  0, 0,  0,  0,  0,  0,  1,  0,   0, 0},
        {KW_BackTab,    0,"KW_BackTab",  0, 0,  0,  0,  0,  0,  1,  0,   0, 0},
        {KW_Juxtapose,  0,"KW_Juxtapose",0, 0,  0,  0,  0,  0,  0,  0, 170, 0},

        {TK_LIMIT,      0,"TK_LIMIT",    0, 0,  0,  0,  0,  0,  1,  0,   0, 0}
};
