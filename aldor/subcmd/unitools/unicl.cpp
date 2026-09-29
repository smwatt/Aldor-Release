///////////////////////////////////////////////////////////////////////////////
//
// unicl.cpp: Universal C compiler and linker.
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
#define STO_DONT_NEW_DEBUG

#include "axlgen.h"
#include "store.h"
#include "cfgfile.h"

#ifndef ABNORMAL
#define smw_stderr stdout
#else
#define smw_stderr stderr
#endif

/*
 * ToDo:
 *	Support for library path expansion
 *	Check weird platforms
 *	Clean up memory leaks
 */

/*
 * This version is a complete rewrite of the older version,
 * and uses a config file for most of its work.
 *
 * `#ifdef' count: 0
 *
 * Idea is that it translates the commands produced by the
 * compiler into platform-specific versions.
 * It is _NOT_ a general purpose compiler.
 */


String helpInfo[100] = {
    "Usage: unicl [options]* file+",
    "This program accepts Unix-style C compile and link options and",
    "parameters and translates them into one or more native commands.",
    "Configuration parameters are retrieved from an ASCII file (see below).",
    "",
    "This program accepts the following options",
    "",
    " -W h        Help: show help information.",
    " -W v=n      Verbose: set verbosity level to n (must be 1-9).",
    "               -Wv=1 shows the generated commands (same as -W v)",
    "               -Wv=2 also shows the system name selected",
    "               -Wv=3 also shows the unicl invocation including the arguments",
    "                     inherited from the environment variable UNICL",
    "               -Wv=4 also shows all the (key,value) pairs",
    " -W v        Verbose: shows the generated commands (same as -Wv=1)",
    " -W n        No-execute: do not execute generated commands.",
    "",
/* not supported yet
    " -R <dir>    Put the resulting files in directory <dir>.",
    "",
*/
    " -D <def>    Pass <def> as a #define to the C compiler.",
    " -U <undef>  Pass <undef> as an #undef to the C compiler.",
    " -I <incdir> Pass the include directory to any command that will use it.",
    " -L <libdir> Pass the library directory to any command that will use it.",
    " -l <lib>    Pass the library name to any command that will use it.",
    "",
    " -O          Use the C compiler optimizer.",
    " -c          Compile but do not link the C source file.",
    " -g          Have the C compiler/linker include debug information.",
    " -o <efile>  Use <efile> as the name of the executable file.",
    " -p          Have the C compiler/linker generate code for profiling.",
    "",
    " -Wopts=... ",
    " -Wtwixt=... ",
    " -Wpost=... ",
    "             Pass additional options to the compiler.  If no string is given,",
    "             then the default augument list will be reset.  Multiple arguments",
    "             can be given, separated by commas, with commas escaped with '\\',",
    "             or extra `-Wopts' can be specified.",
    "             `Wopts` puts options before filenames, `Wpost` puts options after ",
    "             anything else and `Wtwixt` puts options between filenames and ",
    "             libraries at the link stage",
    " -Wstdc      Compile using ANSI C  compiler (default is K&R)",
    " -Wshared    Generate code suitable for shared libraries",
    " -Wfortran   Add options for linking fortran code",
    " -Wfnonstd   Enable fast and possibly non-IEEE-compliant floating point",
    "",
/* not supported yet
    " -Wkey=foo=bar Define the key `foo' to have value `bar'",
    "",
*/
    "The next options should be given FIRST, before any paths are processesed.",
    "",
    " -W config=xxx Set configuration file name.  Default is aldor.conf in $ALDORROOT/include.",
    " -W sys=xxx  Set precise subsystem.  This allows you to change things",
    "             such as optimisation options, linker options and suchlike.",
    "",
    "Before the command line options are processed, this program looks for",
    "an environment variable named UNICL.  This variable should contain",
    "valid command line options. These options are processed before those",
    "on the command line.",

    0
};

static MutString getOption	(int argc, char **argv, String name, MutString);
static FILE * getCfgFile	(int argc, char **argv);
static MutString getCfgName	(int argc, char **argv);
static void  loadConfiguration	(void);
static void  printHelp 	(void);
static void  handleOptions	(int argc, char **argv);
static void  handleOption(int i, int *pNewI, char **argv);
static void  handleSysArgument	(int i, int *pNewI, char **argv);
static MutString getParameterisedArg(int i, int *pNewi, char **argv);
static void  initState		(void);
static bool  setupState	(void);
static void generateCommands();
static bool  executeCommands	(void);
static void  subsumeImplicitArgs(int oargc, char **oargv, int *pargc, char ***pargv);


