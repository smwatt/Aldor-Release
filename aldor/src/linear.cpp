///////////////////////////////////////////////////////////////////////////////
//
// linear.cpp: Bracketing of piles.
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
 * Convert newline and indentation information into bracketing.
 * The NewLine tokens are removed and SetTab, BackSet and BackTabs are added.
 *
 * The original list of tokens is modified.
 *
 * Note that most of the function header comments in this file were added
 * post-facto and may be incorrect.
 */

# include "axlphase.h"

bool    linDebug        = false;
#define linDEBUG(s)     DEBUG_IF(linDebug, s)

/****************************************************************************
 *
 * :: Forward decls
 *
 ****************************************************************************/

local Token     linKeyword              (Token, TokenTag);

local int       linIndentation          (List<Token>);
local List<Token> linUseNeededSep         (List<Token>);
local List<Token> linXTokens              (List<Token>, TokenTag);
local List<Token> linXBlankLines          (List<Token>);
local List<Token> linCheckBalance         (List<Token>);
local List<Token> linISepAfterDontPiles   (List<Token>);
local List<Token> linXSep                 (List<Token>);

# define        linXComments(tl)        linXTokens((tl), TK_Comment)
# define        linXNewLines(tl)        linXTokens((tl), KW_NewLine)
# define        tokIsNonStarter(t)      (t.isFollower() || t.isCloser())

# define        MootIndentation         (-1)

# define        DoPileStart             KW_StartPile    /* #pile    */
# define        DoPileEnd               KW_EndPile      /* #endpile */

# define        DontPileStart           KW_OCurly       /* { */
# define        DontPileSep             KW_Semicolon    /* ; */
# define        DontPileEnd             KW_CCurly       /* } */


/****************************************************************************
 *
 * :: LNodeTree data structure.
 *
 ****************************************************************************/

/*
 * The LNodeTree structure represents in tree form the information
 * about whether to pile and, if so, line boundaries.
 *
 * The input
 *
 * {|
 *     a
 *     b
 *        c
 *        d { e
 *               h } i
 *        l {
 *              n
 *              o {| p
 *                 q
 *                 r |}
 *              t
 *        u }
 *     v
 *     w z
 * |}
 *
 * becomes
 *
 * {| <a> <b> <c> <d { e h } i> <l { n o {| <p> <q> <r> |} t u }> <v> <w z> |}
 *
 * Note that since piling brackets {| and |} have been replaced by #pile
 * and #endpile, the example above is overly complicated and could not be
 * represented in Aldor. Also note that this structure assumes that most
 * programs will be piled whereas now they are normally non-piled. The
 * two forms do not normally appear together in the same source file.
 */


/* Note that LN_1Tok and LN_NTok could be merged */
enum lnodeKind {
        LN_1Tok,        /* a                 */
        LN_NTok,        /* <w z>             -- Has any NewLine token.  */
        LN_NNodes,      /* <d { e h } i>     -- Has any NewLine token.  */
        LN_DoPile       /* {| <p> <q> <r> |} -- Has {| |} tokens.       */
};

typedef enum lnodeKind  LNodeKind;
typedef struct lnode    *LNodeTree;

union lnodeArg {
        Token           tok;
        LNodeTree       lnode;
};

struct lnode {
        LNodeKind       kind;
        ULong           has;
        Length          indent;
        Length          argc;
        union lnodeArg  argv[NARY];
};

#define HAS_NonCom      (1<<0)
#define HAS_NonBlank    (1<<1)

#define linIsBlank(lnt) (!((lnt)->has & HAS_NonBlank))
#define linIsCom(lnt)   (!((lnt)->has & HAS_NonCom))



local LNodeTree lntNewEmpty     (LNodeKind, Length);
local void      lntFree         (LNodeTree);
local void      lntFreeNode     (LNodeTree);

local ULong     lntTokHas       (TokenTag);
local Token     lntFirstTok     (LNodeTree);
local Token     lntLastTok      (LNodeTree);
local Token     lntLastTokLessNL(LNodeTree);

local LNodeTree lntTok          (Token);
local LNodeTree lntConcat       (LNodeTree, LNodeTree);
local LNodeTree lntSeparate     (LNodeTree, TokenTag,  LNodeTree);
local LNodeTree lntWrap         (TokenTag,  LNodeTree, TokenTag);

local List<Token> lntToTokenList  (LNodeTree);
local List<Token> lntToTokenList0 (LNodeTree, List<Token>);
local List<Token> lntConsNL       (List<Token>);
local LNodeTree lntFrTokenList  (List<Token>);

local LNodeTree lin2DRules      (LNodeTree);



local LNodeTree
lntNewEmpty(LNodeKind kind, Length argc)
{
        LNodeTree       lnt;

        lnt = (LNodeTree) stoAlloc(OB_Other,
                                   fullsizeof(struct lnode,
                                              argc, union lnodeArg));
        lnt->kind       = kind;
        lnt->has        = 0;
        lnt->indent     = -1;
        lnt->argc       = argc;
        return lnt;
}

local void
lntFree(LNodeTree lnt)
{
        if (!lnt) return;

        if (lnt->kind != LN_1Tok && lnt->kind != LN_NTok) {
                int     i;
                for (i = 0; i < lnt->argc; i++) lntFree(lnt->argv[i].lnode);
        }
        lntFreeNode(lnt);
}

