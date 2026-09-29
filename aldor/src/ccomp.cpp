///////////////////////////////////////////////////////////////////////////////
//
// ccomp.cpp: C compiler interface.
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

#include "axlphase.h"
#include "compcfg.h"
/*
 * Default compiler and linker.
 * They can be overridden by the env. variable CC or the option * -Ccc=....
 * They can be overridden individually with  -Ccomp=... and -Clink-....
 */
#define CC_DEFAULT      "unicl"
#define LD_DEFAULT      "unicl"
#define RUNTIME_DEFAULT "foam"

/*****************************************************************************
 *
 * :: C compiler + Linker Interface
 *
 ****************************************************************************/

enum ccoptTag {
    CCOPT_START,
        CCOPT_IncPath = CCOPT_START,
        CCOPT_Define,
        CCOPT_Undefine,
        CCOPT_CompileOnly,
        CCOPT_Optimize,
        CCOPT_FNonStd,
        CCOPT_DebugInfo,
        CCOPT_ProfileInfo,
        CCOPT_LibPath,
        CCOPT_Library,
        CCOPT_OutputFile,
        CCOPT_System,
        CCOPT_Fortran,
        CCOPT_Runtime,
    CCOPT_LIMIT
};

struct ccOption {
        enum ccoptTag   tag;
        MutString          arg;
        struct ccOption *next;
};

struct ccOptionInfo {
        enum ccoptTag   tag;
        String     text;

        BPack(bool)     isComp;
        BPack(bool)     isLink;
        BPack(bool)     isDir;
        BPack(bool)     isFile;
};

/*
 * These options are valid on Unix C compilers *and* the "unicl" wrapper.
 * The extra -WV option for "unicl" is passed via the environment.
 * [As we make a vague attempt to call a standard C compiler, we can't
 *  pass it through the command line
 */
struct ccOptionInfo ccOptionTable[] = {
        /*  Tag            Text        C L D F */
        {CCOPT_IncPath,    "-I",       1,0,1,0},
        {CCOPT_Define,     "-D",       1,0,0,0},
        {CCOPT_Undefine,   "-U",       1,0,0,0},
        {CCOPT_CompileOnly,"-c",       1,0,0,0},
        {CCOPT_Optimize,   "-O",       1,1,0,0},
        /* -Wfnonstd is not a valid option to gcc or to ld anymore */
        {CCOPT_FNonStd,    "-Wfnonstd",0,0,0,0},
        {CCOPT_DebugInfo,  "-g",       1,1,0,0},
        {CCOPT_ProfileInfo,"-p",       1,1,0,0},
        {CCOPT_LibPath,    "-L",       0,1,1,0},
        {CCOPT_Library,    "-l",       0,1,0,0},
        {CCOPT_OutputFile, "-o ",      0,1,0,1},   /* Note trailing blank. */
        {CCOPT_System,     "-Wsys=",   1,1,0,0},
};

#define ccIsCompilerOption(tag) (ccOptionTable[tag].isComp)
#define ccIsLinkerOption(tag)   (ccOptionTable[tag].isLink)
#define ccIsDirOption(tag)      (ccOptionTable[tag].isDir)
#define ccIsFileOption(tag)     (ccOptionTable[tag].isFile)


/****************************************************************************
 *
 * :: Option Handling
 *
 ***************************************************************************/

local void ccPushLibrary        (String);

local void      ccPushOptionList(enum ccoptTag, String);
local void      ccPushOption    (enum ccoptTag, MutString);

/*
 * Remember to change comsgdb.msg if these change.
 */
#define COPT_STANDARDC  "standard"
#define COPT_OLDC       "old"
#define COPT_IDHASH     "idhash"
#define COPT_NOIDHASH   "no-idhash"
#define COPT_LINENOS    "lines"
#define COPT_NOLINENOS  "no-lines"
#define COPT_COMP       "comp"
#define COPT_LIB        "lib"
#define COPT_LINK       "link"
#define COPT_CC         "cc"
#define COPT_ARGS       "args"
#define COPT_GO         "go"
#define COPT_SMAX       "smax"
#define COPT_IDLEN      "idlen"
#define COPT_FNAME      "fname"
#define COPT_SYS        "sys"
#define COPT_FORTRAN    "fortran"
#define COPT_RUNTIME    "runtime"

/*
 * Control C Code generation and compilation.
 */
