/******************************************************************************
 *
 * testaldor.c
 *
 * Test driver for the Aldor Compiler.
 * Totally rewritten SMW Feb-May 2021.
 *
 * (C) Copyright 2021 Stephen M. Watt. All rights reserved.
 *
 *****************************************************************************/

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>

#include "util.h"

/******************************************************************************
 *
 * Declarations of things to come.
 *
 *****************************************************************************/

/*
 * Global variables
 */


/* These are set once and for all here. */
MutString Aldor     = "aldor";   /* Aldor program name. */
MutString CC        = "unicl";   /* Common C compile/link driver. */

/*
 * These are set in testAldorInit.
 */
MutString  OAldor;    /* Old Aldor program name. */
MutString  Kind;      /* Test kinds to run. */
Int	    KindAll;   /* 1 => all of given kind, 0 => all except given kind. */
MutString  Action;    /* What to do with each test. */
Int     NoDiff;    /* 1 => don't show diffs. */
Int     KeepTemps; /* 1 => don't delete temp files . */
MutString  InitDir;   /* Initial directory. */
MutString	OutDir;	   /* Test output directory. */
MutString	RefDir;	   /* Reference output directory. */


/*
 * To record list of failed files.
 */
#define		BUFLEN			100000
Int 		failc = 0;		  /* Number of failed tests. */
MutString		failv[BUFLEN];	  /* Vector of failed tests. */

/*
 * Aldor test functions
 */
void		testAldorHelp		_OF((void));
void		testAldorClean		_OF((MutString fn));
void		testAldorScript		_OF((MutString fn));
void		testAldorDispatch	_OF((MutString args, MutString fn));
void		testAldorCompile	_OF((MutString args, MutString fn));
void		testAldorRun		_OF((MutString args, MutString fn));
void		testAldorInterpret	_OF((MutString args, MutString fn));
void		testAldorErrors		_OF((MutString args, MutString fn));
void		testAldorPhase		_OF((MutString args, MutString fn));
void		testAldorGenerate	_OF((MutString args, MutString fn));

/*
 * Aldor test top level
 */
Length		testAldorInit		_OF((Length, MutString *));
void		testAldorFini		_OF((int));
void		testAldorGlean		_OF((Length, MutString *));
Int 		testAldorFilter		_OF((MutString));
int         main                _OF((int, char **));


/*
 * Frames of related files for a test
 */
#define NFSET_MAX 5

struct fset {
    MutString      extn;
    MutString      nameNoDir;
    MutString      nameInRef;
    MutString      nameInOut;
};
struct fframe {
    MutString      baseName;
    MutString      srcDir;
    MutString      srcNameInInit;
    MutString      srcNameNoDir;
    int         setc;
    struct fset setv[NFSET_MAX];
};

struct fframe *fframeAlloc          _OF((MutString fn, MutString srcExt, int setc, ...));
void           fframeRemoveOutFiles _OF((struct fframe *pff));

/*
 * Common parts for test functions.
 */
void reportStart   _OF((MutString name, MutString kind));
void reportResult  _OF((MutString src, int wasErr, MutString outOld, MutString outNew));
int  runWrapped    _OF((MutString cmd, MutString outFNameOrNull));



/******************************************************************************
 *
 * Aldor test top level
 *
 *****************************************************************************/


int
main(int argc, char **argv) {
	Length	i;

	i = testAldorInit(argc, argv);
	argc -= i;
	argv += i;

    debugPrintf("Starting testaldor....\n");

	testAldorGlean(argc, argv);

	if (failc == 0) {
		fprintf(stdout, "== >> No tests failed\n");
        testAldorFini(0);
	}
	else {
		fprintf(stdout, "== >> Failed tests:");
		for (i = 0; i < failc; i += 1)
			fprintf(stdout, " %s", failv[i]);
		fnewline(stdout);
        testAldorFini(1);
	}

    debugPrintf("Finishing testaldor....\n");
    return 0;
}

