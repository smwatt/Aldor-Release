/******************************************************************************
 *
 * util.h
 *
 * Generic versions of low-level utilities.
 *
 *****************************************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

#include "cport.h"

/******************************************************************************
 *
 * Declarations
 *
 *****************************************************************************/

typedef long            Int;

#define	fnewline(s)	    fputc('\n', s)
#define	exitFailure()	exit(1)
#define exitSuccess()	exit(0)

#define	_OF(X)	X

/******************************************************************************
 *
 * MutString utilities
 *
 *****************************************************************************/

MutString		strAlloc		_OF((Length));
Length		strLength		_OF((MutString));
MutString		strCopy			_OF((MutString));
MutString		strConcat		_OF((MutString, MutString));
MutString		strPrintf		_OF((MutString, ...));
MutString		strTrimSpaces	_OF((MutString));
Int         vstrPrintfCC    _OF((MutString, va_list));
Int		    strEqual		_OF((MutString, MutString));
Int		    strIsPrefix		_OF((MutString, MutString));
Int		    strIsSuffix		_OF((MutString, MutString));


/******************************************************************************
 *
 * Debugging
 *
 *****************************************************************************/

extern Int    DoDebug;
extern MutString LogFileName;

Int         debugPrintf     _OF((const char *fmt, ...));
Int         logPrintf       _OF((const char *fmt, ...));


/******************************************************************************
 *
 * OS-level utilities
 *
 *****************************************************************************/

extern MutString OS_PATH_SEP;

Int		osRun			_OF((MutString cmd));
	/*
	 * osRun(cmd)
	 *	Execute the command from the operating system.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osRunOutput		_OF((MutString cmd, MutString fout));
	/*
	 * osRunOutput(cmd)
	 *	Execute the command from the operating system.
	 *	Direct the output from the command to the file fout.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osRunScript		_OF((MutString cmd, MutString fout));
	/*
	 * osRunScript(cmd, fout)
	 *	Execute the command script from the operating system.
	 *	Direct the output from the command to the file fout.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osShowDiff		_OF((MutString src, MutString dest));
	/*
	 * osShowDiff(src, dest)
	 *	Show the differences between the named files.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osPutEnv		_OF((MutString eqn));
	/*
	 * osPutEnv(eqn)
	 *	Add the assignment given by eqn to the current environment.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osGetCurDir		_OF((MutString fn, Length cc));
	/*
	 * osGetCurDir(fn, cc)
	 *	Place the name of the current working directory in fn.
	 *	cc is the length of the destination buffer fn.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osSetCurDir		_OF((MutString fn));
	/*
	 * osSetCurDir(fn)
	 *	Change the current working directory to fn.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osMakeDir		_OF((MutString fn));
	/*
	 * osMakeDir(fn)
	 *	Created the named directory.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osRemoveDir		_OF((MutString fn));
	/*
	 * osRemoveDir(fn)
	 *	Remove the named directory.
	 *	Return 0 on success and -1 on failure.
	 */
Int		osFileIsThere		_OF((MutString fn));
	/*
	 * osFileIsThere(fn)
	 *	Check for the presence of the named file.
	 *	Return 1 if the file is found, 0 otherwise.
	 */

Int		osDirIsThere		_OF((MutString dn));
	/*
	 * osDirIsThere(fn)
	 *	Check for the presence of the named directory.
	 *	Return 1 if the directory is found, 0 otherwise.
	 */

Int		osFileCat		_OF((MutString fn));
	/*
	 * osFileCat(fn)
	 *	Print the contents of the named file on stdout.
	 *	Return 0 on success and -1 on failure.
	 */

Int		osFileCopy		_OF((MutString src, MutString dest));
	/*
	 * osFileCat(src, dest)
	 *	Copy the named file src to dest.
	 *	Return 0 on success and -1 on failure.
	 */

Int		osFileEqual		_OF((MutString src, MutString dest));
	/*
	 * osFileEqual(src, dest)
	 *	Return 1 if the contents of src and dest are the same.
	 *	Return 0 otherwise.
	 */

Int		osFileRemove		_OF((MutString fn));
	/*
	 * osFileRemove(fn)
	 *	Remove the named file.
	 *	Return 0 on success and -1 on failure.
	 */

MutString		osFileBase		_OF((MutString fn));
	/*
	 * osFileBase(fn)
	 *	Return the base name of the named file,
     *  i.e. without directory or extension (trailing ".[^.]*"), if there.
	 */

MutString		osFileDir		_OF((MutString fn, MutString _default));
	/*
	 * osFileDir(fn)
	 *	Return the directory part of the named file.
     *  If there is no directory part, then _default is returned.
	 */

MutString		osFileCombine		_OF((MutString dir, MutString fn));
	/*
	 * osFileCombine(dir, fn)
	 *	Return the file name of a file named fn in the directory dir.
	 */

Int		osFnameTempSeed		_OF((void));
	/*
	 * osFnameTempSeed()
	 * 	Return a unique id for use in constructing temporary names.
	 */

MutString		osTempDirName		_OF((void));
	/*
	 * osTempDirName()
	 *	Return the name of a unique temporary directory.
	 */

#endif	/* !_UTIL_H_ */
