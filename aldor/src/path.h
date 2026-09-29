/*****************************************************************************
 *
 * path.h: File system search paths.
 *
 * This file is part of Aldor.
 *
 * Aldor is licensed under the Apache License, Version 2.0.
 *
 *
 * See legal/LICENSE in the Aldor distribution for details.
 *
 * Copyright (C) 1990-2026 Stephen M. Watt.
 */

#ifndef _PATH_H_
#define _PATH_H_

# include "axlport.h"
# include "list.h"

DECLARE_LIST(MutString);

typedef MutStringList      PathList;

extern void             pathInit                (void);

extern PathList         pathListFrString        (MutString);
extern PathList         pathListFrArray         (MutString *);
extern PathList         pathConsDirectory       (MutString, PathList);
                /*
                 * Form search paths.
                 */

extern PathList         incSearchPath           (void);
extern PathList         libSearchPath           (void);
extern PathList         binSearchPath           (void);
                /*
                 * These list directories to search, including the defaults.
                 */

extern PathList         incSearchPathDelta      (void);
extern PathList         libSearchPathDelta      (void);
extern PathList         binSearchPathDelta      (void);
                /*
                 * These contain what has been added and exclude the defaults.
                 */

extern void             fileAddIncludeDirectory (MutString);
extern void             fileAddLibraryDirectory (MutString);
extern void             fileAddExecuteDirectory (MutString);
                /*
                 * Push directories onto the appropriate lists.
                 */

extern FileName         fileRdFind              (PathList, String, String);
extern MutString           fileSubdir              (String relativeTo, String sd);

#endif /* !_PATH_H_ */