static ConfigItemList uclInitialOptions();
static void  uclAddSysArgs	(MutStringList *plst, String opts);
static void uclReconcileDebug(ConfigItemList lst);
static MutStringList uclFixOptionList(MutStringList opts0, String key);
static MutString 	  uclGetKeyName(String);


/******************************************************************************
 *
 * :: Options -> Command strings
 *
 *****************************************************************************/

static MutString 	  uclGetCommandName();
static MutStringList uclGetGeneralCompileOptions();
static MutStringList uclCompileOnlyArguments();
static MutStringList uclGetPreLinkOptions();
static MutStringList uclGetPostLinkOptions();
static MutStringList uclExpandLibs(MutStringList path0, MutStringList libs, MutString ext);
static int        uclCheckCondition(MutString name);

/******************************************************************************
 *
 * :: Executing commands
 *
 *****************************************************************************/

typedef MutString CCommand;

static void ccInitCommand();
static int      ccPutc                 (int c);
static void     ccPutq                 (String s);
static void     ccPuts                 (String s);

static CCommand ccCompleteCommand	(void);
static void     ccSetCommandName	(MutString name);
static void     ccPushArguments		(MutStringList lst);
static void     ccPushArgument		(MutString s);
static void     ccPrintCommand		(CCommand cmd);
static bool     ccExecute			(CCommand cmd);
static bool     uclPrepareLinkedOutput		(void);
static void     ccSetNoExecute		(void);

/* Command-builder state is used by Darwin post-link preparation too. */
static Buffer ccBuf;
static bool   ccNoExecute;


/******************************************************************************
 *
 * :: "Global" constants
 *
 *****************************************************************************/

/* aldor.conf has preference over axiomxl.conf */
#define CONFFILE "aldor.conf"
#define OLDCONFFILE "axiomxl.conf"

static MutString uclSysName;
static FILE  *uclOptFile;
static ConfigItemList uclOptions;

static int dbgLevel;

/* Needed to keep linker happy */
CREATE_LIST(MutString);

/******************************************************************************
 *
 * :: State
 *
 *****************************************************************************/

bool  	   uclIsCompileOnly;
bool 	   uclIsLink;
bool 	   uclStdc;
bool 	   uclShared;
bool	   uclFortran;
bool 	   uclFloatNonStd;
int	   uclOptimize;
int	   uclDebug;
int	   uclProfile;
MutStringList uclFileList;
MutStringList uclIncludePath;
MutStringList uclLibPath;
MutStringList uclDefines;
MutStringList uclUnDefines;
MutStringList uclLibraries;
MutString	   uclOutputDir;
MutString	   uclOutputFile;
MutStringList uclStartOptions;
MutStringList uclPostOptions;
MutStringList uclTwixtOptions;
MutStringList uclExtraKeys;
MutString uclCommand;

void exitWith(int rc) {
    if (rc != EXIT_SUCCESS) printf("Exiting with code %d.\n", rc);
    exit(rc);
}

int
main(int argc, char *argv[])
{
	int largc;
	char **largv;

	osInit();
	subsumeImplicitArgs(argc, argv, &largc, &largv);
	initState();
	uclSysName = getCfgName(largc, largv);
	uclOptFile = getCfgFile(largc, largv);

    void * p = stoAlloc(0, 10000000);
    stoFree(p);
	if (!uclOptFile) exitWith(2);

	loadConfiguration();
	handleOptions(largc, largv);

	if (!setupState()) exitWith(2);

	if (dbgLevel > 2) {
		int i;
		printf("Command:");
		for (i=0; i<largc; i++) printf(" %s", largv[i]);
		printf("\n");
	}

	generateCommands();

	if (!executeCommands())
        exitWith(EXIT_FAILURE);
    exitWith(EXIT_SUCCESS);

	/* gack */
	return 0;
}

static void
printHelp()
{
	int i;
	for (i=0;helpInfo[i];i++) { fprintf(smw_stderr, "%s\n", helpInfo[i]);}
}


static char *
getOption(int argc, char **argv, String name, MutString def)
{
	int i, len;
	len = strlen(name);

	for (i = argc-1; i>=0; i--) {
		if (strncmp(argv[i], name, len) == 0)
			return strCopy(argv[i]+len);
	}
	return def;
}

