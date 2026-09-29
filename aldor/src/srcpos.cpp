///////////////////////////////////////////////////////////////////////////////
//
// srcpos.cpp: Source position operations.
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

# include "axlgen.h"
# include "comsg.h"

struct SrcPosRawAccess {
        static ULong bits(SrcPos p) noexcept { return p.bits_; }
        static SrcPos make(ULong bits) noexcept
                { return SrcPos(bits, SrcPos::RawTag{}); }
};

#define SPOS_RAW(p)      SrcPosRawAccess::bits(p)
#define SPOS_FROM_RAW(p) SrcPosRawAccess::make(p)

/*
 * The "fields" are
 *      int  mac:  1;  -- 0 means text is not macro expanded, 1 means it is
 *      int  cno: 14;  -- 0 based character position in line
 *      int  lno: nn;  -- 0 based global line number (nn = 16 for 32 bit longs)
 *
 * We use long ints rather than structs with bitfields so we can pass
 * SrcPos values as parameters without complaint in any C compiler.
 *
 * We also keep one bit free for spstack operations.
 */

# define SPOS_STK_NBITS  (1)
# define SPOS_MAC_NBITS  (1)
# define SPOS_CNO_NBITS (14)
# define SPOS_LNO_NBITS \
        (bitsizeof(ULong) - SPOS_CNO_NBITS - SPOS_MAC_NBITS - SPOS_STK_NBITS)

# define SPOS_STK_MASK  ((1L << SPOS_STK_NBITS) - 1)

# define SPOS_MAC_SHIFT (0)
# define SPOS_MAC_MASK  (((1L << SPOS_MAC_NBITS) - 1) << SPOS_MAC_SHIFT)

# define SPOS_CNO_SHIFT (SPOS_MAC_SHIFT + SPOS_MAC_NBITS)
# define SPOS_CNO_MASK  (((1L << SPOS_CNO_NBITS) - 1) << SPOS_CNO_SHIFT)

# define SPOS_LNO_SHIFT (SPOS_CNO_SHIFT + SPOS_CNO_NBITS)
# define SPOS_LNO_MASK  (((1L << SPOS_LNO_NBITS) - 1) << SPOS_LNO_SHIFT)

# define sposSet(l, c) SPOS_FROM_RAW(((l) << SPOS_LNO_SHIFT) | ((c) << SPOS_CNO_SHIFT))

const SrcPos SrcPos::none = sposSet(int0, int0);

#define         TOP_LINE_NO     long0
#define         END_LINE_NO     ((1L << SPOS_LNO_NBITS) - 1)

SrcPos
SrcPos::top() noexcept
{
        return sposSet(TOP_LINE_NO, int0);
}

SrcPos
SrcPos::end() noexcept
{
        return sposSet(END_LINE_NO, int0);
}

SrcPos
SrcPos::fromPacked(ULong bits) noexcept
{
        return SPOS_FROM_RAW(bits);
}

ULong
SrcPos::packed() const noexcept
{
        return SPOS_RAW(*this);
}

typedef struct {
        Length          glno;
        FileName        fn;
        Length          flno;
} GLine;

static int      gloPos;
static int      gloArgc;
static GLine    *gloLineTbl;

static FileName lastfname;
static Length   lastlno, lastftell;

SrcPos
SrcPos::offset(int c) const noexcept
{
    ULong p = SPOS_RAW(*this);
    return SPOS_FROM_RAW((((p >> SPOS_CNO_SHIFT) + c) << SPOS_CNO_SHIFT) |
                         (p & SPOS_MAC_MASK));
}

bool
SrcPos::equals(SrcPos q) const noexcept
{
    ULong p = SPOS_RAW(*this);
    return ((p >> SPOS_CNO_SHIFT) == (SPOS_RAW(q) >> SPOS_CNO_SHIFT));
}

SrcPos
SrcPos::min(SrcPos q) const noexcept
{
    ULong P, Q;

    P = SPOS_RAW(*this) >> SPOS_CNO_SHIFT;
    Q = SPOS_RAW(q) >> SPOS_CNO_SHIFT;

    return (P < Q) ? *this : q;
}

SrcPos
SrcPos::max(SrcPos q) const noexcept
{
    ULong P, Q;

    P = SPOS_RAW(*this) >> SPOS_CNO_SHIFT;
    Q = SPOS_RAW(q) >> SPOS_CNO_SHIFT;

    return (P > Q) ? *this : q;
}

bool
SrcPos::isMacroExpanded() const noexcept
{
    return (SPOS_RAW(*this) & SPOS_MAC_MASK) >> SPOS_MAC_SHIFT;
}

SrcPos
SrcPos::macroExpanded() const noexcept
{
    return SPOS_FROM_RAW(SPOS_RAW(*this) | (1UL << SPOS_MAC_SHIFT));
}