String     ccCompiler   = 0;
String     ccLinker     = 0;
String     ccGoer       = 0;
String     ccOptions    = 0;
String     ccSystem     = 0;
String     ccRuntime    = 0;
bool            ccFortran    = 0;
bool            ccVerboseFlag= false;
bool            ccLineNosFlag= false;
int             ccDoStandardCFlag = -1;
List<MutString>      ccLibraries;

local void ccSetStandardC(bool flg);
local void ccAddExtraOptions(String s);

int
ccOption(MutString opt)
{
        String s;

        if (!*opt) return -1;                  /* Must have letters */

        if      (strAEqual(opt, COPT_STANDARDC))        ccSetStandardC(true);
        else if (strAEqual(opt, COPT_LINENOS))          ccSetLineNos(true);
        else if (strAEqual(opt, COPT_NOLINENOS))        ccSetLineNos(false);
        else if (strAEqual(opt, COPT_OLDC))             ccSetStandardC(false);
        else if (strAEqual(opt, COPT_IDHASH))           genCSetIdHash(true);
        else if (strAEqual(opt, COPT_NOIDHASH))         genCSetIdHash(false);
        else if (strAEqual(opt, COPT_FORTRAN))          ccFortran = true;
        else if ((s=strAIsPrefix(COPT_CC,opt))     !=0) ccCompiler=ccLinker=++s;
        else if ((s=strAIsPrefix(COPT_ARGS,opt))   !=0) ccAddExtraOptions(++s);
        else if ((s=strAIsPrefix(COPT_COMP,opt))   !=0) ccCompiler= ++s;
        else if ((s=strAIsPrefix(COPT_LINK,opt))   !=0) ccLinker  = ++s;
        else if ((s=strAIsPrefix(COPT_GO,opt))     !=0) ccGoer    = ++s;
        else if ((s=strAIsPrefix(COPT_SMAX,opt))   !=0) genCSetSMax(atoi(++s));
        else if ((s=strAIsPrefix(COPT_IDLEN,opt))  !=0) genCSetIdLen(atoi(++s));
        else if ((s=strAIsPrefix(COPT_FNAME,opt))  !=0) emitSetCName(++s);
        else if ((s=strAIsPrefix(COPT_SYS,opt))    !=0) ccSystem  = ++s;
        else if ((s=strAIsPrefix(COPT_RUNTIME,opt))!=0) ccRuntime = ++s;
        else if ((s=strAIsPrefix(COPT_LIB,opt))    !=0) ccPushLibrary(++s);
        else return -1;

        return 0;
}

/*
 * Accumulate extra options to pass to the C compiler.
 */
local void
ccAddExtraOptions(String opts) {
    if (!ccOptions)
        ccOptions = opts;
    else
        ccOptions = strlConcat(ccOptions, " ", opts, (String) NULL);
}

/*
 * Option list to pass to the C compiler.
 * If the option has no arguments, arg should be NULL.
 */
struct ccOption *ccOptionList = 0;

local void
ccPushOptionList(enum ccoptTag opt, String args)
{
        MutString buf = strCopy(args);
        char *ptr   = buf;
        char *start = buf;
        while (*ptr != '\0') {
                if (*ptr == ',') {
                        *ptr = '\0';
                        ccPushOption(opt, start);
                        start = ptr + 1;
                }
                ptr++;
        }

        if (start != ptr)
                ccPushOption(opt, start);

}

local void
ccPushOption(enum ccoptTag opt, MutString arg)
{
        struct ccOption *newOpt;

        newOpt = (struct ccOption *) stoAlloc(OB_Other, sizeof(*newOpt));
        newOpt->tag  = opt;
        newOpt->arg  = arg;
        newOpt->next = ccOptionList;

        ccOptionList = newOpt;
}

local void
ccReverseOptions(void)
{
        struct ccOption *t, *r = 0, *l = ccOptionList;

        while(l) {
                t = l->next;
                l->next = r;
                r = l;
                l = t;
        }
        ccOptionList = r;
}

void
ccSetVerbose(bool wantVerbose)
{
        ccVerboseFlag = wantVerbose;
}

void
ccSetDebug(bool isWanted)
{
        if (isWanted) ccPushOption(CCOPT_DebugInfo, NULL);
}

void
ccSetProfile(bool isWanted)
{
        if (isWanted) ccPushOption(CCOPT_ProfileInfo, NULL);
}