static FILE *
getCfgFile(int argc, char **argv)
{
	FileName cfgFileName;
	FILE *file;
	char *name;

	name = getOption(argc, argv, "-Wconfig=", NULL);

	/* Should strip off any ".conf" */

	if (!name) {
		cfgFileName = cfgFindFile(CONFFILE, "");
		if (!cfgFileName) cfgFileName = cfgFindFile(OLDCONFFILE, "");
	}
	else
		cfgFileName = FileName::parse(name);

	if (!cfgFileName) {
		fprintf(smw_stderr, "Can't find config file\n");
		return NULL;
	}
	if ((file = fileTryOpen(cfgFileName, osIoRdMode)) == NULL)
		fprintf(smw_stderr, "Can't open: %s\n", cfgFileName.unparse());

	return file;
}

static MutString
getCfgName(int argc, char **argv)
{
	char *name;
	name = getOption(argc, argv, "-Wsys=", NULL);
	if (!name) name = getOption(argc, argv, "-WSYS=", NULL);
	if (!name) name = strCopy(CONFIGSYS);

	return name;
}

static void
loadConfiguration()
{
	MutString		defSection;
	ConfigItemList	lst, all;
	MutStringList	sections, tmp;

	all = listNil(ConfigItem);
	sections = listSingleton(MutString)(uclSysName);

	while (sections) {
		/* Read the name of the next section */
		defSection = car(sections);


		/* Remove it from the to-do list */
		sections = cdr(sections);


		/* Get all the options in the section */
		lst = cfgRead(uclOptFile, defSection);


		/* We ignore errors: could display if verbose */
		cfgReadClearErrors();


		/* Add them all to the result list */
		all = listNConcat(ConfigItem)(all, lst);


		/* Get the list of inherited sections */
		tmp = cfgLookupKeyNameList("inherit", lst);


		/* Place them at the front of our to-do list */
		sections = listNConcat(MutString)(tmp, sections);


		/* Back to the start of the file */
		rewind(uclOptFile);
	}


	/* Now add the default options */
	all = listNConcat(ConfigItem)(all, cfgRead(uclOptFile, "default"));


	/* Finally the initial options */
	all = listNConcat(ConfigItem)(all, uclInitialOptions());

	uclOptions = all;
}


static void
handleOptions(int argc, char **argv)
{
	int i = 1;
        if (argc == 1 ) {printHelp();	exitWith(EXIT_SUCCESS);}
        while (i < argc) {
		if (*argv[i] != '-') {
			uclFileList = listCons(MutString)(strCopy(argv[i]), uclFileList);
			i++;
		}
		else {
			int newi;
			handleOption(i, &newi, argv);
			i = newi;
		}
	}
}

#define listNConc1(t, var, v) (listNConcat(t)(var, listSingleton(t)(v)))
static void
handleOption(int i, int *pNewI, char **argv)
{
	switch (argv[i][1]) {
	case 'W':
		handleSysArgument(i, pNewI, argv);
		break;
	case 'R':
		uclOutputDir = argv[i+1];
		*pNewI = i + 2;
		break;
	case 'D':
		uclDefines = listNConc1(MutString, uclDefines,
					getParameterisedArg(i, &i, argv));
		break;
	case 'U':
		uclUnDefines = listNConc1(MutString, uclUnDefines,
					getParameterisedArg(i, &i, argv));
		break;
	case 'I':
		uclIncludePath = listNConc1(MutString, uclIncludePath,
					getParameterisedArg(i, &i, argv));
		break;
	case 'L':
		uclLibPath = listNConc1(MutString, uclLibPath,
					getParameterisedArg(i, &i, argv));
		break;
	case 'l':
		uclLibraries = listNConc1(MutString, uclLibraries,
					getParameterisedArg(i, &i, argv));
		break;
	case 'O':
		uclOptimize = i;
		break;
	case 'c':
		uclIsCompileOnly = true;
		break;
	case 'g':
		uclDebug = i;
		break;
	case 'o':
		uclOutputFile = getParameterisedArg(i, &i, argv);
		break;
	case 'p':
		uclProfile = i;
		break;
	default:
		fprintf(smw_stderr, "unicl: option '%s' not supported.\n", argv[i]);
	}
	*pNewI = i + 1;
}

#define hasArgumentPrefix(a, x) (strncmp((a)+1, x, strlen(x)) == 0)
#define isArgument(a, x)        (strcmp((a)+1, x) == 0)