int
SrcPos::print(FILE *fout) const
{
        int     cc;
        if (isSpecial()) {
                switch (globalLine()) {
                case TOP_LINE_NO:
                        cc = fprintf(fout, "-- TOP --");
                        break;
                case END_LINE_NO:
                        cc = fprintf(fout, "-- END --");
                        break;
                default:
                        cc = fprintf(fout, "-- unknown src pos --");
                        break;
                }
        }
        else
                cc = fprintf(fout, "\"%s\", line %d char %d",
                          file().unparseStatic(),
#if EDIT_1_0_n1_07
                          (int) line(), (int) character());
#else
                          line(), character());
#endif
        return cc;
}

void
SrcPos::show()
{
        int     i;

        assert(gloLineTbl);

        fprintf(stderr, "Begin Show Global Line Table\n");
        for (i = 0; i < gloArgc; i++)
                fprintf(stderr, "\t%s [%d], %d\n",
                       gloLineTbl[i].fn.unparseStatic(),
#if EDIT_1_0_n1_07
                       (int) gloLineTbl[i].glno, (int) gloLineTbl[i].flno);
#else
                       gloLineTbl[i].glno, gloLineTbl[i].flno);
#endif
        fprintf(stderr, "End Show Global Line Table\n");
        return;
}

/*
 * Here we rely on the fact that the line number is in the more
 * significant part of the integer.
 */
int
SrcPos::compare(SrcPos sp2) const noexcept
{
        int cmp = 0;

        ULong sp1 = SPOS_RAW(*this) >> SPOS_CNO_SHIFT;
        ULong p2  = SPOS_RAW(sp2) >> SPOS_CNO_SHIFT;

        if (sp1 < p2)
                cmp = -1;
        else if (sp1 > p2)
                cmp =  1;
        return cmp;
}

bool
SrcPos::isSpecial() const noexcept
{
        ULong   gno = globalLine();

        return gno == TOP_LINE_NO || gno == END_LINE_NO;
}

void
SrcPos::init()
{
        /* set up global line number table */

        lastfname  = 0;
        gloLineTbl = (GLine *) stoAlloc(int0, sizeof(GLine));
        gloArgc    = 1;
        gloPos     = 0;
        gloLineTbl[0].fn   = 0;
        gloLineTbl[0].glno = 0;
        gloLineTbl[0].flno = 0;

        return;
}

void
SrcPos::fini()
{
        /* free global line number table */

        assert(gloLineTbl);

        stoFree((void *) (gloLineTbl));

        if (lastfname) {
                lastfname.free();
                lastfname = 0;
        }
        return;
}

void
SrcPos::tableToBuffer(Buffer buf)
{
        int     i;

        assert(gloLineTbl);

        if ((gloArgc == 1) && (!gloLineTbl[0].fn)) return;

        /* write global line number table to a buffer */
        buf.writeULong(gloArgc);
        for (i = 0; i < gloArgc; i++) {
                String  s;
                int     len;

                buf.writeULong(gloLineTbl[i].glno);
                buf.writeULong(gloLineTbl[i].flno);
                s   = gloLineTbl[i].fn.dir();
                len = strlen(s);
                BUF_PUT_SINT(buf, len);
                buf.writeChars(len, s);
                s   = gloLineTbl[i].fn.name();
                len = strlen(s);
                BUF_PUT_SINT(buf, len);
                buf.writeChars(len, s);
                s   = gloLineTbl[i].fn.type();
                len = strlen(s);
                BUF_PUT_SINT(buf, len);
                buf.writeChars(len, s);
        }
}

void
SrcPos::tableFromBuffer(Buffer buf)
{
        int     i;
        /* set up global line number table */

        SrcPos::init();
        gloArgc = buf.readULong();
        gloPos  = gloArgc;

        gloLineTbl = (GLine *) stoResize(gloLineTbl,
                                         sizeof(GLine)*(gloArgc));
        for (i = 0; i < gloArgc; i++) {
                MutString  dir, name, type;
                int     len;

                gloLineTbl[i].glno = buf.readULong();
                gloLineTbl[i].flno = buf.readULong();
                BUF_GET_SINT(buf, len);
                dir  = buf.readChars(len);
                BUF_GET_SINT(buf, len);
                name = buf.readChars(len);
                BUF_GET_SINT(buf, len);
                type = buf.readChars(len);
                gloLineTbl[i].fn   = FileName::create(dir, name, type);
        }
}

SrcPos
SrcPos::create(FileName fname, Length flno, Length glno, Length cno)
{
        FileName prevName;
        int      prevGlno;

        assert(gloLineTbl);

        if (gloPos < 0 || !fname) return SrcPos::none;

        if (gloPos) {
                prevName = gloLineTbl[gloPos - 1].fn;
                prevGlno = gloLineTbl[gloPos - 1].glno;
        }
        else {
                prevName = gloLineTbl[gloPos].fn;
                prevGlno = gloLineTbl[gloPos].glno;
        }

        if (glno <= prevGlno || prevName == 0 || !fname.equals(prevName))

          registerFileLine(fname, flno, glno);


        return sposSet(glno, cno);
}

