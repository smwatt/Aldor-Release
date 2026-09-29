///////////////////////////////////////////////////////////////////////////////
//
// include.cpp: The source file includer.
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
 * This file contains functions for processing the source file directives
 *
 *  #include   fname
 *  #reinclude fname
 *
 *  #assert    property
 *  #unassert  property
 *
 *  #if        property
 *  #elseif    property
 *  #else
 *  #endif
 *
 *  #line      lno [fname]
 *
 * Other #xxx lines are passed on untouched.
 */

# include "axlphase.h"

/******************************************************************************
 *
 * :: Forward Declarations
 *
 *****************************************************************************/

typedef enum {
        NoIf,
        ActiveIf,
        InactiveIf,
        FormerlyActiveIf
} IfState;

typedef struct {
        MutString     curDir;              /* current directory of the file */
        MutString     curFile;             /* current file name string */
        FileName   curFname;            /* current file name */
        FILE       *infile;             /* file being included */
        List<Hash>   fileCodes;           /* hash codes of active source files */
        List<MutString> fileNames;           /* names of active source files */
        int        lineNumber;          /* file line number */
} FileState;

/*
 * Recursive file includer.
 */
local FileName    inclFind              (MutString fn, MutString cwd);
local List<SrcLine> inclFile              (MutString, bool, bool, long *pnlines);
local List<SrcLine> inclFileContents      (void);
local bool        inclLine              (List<SrcLine> *, InclIsContinuedFun);
local List<SrcLine> inclError             (Msg, ...);

/*
 * Include directives.
 */
local List<SrcLine> inclHandleDirective(void);

/*
 * Utilities
 */
# define           DIRECTIVE_CHAR      '#'  /* Character starting directives */

# define           INCLUDING(state)    ((state)==NoIf || (state)==ActiveIf)

local bool         inclIsDirective     (MutString line);
local MutString       inclCalcIndentLevel (MutString line, int *indent);
local MutString       inclGetLine         (FILE *);
local MutString       inclActiveFileChain (List<MutString>, String);

static ListCons<SrcLine> EIFC      = {0, 0};
static List<SrcLine>            EndifLine = &EIFC;        /* )endif indicator */


/******************************************************************************
 *
 * :: Includer state
 *
 *****************************************************************************/

List<MutString>      globalAssertList = 0;   /* Globally asserted properties */
List<MutString>      localAssertList  = 0;   /* Per-entry asserted properties */
long            inclSerialLineNo = 0;   /* The serial line number */
long            inclFileLineNo   = 1;   /* Filled in at end. */

Buffer          inclBuffer;             /* Input buffer */
MutString          curLineString;          /* The current line being processed */
IfState         ifState;                /* State of current )if */
FileState       fileState;              /* State of current file */
List<Hash>        includedFileCodes = 0;  /* Hash codes of all included files */


/******************************************************************************
 *
 * :: Top-level entry points
 *
 *****************************************************************************/

/*
 * Call either include "includeFile" or "includeLine".
 * If fin is stdin include one line.  Otherwise include the whole file.
 */
List<SrcLine>
include(FileName fn, FILE *fin, int *plineno, InclIsContinuedFun iscont)
{
        if (fn.isStdin() && fin)
                return includeLine(fn, fin, plineno, iscont);
        else
                return includeFile(fn);
}


/*
 * Return a processed source line list of the file contents.
 */
List<SrcLine>
includeFile(FileName fname)
{
        MutString        fnameString;
        List<SrcLine>   r;

        inclSerialLineNo    = 0;
        fnameString         = strCopy(fname.unparseStatic());
        inclBuffer          = Buffer::create();
        includedFileCodes   = 0;
        localAssertList     = listCopy<MutString>(globalAssertList);

        fileState.curDir    = osCurDirName();
        fileState.fileCodes = 0;
        fileState.fileNames = 0;

        r = listNReverse<SrcLine>(
                inclFile(fnameString, false, true, &inclFileLineNo));

        strFree(fnameString);
        inclBuffer.free();
        listFree<MutString>(localAssertList);
        listFree<Hash>(includedFileCodes);

        return r;
}