static void
handleSysArgument(int i, int *pNewI, char **argv)
{
	(void) pNewI;
	/*
	 * unicl's private -W options must be matched exactly unless they are
	 * explicitly parameterised.  Historically this used prefix matching,
	 * so a native compiler option such as -Wno-unused-value matched -Wn
	 * and silently enabled no-execute mode.  That made Darwin links report
	 * success without ever invoking clang.
	 *
	 * Any other -W option belongs to the native compiler/linker and is
	 * passed through unchanged.
	 */
	if (hasArgumentPrefix(argv[i], "Wconfig="))
		/* Dunnit */;
	else if (hasArgumentPrefix(argv[i], "Wsys="))
		/* Dunnit */;
	else if (hasArgumentPrefix(argv[i], "Wopts="))
		uclAddSysArgs(&uclStartOptions, argv[i] + strlen("-Wopts="));
	else if (hasArgumentPrefix(argv[i], "Wpost="))
		uclAddSysArgs(&uclPostOptions, argv[i] + strlen("-Wpost="));
	else if (hasArgumentPrefix(argv[i], "Wtwixt="))
		uclAddSysArgs(&uclTwixtOptions, argv[i] + strlen("-Wtwixt="));
	else if (hasArgumentPrefix(argv[i], "Wkey="))
		uclAddSysArgs(&uclExtraKeys, argv[i] + strlen("-Wkey="));
	else if (hasArgumentPrefix(argv[i], "Wv=") ||
	         hasArgumentPrefix(argv[i], "WV="))
		dbgLevel = argv[i][4] > '\0' ? argv[i][4] - '0' : 1;
	else if (isArgument(argv[i], "Wstdc"))
		uclStdc = true;
	else if (isArgument(argv[i], "Wfnonstd"))
		uclFloatNonStd = true;
	else if (isArgument(argv[i], "Wfortran"))
		uclFortran = true;
	else if (isArgument(argv[i], "Wn") || isArgument(argv[i], "WN"))
		ccSetNoExecute();
	else if (isArgument(argv[i], "Wv") || isArgument(argv[i], "WV"))
		dbgLevel = 1;
	else if (isArgument(argv[i], "Wshared"))
		uclShared = true;
	else if (isArgument(argv[i], "Wh") || isArgument(argv[i], "WH"))
		printHelp();
	else
		uclStartOptions = listNConc1(MutString, uclStartOptions,
		                               strCopy(argv[i]));
}

static MutString
getParameterisedArg(int i, int *pNewI, char **argv)
{
	if (argv[i][2] != '\0') {
		*pNewI = i;
		return strCopy(argv[i]+2);
	}


	*pNewI = i+1;
	return strCopy(argv[i+1]);
}


static void
uclAddSysArgs(MutStringList *plst, String opts)
{
	char **argv;
	int i, argc;

	if (opts[0] == '\0') {
		*plst = listSingleton(MutString)(strCopy(""));
		return;
	}

	cstrParseCommaified(opts, &argc, &argv);
	for (i=0; i<argc; i++)
		*plst = listNConcat(MutString)(*plst, listCons(MutString)(argv[i], listNil(MutString)));
	stoFree(argv);
}

/******************************************************************************
 *
 * :: Generating commands
 *
 *****************************************************************************/

static String
uclState(bool flg)
{
	return flg ? "true" : "false";
}

static void
generateCommands()
{
	ConfigItemList lst = uclOptions;

	if (dbgLevel > 1)
		printf("SysName: %s\n", uclSysName);

	if (dbgLevel > 3) {
		printf("[Config]\n");
		while (lst != listNil(ConfigItem)) {
			printf("\t`%s' = `%s'\n",
			       cfgName(car(lst)), cfgVal(car(lst)));
			lst = cdr(lst);
		}
		printf("[C'est Tout]\n");

		printf("Compile Only: %s\n", uclState(uclIsCompileOnly));
		printf("Link Only:    %s\n", uclState(uclIsLink));
		printf("Optimize:     %s\n", uclState(uclOptimize));
		printf("Debug:        %s\n", uclState(uclDebug));
		printf("Profile:      %s\n", uclState(uclIsCompileOnly));
		printf("OutputDir:    %s\n", uclOutputDir ? uclOutputDir : "(null)");
		printf("OutputFile:   %s\n", uclOutputFile ? uclOutputFile : "(null)");
	}

	ccInitCommand();
	ccSetCommandName(uclGetCommandName());
	ccPushArguments(uclStartOptions);

	if (uclIsCompileOnly) {
		/* -D, -I, -U, -g, -O, -p, -Wfnonstd */
		ccPushArguments(uclGetGeneralCompileOptions());
		ccPushArguments(uclCompileOnlyArguments());
		ccPushArgument(car(uclFileList));
		ccPushArguments(uclPostOptions);
	}
	else {
		ccPushArguments(uclGetPreLinkOptions());
		ccPushArguments(uclFileList);
		ccPushArguments(uclTwixtOptions);
		ccPushArguments(uclGetPostLinkOptions());
		ccPushArguments(uclPostOptions);
	}
	uclCommand = ccCompleteCommand();
}