Length
testAldorInit(Length argc, MutString *argv) {
	Length	i;

    InitDir     = 0;
    OutDir      = 0;
    RefDir      = 0;

    OAldor      = "aldor";
    Kind        = "all";
    KindAll     = 1;
    Action      = "compare";
    NoDiff      = 0;
    KeepTemps   = 0;
    LogFileName = 0;

    if (DoDebug) {
        debugPrintf("Invoked with arguments: [");
        /* Use regular printf here to get it on one line. */
        for (i = 0; i < argc; i++) {
            if (i > 0) fprintf(stderr, ", ");
            fprintf(stderr, "'%s'", argv[i]);
        }
        fprintf(stderr, "]\n");
    }

	for (i = 1; i < argc; i += 1) {
		if (strEqual(argv[i], "-help")) {
			testAldorHelp();
			exitSuccess();
		}
		else if (strEqual(argv[i], "-compare"))
			Action = "compare";
		else if (strEqual(argv[i], "-install"))
			Action = "install";
		else if (strEqual(argv[i], "-show"))
			Action = "show";
		else if (strEqual(argv[i], "-nodiff"))
			NoDiff = 1;
		else if (strEqual(argv[i], "-oldaldor"))
			OAldor = strCopy(argv[++i]);
		else if (strEqual(argv[i], "-only")) {
			Kind    = strCopy(argv[++i]);
			KindAll = 1;
		}
		else if (strEqual(argv[i], "-but")) {
			Kind = strCopy(argv[++i]);
			KindAll = 0;
		}
		else if (strEqual(argv[i], "-debug")) {
			DoDebug     = 1;
        }
		else if (strEqual(argv[i], "-refdir"))
			RefDir = strCopy(argv[++i]);
		else if (strEqual(argv[i], "-outdir"))
			OutDir = strCopy(argv[++i]);
		else if (strEqual(argv[i], "-keep"))
			KeepTemps = 1;
		else
			break;
	}

    if (!InitDir) {
        Length	cc = BUFLEN, rc;
        MutString	buf = strAlloc(cc);
        rc = osGetCurDir(buf, cc);
        if (rc != 0) exitFailure();
        InitDir = strCopy(buf);
    }
    if (!OutDir) {
        OutDir = osTempDirName();
        osMakeDir(OutDir);
    }
	if (!RefDir)
        RefDir = osFileCombine("..", "testout");
    if (DoDebug)
        LogFileName = osFileCombine(OutDir, "testaldor.log");


	if (signal(SIGINT,  testAldorFini) == SIG_ERR) exitFailure();
	if (signal(SIGABRT, testAldorFini) == SIG_ERR) exitFailure();
	if (signal(SIGTERM, testAldorFini) == SIG_ERR) exitFailure();
#if SIGQUIT != SIGFAKE
	if (signal(SIGQUIT, testAldorFini) == SIG_ERR) exitFailure();
#endif

    if (DoDebug) {
        debugPrintf("InitDir     = '%s'\n", InitDir);
        debugPrintf("OutDir      = '%s'\n", OutDir);
        debugPrintf("RefDir      = '%s'\n", RefDir);
        debugPrintf("OAldor      = '%s'\n", OAldor);
        debugPrintf("Kind        = '%s'\n", Kind);
        debugPrintf("KindAll     = %d\n",   KindAll);
        debugPrintf("Action      = '%s'\n", Action);
        debugPrintf("NoDiff      = %d\n",   NoDiff);
        debugPrintf("KeepTemps   = %d\n",   KeepTemps);
        debugPrintf("LogFileName = '%s'\n",   LogFileName);
    }

	return i;
}

void
testAldorFini(int sig) {
	osSetCurDir(InitDir);

    /** If we didn't make the directory, we shouldn't delete it! **/
    /* if (!KeepTemps) osRemoveDir(OutDir); */

    if (sig == 0) exitSuccess(); else exitFailure();
}