local void
lntFreeNode(LNodeTree lnt)
{
        stoFree((void *) lnt);
}


local int
lntPrint(FILE *fout, LNodeTree lnt)
{
        int     cc, i, n;
        static int      d = 0;

        if (!lnt) return fprintf(fout, "|0|");

        n = lnt->argc;
        cc = 0;

        d++;
        switch (lnt->kind) {
        case LN_1Tok:
                cc  = fprintf(fout, "(: ");
                cc += (lnt->argv[0].tok).print(fout);
                cc += fprintf(fout, " :)");
                break;
        case LN_NTok:
                cc = fprintf(fout, "<: ");
                for (i = 0; i < n; i++) {
                        cc += fprintf(fout, " ");
                        cc += (lnt->argv[i].tok).print(fout);
                }
                cc += fprintf(fout, " :>");
                break;
        case LN_NNodes:
                cc = fprintf(fout, "< %d: ", d);
                for (i = 0; i < n; i++)
                        cc += lntPrint(fout, lnt->argv[i].lnode);
                cc += fprintf(fout, " :%d >", d);
                break;
        case LN_DoPile:
                cc = fprintf(fout, "{ %d: ", d);
                for (i = 0; i < n; i++)
                        cc += lntPrint(fout, lnt->argv[i].lnode);
                cc += fprintf(fout, " :%d }", d);
                break;
        }
        d--;
        cc += fprintf(fout, "%lx", lnt->has);

        return cc;
}

/*****************************************************************************
 *
 * :: LNodeTree utilities
 *
 ****************************************************************************/

local LNodeTree
lntTok(Token tok)
{
        LNodeTree lnt   = lntNewEmpty(LN_1Tok, 1);
        lnt->argv[0].tok= tok;
        lnt->has        = lntTokHas(tok.tag());
        lnt->indent     = tok.position().character();
        return lnt;
}

local LNodeTree
lntConcat(LNodeTree lnt, LNodeTree rnt)
{
        LNodeTree tnt;

        if (! lnt)
                tnt = rnt;
        else if (! rnt)
                tnt = lnt;
        else {
                tnt = lntNewEmpty(LN_NNodes, 2);
                tnt->indent        = lnt->indent;
                tnt->has           = lnt->has | rnt->has;
                tnt->argv[0].lnode = lnt;
                tnt->argv[1].lnode = rnt;
        }
        return tnt;
}

local LNodeTree
lntSeparate(LNodeTree lnt, TokenTag sep, LNodeTree rnt)
{
        LNodeTree tnt;
        Token     tok  = lntLastTok(lnt);

        tnt = lntNewEmpty(LN_NNodes, 3);
        tnt->indent        = lnt->indent;
        tnt->has           = lnt->has | lntTokHas(sep) | rnt->has;
        tnt->argv[0].lnode = lnt;
        tnt->argv[1].lnode = lntTok(linKeyword(tok, sep));
        tnt->argv[2].lnode = rnt;

        return tnt;
}

local LNodeTree
lntWrap(TokenTag open, LNodeTree lnt, TokenTag close)
{
        LNodeTree tnt;

        Token     otok  = lntFirstTok(lnt);
        Token     ctok  = lntLastTok(lnt);

        tnt                = lntNewEmpty(LN_NNodes, 3);
        tnt->indent        = lnt->indent;
        tnt->has           = lntTokHas(open) | lnt->has | lntTokHas(close);
        tnt->argv[0].lnode = lntTok(linKeyword(otok, open));
        tnt->argv[1].lnode = lnt;
        tnt->argv[2].lnode = lntTok(linKeyword(ctok, close));

        return tnt;
}

local ULong
lntTokHas(TokenTag tt)
{
        switch (tt) {
        case KW_NewLine: case TK_Comment: return 0;
        case TK_PostDoc: case TK_PreDoc:  return HAS_NonBlank;
        default:                          return HAS_NonBlank | HAS_NonCom;
        }
}

local Token
lntFirstTok(LNodeTree lnt)
{
        if (!lnt) return 0;

        for (;;) {
                if (lnt->argc == 0) return 0;

                switch (lnt->kind) {
                case LN_1Tok:
                case LN_NTok:
                        return lnt->argv[0].tok;
                case LN_NNodes:
                case LN_DoPile:
                        lnt = lnt->argv[0].lnode;
                }
        }
}

local Token
lntLastTok(LNodeTree lnt)
{
        if (!lnt) return 0;

        for (;;) {
                int n = lnt->argc;

                if (n == 0) return 0;

                switch (lnt->kind) {
                case LN_1Tok:
                        return lnt->argv[0].tok;
                case LN_NTok:
                        return lnt->argv[n-1].tok;
                case LN_NNodes:
                case LN_DoPile:
                        lnt = lnt->argv[n-1].lnode;
                }
        }
}

local Token
lntLastTokLessNL(LNodeTree lnt)
{
        int     n;
        Token   tok;

        if (!lnt) return 0;

        switch (lnt->kind) {
        case LN_1Tok:
        case LN_NTok:
                for (n = lnt->argc; n > 0; n--) {
                        tok = lnt->argv[n-1].tok;
                        if (!tok.is(KW_NewLine)) return tok;
                }
                return 0;
        case LN_NNodes:
        case LN_DoPile:
                for (n = lnt->argc; n > 0; n--) {
                        tok = lntLastTokLessNL(lnt->argv[n-1].lnode);
                        if (tok && !tok.is(KW_NewLine)) return tok;
                }
                return 0;
        default:
                NotReached(return 0);
        }
}