static bool
executeCommands()
{
	bool ok;

	if (dbgLevel > 0) {
		printf("Exec: ");
		ccPrintCommand(uclCommand);
		printf("\n");
		fflush(stdout);
	}
	ok = ccExecute(uclCommand);
	if (!ok) return false;

	return uclPrepareLinkedOutput();
}

/*
 * The old Darwin dounicl wrapper made a freshly linked program executable,
 * removed browser quarantine metadata, and ad-hoc signed it before Aldor
 * attempted -grun.  The C++ unicl executable replaced that wrapper, so this
 * post-link step must live here rather than in an external shell wrapper.
 *
 * Other systems need no post-processing.  In no-execute mode there is no
 * output file to prepare.
 */
static bool
uclPrepareLinkedOutput()
{
#if defined(OS_MAC_OSX)
	MutString command;
	int rc;

	if (ccNoExecute || !uclIsLink || uclShared || !uclOutputFile || !*uclOutputFile)
		return true;

	ccBuf = Buffer::create();
	ccPuts("chmod ugo+x ");
	ccPutq(uclOutputFile);
	command = ccBuf.liberate();
	rc = osRun(command);
	strFree(command);
	if (rc != 0) return false;

	/* xattr may be absent and the quarantine attribute normally is absent. */
	ccBuf = Buffer::create();
	ccPuts("xattr -d com.apple.quarantine ");
	ccPutq(uclOutputFile);
	ccPuts(" >/dev/null 2>&1 || true");
	command = ccBuf.liberate();
	(void) osRun(command);
	strFree(command);

	ccBuf = Buffer::create();
	ccPuts("codesign --force --sign - ");
	ccPutq(uclOutputFile);
	ccPuts(" >/dev/null 2>&1");
	command = ccBuf.liberate();
	rc = osRun(command);
	strFree(command);
	if (rc != 0) {
		fprintf(stderr, "unicl: unable to code sign %s\n", uclOutputFile);
		return false;
	}
#endif
	return true;
}


/******************************************************************************
 *
 * :: Options -> Command strings
 *
 *****************************************************************************/
static MutStringList uclConstructOptList(String name, MutStringList given);

static MutString
uclGetCommandName()
{
	MutString str = uclGetKeyName("name");
	str = cfgLookupString(str, uclOptions);
	return str;
}

static MutStringList
uclGetGeneralCompileOptions()
{
	MutStringList res;
	res = listNil(MutString);

	/* -I */
	res = listNConcat(MutString)(res, uclConstructOptList("include", uclIncludePath));
	/* -D */
	res = listNConcat(MutString)(res, uclConstructOptList("define", uclDefines));
	/* -U */
	res = listNConcat(MutString)(res, uclConstructOptList("undefine", uclUnDefines));

	if (uclDebug)
		res = listNConcat(MutString)(res, cfgLookupStringList(uclGetKeyName("debug"), uclOptions));
	if (uclProfile)
		res = listNConcat(MutString)(res, cfgLookupStringList(uclGetKeyName("profile"), uclOptions));

	if (uclOptimize) {
		if (!uclFloatNonStd)
			res = listNConcat(MutString)(res, cfgLookupStringList(uclGetKeyName("optimize"),
									   uclOptions));
		else
			res = listNConcat(MutString)(res, cfgLookupStringList(uclGetKeyName("non-std-float"),
									   uclOptions));
	}
	return res;
}


static MutStringList
uclGetPreLinkOptions()
{
	MutStringList res;
	res = listNil(MutString);

	/* -I */
	res = listNConcat(MutString)(res, uclConstructOptList("include", uclIncludePath));
	/* -D */
	res = listNConcat(MutString)(res, uclConstructOptList("define", uclDefines));
	/* -U */
	res = listNConcat(MutString)(res, uclConstructOptList("undefine", uclUnDefines));

	if (uclDebug)
		res = listNConcat(MutString)(res,
					  cfgLookupStringList(uclGetKeyName("debug"),
							      uclOptions));
	if (uclProfile)
		res = listNConcat(MutString)(res,
					  cfgLookupStringList(uclGetKeyName("profile"),
							      uclOptions));
	if (uclOptimize) {
		if (!uclFloatNonStd)
			res = listNConcat(MutString)(res,
						  cfgLookupStringList(uclGetKeyName("optimize"),
								      uclOptions));
		else
			res = listNConcat(MutString)(res,
						  cfgLookupStringList(uclGetKeyName("non-std-float"),
								      uclOptions));
	}

	if (uclOutputFile)
		res = listNConcat(MutString)(res,
				  uclConstructOptList("output-name",
						      listSingleton(MutString)(uclOutputFile)));
	return res;
}