void
testAldorGlean(Length argc, MutString *argv) {
	Length	i, cc = BUFLEN;
	MutString	buf = strAlloc(cc);

	for (i = 0; i < argc; i += 1) {
		if (!osFileIsThere(argv[i])) continue;

		if (strIsSuffix(".as", argv[i])) {
			FILE *	argi;
			MutString	key = "--> ";
			Int	c, k, atLineStart;
            char line[1024];

			argi = fopen(argv[i], "r");
			if (argi == NULL) exitFailure();

            for (atLineStart = 1;;) {
                char *s = fgets(line, sizeof(line), argi);
                if (s == NULL) break;

                {
                    size_t slen = strlen(s);
                    int hadNewline = slen > 0 && s[slen-1] == '\n';
                    if (atLineStart && strIsPrefix(key, s)) {
                        s += strlen(key);
                        s = strTrimSpaces(s);
                        if (testAldorFilter(s))
                            testAldorDispatch(s, argv[i]);
                    }
                    atLineStart = hadNewline;
                }
            }

			fclose(argi);
		}
		else if (strIsSuffix(".sh", argv[i]))
			if (testAldorFilter("testscript"))
				testAldorScript(argv[i]);
	}
}


Int
testAldorFilter(MutString buf) {
	if (strEqual(Kind, "all"))
		return 1;
	else
		return KindAll == (strncmp(buf, Kind, strlen(Kind)) == 0);
}

MutString	HelpMsg[] = {
	"testAldor [option...] [dir|file..]",
	"",
	"    The possible options are:",
	"        -help         Display this information",
	"        -show         Run the tests and display the output",
	"        -install      Run the tests and install the output as correct",
    "        -nodiff       Do not show differences",
	"        -oldaldor xx  Run the tests, using xx to display old .ao files",
	"        -only kk      Run only tests of kind kk, where kk may be",
	"		                 one of test{script,comp,run,errs,phase,gen,int}.",
	"        -but kk       Run all the tests, except those of kind kk.",
    "        -debug        Turn on debugging information.",
    "        -keep         Don't delete temporary files.",
    "",
    "        -outdir dd    Use dd for test output files instead of a new one.",
    "        -refdir dd    Use dd for reference outputs instead of ../testout",
    "                      (implies -keep).",
	"",
	"    If files are given as args, they are the tests to be used.",
	"    If a directory is given as an arg, then all the test files",
	"        in it are used. (.as .sh files)",
	"    Otherwise, all the test files in the current directory",
	"        are used. (.as .sh files)",
	"",
	"    .as files contain one or more lines like:",
	"        --> testcomp          (compile and compare byte-code output)",
	"        --> testrun           (compile, run and compare output)",
	"        --> testint           (interpret and compare output)",
	"        --> testerrs          (-D TestErrorsToo and compare msgs)",
	"        --> testphase phname  (trace phase and compare output)",
	"        --> testgen   ftype   (for generated files of a given type)",
	"",
	"    .sh files do not contain any special lines."
};

/*
 * Display help message.
 */
void
testAldorHelp()
{
	Length	i, msgc = sizeof(HelpMsg)/sizeof(HelpMsg[0]);

	for (i = 0; i < msgc; i += 1) {
		fputs(HelpMsg[i], stdout);
		fnewline(stdout);
	}
}

/*
 * Remove all generated files.
 */

MutString	Ext[] = { "ax", "ai", "ao", "fm", "asy", "l", "c", "o" };

void
testAldorClean(MutString fn)
{
	MutString	bn = osFileCombine(OutDir, osFileBase(fn));
	MutString	on;
	Length	i, extc = sizeof(Ext)/sizeof(Ext[0]);

	osSetCurDir(OutDir);

	for (i = 0; i < extc; i += 1) {
		on = strPrintf("%s.%s", bn, Ext[i]);
		if (!KeepTemps) osFileRemove(on);
	}
	if (!KeepTemps) osFileRemove(bn);

	osSetCurDir(InitDir);
}