/*****************************************************************************
 *
 * :: Convert  LNodeTree -> List<Token>
 *
 ****************************************************************************/

local List<Token>
lntToTokenList(LNodeTree lnt)
{
        return listNReverse<Token>(lntToTokenList0(lnt, NULL));
}

local List<Token>
lntToTokenList0(LNodeTree lnt, List<Token> rsofar)
{
        int     i, n;

        if (!lnt) return rsofar;

        n = lnt->argc;

        switch (lnt->kind) {
        case LN_1Tok:
                rsofar = listCons<Token>(lnt->argv[0].tok, rsofar);
                break;
        case LN_NTok:
                for (i = 0; i < n; i++)
                        rsofar = listCons<Token>(lnt->argv[i].tok, rsofar);
                break;
        case LN_NNodes:
                for (i = 0; i < n; i++)
                        rsofar = lntToTokenList0(lnt->argv[i].lnode, rsofar);
                break;
        case LN_DoPile:
                /* Insert newlines between each pair of args. */
                rsofar = lntToTokenList0(lnt->argv[0].lnode, rsofar);
                rsofar = lntConsNL(rsofar);
                for (i = 1; i < n-1; i++)  {
                        rsofar = lntToTokenList0(lnt->argv[i].lnode, rsofar);
                        rsofar = lntConsNL(rsofar);
                }
                rsofar = lntToTokenList0(lnt->argv[n-1].lnode, rsofar);
                break;
        }

        return rsofar;
}

local List<Token>
lntConsNL(List<Token> tl)
{
        Token   nl = linKeyword(tl ? car(tl) : 0, KW_NewLine);

        return listCons<Token>(nl, tl);
}

/*****************************************************************************
 *
 * :: Convert  List<Token> -> LNodeTree
 *
 ****************************************************************************/

/*
 * Use a recursive descent parser to convert from lists to trees.
 */
local LNodeTree lntFrTL_DoPile          (List<Token> *);
local LNodeTree lntFrTL_DoLine          (List<Token> *);
local LNodeTree lntFrTL_DontPile        (List<Token> *);
local LNodeTree lntFrTL_DontLine        (List<Token> *, bool isStacking);
local LNodeTree lntFrTL_1Tok            (List<Token> *);
local LNodeTree lntFrTL_MakeLine        (List<LNodeTree>,int,Length,Length);

local int depthDoPileNo;
local int depthDontPileNo;

/* Top-level entry-point for converting token lists into lnode trees */
local LNodeTree
lntFrTokenList(List<Token> tl)
{
        depthDoPileNo = 0;
        depthDontPileNo = 0;

        return lntFrTL_DontLine(&tl, false);
}

/*
 * Convert a list of tokens for a piled section of code into a tree. If
 * the end of the stream is reached without finding a closing #endpile
 * then one is automatically added. The first child of the result tree
 * is the #pile token and the last is a #endpile token. The other nodes
 * correspond to complete lines from source code or embedded piled or
 * non-piled sections blocks.
 */
local LNodeTree
lntFrTL_DoPile(List<Token> *ptl)
{
        List<Token>       tl0 = *ptl;
        List<LNodeTree>   ll, l;
        LNodeTree       lnt;
        Token           tk;
        int             i, n, in0;
        ULong           has;

        assert(tl0);
        assert(car(tl0).tag() == DoPileStart);

        depthDoPileNo += 1;
        in0 = linIndentation(tl0);
        ll  = listCons<LNodeTree>(lntFrTL_1Tok(&tl0), NULL);
        n   = 1;

        while (tl0 && car(tl0).tag() != DoPileEnd) {
                lnt = lntFrTL_DoLine(&tl0);
                ll  = listCons<LNodeTree>(lnt, ll);
                n++;
        }
        if (tl0 && car(tl0).tag() == DoPileEnd) {
                ll = listCons<LNodeTree>(lntFrTL_1Tok(&tl0), ll);
                n++;
        }
        /* Insert extra #endpile if needed. */
        else if (!tl0) {
                tk = Token::keyword(SrcPos::none, SrcPos::none, DoPileEnd);
                ll = listCons<LNodeTree>(lntTok(tk), ll);
                n++;
        }

        lnt = lntNewEmpty(LN_DoPile, n);
        has = 0;
        for (i = n-1, l = ll; i >= 0 ; i--, l = cdr(l)) {
                lnt->argv[i].lnode = car(l);
                has |= car(l)->has;
        }
        lnt->indent = in0;
        lnt->has    = has;

        *ptl = tl0;
        listFree<LNodeTree>(ll);
        depthDoPileNo -= 1;
        return lnt;
}


/*
 * Convert a list of tokens for a single line of code into a tree. If a
 * #pile token is found then the embedded piled section is converted into
 * a single tree node; likewise for embedded non-piled sections. All
 * other tokens are added to the result tree as leaves. Processing stops
 * when a newline token is found, the end of the token stream is reached,
 * or when a #endpile or } token is found without a matching #pile or {.
 */
