/******************************************************************************
 *
 * util.c
 *
 * Low-level utilities.
 *
 *****************************************************************************/

#include "util.h"

#include <stdio.h>
#include <ctype.h>

MutString		strAlloc		_OF((Length));

/******************************************************************************
 *
 * OS-level utilities
 *
 *****************************************************************************/

/*
 * Select OS-specific file.
 */
#if defined(OS_UNIX)
#include "tx_unix.c0"
#endif

#if defined(OS_WINDOWS)
#include "tx_windows.c0"
#endif


#if !defined(OS_Has_Run)

Int
osRun(MutString cmd)
{
	fflush(stdout);
	fflush(stderr);

    char *fmt = "Running command >%s<\n";
    logPrintf  (fmt, cmd);
    debugPrintf(fmt, cmd);

	return system(cmd);
}

#endif	/* ! OS_Has_Run */


#if !defined(OS_Has_RunOutput)

Int
osRunOutput(MutString cmd, MutString fout)
{
	if (!fout) fout = "/dev/null";
	return osRun(strPrintf(OSFmtOutput, cmd, fout));
}

#endif	/* ! OS_Has_RunScript */


#if !defined(OS_Has_RunScript)

Int
osRunScript(MutString cmd, MutString fout)
{
	return osRunOutput(strPrintf(OSFmtScript, cmd), fout);
}

#endif	/* ! OS_Has_RunScript */


#if !defined(OS_Has_ShowDiff)

Int
osShowDiff(MutString fname1, MutString fname2)
{
	return osRun(strPrintf(OSFmtDiff, fname1, fname2));
}

#endif	/* ! OS_Has_ShowDiff */


#if !defined(OS_Has_PutEnv)

Int
osPutEnv(MutString eqn)
{
	return putenv(eqn);
}

#endif	/* ! OS_Has_PutEnv */


#if !defined(OS_Has_GetCurDir)

Int
osGetCurDir(MutString fn, Length cc)
{
	return getcwd(fn, cc) ? 0 : -1;
}

#endif	/* ! OS_Has_GetCurDir */


#if !defined(OS_Has_SetCurDir)

Int
osSetCurDir(MutString fn)
{
	return chdir(fn);
}

#endif	/* ! OS_Has_SetCurDir */


/* No generic version of osMakeDir. */


/* No generic version of osRemoveDir. */


#if !defined(OS_Has_FileIsThere)

Int
osFileIsThere(MutString fn)
{
	FILE *	f = fopen(fn, "r");

	if (f) fclose(f);

	return f != 0;
}

#endif	/* ! OS_Has_FileIsThere */


#if !defined(OS_Has_DirIsThere)

#include <dirent.h>

Int
osDirIsThere(MutString dn)
{
    DIR *d = opendir(dn);
    if (d) closedir(d);
    return d != 0;
}

#endif /* ! OS_Has_DirIsThere */


#if !defined(OS_Has_FileCat)

Int
osFileCat(MutString fn)
{
	FILE *	fin = fopen(fn, "r");
	Int	c;

	if (!fin) return -1;
	while ((c = fgetc(fin)) != EOF)
		fputc(c, stdout);

	fclose(fin);
	return 0;
}

#endif	/* ! OS_Has_FileCat */


#if !defined(OS_Has_FileCopy)

Int
osFileCopy(MutString src, MutString dest)
{
	FILE *	fin;
	FILE *	fout;
	Int	c;

	fin = fopen(src, "r");
	if (!fin) return -1;

	fout = fopen(dest, "w");
	if (!fout) { fclose(fin); return -1; }

	while ((c = fgetc(fin)) != EOF)
		fputc(c, fout);

	fclose(fout);
	fclose(fin);
	return 0;
}

#endif	/* ! OS_Has_FileCopy */


#if !defined(OS_Has_FileEqual)

Int
osFileEqual(MutString src, MutString dest)
{
	FILE *	f1;
	FILE *	f2;
	Int	c1, c2;

	f1 = fopen(src, "r");
	if (!f1) return 0;

	f2 = fopen(dest, "r");
	if (!f2) { fclose(f1); return 0; }

	c1 = c2 = '\0';
	while (c1 == c2 && c1 != EOF && c2 != EOF) {
		c1 = fgetc(f1);
		c2 = fgetc(f2);
		if (c1 == '\r') c1 = fgetc(f1);
		if (c2 == '\r') c2 = fgetc(f2);
		if (c1 == OS_PATH_SEP[0]) c1 = '/';
		if (c2 == OS_PATH_SEP[0]) c2 = '/';
	}

	fclose(f1);
	fclose(f2);

	return c1 == c2;
}

#endif	/* ! OS_Has_FileEqual */