void
ccSetOptimize(bool isNonStd,bool isWanted)
{
        if (isWanted) ccPushOption(CCOPT_Optimize, NULL);
        if (isNonStd) ccPushOption(CCOPT_FNonStd,NULL);
}

void
ccSetOutputFile(MutString fn)
{
        ccPushOption(CCOPT_OutputFile, fn);
}


void
ccSetLineNos(bool flg)
{
        ccLineNosFlag = flg;
}

bool
ccLineNos()
{
        return ccLineNosFlag;
}

local void
ccSetStandardC(bool flg)
{
        ccDoStandardCFlag = flg;
}

local void
ccPushLibrary(String string)
{
        ccLibraries = listCons<MutString>(strCopy(string), ccLibraries);
}

bool
ccDoStandardC()
{
        if (ccDoStandardCFlag == -1)
                ccDoStandardCFlag = compCfgLookupBoolean("generate-stdc");

        return ccDoStandardCFlag;
}

/*****************************************************************************
 *
 * :: Command formulation
 *
 ****************************************************************************/

static Buffer ccBuf;
static int    ccPutc    (int c)    { return ccBuf.putc(c); }
static void   ccPutq    (String s) { osRunQuoteArg(s, ccPutc); }
static void   ccPuts    (String s) { ccBuf.puts(s); }
static void   ccPutOpt            (struct ccOption *, MutString newwd, MutString oldwd);
static void   ccPutFname          (FileName,          MutString newwd, MutString oldwd);
local  void   ccAppendToHiddenArgs(String str);

/*
 * Construct a compile command suitable to run in the directory "newwd".
 * The paths and file names (including "newwd") are given relative to "oldwd".
 */
MutString
ccCompileCommand(String compiler, String options, int numFiles, FileName *fns,
                 struct ccOption *opts,
                 MutString newwd, MutString oldwd)
{
        struct ccOption *o;
        int              i;

        ccBuf = Buffer::create();

        if (!compiler) compiler = CC_DEFAULT;

        ccPutq(compiler);
        if (options) {ccPutc(' '); ccPuts(options);} /* Don't quote options */

        for (o = opts; o; o = o->next)
                if (ccIsCompilerOption(o->tag)) ccPutOpt(o, newwd, oldwd);

        ccPutc(' '); ccPutq(ccOptionTable[CCOPT_CompileOnly].text);

        for(i = 0; i < numFiles; i++) ccPutFname(fns[i], newwd, oldwd);

        return ccBuf.liberate();
}

MutString
ccLinkCommand(String linker, String options, int numFiles, FileName *fns,
              struct ccOption *opts,
              MutString newwd, MutString oldwd)
{
        struct ccOption *o;
        int              i;

        ccBuf = Buffer::create();

        if (!linker) linker = LD_DEFAULT;

        ccPutq(linker);
        if (options) {ccPutc(' '); ccPuts(options);} /* Don't quote options */

        for(i = 0; i < numFiles; i++)
                ccPutFname(fns[i], newwd, oldwd);

        for(o = opts; o; o = o->next)
                if (o->tag == CCOPT_LibPath)
                        ccPutOpt(o, newwd, oldwd);
        for(o = opts; o; o = o->next)
                if (ccIsLinkerOption(o->tag) && o->tag != CCOPT_LibPath)
                        ccPutOpt(o, newwd, oldwd);

        return ccBuf.liberate();
}

local void
ccPutOpt(struct ccOption *o, MutString newwd, MutString oldwd)
{
        ccPutc(' ');
        ccPuts(ccOptionTable[o->tag].text);

        if (!o->arg) return;

        /*
         * File options are named in the caller's directory, while the
         * compiler command is executed after ccSwapDir(newwd).  Translate
         * them even when newwd is now the process current directory.
         * In particular, -o <newwd>/prog should become -o prog.
         */
        if (ccIsFileOption(o->tag))
                ccPutFname(FileName::parseStatic(o->arg), newwd, oldwd);
        else if (strEqual(newwd, osCurDirName()))
                ccPutq(o->arg);
        else if (ccIsDirOption(o->tag)) {
                MutString s = fileSubdir(oldwd, o->arg);
                ccPutq (s);
                strFree(s);
        }
        else
                ccPutq(o->arg);
}