local LNodeTree
lntFrTL_DoLine(List<Token> *ptl)
{
        List<Token>       tl0 = *ptl;
        TokenTag        tag;
        List<LNodeTree>   ll;
        LNodeTree       lnt;
        int             n, ntoks, in0;

        if (!tl0) {
                lnt = lntNewEmpty(LN_NTok, int0);
                lnt->indent = 0;
                lnt->has    = 0;
                return lnt;
        }

        in0   = linIndentation(tl0);
        ll    = 0;
        n     = 0;
        ntoks = 0;

        /* This test always succeeds and is therefore redundant */
        if (tl0) do {
                tag = car(tl0).tag();

                switch (tag) {
                case DoPileStart:
                        lnt = lntFrTL_DoPile(&tl0);
                        ll  = listCons<LNodeTree>(lnt, ll);
                        n++;
                        break;
                case DontPileStart:
                        lnt = lntFrTL_DontPile(&tl0);
                        ll  = listCons<LNodeTree>(lnt, ll);
                        n++;
                        break;
                case DoPileEnd:
                case DontPileEnd:
                        if (tag == DoPileEnd)
#if 1
                                break;  /*XXXXXXX*/
#else
                                if (depthDoPileNo)
                                        break;
                                else if (depthDontPileNo)
                                        break;
#endif
                default:
                        lnt = lntFrTL_1Tok(&tl0);
                        ll  = listCons<LNodeTree>(lnt, ll);
                        n++;
                        ntoks++;
                        break;
                }
        } while (tag != KW_NewLine &&
                 (tag != DoPileEnd || depthDoPileNo == 0) &&
                 (tag != DontPileEnd || depthDontPileNo == 0) &&
                 tl0);

        lnt  = lntFrTL_MakeLine(ll, in0, n, ntoks);
        *ptl = tl0;
        listFree<LNodeTree>(ll);
        return lnt;
}

/*
 * Convert a list of tokens for a non-piled section into an lnode tree.
 * For correctly balanced programs the result tree will have three child
 * nodes: the first will be a { leaf and the third will be a } leaf. The
 * second node will contain all tokens within the non-piled section with
 * the nesting of non-piled sections removed. Piled sections will have
 * their own tree nodes as normal.
 */
local LNodeTree
lntFrTL_DontPile(List<Token> *ptl)
{
        List<Token>       tl0 = *ptl;
        List<LNodeTree>   ll, l;
        LNodeTree       lnt;
        int             i, n, in0;
        ULong           has;

        assert(tl0);
        assert(car(tl0).tag() == DontPileStart);

        depthDontPileNo += 1;
        in0   = linIndentation(tl0);
        ll    = listCons<LNodeTree>(lntFrTL_1Tok(&tl0), NULL);
        ll    = listCons<LNodeTree>(lntFrTL_DontLine(&tl0, true), ll);
        n     = 2;
        if (tl0 && car(tl0).tag() == DontPileEnd) {
                ll = listCons<LNodeTree>(lntFrTL_1Tok(&tl0), ll);
                n++;
        }

        lnt = lntNewEmpty(LN_NNodes, n);
        has = 0;
        for (i = n-1, l = ll; i >= 0 ; i--, l = cdr(l)) {
                lnt->argv[i].lnode = car(l);
                has |= car(l)->has;
        }
        lnt->indent = in0;
        lnt->has    = has;

        *ptl = tl0;
        listFree<LNodeTree>(ll);
        depthDontPileNo -= 1;
        return lnt;
}

/*
 * Convert a list of tokens in a non-piled section into a tree. If the
 * isStacking flag is false then every token is placed in the resulting
 * tree even if it is not correctly balanced. Otherwise only the tokens
 * up to the end of the non-piled section are consumed taking into account
 * the balancing of { } around non-piled sections.
 */
local LNodeTree
lntFrTL_DontLine(List<Token> *ptl, bool isStacking)
{
        List<Token>       tl0 = *ptl;
        TokenTag        tag;
        List<LNodeTree>   ll;
        LNodeTree       lnt;
        int             n, ntoks, in0, depth;

        if (!tl0) {
                lnt = lntNewEmpty(LN_NTok, int0);
                lnt->indent = 0;
                lnt->has    = 0;
                return lnt;
        }

        in0   = linIndentation(tl0);
        depth = 0;
        n     = 0;
        ntoks = 0;
        ll    = 0;

        while (tl0) {
                tag = car(tl0).tag();

                if (tag == DoPileStart) {
                        lnt = lntFrTL_DoPile(&tl0);
                        ll  = listCons<LNodeTree>(lnt, ll);
                        n++;
                }
                else {
                        if (isStacking && tag == DontPileStart) {
                                depth++;
                        }
                        if (isStacking && tag == DontPileEnd) {
                                depth--;
                        }
                        if (depth < 0) break;

                        lnt = lntFrTL_1Tok(&tl0);
                        ll  = listCons<LNodeTree>(lnt, ll);
                        n++;
                        ntoks++;
                }
        }

        lnt  = lntFrTL_MakeLine(ll, in0, n, ntoks);
        *ptl = tl0;
        listFree<LNodeTree>(ll);
        return lnt;
}


/* Remove a token from the token list and convert into an lnode tree leaf */
local LNodeTree
lntFrTL_1Tok(List<Token> *ptl)
{
        LNodeTree       lnt;

        assert(*ptl);

        lnt  = lntTok(car(*ptl));
        *ptl = cdr(*ptl);

        return lnt;
}