/*
 * Return a processed source line list for the next line of the file.
 * The source line list may have several entries if the line is an
 * includer directive or requires a continuation.
 */
List<SrcLine>
includeLine(FileName fn, FILE *fin, int *plineno, InclIsContinuedFun iscont)
{
        List<SrcLine>   r;

        inclBuffer          = Buffer::create();
        localAssertList     = globalAssertList;

        fileState.curDir    = osCurDirName();
        fileState.curFile   = strCopy(fn.unparseStatic());
        fileState.curFname  = fn;
        fileState.infile    = fin;
        fileState.fileCodes = 0;
        fileState.fileNames = 0;
        fileState.lineNumber= *plineno;

        ifState = NoIf;

        r = 0;
        inclLine(&r, iscont);
        r = listNReverse<SrcLine>(r);

        /* Leave the assert list and included files for the next call. */
        globalAssertList = localAssertList;

        inclBuffer.free();
        /* strFree(fileState.curFile);  !!This is held on to by the srclines.*/
        listFree<Hash>  (fileState.fileCodes);
        listFree<MutString>(fileState.fileNames);

        *plineno       = fileState.lineNumber;
        inclFileLineNo = fileState.lineNumber;

        return r;
}

/*
 * Return line counts from previous include.
 */
long
inclTotalLineCount(void)
{
        return inclSerialLineNo;
}

long
inclFileLineCount(void)
{
        return inclFileLineNo;
}

/*
 * Write included lines in a form suitable for re-inclusion.
 * The number of characters written is returned.
 */
int
inclWrite(FILE *file, List<SrcLine> sll)
{
        int      cc = 0;
        FileName slfile, curfile  = 0; /* Guarantee first line is #line ... */
        Length   slline, curline = 0;

        for ( ; sll; sll = cdr(sll)) {
                SrcLine sl = car(sll);

                if (sl.isSysCmd() && sl.sysCmdHandled()) continue;

                slfile = sl.position().file();
                slline = sl.position().line();
                if (curline != slline - 1
                ||  (curfile && !curfile.equals(slfile)))
                {
                        if ((curfile && curfile.equals(slfile)))
                                cc += fprintf(file, "%cline %d\n",
                                              DIRECTIVE_CHAR,
#if EDIT_1_0_n1_07
                                              (int) slline);
#else
                                              slline);
#endif
                        else {
                                cc += fprintf(file, "%cline %d \"%s\"\n",
                                              DIRECTIVE_CHAR,
#if EDIT_1_0_n1_07
                                              (int) slline,
#else
                                              slline,
#endif
                                              slfile.unparseStatic());
                                curfile = slfile;
                        }
                }
                curline = slline;

                cc += fprintf(file, "%*s%s", sl.indentation(), "", sl.text());
        }
        return cc;
}

/*
 * Free a list of source lines.
 */
void
inclFree(List<SrcLine> sll)
{
        listFreeDeeply<SrcLine>(sll, [](SrcLine line) { line.free(); });
}

local bool
inclStringEqual(MutString a, MutString b)
{
        return strEqual(a, b);
}

/*
 * Assertions for conditional includes.
 */
# define INCL_Assert(pl, p)   (*(pl)=listCons<MutString>((p), *(pl)))
# define INCL_Unassert(pl, p) (*(pl)=listNRemove<MutString>(*(pl),(p),inclStringEqual))
# define INCL_IsAssert(pl, p) (listMember<MutString>(*(pl), (p), inclStringEqual))

void
inclGlobalAssert(MutString property)
{
        INCL_Assert(&globalAssertList, property);
}

void
inclGlobalUnassert(MutString property)
{
        INCL_Unassert(&globalAssertList, property);
}

/******************************************************************************
 *
 * :: Recursive file inclusion
 *
 *****************************************************************************/