local void
ccPutFname(FileName fn, MutString newwd, MutString oldwd)
{
        MutString  s;

        ccPutc(' ');

        if (strEqual(fn.dir(), newwd)) {
                s = fn.unparseStaticWithout();
                ccPutq(s);
        }
        else {
                MutString odir = fn.dir();
                fn.setDir(fileSubdir(oldwd, odir));
                s = fn.unparseStatic();
                ccPutq(s);
                fn.setDir(odir);
        }
}

/******************************************************************************
 *
 * :: Command issue
 *
 *****************************************************************************/

local MutString ccOutputDir(MutString, struct ccOption *);
local void   ccSwapDir  (MutString, MutString, Length);
local void   ccEchoIf   (String fmt, String arg);

/*
 * Reconcile the options and initialize the commands.
 */

void
ccGetReady(void)
{
        PathList l;
        List<MutString> tmp;
        String env;
        MutString   s;
        static bool     isReady = false;

        if (isReady) return;

        if (!ccRuntime)
                ccRuntime = RUNTIME_DEFAULT;

        if (!ccSystem) ccSystem = compCfgGetSysName();

        if (!strEqual(ccSystem, CONFIGSYS)) {

                /* Treat paths as stacks -- newer covers older. */
                for(l = libSearchPathDelta(); l; l = cdr(l)) {
                        /* !!! This is UNIX (+ DOS) Specific */
                        MutString s0, s1;
                        s0 = strConcat(car(l), "/");
                        s1 = strConcat(s0, ccSystem);
                        strFree(s0);
                        ccPushOption(CCOPT_LibPath, s1);
                }
        }
        for(l = libSearchPathDelta(); l; l = cdr(l))
                ccPushOption(CCOPT_LibPath, car(l));
        for(l = incSearchPathDelta(); l; l = cdr(l))
                ccPushOption(CCOPT_IncPath, car(l));

        for(l = arLibraryKeys(); l; l = cdr(l))
                ccPushOption(CCOPT_Library, car(l));

        tmp = listReverse<MutString>(ccLibraries);
        for(l = tmp; l; l = cdr(l))
                ccPushOption(CCOPT_Library, car(l));
        listFree<MutString>(tmp);

        ccPushOptionList(CCOPT_Library, ccRuntime);

        /*
         * A direct C toolchain does not pass through unicl, which historically
         * supplied target-system libraries such as libm after the Aldor runtime.
         * Keep these distinct from -Clib: user libraries precede the runtime,
         * while system libraries must follow it for one-pass static linking.
         * The value is a comma-separated library-name list, without -l.
         */
        env = osGetEnv("ALDOR_SYSTEM_LIBS");
        if (env && *env)
                ccPushOptionList(CCOPT_Library, env);

        ccReverseOptions();

        env = osGetEnv("CC");
        if (env) {
                if (!ccCompiler) ccCompiler = strCopy(env);
                if (!ccLinker)   ccLinker   = strCopy(env);
        }
        env = osGetEnv("CGO");
        if (env) {
                if (!ccGoer)     ccGoer     = strCopy(env);
        }

        if (ccVerboseFlag)
                ccAppendToHiddenArgs("-WV");

        if (ccFortran)
                ccAppendToHiddenArgs("-Wfortran");

        s = strConcat("-Wsys=", ccSystem);
        ccAppendToHiddenArgs(s);
        strFree(s);

        if (ccDoStandardC())
                ccAppendToHiddenArgs("-Wstdc");

        isReady = true;
}

local void
ccAppendToHiddenArgs(String str)
{
        String oldArgs;
        MutString s;
        oldArgs = osGetEnv("UNICL");
        oldArgs = oldArgs ? oldArgs : "";
        if (oldArgs[0] != '\0')
                str = strConcat(str, " ");
        s = strConcat(str, oldArgs);
        s = strConcat("UNICL=", s);
        osPutEnv(s);
        if (!osPutEnvIsKept()) strFree(s);
}

/*
 * Compile a single C file.
 * Have to assume output will go in the current directory of the compile.
 */
void
ccCompileFile(MutString newwd, FileName fn)
{
        MutString  command;
        char    oldwd[1024];
        int     rc;

        newwd = ccOutputDir(newwd, NULL /* ccOptionList */);
        ccSwapDir(newwd, oldwd, sizeof(oldwd));

        command = ccCompileCommand(ccCompiler, ccOptions, 1, &fn,
                                   ccOptionList, newwd, oldwd);

        ccEchoIf("Exec: %s\n", command);
        rc = osRun(command);
        if (rc != 0) comsgFatal(NULL, ALDOR_F_CcFailed, command);

        ccSwapDir(oldwd, NULL, int0);
        strFree(command);
        strFree(newwd);
}