/* Convert a list of tree nodes into a single n-ary tree node */
local LNodeTree
lntFrTL_MakeLine(List<LNodeTree> ll, int in0, Length n, Length ntoks)
{
        int             i;
        List<LNodeTree>   l;
        LNodeTree       lnt;
        ULong           has;

        has = 0;
        for (l = ll; l; l = cdr(l))
                has |= car(l)->has;

        if (n == 1) {
                lnt = car(ll);
        }
        else if (n == ntoks) {
                /*
                 * Compactify the case when all subtrees are tokens.
                 */
                lnt = lntNewEmpty(LN_NTok, n);
                lnt->indent = in0;
                lnt->has    = has;

                for (i = n-1, l = ll; i >= 0; i--, l = cdr(l)) {
                        lnt->argv[i].tok = car(l)->argv[0].tok;
                        lntFree(car(l));
                }
        }
        else {
                lnt = lntNewEmpty(LN_NNodes, n);
                lnt->indent = in0;
                lnt->has    = has;

                for (i = n-1, l = ll; i >= 0; i--, l = cdr(l))
                        lnt->argv[i].lnode = car(l);
        }

        return lnt;
}


/****************************************************************************
 *
 * :: Verify balance of piling braces
 *
 ***************************************************************************/

# define LIN_NOT_OK     0
# define LIN_OK         1

local void
serrorUnbalanced(Token tok,bool errorIfTrue)
{
        SrcPos   pos  = tok.position();
#if EDIT_1_0_n2_02
        TokenTag tag  = tok.tag(), mate = tok.tag();
#else
        TokenTag tag  = tok.tag(), mate;
#endif

        switch (tag) {
        case DoPileStart:       mate = DoPileEnd;       break;
        case DoPileEnd:         mate = DoPileStart;     break;
        case DontPileStart:     mate = DontPileEnd;     break;
        case DontPileEnd:       mate = DontPileStart;   break;
        default:                comsgFatal(abNewNothing(pos), ALDOR_F_Bug,
                                        "unexpected unbalanced token");
        }

        if (errorIfTrue) {
                comsgError(abNewNothing(pos), ALDOR_E_LinUnbalanced,
                        keyString(tag), keyString(mate));
        }
        else {
                 comsgWarning(abNewNothing(pos), ALDOR_E_LinUnbalanced,
                        keyString(tag), keyString(mate));
        }
}

/*
 * Look for unmatched {,},{|,|} and give error msg.
 * It also sets tok->extra field for each token in the token list.
 */
local void
linCheckBalance0(List<Token> *ptl, Token lastOpener, int depth)
{
        TokenTag        tag;
        Token           tok;

        linDEBUG(
             fprintf(dbOut,"->> linCheckPile0: %d--\n",lastOpener.tag()););

        lastOpener.setExtra(LIN_OK);

        while (*ptl) {
                tok  = car(*ptl);
                tag  = tok.tag();
                *ptl = cdr(*ptl);

                switch(tag) {
                case DoPileStart:
                case DontPileStart:
                        linCheckBalance0(ptl, tok, depth+1);
                        break;
                case DoPileEnd:
                        if (lastOpener.tag() == DoPileStart) {
                                tok.setExtra(LIN_OK);
                                linDEBUG(
                                  fprintf(dbOut,"-<< linCheckPile0- |} --\n"));
                                return;
                        }
                        else {
                                tok.setExtra(LIN_NOT_OK);
                                serrorUnbalanced(tok,true);
                        }
                        break;
                case DontPileEnd:
                        if (lastOpener.tag() == DontPileStart) {
                                tok.setExtra(LIN_NOT_OK);
                                linDEBUG(
                                    fprintf(dbOut,"-<< linCheckPile0- } --\n"));
                                return;
                        }
                        else {
                                tok.setExtra(LIN_NOT_OK);
                                serrorUnbalanced(tok,true);
                        }
                        break;
                default:
                        break;
                }
        }

        /* Detect unmatched opener, but allow missing #endpile at top level.*/
        tag = lastOpener.tag();
        if (tag != TK_Blank && tag != DoPileStart ) {
                lastOpener.setExtra(LIN_NOT_OK);
                serrorUnbalanced(lastOpener,true);
        }
        if (tag != TK_Blank && depth > 1 ) {
                lastOpener.setExtra(LIN_NOT_OK);
                serrorUnbalanced(lastOpener,false);
        }
        linDEBUG(fprintf(dbOut,"-<< linCheckPile0-NULL--\n"););
}


/*
 * This function returns the original list but we should suppose that
 * it could be different -- we may later insert correction tokens.
 */
local List<Token>
linCheckBalance(List<Token> tl0)
{
        List<Token>       tl = tl0;
        Token           tok;

        /* Mark all nodes OK. */
        for (tl = tl0; tl; tl = cdr(tl))
                car(tl).setExtra(LIN_OK);

        /* Do the balancing act. */
        tl  = tl0;
        tok = Token::blank(SrcPos::none, SrcPos::none, nullptr);
        linCheckBalance0(&tl, tok, int0);
        tok.free();

        return tl0;
}

/*****************************************************************************
 *
 * :: Main entry point of linearizer
 *
 ****************************************************************************/

