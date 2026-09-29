/*****************************************************************************
 *
 * archive.h: Operations for manipulating compiler library archives.
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

#ifndef _ARCHIVE_H_
#define _ARCHIVE_H_

#include "axlobs.h"

/*****************************************************************************
 *
 * :: Archive file lists
 *
 ****************************************************************************/

extern void     arInit                  (MutString * files, MutString * keys);
extern void     arFileInit              (FileName, String);

extern PathList arLibraryFiles          (void);
extern PathList arLibraryKeys           (void);

extern void     arAddLibraryFile        (MutString);
extern void     arAddLibraryKey         (MutString);

/*****************************************************************************
 *
 * :: Archive file formats
 *
 ****************************************************************************/

enum arFmtTag {
        AR_START,
        AR_Arch = AR_START,
        AR_LIMIT
};

typedef enum arFmtTag          ArFmtTag;

/*****************************************************************************
 *
 * :: Archive structures
 *
 ****************************************************************************/

DECLARE_LIST(ArEntry);

struct ar_entry {
        MutString          name;           /* Member name. */
        Archive         ar;             /* Containing archive. */
        Offset          pos;            /* Member position. */
        Lib             lib;            /* Library for the member. */
        BPack(bool)     mark;           /* Filter mark. */
};

struct archive {
        FileName        name;           /* File path name. */
        BPack(bool)     hasFile;        /* Have we opened the stream? */
        BPack(bool)     hasIntermed;    /* Do we contain an ao file? */
        FILE *          file;           /* Stream. */

        ArFmtTag        format;         /* Archive file format. */
        Offset          size;           /* Archive file length. */

        MutString          item;           /* Name of the mru item. */
        Offset          pos;            /* Position of the mru item data. */
        Offset          __next;         /* Position of the next item hdr. */

        ArEntryList     members;        /* Archive members. */
        SymeList        symes;          /* List of symes. */

};

/******************************************************************************
 *
 * :: Basic operations
 *
 *****************************************************************************/

#define         arHasIntermed(ar)       ((ar)->hasIntermed)
#define         arFile(ar)              ((ar)->file)
#define         arSize(ar)              ((ar)->size)
#define         arItem(ar)              ((ar)->item)
#define         arPosition(ar)          ((ar)->pos)

extern Archive  arRead                  (FileName);
extern void     arClose                 (Archive);
extern bool     arEqual                 (Archive, Archive);

extern Archive  arFrString              (String);
#define         arToString(ar)          ((ar)->name).unparse()
#define         arToStringStatic(ar)    ((ar)->name).unparseStaticWithout()

extern Lib      arFind                  (PathList, MutString);
extern Lib      arFindInArchive         (Archive,  MutString);
extern SymeList arGetLibrarySymes       (Archive);
extern SymeList arGetSymes              (Archive);
extern Syme     arLibrarySyme           (Archive, Syme);
extern bool     arLibraryIsMember       (Archive, Lib);
extern bool     arHasBasicLib           (Archive);
extern void     arUseExpandedReplacement(void);
extern AbSyn    arGetGlobalMacros       (Archive);

#endif /* !_ARCHIVE_H_ */