static MutStringList
uclGetPostLinkOptions()
{
	MutStringList res;
	bool flg;
	res = listNil(MutString);

	flg = cfgLookupBoolean("expand-libs", uclOptions);

	if (flg) {
		MutString ext = cfgLookupString("lib-ext", uclOptions);
		res = uclExpandLibs(uclLibPath, uclLibraries, ext);
	}
	else {
		/* -L */
		res = listNConcat(MutString)(res, uclConstructOptList("libpath", uclLibPath));
		/* -l */
		res = listNConcat(MutString)(res, uclConstructOptList("library", uclLibraries));
	}

	if (uclFortran) {
		MutStringList lst = cfgLookupStringList("fortran-libraries", uclOptions);
		res = listNConcat(MutString)(res, lst);
	}

	return res;
}

static MutStringList
uclExpandLibs(MutStringList path0, MutStringList libs, MutString ext)
{
	MutStringList res = listNil(MutString);

	while (libs != listNil(MutString)) {
		MutString basename = car(libs);
		MutStringList path = path0;
                basename = strConcat("lib", basename);
		while (path != listNil(MutString)) {
			FileName name = FileName::create(car(path), basename, ext);
			if (fileIsOpenable(name, osIoRdMode)) {
				res = listCons(MutString)(name.unparse(), res);
				break;
			}
			path = cdr(path);
		}
		if (path == listNil(MutString))
			printf("Warning: %s not found\n", basename);
                strFree(basename);
		libs = cdr(libs);
	}
	return listNReverse(MutString)(res);
}

static MutStringList
uclCompileOnlyArguments()
{
	MutStringList lst = cfgLookupStringList("compile-only", uclOptions);
	/* "-c" or whatnot */
	return lst;
}

static MutStringList
uclConstructOptList(String name, MutStringList given)
{
	MutStringList res;
	MutString flag;
	MutString tmp;
	bool sep;

	flag = cfgLookupString(name, uclOptions);

	tmp = strConcat(name, "-sep");
	sep = cfgLookupBoolean(tmp, uclOptions);
	strFree(tmp);

	res = listNil(MutString);
	while (given) {
		if (sep) {
			res = listCons(MutString)(flag, res);
			res = listCons(MutString)(car(given), res);
		}
		else {
			tmp = strConcat(flag, car(given));
			res = listCons(MutString)(tmp, res);
		}
		given = cdr(given);
	}
	return listNReverse(MutString)(res);
}

/******************************************************************************
 *
 * :: State
 *
 *****************************************************************************/

static void initPath();

static void
initState()
{
	uclIsCompileOnly = false;
	uclIsLink   = false;
	uclOptimize = false;
	uclDebug    = false;
	uclProfile  = false;
	uclFortran  = false;
	uclStdc     = false;
	uclFileList = listNil(MutString);
	uclIncludePath = listNil(MutString);
	uclDefines = listNil(MutString);
	uclUnDefines = listNil(MutString);
	uclOptFile = NULL;

	cfgSetCondFunc(uclCheckCondition);
	initPath();
}

static String uclDefaultPath = ".%c%s/include%c%s/share/include";

static void
initPath()
{
	char *root, *path;


	/* $ALDORROOT overrides $AXIOMXLROOT */
	root = getenv("ALDORROOT");
	if (!root) root = getenv("AXIOMXLROOT");
	if (!root) path = strCopy(".");
	else {
		size_t pathSize =
			strlen(uclDefaultPath) + 2 * strlen(root) + 1;
		path = (MutString) stoAlloc(OB_Other, pathSize);
		snprintf(path, pathSize, uclDefaultPath,
				osPathSeparator(), root, osPathSeparator(), root);
	}

	cfgSetConfPath(path);
	strFree(path);
}

/*
 * This function cleans up the state so that we never
 * generate a bad sequence of options
 */