void
SrcPos::registerFileLine(FileName fname, Length flno, Length glno)
{
  if (gloPos > 0) {
    gloLineTbl = (GLine *)
      stoResize(gloLineTbl,
                sizeof(GLine)*(gloArgc+1));
    gloArgc++;
  }
  gloLineTbl[gloPos].glno = glno;
  gloLineTbl[gloPos].flno = flno;
  gloLineTbl[gloPos].fn = fname.copy();
  gloPos++;
}

SrcPos
SrcPos::fromGlobal(Length glno, Length cno) noexcept
{
        return sposSet(glno, cno);
}

FileName
SrcPos::file() const
{
        Length  gLineNo;
        int     i;

        assert(gloLineTbl);

        if (isSpecial()) return gloLineTbl[0].fn;

        gLineNo = globalLine();

        if (gloPos && gLineNo) {
                for (i = 0; i < gloArgc-1; i++) {
                        if (gLineNo >= gloLineTbl[i].glno &&
                            gLineNo < gloLineTbl[i+1].glno)
                                return gloLineTbl[i].fn;
                }
                return gloLineTbl[gloArgc-1].fn;
        }
        else {
                return gloLineTbl[0].fn;
        }
}

Length
SrcPos::globalLine() const noexcept
{
        return (SPOS_RAW(*this) & SPOS_LNO_MASK) >> SPOS_LNO_SHIFT;
}

Length
SrcPos::globalLine(FileName fname, Length fLineNo)
{
        Length  i, glopos = 1;

        /* retrieve global line number from a file name and file line */

        assert(gloLineTbl);
        for (i = 0; i < gloArgc; i++)
                if (gloLineTbl[i].fn.equals(fname))
                        if (fLineNo >= gloLineTbl[i].flno)
                                glopos = gloLineTbl[i].glno + (fLineNo - gloLineTbl[i].flno);
        return glopos;
}

Length
SrcPos::line() const
{
        Length  gLineNo;
        int     i;

        assert(gloLineTbl);

        if (isSpecial()) return 0;

        gLineNo = globalLine();

        if (gloPos && gLineNo) {
                for (i = 0; i < gloArgc-1; i++) {
                        if (gLineNo >= gloLineTbl[i].glno &&
                            gLineNo < gloLineTbl[i+1].glno)
                                return (gLineNo - gloLineTbl[i].glno) +
                                       (gloLineTbl[i].flno);
                }
                return (gLineNo - gloLineTbl[gloArgc-1].glno) +
                       (gloLineTbl[gloArgc-1].flno);
        }
        else
                return (gloLineTbl[0].glno + gloLineTbl[0].flno);
}

Length
SrcPos::character() const noexcept
{
        return (SPOS_RAW(*this) & SPOS_CNO_MASK) >> SPOS_CNO_SHIFT;
}

int
SrcPos::lineText(Buffer buf) const
{
        FileName        fname;
        FILE            *f;
        int             c = 0, i, lno, rc;

        if (isSpecial())
                return -1;

        fname = file();
        lno   = line();

        /* Add null in case of errors. */
        BUF_ADD1(buf, char0);
        BUF_BACK1(buf);

        if (fname.isStdin())
                return -1;

        f = fileRdOpen(fname);

        /* Are we continuing from where we last were? */
        if (lastfname && lastfname.equals(fname) && lastlno <= lno)
                fseek(f, lastftell, SEEK_SET);
        else
                lastlno = 1;

        for (i = lastlno; i < lno; ) {
                c = getc(f);
                if (c == '\n') i++;
                if (c == EOF) break;
        }

        if (c == EOF || i != lno)
                rc = -1;
        else {
                do {
                        c = getc(f);
                        if (c == EOF) break;
                        BUF_ADD1(buf, c);
                } while (c != '\n');
                BUF_ADD1(buf, char0);
                rc = buf.size();
        }

        /* Remember where we were. lastfname is cleared by srcposFini. */
        if (!lastfname || !lastfname.equals(fname)) {
                if (lastfname) lastfname.free();
                lastfname = fname.copy();
        }
        /* Make lastftell be the position of the beginning of lastlno. */
        lastlno   = lno + 1;
        lastftell = ftell(f);

        fclose(f);
        return rc;
}

/*
 * SrcPosStack functions
 */
SrcPosStack     spstackEmpty;
SrcPosStack     spstackStatic;