List<Token>
linearize(List<Token> tl)
{
        LNodeTree lnt;

        linDEBUG({
                fprintf(dbOut,"-------------- Starting with -------------\n");
                toklistPrint(dbOut, tl);
                fnewline(dbOut);
        });

        tl  = linXComments(tl);
        tl  = linXBlankLines(tl);

        /* Add a `#pile' if in interactive mode, so that the user can use
         * TAB for an easier input.
         */

        if (fintMode == FINT_LOOP) {
                Token tok = Token::keyword(SrcPos::none, SrcPos::none, KW_StartPile);
                listPush(Token, tok, tl);
        }

        tl  = linCheckBalance(tl);
        lnt = lntFrTokenList(tl);
        listFree<Token>(tl);
        linDEBUG({
                fprintf(dbOut,"-------------- Converted to --------------\n");
                lntPrint(dbOut, lnt);
                fnewline(dbOut);
        });

        lnt = lin2DRules(lnt);
        linDEBUG({
                fprintf(dbOut,"-------------- Converted to -------------\n");
                lntPrint(dbOut, lnt);
                fnewline(dbOut);
        });

        tl = lntToTokenList(lnt);
        tl = linXNewLines(tl);
        lntFree(lnt);
        linDEBUG({
                fprintf(dbOut,"-------------- Ending with -------------\n");
                toklistPrint(dbOut, tl);
                fnewline(dbOut);
        });

        tl = linUseNeededSep(tl);

        linDEBUG({
                fprintf(dbOut,"-------------- Leaving with ------------\n");
                toklistPrint(dbOut, tl);
                fnewline(dbOut);
        });

        return tl;
}

/* This function inserts or deletes statement separators (;) as required */
local List<Token>
linUseNeededSep(List<Token> tl)
{
        tl = linISepAfterDontPiles(tl);

        linDEBUG({
                fprintf(dbOut,"------- linUseNeededSep (mid) ----------\n");
                toklistPrint(dbOut, tl);
                fnewline(dbOut);
        });

        tl = linXSep(tl);

        linDEBUG({
                fprintf(dbOut,"------- linUseNeededSep (xit) ----------\n");
                toklistPrint(dbOut, tl);
                fnewline(dbOut);
        });
        return tl;
}

/*****************************************************************************
 *
 * :: List<Token> Utilities
 *
 ****************************************************************************/

local Token
linKeyword(Token org, TokenTag key)
{
        SrcPos    spos = org ? org.position() : SrcPos::none;
        SrcPos    epos = org ? org.end() : SrcPos::none;

        return Token::keyword(spos, epos, key);
}

/*
 * Free the specified tokens.  (Modifies the original list.)
 */
local List<Token>
linXTokens(List<Token> tl, TokenTag tag)
{
        ListCons<Token>           head;
        List<Token>               prev;

        setcar(&head, NULL);
        setcdr(&head, tl);

        for (prev = &head; cdr(prev); ) {
                List<Token> curr = cdr(prev);
                Token     tok  = car(curr);

                if (tok.tag() == tag) {
                        setcdr(prev, cdr(curr));
                        listFreeCons<Token>(curr);
                        tok.free();
                }
                else
                        prev = curr;
        }
        return cdr(&head);
}

/*
 * Free leading newlines and all but the first of consecutive newlines.
 * (Modifies the original list.)
 */
local List<Token>
linXBlankLines0(List<Token> tl)
{
        while (tl && car(tl).tag() == KW_NewLine) {
                Token     tk = car(tl);
                List<Token> rl = cdr(tl);

                tk.free();
                listFreeCons<Token>(tl);
                tl = rl;
        }
        return tl;
}

local List<Token>
linXBlankLines(List<Token> tl0)
{
        List<Token> tl;

        tl0 = linXBlankLines0(tl0);

        for (tl = tl0; tl; tl = cdr(tl)) {
                Token     tok  = car(tl);
                List<Token> rest = cdr(tl);

                if (tok.tag() == KW_NewLine || tok.tag() == DoPileStart) {
                        /* !!!FIXME!!! Loop executes just once? */
                        while (rest && car(rest).tag() == KW_NewLine)
                                rest = linXBlankLines0(rest);
                        setcdr(tl, rest);
                }
        }
        return tl0;
}


/*
 * Insert ";" after all "}".  (Modifies the original list.) This function
 * ought to be modified to use a two-token look-ahead. If the next token
 * is not DontPileSep and the one after is not a follower or a closer
 * then the insertion can take place. If this change is made then linXSep
 * is redundant and the compiler will make fewer strange parsing decisions.
 */
local List<Token>
linISepAfterDontPiles(List<Token> tl)
{
        List<Token> t;
        for (t = tl; t; t = cdr(t)) {
                if (car(t).is(DontPileEnd)) {
                        List<Token> trest = cdr(t);

                        if (trest && !car(trest).is(DontPileSep)) {
                                Token tok = linKeyword(car(t),DontPileSep);
                                trest = listCons<Token>(tok, trest);
                                setcdr(t, trest);
                        }
                }
        }

        return tl;
}

/*
 * Delete ";"  before non-starters. This function would be redundant if
 * linISepAfterDontPiles() used a two-token look-ahead.
 */
local List<Token>
linXSep(List<Token> tl)
{
        List<Token> t;

        /* Delete leading string ";". */
        while (tl && car(tl).is(DontPileSep))
                tl = listFreeDeeplyTo<Token>(tl, cdr(tl), [](Token tok) { tok.free(); });

        /* Delete ";" before non-starters. */
        for (t = tl; t; t = cdr(t)) {
                List<Token> tcdr, tcddr;
                tcdr = cdr(t);
                if (!tcdr) break;
                if (!car(tcdr).is(DontPileSep)) continue;
                tcddr = cdr(tcdr);
                if (!tcddr || tokIsNonStarter(car(tcddr)))
                        setcdr(t, listFreeDeeplyTo<Token>(tcdr, tcddr, [](Token tok) { tok.free(); }));
        }

        return tl;
}