#if !defined(OS_Has_FileRemove)

Int
osFileRemove(MutString fn)
{
	return unlink(fn);
}

#endif	/* ! OS_Has_FileRemove */


#if !defined(OS_Has_FileBase)

MutString
osFileBase(MutString fn)
{
	MutString	res, start, limit;
	Length	cc;

	limit = strrchr(fn, '.');
	if (!limit) return fn;

    start = strrchr(fn, OS_PATH_SEP[0]);
    if (!start) start = fn; else start++;

	cc  = limit - start;
	res = strAlloc(cc + 1);
	strncpy(res, start, cc);
    res[cc] = 0;
	return res;
}

#endif	/* ! OS_Has_FileBase */


#if !defined(OS_Has_FileDir)

MutString
osFileDir(MutString fn, MutString _default)
{
	MutString	res, limit;
	Length	cc;

	limit = strrchr(fn, OS_PATH_SEP[0]);
	if (!limit) return _default;

	cc  = limit - fn;
	res = strAlloc(cc + 1);
	strncpy(res, fn, cc);
    res[cc] = 0;
	return res;
}

#endif	/* ! OS_Has_FileDir */


/******************************************************************************
 *
 * MutString utilities
 *
 *****************************************************************************/

#if !defined(OS_Has_FileCombine)

MutString
osFileCombine(MutString dir, MutString fn)
{
	return strPrintf("%s%s%s", dir, OS_PATH_SEP, fn);
}

#endif	/* ! OS_Has_FileBase */


#if !defined(OS_Has_FnameTempSeed)

Int
osFnameTempSeed(void)
{
	return getpid();
}

#endif	/* ! OS_Has_FnameTempSeed */


/* No generic version of osTempDirName. */

#define STR_FUDGE   5
MutString
strAlloc(Length cc)
{
	MutString	s = (MutString) malloc(STR_FUDGE*(cc + 1));

	s[0]  = '\0';
	s[cc] = '\0';

	return s;
}

Length
strLength(MutString s)
{
	return strlen(s);
}

MutString
strCopy(MutString s)
{
	Length	cc = strLength(s);
	MutString	t = strAlloc(cc);

	strcpy(t, s);
	return t;
}

MutString
strConcat(MutString s, MutString t)
{
	Length	cc = strLength(s) + strLength(t);
	MutString	r = strAlloc(cc);

	strcpy(r, s);
	strcat(r, t);
	return r;
}

Int
vstrPrintfCC(MutString fmt, va_list argp)
{
	int	    cc;

    FILE *fout = fopen("/dev/null",  "w");
    cc = vfprintf(fout, fmt, argp);
    fclose(fout);

    return cc;
}

MutString
strPrintf(MutString fmt, ...)
{
	Length	cc;
	MutString	s;
	va_list	argp;

    va_start(argp, fmt);
    cc = vstrPrintfCC(fmt, argp) + 1;
    va_end(argp);

	s = strAlloc(cc);
	va_start(argp, fmt);
	vsprintf(s, fmt, argp);
	va_end(argp);

	return s;
}

Int
strEqual(MutString s, MutString t)
{
	return strcmp(s, t) == 0;
}

Int
strIsPrefix(MutString pre, MutString s)
{
	Int	c0, cc;

	c0 = strLength(pre);
	cc = strLength(s);
	if (cc < c0) return 0;
	return strncmp(pre, s, c0) == 0;
}

Int
strIsSuffix(MutString suf, MutString s)
{
	Int	c0, cc;

	c0 = strLength(suf);
	cc = strLength(s);
	if (cc < c0) return 0;
	return strncmp(suf, s + cc - c0, c0) == 0;
}

MutString
strTrimSpaces(MutString s)
{
    Int len;
    while (isspace(*s)) s++;

    len = strlen(s);
    while (len > 0 && isspace(s[len-1])) {
        s[len-1] = 0;
        len--;
    }
    return s;
}


/******************************************************************************
 *
 * Debugging
 *
 *****************************************************************************/

Int  DoDebug = 0;

Int
debugPrintf(const char *fmt, ...) {
    int cc;
    if (!DoDebug) return 0;

    fprintf(stderr, "testAldor debug: ");
    va_list argp;
    va_start(argp, fmt);
    cc = vfprintf(stderr, fmt, argp);
    va_end(argp);

    return cc;
}

MutString LogFileName = 0;

Int
logPrintf(const char *fmt, ...) {
    int cc;
    if (!LogFileName) return 0;

    FILE *log = fopen(LogFileName, "a");
    if (log == NULL) return 0;

    va_list argp;
    va_start(argp, fmt);
    cc = vfprintf(log, fmt, argp);
    va_end(argp);

    fclose(log);

    return cc;
}