#define SysCmdLine(isHandled) \
        SrcLine::createSysCmd(SrcPos::create(fileState.curFname, \
                               fileState.lineNumber, \
                               inclSerialLineNo, \
                               1),\
                         0, curLineString, isHandled)

#define addSysCmd(sll,sl) listNConcat<SrcLine>\
        ((sll),listCons<SrcLine>(sl, listNil<SrcLine>))

#define botchSysCmd(kind) addSysCmd(inclError(ALDOR_E_SysCmdBad, kind), \
                                       SysCmdLine(true))

#define isThisEndifLine(sll) (sll && \
        (sll == EndifLine || car(listLastCons<SrcLine>(sll)).isEndifLine()))


local List<SrcLine>
inclFile(MutString fname, bool reincluding, bool top, long *pnlines)
{

        List<SrcLine>     sll;
        Hash            fhash;
        FileName        fn;
        FileState       o_fileState;
        Fluid fluidBinding_ifState(ifState);
        MutString          curdir;

        o_fileState          = fileState;     /* no fluid(struct) */
        ifState              = NoIf;
        fileState.lineNumber = 0;

        fn = inclFind(fname, fileState.curDir);

        if (fn != 0) {
                fileState.curDir   = strCopy(fn.dir());
                fileState.curFile  = strCopy(fn.unparseStatic());
                fileState.curFname = fn;
        }
        curdir = fileState.curDir;

        if (fn == 0) {
                fileState = o_fileState;
                if (top) {
                        comsgFatal(NULL, ALDOR_F_CantOpen, fname);
                        NotReached(sll = 0);
                }
                else
                        sll = inclError(ALDOR_F_CantOpen, fname);
        }
        else {
                fhash = fileHash(fn);
                fname = strCopy (fn.unparseStatic());

                if (!reincluding && listMemq<Hash>(includedFileCodes, fhash)) {
                        sll = listNil<SrcLine>;
                }
                else if (listMemq<Hash>(fileState.fileCodes, fhash)) {
                        MutString s = inclActiveFileChain
                                (fileState.fileNames, "->");
                        fileState = o_fileState;
                        sll = inclError(ALDOR_E_InclInfinite, s);
                        strFree(s);
                }
                else {
                        includedFileCodes   =
                           listCons<Hash>  (fhash,includedFileCodes);
                        fileState.fileCodes =
                           listCons<Hash>  (fhash,fileState.fileCodes);
                        fileState.fileNames =
                           listCons<MutString>(fname,fileState.fileNames);
                        fileState.infile    = fileRdOpen(fn);

                        sll = inclFileContents();

                        listFreeCons<Hash>  (fileState.fileCodes);
                        listFreeCons<MutString>(fileState.fileNames);
                        fclose(fileState.infile);
                }
                fn.free();
                strFree(curdir);
                                 /*!! curFile is used in src lines */
                strFree(fname);
        }
        if (pnlines) *pnlines = fileState.lineNumber;
        fileState = o_fileState;
        return sll;
}

/*
 * Open a file, looking first in the current directory, then
 * in the list of include directories.
 */
local FileName
inclFind(MutString fname, MutString curdir)
{
        List<MutString>    dl;
        FileName      fn;

        dl = listCons<MutString>(curdir, incSearchPath());
        fn = fileRdFind(dl, fname, FTYPE_SRC);
        listFreeCons<MutString>(dl);
        return fn;
}

/*
 * Conses (reversed) lines for the file.
 */
local List<SrcLine>
inclFileContents(void)
{
        List<SrcLine>     sll = listNil<SrcLine>;

        while (inclLine(&sll, (InclIsContinuedFun) NULL))
                ;

        return sll;
}

/*
 * Decide whether the line is an include or system command directive.
 */
local bool
inclIsDirective(MutString line)
{
        return *line == DIRECTIVE_CHAR;
}

/*
 * Conses (reversed) lines which arise from "including" this one line.
 * Returns true if there may be more.
 */