/*
 * Determine the true indentation a line, taking into account labels.
 */
local int
linIndentation(List<Token> tl)
{
        int     ind;

        linDEBUG({if (tl) car(tl).print(dbOut);});

        if (tl && car(tl).is(KW_At)) {
                /* Skip '@' and 'id' if there. */
                tl = cdr(tl);
                if (tl) tl = cdr(tl);
        }
        ind = tl && !car(tl).is(KW_NewLine)
                ? car(tl).position().character()
                : MootIndentation;

        DO_DEBUG(fprintf(dbOut, " line indented to: %d\n", ind));

        return ind;
}

/****************************************************************************
 *
 * :: Apply piling rules.
 *
 ****************************************************************************/

local LNodeTree lin2DRulesPile    (LNodeTree);
local LNodeTree lin2DRulesPile0   (LNodeTree context, LNodeTree, int *, int *);

local LNodeTree joinUp            (LNodeTree context, List<LNodeTree>);
local bool      isBackSetRequired (LNodeTree context, LNodeTree, LNodeTree);
local bool      isPileRequired    (LNodeTree context, LNodeTree);

local LNodeTree
lin2DRules(LNodeTree lnt)
{
        int     i, n;
        n = lnt->argc;

        switch (lnt->kind) {
        case LN_1Tok:
        case LN_NTok:
                break;
        case LN_NNodes:
                for (i = 0; i < n; i++)
                        lnt->argv[i].lnode = lin2DRules(lnt->argv[i].lnode);
                break;
        case LN_DoPile:
                for (i = 0; i < n; i++)
                        lnt->argv[i].lnode = lin2DRules(lnt->argv[i].lnode);
                lnt = lin2DRulesPile(lnt);
                break;
        }

        return lnt;
}

#define linIsArgKW(lnt,n,k) \
        ((lnt)->argc > (n) && (lnt)->argv[n].lnode->kind == LN_1Tok && \
         ((lnt)->argv[n].lnode->argv[0].tok).is(k))

local LNodeTree
lin2DRulesPile(LNodeTree lnt)
{
        bool            hasStarter, hasEnder;
        int             iS, iE;
        LNodeTree       rnt;

        assert(lnt->kind == LN_DoPile);

        hasStarter = linIsArgKW(lnt, int0,        DoPileStart);
        hasEnder   = linIsArgKW(lnt, lnt->argc-1, DoPileEnd);

        iS  = hasStarter ? 1 : 0;
        iE  = hasStarter && hasEnder ? lnt->argc - 2 : lnt->argc - 1;

        /*
         * Consider     xxx1
         *                 xxx2
         *                 xxx3
         *              xxx4
         *           xxx5
         */

        /* Handle pile starting with first line: xxx1..xxx4 */
        rnt = lin2DRulesPile0(NULL, lnt, &iS, &iE);

        /* Handle subsequent outdented piles: e.g., xxx5 */
        while (iS <= iE)
                rnt = lin2DRulesPile0(rnt, lnt, &iS, &iE);

        lntFreeNode(lnt);
        return rnt;
}


/*
 * Piles a range of descendents. Note that since the final argument is
 * never updated it ought to be passed by value.
 */
local LNodeTree
lin2DRulesPile0(LNodeTree context, LNodeTree lnt, int *piS, int *piE)
{
        int             iS = *piS, iE = *piE;
        int             indentS, indent0;
        LNodeTree       lnt0;
        List<LNodeTree>   sofar;

        DO_DEBUG(fprintf(dbOut," ==== Linearizing parts %d to %d \n", *piS, *piE));

        if (iS > iE) return lntNewEmpty(LN_NNodes, int0);

        indentS = lnt->argv[iS].lnode->indent;
        sofar   = 0;

        DO_DEBUG(fprintf(dbOut,"indentS = %d\n", indentS));
        while (iS <= iE) {
                lnt0    = lnt->argv[iS].lnode;
                indent0 = lnt0->indent;

                DO_DEBUG({
                        fprintf(dbOut,"%d/%d/%d. indent0 = %d (argc = %d)\n",
                                iS, iE, (int) lnt->argc,
                                indent0, (int) lnt0->argc);
                        lntPrint(dbOut, lnt0);
                        fnewline(dbOut);
                });

                if (linIsBlank(lnt0) || indent0 == MootIndentation) {
                        iS++;
                        DO_DEBUG(fprintf(dbOut,"Ah.... a blank line _________\n"));
                        sofar  = listCons<LNodeTree>(lnt0, sofar);
                        continue;
                }

                DO_DEBUG(fprintf(dbOut,"%d :: %d\n", indent0, indentS));
                if (indent0 < indentS)
                        break;

                if (indent0 == indentS) {
                        iS++;
                        sofar  = listCons<LNodeTree>(lnt0, sofar);
                        DO_DEBUG({
                                fprintf(dbOut,"Adding one more +++++++++++++ ");
                                listPrint<LNodeTree>(dbOut, sofar, lntPrint);
                                fnewline(dbOut);
                        });
                }
                /* (indent0 > indentS) */
                else {
                        setcar(sofar, lin2DRulesPile0(car(sofar),lnt,&iS,&iE));
                        DO_DEBUG({
                                fprintf(dbOut,"Joining indentee >>>>>>>>>>>> ");
                                listPrint<LNodeTree>(dbOut, sofar, lntPrint);
                                fnewline(dbOut);
                        });
                }
        }
        sofar = listNReverse<LNodeTree>(sofar);
        lnt0  = lntConcat(context, joinUp(context, sofar));

        DO_DEBUG({
                fprintf(dbOut,"Result of joinUp is !!!!!!!!!!!!! ");
                lntPrint(dbOut, lnt0);
                fnewline(dbOut);
        });

        /* Free the list we just consed. */
        listFree<LNodeTree>(sofar);
        *piS = iS;
        *piE = iE;
        return lnt0;
}