/******************************************************************************
 *
 * Functions for specific types of tests.
 *
 *****************************************************************************/

/*
 * Dispatch function for .as tests.
 */
typedef void	(*TaxFun)	_OF((MutString, MutString));

struct test_info {
	MutString		tag;
	TaxFun		fn;
};
struct test_info testInfoTable[] = {
	{ "testcomp",	testAldorCompile },
	{ "testrun",	testAldorRun },
	{ "testint",	testAldorInterpret },
	{ "testerrs",	testAldorErrors },
	{ "testphase",	testAldorPhase },
	{ "testgen",	testAldorGenerate }
};

#define	TestInfoTag(i)	(testInfoTable[i].tag)
#define TestInfoFun(i)	(testInfoTable[i].fn)

void
testAldorDispatch(MutString args, MutString fn)
{
	Length	i, taxc = sizeof(testInfoTable) / sizeof(testInfoTable[0]);

	for (i = 0; i < taxc; i += 1)
		if (strIsPrefix(TestInfoTag(i), args)) {
            debugPrintf("Dispatching on: %s\n", args);
			args += strlen(TestInfoTag(i));
			TestInfoFun(i)(args, fn);
			testAldorClean(fn);
			return;
		}
}

/*
 * Test .sh files.
 */
void
testAldorScript(MutString fn)
{
    struct fframe *pff  = fframeAlloc(fn, "sh", 1, "shR");
    struct fset   *pres = pff->setv + 0;

    reportStart(pff->srcNameNoDir, "script");

	osPutEnv(strPrintf("TMPDIR=%s", OutDir));
    int rc = osRunScript(pff->srcNameInInit, pres->nameInOut);

    reportResult(pff->srcNameNoDir, rc != 0, pres->nameInRef, pres->nameInOut);
    fframeRemoveOutFiles(pff);
}


/*
 * For .as files containing a line '--> testcomp'
 *
 * This one is different from all the others because we decompile .ao files
 * to show the output or diffs.  We could make those actions smarter instead.
 */
void
testAldorCompile(MutString args, MutString fn) {
    struct fframe *pff  = fframeAlloc(fn, "as", 2, "ao", "fm");
    struct fset   *pao  = pff->setv + 0;
    struct fset   *pfm  = pff->setv + 1;

	MutString	fmNameInOutFromNew =
                    osFileCombine(OutDir, strPrintf("new-%s",pfm->nameNoDir));
	MutString	fmNameInOutFromOld =
                    osFileCombine(OutDir, strPrintf("std-%s",pfm->nameNoDir));

    reportStart(pff->srcNameNoDir, "compilation");

	MutString cmd = strPrintf("'%s' -R '%s' %s '%s'",
			Aldor, OutDir, args, pff->srcNameInInit);
	if (runWrapped(cmd, NULL) != 0) {
		fprintf(stdout, "ERROR\n");
		failv[failc++] = pff->srcNameNoDir;
	}
	else if (strEqual(Action, "show")) {
		fnewline(stdout);
		cmd = strPrintf("'%s' -R '%s' -F fm '%s'",
            Aldor, OutDir, pao->nameInOut);
		runWrapped(cmd, NULL);
		osFileCat(pfm->nameInOut);
		if (!KeepTemps) osFileRemove(pfm->nameInOut);
	}
	else if (strEqual(Action, "install")) {
		fprintf(stdout, "\n>> Installing result file %s\n", pao->nameInRef);
		osFileCopy(pao->nameInOut, pao->nameInRef);
	}
	else if (osFileEqual(pao->nameInRef, pao->nameInOut)) {
		fprintf(stdout, "OK\n");
	}
	else {
		fprintf(stdout, "(not identical) "); fflush(stdout);

		cmd = strPrintf("'%s' -R '%s' -F fm '%s'",
				Aldor, OutDir, pao->nameInOut);
		runWrapped(cmd, NULL);

		osFileCopy(pfm->nameInOut, fmNameInOutFromNew);
		if (!KeepTemps) osFileRemove(pfm->nameInOut);

		cmd = strPrintf("'%s' -R '%s' -F fm '%s'",
            OAldor, OutDir, pao->nameInRef);
		runWrapped(cmd, NULL);
		osFileCopy(pfm->nameInOut, fmNameInOutFromOld);
		if (!KeepTemps) osFileRemove(pfm->nameInOut);

		if (osFileEqual(fmNameInOutFromOld, fmNameInOutFromNew)) {
			fprintf(stdout, "OK\n");
		}
		else {
			fprintf(stdout, "DIFFERENT\n");
			if (!NoDiff)
                osShowDiff(fmNameInOutFromOld, fmNameInOutFromNew);
			failv[failc++] = pff->srcNameNoDir;
		}
		if (!KeepTemps) {
            osFileRemove(fmNameInOutFromNew);
            osFileRemove(fmNameInOutFromOld);
        }
	}
    fframeRemoveOutFiles(pff);
}