local bool
inclLine(List<SrcLine> *psll, InclIsContinuedFun isCont)
{
        int             indent;
        MutString          s;
        SrcLine         sl;
        SrcPos          spos;
        List<SrcLine>     d_sll;

        do {
                curLineString = inclGetLine(fileState.infile);
                if (!curLineString) {
                        if (ifState != NoIf)
                        *psll = listNConcat<SrcLine>
                        (inclError(ALDOR_E_InclIfEof), *psll);
                        return false;
                }
                fileState.lineNumber++;
                inclSerialLineNo++;
                if (inclIsDirective(curLineString)) {
                        /*!! This may be too costly with deep nesting. */
                        d_sll = inclHandleDirective();
                        if (isThisEndifLine(d_sll)) {
                                if (d_sll == EndifLine) return false;
                                *psll = listNConcat<SrcLine>(d_sll, *psll);
                                return false;
                        }
                        *psll = listNConcat<SrcLine>(d_sll, *psll);
                }
                else if (INCLUDING(ifState)) {
                        s = inclCalcIndentLevel(curLineString, &indent);
                        spos = SrcPos::create(fileState.curFname, fileState.lineNumber,
                                       inclSerialLineNo, 1);
                        sl = SrcLine::create(spos, indent, s);
                        *psll = listCons<SrcLine>(sl, *psll);
                }

        } while (isCont && (*isCont)(curLineString));

         return true;
}

local List<SrcLine>
inclError(Msg msg, ...)
{
        SrcPos  spos;
        va_list argp;

        spos = SrcPos::create(fileState.curFname,
                       fileState.lineNumber,
                       inclSerialLineNo,
                       1);

        va_start(argp, msg);
        comsgVError(abNewNothing(spos), msg, argp);
        va_end(argp);

        return listNil<SrcLine>;
}

/******************************************************************************
 *
 * :: Handle includer directives such as ")include file", etc.
 *
 *****************************************************************************/

local List<SrcLine> inclHandleReinclude   (MutString fname);
local List<SrcLine> inclHandleInclude     (MutString fname);
local List<SrcLine> inclHandleIncludeDir  (MutString dname);
local List<SrcLine> inclHandleAssert      (MutString property);
local List<SrcLine> inclHandleUnassert    (MutString property);
local List<SrcLine> inclHandleIf          (MutString property);
local List<SrcLine> inclHandleElseif      (MutString property);
local List<SrcLine> inclHandleElse        (void);
local List<SrcLine> inclHandleEndif       (void);
local List<SrcLine> inclHandleLine        (int lno, MutString fname);
local List<SrcLine> inclHandleUnknown     (void);

local List<SrcLine>
inclHandleDirective(void)
{
        MutString  s, s0, fname, property;
        int     lno;

        s = s0 = curLineString;

        if ((s = scmdIsDirective(s0,"include")) != 0) {
                if (!scmdScanFName(s, &fname)) return botchSysCmd("include");
                return inclHandleInclude(fname);
        }
        if ((s = scmdIsDirective(s0,"reinclude")) != 0) {
                if (!scmdScanFName(s, &fname)) return botchSysCmd("reinclude");
                return inclHandleReinclude(fname);
        }
        if ((s = scmdIsDirective(s0,"includeDir")) != 0) {
                if (!scmdScanFName(s, &fname)) return botchSysCmd("includeDir");
                return inclHandleIncludeDir(fname);
        }
        if ((s = scmdIsDirective(s0,"assert")) != 0) {
                if (!scmdScanId(s, &property)) return botchSysCmd("assert");
                return inclHandleAssert(property);
        }
        if ((s = scmdIsDirective(s0,"unassert")) != 0) {
                if (!scmdScanId(s, &property)) return botchSysCmd("unassert");
                return inclHandleUnassert(property);
        }
        if ((s = scmdIsDirective(s0,"if")) != 0) {
                if (!scmdScanId(s, &property)) return botchSysCmd("if");
                return inclHandleIf(property);
        }
        if ((s = scmdIsDirective(s0,"elseif")) != 0) {
                if (!scmdScanId(s, &property)) return botchSysCmd("elseif");
                return inclHandleElseif(property);
        }
        if ((s = scmdIsDirective(s0,"endif")) != 0) {
                return inclHandleEndif();
        }
        if ((s = scmdIsDirective(s0,"else")) != 0) {
                return inclHandleElse();
        }
        if ((s = scmdIsDirective(s0,"line")) != 0) {
                if ((s = scmdScanInteger(s, &lno)) == 0)
                        return botchSysCmd("line");
                if (!scmdScanFName(s, &fname))  fname = 0;
                return inclHandleLine(lno, fname);
        }

        return inclHandleUnknown();
}