/*
 * Insert SetTab .. BackSet .. BackSet ... BackTab
 */
local LNodeTree
joinUp(LNodeTree context, List<LNodeTree> tll)
{
        List<LNodeTree>   l;
        LNodeTree       lnt, t0, t1;
        bool            hadBackSet;

        assert(tll != 0);

        /* Insert BackSets between the lines, if necessary. */
        lnt        = car(tll);
        hadBackSet = false;

        for (l = tll, t0 = lnt; cdr(l); l = cdr(l), t0 = t1) {
                t1 = car(cdr(l));

                if (isBackSetRequired(context, t0, t1)) {
                        hadBackSet = true;
                        lnt = lntSeparate(lnt, KW_BackSet, t1);
                }
                else
                        lnt = lntConcat(lnt, t1);
        }

        /* Surround with SetTab and BackTab, if necessary. */
        if (hadBackSet || isPileRequired(context, lnt))
                lnt = lntWrap(KW_SetTab, lnt, KW_BackTab);

        return lnt;
}

/*
 * "isPileRequired" decides whether a SetTab/BackTab empiling is needed
 *  for the line which is to be joined to *pcontext.
 *
 *  A single line pile is formed whenever the previous word is an alphabetic
 *  language keyword, e.g.  "return", "then", "else", etc.
 *  (Note, this does not include user-definable operators such as "quo".)
 */
local bool
isPileRequired(LNodeTree context, LNodeTree lnt)
{
        /*ARGSUSED*/
        Token   tok = lntLastTokLessNL(context);

        DO_DEBUG(fprintf(dbOut,"In isPileRequired %s",
                      tok? "with token: " : "NO TOK\n"));

        if (!tok) return false;

        DO_DEBUG(tok.print(dbOut));

        return  tok.is(KW_Then)    || tok.is(KW_Else)    ||
                tok.is(KW_With)    || tok.is(KW_Add)     ||
                tok.is(KW_Try)     || tok.is(KW_But)     ||
                tok.is(KW_Catch)   || tok.is(KW_Finally) ||
                tok.is(KW_Always);
}


/*
 * "isBackSetRequired" decides whether a BackSet is needed between tl1 and tl2.
 *  Normally a BackSet is needed.   The exceptions are:
 *
 * 1. tl1 contains only ++ comments (-- comments already deleted)
 *    This allows
 *
 *      ++ xxx
 *      f: T -> Y
 *
 * 2. tl1 ends with "," or an opener ("(" "[" etc).
 *    This allows
 *
 *      f(x,              a := [
 *        y,                 1, 2, 3,
 *        z)                 4, 5, 6 ]
 *
 * 3. tl2 begins with "in", "then", "else" or a closer (")" "]" "}" etc)
 *    (i.e. words which CANNOT start an expression).
 *    This allows
 *
 *      if aaa            let                f(x,         a := [1, 2, 3,
 *      then bbb             f == 1            y                4, 5, 6
 *      else ccc          in x := f+f        )                 ]
 *
 * 4. The lines occur within "{ }"
 *
 * Note: case 4 appears to be ignored and the context is not used.
 */
local bool
isBackSetRequired(LNodeTree context, LNodeTree lnt1, LNodeTree lnt2)
{
        /*ARGSUSED*/
        Token   tok1 = lntLastTokLessNL (lnt1);
        Token   tok2 = lntFirstTok      (lnt2);

        DO_DEBUG({
                fprintf(dbOut,"In isBackSetRequrired ");
                fprintf(dbOut,"First tree ");
                lntPrint(dbOut, lnt1);
                fnewline(dbOut);
                fprintf(dbOut,"Second tree ");
                lntPrint(dbOut, lnt2);
                fnewline(dbOut);

                if (tok1){ fprintf(dbOut," with tok1 "); tok1.print(dbOut); }
                else {fprintf(dbOut,"No first token\n"); }
                if (tok2){ fprintf(dbOut," with tok2 "); tok2.print(dbOut); }
                else {fprintf(dbOut,"No second token\n"); }
                fnewline(dbOut);
        });

        /* Rule 1 */
        if (linIsCom(lnt1) || linIsBlank(lnt1) || linIsBlank(lnt2))
                return false;

        if (!tok1 || !tok2)
                return true;

        /* Rule 2 */
        if (tok1.is(KW_Comma) || tok1.isOpener())
                return false;

        /* Rule 3 */
        if (tok2.isFollower() || tok2.isCloser())
                return false;

        return true;
}