/*
 * For .as files containing a line '--> testrun'
 */
static void
cleanGeneratedMain(void)
{
    const char *names[] = { "aldormain.c", "aldormain.o", "aldormain.exe", "aldormain.obj" };
    for (unsigned i = 0; i < sizeof(names)/sizeof(names[0]); i++) {
        MutString path = osFileCombine(OutDir, (MutString) names[i]);
        osFileRemove(path);
    }
}

void
testAldorRun(MutString args, MutString fn) {
    struct fframe *pff  = fframeAlloc(fn, "as", 1, "out");
    struct fset   *pout = pff->setv + 0;

    reportStart(pff->srcNameNoDir, "execution");
    cleanGeneratedMain();

	MutString cmd = strPrintf(
        "'%s' -R '%s' -M base='%s%s' -Ccc=\"%s\" -grun %s '%s'",
		Aldor, OutDir, pff->srcDir, OS_PATH_SEP, CC, args, pff->srcNameInInit);
	int rc = runWrapped(cmd, pout->nameInOut);

    reportResult(pout->nameNoDir, rc != 0, pout->nameInRef, pout->nameInOut);
    fframeRemoveOutFiles(pff);
}

/*
 * For .as files containing a line '--> testint'
 */
void
testAldorInterpret(MutString args, MutString fn) {
    struct fframe *pff  = fframeAlloc(fn, "as", 1, "out");
    struct fset   *pout = pff->setv + 0;

    reportStart(pff->srcNameNoDir, "execution");

	MutString cmd = strPrintf("'%s' -R '%s' -M base='%s%s' -ginterp %s '%s'",
		Aldor, OutDir, pff->srcDir, OS_PATH_SEP, args, pff->srcNameInInit);
	int rc = runWrapped(cmd, pout->nameInOut);

    reportResult (pff->srcNameNoDir, rc!=0, pout->nameInRef, pout->nameInOut);
    fframeRemoveOutFiles(pff);
}

/*
 * For .as files containing a line '--> testerrs'
 */
void
testAldorErrors(MutString args, MutString fn) {
    struct fframe *pff  = fframeAlloc(fn, "as", 2, "E", "ao");
    struct fset   *pout = pff->setv + 0;

	reportStart(pff->srcNameNoDir, "error report");
	MutString cmd = strPrintf(
        "'%s' -R '%s' -M base='%s%s' -D TestErrorsToo %s '%s'",
		Aldor, OutDir, pff->srcDir, OS_PATH_SEP, args, pff->srcNameNoDir);
	runWrapped(cmd, pout->nameInOut);

    reportResult (pff->srcNameNoDir, 0, pout->nameInRef, pout->nameInOut);
    fframeRemoveOutFiles(pff);
}

/*
 * For .as files containing a line '--> testphase phase-name'
 */