/*
 * Handle the include directive if we are in an active section
 */
local List<SrcLine>
inclHandleReinclude(MutString fname)
{
        if (INCLUDING(ifState)) {
                SrcLine sl = SysCmdLine(true);
                return addSysCmd(inclFile(fname, true, false, NULL), sl);
        }
        return listNil<SrcLine>;
}

/*
 * Handle the include directive if we are in an active section
 */
local List<SrcLine>
inclHandleInclude(MutString fname)
{
        if (INCLUDING(ifState)) {
                SrcLine sl = SysCmdLine(true);
                return addSysCmd(inclFile(fname, false, false, NULL), sl);
        }
        return listNil<SrcLine>;
}

local List<SrcLine>
inclHandleIncludeDir(MutString dname)
{
        if (INCLUDING(ifState)) {
                SrcLine sl = SysCmdLine(true);
                if (scmdHandleIncludeDir(dname) == -1)
                        return botchSysCmd("includeDir");
                return addSysCmd(listNil<SrcLine>, sl);
        }
        return listNil<SrcLine>;
}

local List<SrcLine>
inclHandleAssert(MutString property)
{
        if (INCLUDING(ifState)) {
                SrcLine sl = SysCmdLine(true);
                INCL_Assert(&localAssertList, property);
                return addSysCmd(listNil<SrcLine>, sl);
        }
        return listNil<SrcLine>;
}

local List<SrcLine>
inclHandleUnassert(MutString property)
{
        if (INCLUDING(ifState)) {
                SrcLine sl = SysCmdLine(true);
                INCL_Unassert(&localAssertList, property);
                return addSysCmd(listNil<SrcLine>, sl);
        }
        return listNil<SrcLine>;
}

local List<SrcLine>
inclHandleIf(MutString property)
{

        Fluid fluidBinding_ifState(ifState);
        List<SrcLine>     result = listNil<SrcLine>;

        if (INCLUDING(ifState)) {
                SrcLine sl = SysCmdLine(true);

                if (INCL_IsAssert(&localAssertList, property)) {
                        ifState = ActiveIf;
                        result = addSysCmd(inclFileContents(),sl);
                }
                else {
                        ifState = InactiveIf;
                        result = addSysCmd(inclFileContents(),sl);
                }
        }
        else {
                ifState = FormerlyActiveIf;
                result = inclFileContents();
        }
        return result;
}

local List<SrcLine>
inclHandleElseif(MutString property)
{
        SrcLine sl = SysCmdLine(true);
        if (ifState == NoIf)
                return addSysCmd(inclError(ALDOR_E_InclUnbalElseif), sl);
        if (ifState == InactiveIf) {
                if (INCL_IsAssert(&localAssertList, property))
                        ifState = ActiveIf;
                return addSysCmd(listNil<SrcLine>, sl);
        }
        else {
                if (ifState == ActiveIf) {
                        ifState = FormerlyActiveIf;
                        return addSysCmd(listNil<SrcLine>, sl);
                }
                ifState = FormerlyActiveIf;
        }
        return listNil<SrcLine>;
}

local List<SrcLine>
inclHandleElse(void)
{
        SrcLine sl = SysCmdLine(true);
        if (ifState == NoIf)
                return addSysCmd(inclError(ALDOR_E_InclUnbalElse), sl);
        else if (ifState == ActiveIf) {
                ifState = InactiveIf;
                return addSysCmd(listNil<SrcLine>, sl);
        }
        else if (ifState == InactiveIf) {
                ifState = ActiveIf;
                return addSysCmd(listNil<SrcLine>, sl);
        }
        return listNil<SrcLine>;
}