/*
 * Link a program.
 * Have to assume output will go in the current directory of the link.
 */
void
ccLinkProgram(MutString newwd, FileName *files, int numFiles)
{
        MutString  command;
        char    oldwd[1024];
        int     rc;

        newwd = ccOutputDir(newwd, ccOptionList);
        ccSwapDir(newwd, oldwd, sizeof(oldwd));

        command = ccLinkCommand(ccLinker, ccOptions, numFiles, files,
                                ccOptionList, newwd, oldwd);

        ccEchoIf("Exec: %s\n", command);
        rc = osRun(command);
        if (rc != 0) comsgFatal(NULL, ALDOR_F_LinkFailed, command);

        /*
         * A successful linker invocation must have produced the requested
         * executable in the link directory.  Check this before changing
         * directory so a driver/path disagreement is reported at its source
         * instead of later as a mysterious -grun ENOENT.
         */
        {
                struct ccOption *o;
                for (o = ccOptionList; o; o = o->next) {
                        if (o->tag == CCOPT_OutputFile && o->arg) {
                                FileName requested = FileName::parseStatic(o->arg);
                                FileName localOut = FileName::create(
                                        "", requested.name(), requested.type());
                                if (!fileIsThere(localOut)) {
                                        fprintf(osStderr,
                                                "Aldor: linker returned success but output '%s' "
                                                "was not created in '%s'.\n",
                                                o->arg, osCurDirName());
                                        fprintf(osStderr, "Aldor: link command: %s\n",
                                                command);
                                        comsgFatal(NULL, ALDOR_F_LinkFailed, command);
                                }
                                localOut.free();
                                break;
                        }
                }
        }

        ccSwapDir(oldwd, NULL, int0);
        strFree(command);
        strFree(newwd);
}

/*
 * Run a program
 */
int
ccGoProgram(FileName fn, int argc1, MutString *argv1)
{
        int     i;
        int     rc;
        Buffer  cmdbuf = Buffer::create();

        /* emitLink has already verified this invariant.  Check it once more
         * at the execution boundary so an output-path or cleanup regression
         * is diagnosed by Aldor rather than reported later by the shell. */
        if (!fileIsThere(fn)) {
                MutString name = fn.unparse();
                fprintf(osStderr,
                        "Aldor: executable is missing immediately before run: %s\n",
                        name);
                strFree(name);
                cmdbuf.free();
                return 127;
        }

        if (ccGoer)
                cmdbuf.printf("%s ", ccGoer);

        cmdbuf.printf("'%s'", fn.unparseStaticWith());

        for (i = 0; i < argc1; i++)
                cmdbuf.printf("  '%s'", argv1[i]);

        ccEchoIf("Exec: %s\n", cmdbuf.chars());

        rc = osRun(cmdbuf.chars());

        cmdbuf.free();
        return rc;
}

/*
 * Find the directory in which the linked program will be produced.
 */
local MutString
ccOutputDir(MutString newwd, struct ccOption *opts)
{
        MutString  ofiledir = 0;
        FileName fn;

        while (opts) {
                if (opts->tag == CCOPT_OutputFile) {
                        if (ofiledir) strFree(ofiledir);
                        fn = FileName::parseStatic(opts->arg);
                        ofiledir = strCopy(fn.dir());
                }
                opts = opts->next;
        }
        if (!ofiledir)
                ofiledir = strCopy(newwd);
        return ofiledir;
}

/*
 * Change to the desired directory.
 */
local void
ccSwapDir(MutString newwd, MutString oldwdbuf, Length oldwdsize)
{
        int rc;

        if (strEqual(newwd, osCurDirName()) || strEqual(newwd, "")) {
                if (oldwdbuf) strcpy(oldwdbuf, osCurDirName());
        }
        else {
                ccEchoIf("Cd:   %s\n", newwd);
                rc = osDirSwap(newwd, oldwdbuf, oldwdsize);
                if (rc != 0) comsgFatal(NULL, ALDOR_F_CdFailed, newwd);
        }
}

local void
ccEchoIf(String fmt, String arg)
{
        if (ccVerboseFlag) { fprintf(osStdout, fmt, arg); fflush(osStdout); }
}
