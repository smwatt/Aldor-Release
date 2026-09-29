///////////////////////////////////////////////////////////////////////////////
//
// path.cpp: File system search paths.
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
# include "fname.h"
# include "file.h"
# include "list.h"

CREATE_LIST(MutString);

PathList        incDirectoryList = 0;   /* include directory search path */
PathList        libDirectoryList = 0;   /* library directory search path */
PathList        binDirectoryList = 0;   /* execute directory search path */

PathList        incDirectoryDelta = 0;  /* as above, but excluding defaults */
PathList        libDirectoryDelta = 0;
PathList        binDirectoryDelta = 0;

/*****************************************************************************
 *
 * Search paths
 *
 ****************************************************************************/


void
pathInit(void)
{
        incDirectoryList = pathListFrString(osIncludePath());
        libDirectoryList = pathListFrString(osLibraryPath());
        binDirectoryList = pathListFrString(osExecutePath());

        incDirectoryDelta = 0;
        libDirectoryDelta = 0;
        binDirectoryDelta = 0;
}

PathList
pathListFrString(MutString path)
{
        Length          n = osPathLength(path);
        MutString          sbuf = strAlloc(strLength(path));
        MutString *        pathv = (MutString *) stoAlloc((int) OB_Other,
                                                    n * sizeof(MutString));
        MutStringList      result = listNil(MutString);

        osPathParse(pathv, sbuf, path);
        while (n > 0)
                result = listCons(MutString)(pathv[--n], result);
        stoFree((void *) pathv);

/*      assert( result != listNil(MutString) ); */
        return (PathList) result;
}

PathList
pathListFrArray(MutString * pathv)
{
        Length          n = 0;
        MutStringList      result = listNil(MutString);

        while (pathv[n] != 0)
                result = listCons(MutString)(pathv[n++], result);
        result = listNReverse(MutString)(result);

        assert( result != listNil(MutString) );
        return (PathList) result;
}

/* Add the directory to the front of the list dl.
 * Return the new list.
 */
PathList
pathConsDirectory(MutString directory, PathList pl)
{
        return (PathList) listCons(MutString)(directory, (MutStringList) pl);
}

PathList incSearchPath(void)      { return incDirectoryList; }
PathList libSearchPath(void)      { return libDirectoryList; }
PathList binSearchPath(void)      { return binDirectoryList; }

PathList incSearchPathDelta(void) { return incDirectoryDelta; }
PathList libSearchPathDelta(void) { return libDirectoryDelta; }
PathList binSearchPathDelta(void) { return binDirectoryDelta; }

void
fileAddIncludeDirectory(MutString directory)
{
        incDirectoryList  = pathConsDirectory(directory, incDirectoryList);
        incDirectoryDelta = pathConsDirectory(directory, incDirectoryDelta);
}

void
fileAddLibraryDirectory(MutString directory)
{
        libDirectoryList  = pathConsDirectory(directory, libDirectoryList);
        libDirectoryDelta = pathConsDirectory(directory, libDirectoryDelta);
}

void
fileAddExecuteDirectory(MutString directory)
{
        binDirectoryList  = pathConsDirectory(directory, binDirectoryList);
        binDirectoryDelta = pathConsDirectory(directory, binDirectoryDelta);
}

FileName
fileRdFind(PathList path, String fn, String ft)
{
        MutStringList      dl = (MutStringList) path;
        FileName        filename;

        for( ; dl != listNil(MutString); dl = cdr(dl) ) {
                filename = FileName::parseStaticWithin(fn, car(dl));
                if (fileIsReadable(filename))
                        return filename.copy();
                if (*filename.type() == '\0') {
                        FileName candidate = filename.withType(ft);
                        if (fileIsReadable(candidate))
                                return candidate;
                        candidate.free();
                }
        }
        return 0;
}

MutString
fileSubdir(String relativeTo, String sd)
{
        Length l = osSubdirLength(relativeTo, sd);
        MutString b = strAlloc(l);

        osSubdir(b, relativeTo, sd);

        return b;
}