# define spstackImmed(spos)     \
        (spstackStatic.spos = SPOS_FROM_RAW((SPOS_RAW(spos) << SPOS_STK_NBITS) | SPOS_STK_MASK), \
         spstackStatic)

# define spstackIsEmpty(stk)    ((stk).stack == NULL)
# define spstackIsImmed(stk)    (SPOS_RAW((stk).spos) & SPOS_STK_MASK)

SrcPosStack
spstackPush(SrcPos spos, SrcPosStack rest)
{
        SrcPosStack     sposStk;
        SrcPosCell      sposCell;

        sposCell = (SrcPosCell) stoAlloc(OB_Other, sizeof(*sposCell));

        sposCell->spos  = spos;
        sposCell->rest  = rest;

        sposStk.stack = sposCell;
        return sposStk;
}

SrcPosStack
spstackCopy(SrcPosStack sposStk)
{
        if (spstackIsEmpty(sposStk) || spstackIsImmed(sposStk))
                return sposStk;

        return spstackPush(spstackFirst(sposStk),
                           spstackCopy(spstackRest(sposStk)));
}

void
spstackFree(SrcPosStack sposStk)
{
        if (spstackIsEmpty(sposStk) || spstackIsImmed(sposStk))
                return;

        spstackFree(spstackRest(sposStk));
        stoFree((void *) sposStk.stack);
}

SrcPos
spstackFirst(SrcPosStack sposStk)
{
        if (spstackIsEmpty(sposStk))
                return SrcPos::none;
        else if (spstackIsImmed(sposStk))
                return SPOS_FROM_RAW(SPOS_RAW(sposStk.spos) >> SPOS_STK_NBITS);

        return sposStk.stack->spos;
}

SrcPosStack
spstackRest(SrcPosStack sposStk)
{
        if (spstackIsEmpty(sposStk) || spstackIsImmed(sposStk))
                return spstackEmpty;

        return sposStk.stack->rest;
}

SrcPosStack
spstackSetFirst(SrcPosStack sposStk, SrcPos spos)
{
        if (spstackIsEmpty(sposStk) || spstackIsImmed(sposStk))
                return spstackImmed(spos);

        sposStk.stack->spos = spos;
        return sposStk;
}

SrcPosStack
spstackSetSecond(SrcPosStack sposStk, SrcPos spos)
{
        if (spstackIsEmpty(sposStk) || spstackIsImmed(sposStk))
                return spstackPush(spstackFirst(sposStk), spstackImmed(spos));

        sposStk.stack->rest = spstackSetFirst(spstackRest(sposStk), spos);
        return sposStk;
}


/*
 * Print a "...^..^" indicator line (hacked from comsgPrintDots)
 */
local void
spstackPrintDots(SrcPos spos)
{
        if (!spos.isSpecial()) {
                int     Dcno = spos.character() - 1;

                if (Dcno >= 0) {
                        fputcTimes('.', Dcno, dbOut);
                        (void)fprintf(dbOut, "^");
                }

                (void)fprintf(dbOut, "\n");
        }
}

/*
 * Print a source code line (hacked from comsgPrintLine)
 */
int
spstackPrintLine(FILE *fout, SrcPos spos)
{
        Buffer  buf;
        MutString  s;
        int     rc, cc;
        Length  n, nu;
        bool    splitLine = false;
        String fFmt = "\"%s\", line %d: ";
        int     LINE_LENGTH = 80;

        /*
         * Get the source line.
         */
        buf = Buffer::create();
        rc = spos.lineText(buf);
        if (rc == -1) {
                buf.free();
                return -1;
        }

        s  = buf.liberate();
        n  = strLength(s);
        nu = strUntabLength(s, TABSTOP);

        /*
         * Print the position and source line,  splitting them if necessary.
         */
        if (spos.isSpecial())
                cc = spos.print(fout);
        else {
                FileName        fn = spos.file();

                cc = fprintf(fout, fFmt,
                             fn.unparseStatic(),
                             spos.line());

                if (fn.hasDir() && !strEqual(fn.dir(), osCurDirName()))
                        splitLine = true;
        }

        if (cc + nu >= LINE_LENGTH) splitLine = true;
        if (splitLine) fputc('\n', fout);

        fputsUntab(s, TABSTOP, fout);
        if (n == 0 || s[n-1] != '\n') fputc('\n', fout);

        return splitLine ? 0 : cc;
}


void
spstackPrintDb(SrcPosStack sposStk)
{
        while (!spstackIsEmpty(sposStk))
        {
                /* Get the line at the top of the stack */
                SrcPos pos = spstackFirst(sposStk);


                /* Display the line neatly */
                int indent = spstackPrintLine(dbOut, pos);


                /* Highlight the position on the line */
                fputcTimes(' ', indent, dbOut);
                spstackPrintDots(pos);


                /* Move down the stack */
                sposStk = spstackRest(sposStk);
        }
}