void
testAldorPhase(MutString args, MutString fn) {
    struct fframe *pff  = fframeAlloc(fn, "as", 2, "R", "ao");
    struct fset   *pout = pff->setv + 0;

	while (args[0] == ' ') args++;
	reportStart(pff->srcNameNoDir, strPrintf("phase %s", args));

	MutString cmd = strPrintf("'%s' -WTrt+%s -R '%s' '%s'",
		Aldor, args, OutDir, pff->srcNameNoDir);
	int rc = runWrapped(cmd, pout->nameInOut);

    reportResult (pff->srcNameNoDir, rc!=0, pout->nameInRef, pout->nameInOut);
    fframeRemoveOutFiles(pff);
}

/*
 * srcpos.as intentionally checks generated #line directives.  Canonicalize
 * only the installed axllib include prefix so the golden file is independent
 * of the build root while still checking the source file and line number.
 */
static int
canonicalizeSrcposOutput(MutString fname)
{
    FILE *in, *out;
    char line[8192];
    MutString tmp = strConcat(fname, ".canon");

    in = fopen(fname, "r");
    out = fopen(tmp, "w");
    if (!in || !out) {
        if (in) fclose(in);
        if (out) fclose(out);
        free(tmp);
        return 1;
    }

    while (fgets(line, sizeof(line), in)) {
        if (strncmp(line, "#line ", 6) == 0 &&
            (strstr(line, "/include/axllib.as\"") ||
             strstr(line, "\\include\\axllib.as\""))) {
            char *quote = strchr(line, '\"');
            if (quote) {
                *quote = '\0';
                fprintf(out,
                        "%s\"<ALDORROOT>/include/axllib.as\"\n",
                        line);
                continue;
            }
        }
        fputs(line, out);
    }

    fclose(in);
    if (fclose(out) != 0 || osFileCopy(tmp, fname) != 0) {
        osFileRemove(tmp);
        free(tmp);
        return 1;
    }
    osFileRemove(tmp);
    free(tmp);
    return 0;
}

/*
 * For .as files containing a line  '--> testgen ftype'
 * It gets called as: testgen flet fname.as, where flet is one of i a o y f l c
 */
void
testAldorGenerate(MutString args, MutString fn) {
	MutString	fext;

	while (args[0] == ' ') args++;
	switch (args[0]) {
	case 'i': fext = "ai";  break;
	case 'a': fext = "ax";  break;
	case 'o': fext = "ao";  break;
	case 'y': fext = "asy"; break;
	case 'f': fext = "fm";  break;
	case 'c': fext = "c";   break;
	case 'l': fext = "lsp"; break;
	default:  fext = "unk"; break;
	}
	args++;
	while (args[0] == ' ') args++;

    struct fframe *pff  = fframeAlloc(fn, "as", 3, "E", "ao", fext);
    struct fset   *pgen = pff->setv + 2;

    reportStart(pff->srcNameNoDir, strPrintf("generation .%s", fext));

	/* Run from InitDir, so use the basename.  This keeps generated #line
	 * directives and include diagnostics independent of the package path. */
	MutString cmd = strPrintf("'%s' -F %s -R '%s' %s '%s'",
			Aldor, fext, OutDir, args, pff->srcNameNoDir);
	int rc = runWrapped(cmd, NULL);

    if (rc == 0 && strEqual(pff->srcNameNoDir, "srcpos.as") &&
        strEqual(fext, "c"))
        rc = canonicalizeSrcposOutput(pgen->nameInOut);

    reportResult(pff->srcNameNoDir, rc != 0, pgen->nameInRef, pgen->nameInOut);
    fframeRemoveOutFiles(pff);
}


/******************************************************************************
 *
 * Support functions
 *
 *****************************************************************************/

/*
 * Construct the necessary file names.
 * Following srcExt is an explicitly counted argument list of extensions.
 *
 * Do not use a null sentinel here.  Passing an untyped integer 0 through
 * varargs and retrieving it as MutString is undefined and fails on Win64.
 */