static bool
setupState()
{
	MutString key;
	if (uclExtraKeys) {
		printf("Extra keys not supported yet\n");
		return false;
	}

	uclReconcileDebug(uclOptions);

	if (uclIsCompileOnly && cdr(uclFileList)) {
		printf("More than one file specified for compilation only\n");
		return false;
	}

	/* Should deal with '-L' and '-l' expansion here if necessary */

	/* If we're not compiling, then we are linking */
	if (!uclIsCompileOnly) uclIsLink = true;

	if (uclOutputDir) {
		printf("-R: Not supported\n");
		return false;
	}

	/* Start Options */
	key = uclGetKeyName("opts");
	uclStartOptions = uclFixOptionList(uclStartOptions, key);

	/* Post Options */
	key = uclGetKeyName("post");
	uclPostOptions = uclFixOptionList(uclPostOptions, key);


	if (uclIsLink) {
	        /* Extra libraries */
		MutStringList libs = cfgLookupStringList("lib-extra", uclOptions);
		uclLibraries = listNConcat(MutString)(uclLibraries, libs);
		uclLibPath   = uclFixOptionList(uclLibPath, "lib-default-path");
        	/* Twixt Options */
	        key = uclGetKeyName("twixt");
	        uclTwixtOptions = uclFixOptionList(uclTwixtOptions, key);
	}
	if (uclIsCompileOnly) {
	        uclIncludePath = uclFixOptionList(uclIncludePath,"include-default-path");
	}

	return true;
}

static MutString
uclGetKeyName(String s)
{
	MutString r = strCopy(s);
	String mode, std;
	MutString tmp;
	mode   = uclIsCompileOnly ? "cc-" 	: "link-";
	std    = uclStdc 	  ? "std-" 	: "";
	tmp = r;

	r = strConcat(mode, r);
	strFree(tmp); tmp = r;
	r = strConcat(std, r);
	strFree(tmp);

	return r;
}

static MutStringList
uclFixOptionList(MutStringList opts0, String key)
{
	MutStringList opts;

	if (opts0 && car(opts0)[0] == '\0')
		opts = cdr(opts0);
	else {
		opts = cfgLookupStringList(key, uclOptions);
		opts = listNConcat(MutString)(opts, opts0);
	}

	return opts;
}

/*
 * Assumption is that profile/Optimize is always OK
 * debug/profile may be bad
 * debug/optimize may be bad
 */
static void
uclReconcileDebug(ConfigItemList lst)
{
	bool wantDebug, wantProf, wantOpt;

	wantDebug = uclDebug != 0;
	wantProf  = uclProfile != 0;
	wantOpt   = uclOptimize != 0;

	if (wantDebug && wantOpt && !cfgLookupBoolean("debug-optimize-ok", lst)) {
		if (uclOptimize > uclDebug) {
			printf("Warning: Both -O and -g specified, ignoring -g\n");
			wantDebug = 0;
		}
		else {
			printf("Warning: Both -O and -g specified, ignoring -O\n");
			wantOpt = 0;
		}
	}
	if (wantDebug && wantProf && !cfgLookupBoolean("debug-profile-ok", lst)) {
		if (uclProfile > uclDebug) {
			printf("Warning: Both -p and -g specified, ignoring -g\n");
			wantDebug = 0;
		}
		else {
			printf("Warning: Both -p and -g specified, ignoring -p\n");
			wantProf = 0;
		}
	}

	uclOptimize = wantOpt;
	uclDebug    = wantDebug;
	uclProfile  = wantProf;
}


/******************************************************************************
 *
 * :: Reading @ and environment options
 *
 * Actually, '@' isn't supported just yet
 *
 *****************************************************************************/

static void
subsumeImplicitArgs(int oargc, char **oargv, int *pargc, char ***pargv)
{
	char **largv, **argv;
	char *env_args;
	char **targv;
	int largc, argc;
	int i;
	largc = 0;
	largv = NULL;

	env_args = osGetEnv("UNICL");

	if (env_args) {
		cstrParseUnquoted(env_args, &largc, &largv);
	}

	targv = largv;
	argc = largc + oargc;
	argv = (char **) stoAlloc(OB_Other, (argc +1 )* sizeof(char *));
	argv[0] = oargv[0];
	for (i=0; i<largc; i++) argv[i+1] = targv[i];
	for (i=1; i<oargc; i++) argv[i + largc] = oargv[i];
    argv[argc] = 0;

	*pargc = argc;
	*pargv = argv;
}

