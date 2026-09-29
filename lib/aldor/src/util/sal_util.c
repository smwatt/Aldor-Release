/****************************** sal_util.c *********************************
 * Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-97       *
 * Copyright (c) Manuel Bronstein 1994-2000                                *
 * Copyright (c) INRIA 1998, Version 29-10-98                              *
 * Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998            *
 ***************************************************************************/

#include "cport.h"

long cerrno(void) { return((long) errno); }
long lfputc(long c, FILE *stream)  { return((long) fputc((int) c, stream)); }
long lungetc(long c, FILE *stream) { return((long) ungetc((int) c, stream)); }
long fseekset(FILE *s, long n)     { return((long) fseek(s, n, SEEK_SET)); }
long fseekcur(FILE *s, long n)     { return((long) fseek(s, n, SEEK_CUR)); }
long fseekend(FILE *s, long n)     { return((long) fseek(s, n, SEEK_END)); }

/*
 * Adapter for File.uniqueName.  Generated C represents the Aldor-level
 * Pointer -> MachineInteger signature as word-sized values; declaring libc
 * mkstemp with that signature conflicts with the system prototype on hosts
 * (notably macOS) where mkstemp is visible from <stdlib.h>.  Keep the libc
 * declaration here and expose a distinct Aldor ABI entry point instead.
 *
 * The supported Unix/Cygwin targets are LP64, so long and a data pointer are
 * both one machine word.  Close the descriptor immediately: uniqueName needs
 * the atomically-created pathname, not an open descriptor.
 */
unsigned long
salMkstemp(unsigned long nameWord)
{
#if defined(ENV_MSVC) || defined(ENV_MINGW)
	(void) nameWord;
	return (unsigned long) -1;
#else
	char *name = (char *) (uintptr_t) nameWord;
	int fd = mkstemp(name);
	if (fd >= 0)
		close(fd);
	return (unsigned long) (long) fd;
#endif
}

unsigned long
salUnlink(unsigned long nameWord)
{
	const char *name = (const char *) (uintptr_t) nameWord;
	return (unsigned long) (long) remove(name);
}

#if defined(ENV_MSVC) || defined(ENV_MINGW)
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

/* returns a time-based seed for the random number generator */
long randomSeed(void)
{
#if defined(ENV_MSVC) || defined(ENV_MINGW)
	struct _timeb tv;	/* Microsoft compiler does not offer */
	_ftime(&tv);		/* timeval and gettimeofday */
	return (tv.millitm - tv.time); /* closest match is _ftime */
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);        /* supported by most O/S */
	return(tv.tv_usec - tv.tv_sec); /* some minimal shuffling */
#endif
}