struct fframe *fframeAlloc(MutString fn, MutString srcExt, int setc, ...) {
    struct fframe *pff = (struct fframe *) malloc(sizeof(*pff));
    if (!pff) { fprintf(stderr, "Cannot allocate.\n"); exit(EXIT_FAILURE); }

 	pff->baseName      = osFileBase(fn);
    pff->srcDir        = osFileDir(fn, InitDir);
 	pff->srcNameNoDir  = strConcat(pff->baseName, strPrintf(".%s", srcExt));
    pff->srcNameInInit = osFileCombine(pff->srcDir, pff->srcNameNoDir);

    if (setc < 0 || setc > NFSET_MAX) {
        fprintf(stderr, "Invalid file-frame extension count %d\n", setc);
        exit(EXIT_FAILURE);
    }
    pff->setc = setc;

    va_list extn_list;
    va_start(extn_list, setc);
    for (int i = 0; i < setc; i++) {
        MutString extn = va_arg(extn_list, MutString);
        struct fset *ps = pff->setv + i;
        ps->extn = strPrintf(".%s", extn);
        ps->nameNoDir = strConcat(pff->baseName, ps->extn);
        ps->nameInRef = strConcat(pff->baseName, ps->extn);
        ps->nameInOut = osFileCombine(OutDir, ps->nameNoDir);
        ps->nameInRef = osFileCombine(RefDir, ps->nameNoDir);
    }
    va_end(extn_list);
    return pff;
}

void
fframeRemoveOutFiles(struct fframe *pff)
{
    for (int i = 0; i < pff->setc; i++)
        /* The files might not have been created, but trying remove anyway. */
        osFileRemove(pff->setv[i].nameInOut);
}


void
reportStart(MutString name, MutString kind) {
	fprintf(stdout, ">> %s: (%s) ", name, kind);
	fflush(stdout);
}

void
reportResult(MutString src, int wasErr, MutString outOld, MutString outNew)
{
    int diffedEqual = 0;;

	if (wasErr) {
		fprintf(stdout, "ERROR\n");
        failv[failc++] = src;
        fprintf(stdout, "--- execution/compiler output: %s ---\n", outNew);
        osFileCat(outNew);
        return;
    }
	else if (osFileEqual(outOld, outNew)) {
		fprintf(stdout, "OK");
        diffedEqual = 1;
    }
    /*
     * The historical msg transcript is intentionally compared with
     * `diff -b` when differences are shown.  Treat a whitespace-only
     * change the same way, rather than reporting DIFFERENT with an empty
     * diff body.
     */
    else if (strEqual(src, "msg.sh") && osShowDiff(outOld, outNew) == 0) {
        fprintf(stdout, "OK");
        diffedEqual = 1;
    }
	else {
		fprintf(stdout, "DIFFERENT");
        failv[failc++] = src;
        if (!osFileIsThere(outOld)) {
            fprintf(stdout, " < reference missing; new output follows >\n");
            osFileCat(outNew);
            return;
        }
    }

    if (strEqual(Action, "show")) {
        fprintf(stdout, " New output is:\n");
        osFileCat(outNew);
    }
	else if (strEqual(Action, "install")) {
		fprintf(stdout, " Installing %s\n", outOld);
		osFileCopy(outNew, outOld);
	}
    else if (!wasErr && !NoDiff && !diffedEqual) {
        fprintf(stdout, " < Old, > new\n");
        osShowDiff(outOld, outNew);
	}
    else
        fprintf(stdout, "\n");
}


int runWrapped(MutString cmd, MutString outFNameOrNull) {
    debugPrintf("Running command: %s\n", cmd);
	int rc = osRunOutput(cmd, outFNameOrNull);
    MutString fname = outFNameOrNull ? outFNameOrNull : "/dev/null";
    debugPrintf("Return Code = %d, Output File = %s\n", rc, fname);
    return rc;
}