static int
uclCheckCondition(MutString name)
{
	if (strEqual(name, "stdc"))
		return uclStdc;
	if (strEqual(name, "link"))
		return uclIsLink;
	if (strEqual(name, "compile"))
		return uclIsCompileOnly;
	if (strEqual(name, "shared"))
		return uclShared;
	if (strEqual(name, "optimize"))
		return uclOptimize;
	if (strEqual(name, "nonstdfloat"))
		return uclFloatNonStd;
	if (strEqual(name, "stdfloat"))
		return !uclFloatNonStd;
	if (strEqual(name, "profile"))
		return uclProfile;
	printf("Warning: `%s' not a known keyword\n", name);
	return -1;
}

/******************************************************************************
 *
 * :: Command retrieval
 *
 *****************************************************************************/

/*
 * Strategy is that args should be comma separated, and if commas have to be
 * passed through, then escape them with "\"
 */

struct _option {
	String name;
	String value;
};

struct _option defaultOptions[] =
{
	/* -g */
	{ "debug",  "-g" },
	{ "link-debug", "$debug"},

	/* -p */
	{ "profile", "-pg" },
	{ "profile", "$profile" },

	/* -O */
	{ "optimize", "-pg" },
	{ "link-optimize", "$optimize" },

	/* -l */
	{ "library", "-l" },
	{ "library-sep", "false" },

	/* -L */
	{ "libpath", "-L" },
	{ "libpath-sep", "true" },
	{ "expand-libs", "false"},
	{ "lib-default-path", ""},

	/* -R */
	{ "output-dir-strategy", "prepend"},

	/* -D */
	{ "define", "-D"},
	{ "define-sep", "false"}, 	/* Some compilers like '-D foo' for definitions */

	/* -U */
	{ "undefine", "-U"},
	{ "undefine-sep", "false" },

	/* -I */
	{ "include", "-I" },
	{ "include-sep", "true" },
	{ "include-default-path", ""},

	/* -c */
	{ "compile-only", "-c" },


	/* -o */
	{ "output-name", "-o" },
	{ "output-name-sep", "false" },

	/* -Wfnonstd */
	{ "non-std-float-args", ""},

	/* General things */
	{ "cc-name", "cc" },
	{ "link-name", "$cc-name" },
	{ "std-cc-name" , "cc" },
	{ "std-link-name" , "$std-cc-name" },
	{ "debug-profile-ok", "false"},
	{ "debug-optimize-ok", "false"},
	{NULL, NULL}
};

static ConfigItemList
uclInitialOptions()
{
	ConfigItemList lst = listNil(ConfigItem);
	int i = 0;
	while (defaultOptions[i].name != NULL) {
		lst = listCons(ConfigItem)(cfgNew(defaultOptions[i].name, defaultOptions[i].value),
					   lst);
		i++;
	}
	return lst;
}



/******************************************************************************
 *
 * :: Executing commands
 *
 *****************************************************************************/

static int    ccPutc    (int c)       { return ccBuf.putc(c); }
static void   ccPutq    (String s)  { osRunQuoteArg(s, ccPutc); }
static void   ccPuts    (String s)  { ccBuf.puts(s); }

static void
ccInitCommand()
{
	ccBuf = Buffer::create();
}

static CCommand
ccCompleteCommand()
{
	MutString s = ccBuf.liberate();
	ccBuf = NULL;
	return s;
}

static void
ccSetCommandName(MutString name)
{
	ccPutq(name);
	ccPutc(' ');
}

static void
ccPushArguments(MutStringList lst)
{
	while (lst != listNil(MutString)) {
		ccPushArgument(car(lst));
		lst = cdr(lst);
	}
}

static void
ccPushArgument(MutString s)
{
	ccPutq(s);
	ccPutc(' ');
}

static void
ccPrintCommand(CCommand cmd)
{
	printf("%s", cmd);
}

#if defined (ENV_MSVC)
#include <windows.h>
#endif

static bool
ccExecute(CCommand cmd)
{
#if defined (ENV_MSVC)
	HANDLE x;
	int    r;
	if (ccNoExecute) return true;
	x = CreateFile("xxx", GENERIC_WRITE, 0, NULL, TRUNCATE_EXISTING,
	               FILE_ATTRIBUTE_NORMAL, NULL);
	r = osRunRedirect(cmd, TRUE, NULL, x, NULL);
	CloseHandle(x);
	DeleteFile(TEXT("xxx"));
	return (r == 0);
#else
	if (ccNoExecute) return true;
	return (osRun(cmd) == 0);
#endif
}

static void
ccSetNoExecute()
{
	ccNoExecute = true;
}