local List<SrcLine>
inclHandleEndif(void)
{
        SrcLine sl = SysCmdLine(true);
        sl.setEndifLine();
        if (ifState == NoIf)
                return addSysCmd(inclError(ALDOR_E_InclUnbalEndif), sl);
        if (ifState == ActiveIf || ifState == InactiveIf)
                return addSysCmd(listNil<SrcLine>, sl);
        return EndifLine;
}

local List<SrcLine>
inclHandleLine(int lno, MutString fname)
{
        if (INCLUDING(ifState)) {

                fileState.lineNumber = lno - 1; /* The next line is 'lno' */

#if EDIT_1_0_n2_09
                if (fname) {
                  fileState.curFile = fname;
                  /* rhx: We trust the programmer of the #line statement
                     in the .as file that the filename is correct.
                     If in an error case the file cannot be found the
                     compiler aborts with a (Fatal Error) message.

                     #1 (Fatal Error) Could not open file `dir/file.ext' with mode `r'.

                     It would actually be very helpful for a
                     programmer if we could abort the compilation
                     already here with an error message saying that
                     the filename appearing in the #line directive
                     cannot be found.
                  */
                  fileState.curFname = FileName::parse(fname);
                }
                  SrcPos::registerFileLine(fileState.curFname, fileState.lineNumber,
                                     inclSerialLineNo);
#else
                if (fname) {
                  fileState.curFile = fname;
                  fileState.curFname = inclFind(fname, fileState.curDir);
                }
                else
                  SrcPos::registerFileLine(fileState.curFname, fileState.lineNumber,
                                     inclSerialLineNo);
#endif


                return listNil<SrcLine>;
        }
        return listNil<SrcLine>;
}

local List<SrcLine>
inclHandleUnknown(void)
{
        if (INCLUDING(ifState)) {
                SrcLine sl  = SysCmdLine(false);
                return addSysCmd(listNil<SrcLine>, sl);
        }
        else {
                SrcPos spos = SrcPos::create(fileState.curFname,
                                      fileState.lineNumber,
                                      inclSerialLineNo,
                                      1);
                scmdCheck(spos, curLineString);
        }
        return listNil<SrcLine>;
}

/******************************************************************************
 *
 * :: General Utilities
 *
 ****************************************************************************/

local MutString
inclCalcIndentLevel(MutString ln, int *indent)
{
        MutString s;
        int i;
        for (s = ln, i = 0; ; s++) {
                if (*s == ' ')
                        i++;
                else if (*s == '\t') {
                        if (i % TABSTOP == 0) i += TABSTOP;
                        else i = ROUND_UP(i, TABSTOP);
                }
                else
                        break;
        }
        *indent = i;
        return s;
}


local MutString
inclGetLine(FILE *file)
{
        int     c;
        MutString  s;

        BUF_START(inclBuffer);
        while ((c = osGetc(file)) != EOF) {
                BUF_ADD1(inclBuffer, c);
                if (c == '\n') break;
        }
        BUF_ADD1(inclBuffer, char0);

        s = inclBuffer.chars();
        if (c == EOF && *s == 0) return 0;
        return s;
}

/*
 * Given the list fnames [z,...,b,a] and punct "->",
 * allocate the string "'a'->'b'->...->'z'".
 */
local MutString
inclActiveFileChain(List<MutString> fnames, String punct)
{
        List<MutString> sl0, sl;
        Buffer     buf;
        MutString     s;
        int        i;

        buf = Buffer::create();
        sl0 = listReverse<MutString>(fnames);

        for (sl = sl0, i = 0; sl; sl = cdr(sl), i++) {
                if (i > 0) buf.puts(punct);
                buf.printf("'%s'", car(sl));
        }
        s = buf.liberate();
        listFree<MutString>(sl0);

        return s;
}
