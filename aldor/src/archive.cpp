///////////////////////////////////////////////////////////////////////////////
//
// archive.cpp: Operations for manipulating compiler library archives.
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

#include "axlobs.h"
#include "alar.h"

bool    arDebug         = false;
#define arDEBUG(s)      DEBUG_IF(arDebug, s)

#define AR_SEEK(ar,pos) fseek((ar)->file, pos, SEEK_SET)

/*****************************************************************************
 *
 * :: Local function declarations
 *
 ****************************************************************************/


local bool              arItemIsIntermed        (MutString);

local ArEntry           arAllocEntry            (MutString, Archive, Offset);
local void              arFreeEntry             (ArEntry);
local MutString            arEntryKey              (MutString);
local ArEntry           arFindEntry             (Archive, MutString);
local ArEntry           arFindLibEntry          (Archive, Lib);
local Lib               arExtractEntry          (Archive, ArEntry);
#define                 arEntryLib(ar,e)        \
        ((e)->lib ? (e)->lib : arExtractEntry(ar,e))

local void              arRdFormat              (Archive);
local void              arRdTable               (Archive);



local Archive           arNew                   (FileName, FILE *);

local void              arFilter                (Archive);
local void              arFilterScanMember      (Archive, ArEntry);
local void              arFilterMarkMember      (Archive, ArEntry);
local void              arFilterMarkExpanded    (Archive, ArEntry);





/*****************************************************************************
 *
 * :: Archive file lists
 *
 ****************************************************************************/

static char     arEmptyString[] = "";
MutString          arCurrentFileName = arEmptyString; /* archive member being compiled */
MutString          arCurrentFileId   = arEmptyString; /* ID of member being compiled */
PathList        arLibraryFileList = 0;  /* library archive file names */
PathList        arLibraryKeyList  = 0;  /* library archive file keys */

/*
 * Before processing the command line, reset the archive lists to those
 * archives on the default lists.
 */
void
arInit(MutString * files, MutString * keys)
{
        arLibraryFileList = pathListFrArray(files);
        arLibraryKeyList  = pathListFrArray(keys);

        return;
}

/*
 * Before processing each file, reset the archive lists to those archives
 * listed on the command line.
 */
void
arFileInit(FileName fn, String id)
{
        arCurrentFileName = strCopy(fn.unparseStaticWithout());
        if (strEqual(id, "axiom")) id = "";
        arCurrentFileId   = strCopy(id);
        return;
}

PathList
arLibraryFiles(void)
{
        return arLibraryFileList;
}

PathList
arLibraryKeys(void)
{
        return arLibraryKeyList;
}

void
arAddLibraryFile(MutString name)
{
        arLibraryFileList = pathConsDirectory(name, arLibraryFileList);
}

void
arAddLibraryKey(MutString key)
{
        arLibraryKeyList = pathConsDirectory(key, arLibraryKeyList);
}

local bool
arItemIsIntermed(MutString item)
{
        return ftypeEqual(FileName::parseStatic(item).type(), FTYPE_INTERMED);
}

/*****************************************************************************
 *
 * :: Archive entries
 *
 ****************************************************************************/

local ArEntry
arAllocEntry(MutString name, Archive ar, Offset pos)
{
        ArEntry         arent;

        arent = (ArEntry) stoAlloc(OB_Other, sizeof(*arent));

        arent->name     = name;
        arent->ar       = ar;
        arent->pos      = pos;
        arent->lib      = NULL;
        arent->mark     = true;

        return arent;
}

local void
arFreeEntry(ArEntry arent)
{
        stoFree((void *) arent);
}

local MutString
arEntryKey(MutString name)
{
        FileName        item = FileName::parseStatic(name);
        return item.unparseStaticWithout();
}

local ArEntry
arFindEntry(Archive ar, MutString name)
{
        List<ArEntry>     alist;

        arDEBUG(fprintf(dbOut, "Looking for \"%s\"", name));

        name = arEntryKey(name);
        arDEBUG(fprintf(dbOut, " as archive key \"%s\"", name));

        for (alist = ar->members; alist; alist = cdr(alist)) {
                ArEntry         arent = car(alist);
                if (strAEqual(name, arent->name)) {
                        arDEBUG(fprintf(dbOut, " at offset %ld\n",arent->pos));
                        return arent;
                }
        }

        arDEBUG(fprintf(dbOut, " not found.\n"));
        return NULL;
}

local ArEntry
arFindLibEntry(Archive ar, Lib lib)
{
        if (lib->arent == NULL)
                lib->arent = arFindEntry(ar, libToStringStatic(lib));

        if (lib->arent && lib->arent->ar == ar)
                return lib->arent;
        else
                return NULL;
}

local Lib
arExtractEntry(Archive ar, ArEntry arent)
{
        Lib     lib;

        lib = libExtract(FileName::parse(arent->name), arFile(ar), arent->pos);
        lib->arent = arent;

        arent->lib = lib;
        return lib;
}

/*****************************************************************************
 *
 * :: Portable Aldor archive format
 *
 ****************************************************************************/

/*
 * .al files are canonical BSD-style ar archives.  The common alar module is
 * also compiled into uniar, so the compiler and build tool have one parser.
 * Historical AIX/CMS/GNU-specific archive parsing has intentionally been
 * removed.  Pre-v0.18 .al files must be rebuilt once.
 */

local void
arRdFormat(Archive ar)
{
        if (!alarReadMagic(ar->file)) {
                ar->format = (ArFmtTag) 0;
                ar->size = 0;
                return;
        }
        ar->format = AR_Arch;
        ar->size = fileSize(ar->name);
}

local void
arRdTable(Archive ar)
{
        std::vector<AlarMember> table;
        List<ArEntry> members = listNil<ArEntry>;

        if (ar->format != AR_Arch ||
            !alarList((ar->name).unparseStatic(), &table))
                return;

        for (const auto &m: table) {
                MutString name = strCopy(m.name.c_str());
                if (arItemIsIntermed(name)) {
                        /* Archive lookup is by Aldor library key (e.g. "lang"),
                         * not by the physical member filename ("lang.ao"). */
                        MutString key = strCopy(arEntryKey(name));
                        strFree(name);
                        ArEntry arent = arAllocEntry(key, ar, (Offset) m.dataOffset);
                        members = listCons<ArEntry>(arent, members);
                        ar->hasIntermed = true;
                }
                else
                        strFree(name);
        }
        ar->members = listNReverse<ArEntry>(members);
}

/*****************************************************************************
 *
 * :: Archive files
 *
 ****************************************************************************/

local Archive
arNew(FileName fname, FILE *f)
{
        Archive         ar;

        ar = (Archive) stoAlloc(OB_Archive, sizeof(*ar));

        ar->name        = fname.copy();
        ar->hasFile     = true;
        ar->hasIntermed = false;
        ar->file        = f;

        ar->format      = (ArFmtTag) 0;
        ar->size        = 0;

        ar->item        = 0;
        ar->pos         = 0;
        ar->__next      = 0;

        ar->members     = listNil<ArEntry>;
        ar->symes       = listNil<Syme>;


        return ar;
}

Archive
arRead(FileName fname)
{
        Archive         ar = arNew(fname, fileRbOpen(fname));

        arDEBUG(fprintf(dbOut, "Opening archive \"%s\" for reading.\n",
                        fname.unparseStatic()));
        arRdFormat(ar);
        arRdTable(ar);

        return ar;
}

void
arClose(Archive ar)
{
        (ar->name).free();
        if (ar->hasFile) fclose(ar->file);
        listFreeDeeply<ArEntry>(ar->members, arFreeEntry);
        listFree<Syme>(ar->symes);

        stoFree((void *) ar);
}

bool
arEqual(Archive ar1, Archive ar2)
{
        return (ar1->name).equals(ar2->name);
}

Archive
arFrString(String name)
{
        static Table<String, Archive> tbl = nullptr;
        Archive         ar;
        FileName        fn;

        arDEBUG(fprintf(dbOut, "Looking for archive \"%s\"\n", name));

        if (tbl == 0)
                tbl = Table<String, Archive>::create<strAHash, strAEqual>();

        if ((ar = tbl.get(name, nullptr)) != 0)
                return ar;

        if ((fn = fileRdFind(libSearchPath(), name, FTYPE_AR_INT)) != 0)
                ar = arRead(fn);
        else {
                comsgWarning(NULL, ALDOR_W_CantUseArchive, name);
                ar = 0;
        }

        tbl.set(name, ar);
        return ar;
}

Lib
arFind(PathList path, MutString name)
{
        Archive         ar;
        ArEntry         arent;

        arDEBUG(fprintf(dbOut, "Looking for '%s' in the archives\n", name));

        for (; path != 0; path = cdr(path)) {
                arDEBUG(fprintf(dbOut, ">> Checking archive %s\n", car(path)));

                ar = arFrString(car(path));
                if (!ar) continue;

                arent = arFindEntry(ar, name);
                if (!arent) continue;

                return arEntryLib(ar, arent);
        }
        return NULL;
}

Lib
arFindInArchive(Archive ar, MutString name)
{
        ArEntry         arent;

        arent = arFindEntry(ar, name);

        if (!arent)
                return NULL;
        else
                return arent->lib;
}

AbSyn
arGetGlobalMacros(Archive ar)
{
        List<ArEntry> alist;
        List<AbSyn> lst = listNil<AbSyn>;

        if (!ar) return NULL;
        for (alist = ar->members; alist; alist = cdr(alist)) {
                AbSyn ab = libGetMacros(arEntryLib(ar, car(alist)));
                listPush(AbSyn, ab, lst);
        }
        return abNewSequenceL(SrcPos::none, lst);
}

List<Syme>
arGetLibrarySymes(Archive ar)
{
        List<ArEntry>     alist;
        List<Syme>        symes = listNil<Syme>;

        if (!ar) return symes;

        for (alist = ar->members; alist; alist = cdr(alist)) {
                Syme    lib = libLibrarySyme(arEntryLib(ar, car(alist)));
                listPush(Syme, lib, symes);
        }

        return listNReverse<Syme>(symes);
}

List<Syme>
arGetSymes(Archive ar)
{
        List<ArEntry>     alist;
        List<Syme>        symes;

        if (!ar) return 0;

        if (ar->symes)
                return ar->symes;

        arFilter(ar);

        for (alist = ar->members; alist; alist = cdr(alist)) {
                ArEntry         arent = car(alist);
                if (!arent->mark) continue;
                symes = listReverse<Syme>(libGetSymes(arEntryLib(ar, arent)));
                ar->symes = listNConcat<Syme>(symes, ar->symes);
        }

        ar->symes = listNReverse<Syme>(ar->symes);
        return ar->symes;
}

Syme
arLibrarySyme(Archive ar, Syme syme)
{
        syme = symeOriginal(syme);

        assert(symeLib(syme));
        return libLibrarySyme(symeLib(syme));
}

bool
arLibraryIsMember(Archive ar, Lib lib)
{
        return arFindLibEntry(ar, lib) != NULL;
}

bool
arHasBasicLib(Archive ar)
{
        static MutString   s = NULL;

        if (s == NULL) {
                FileName        fn = FileName::create("", "basic", FTYPE_INTERMED);
                s = fn.unparse();
                fn.free();
        }

        return arFindEntry(ar, s) != NULL;
}

/*****************************************************************************
 *
 * :: Archive member replacement
 *
 ****************************************************************************/

static bool     arUseExpanded = false;

void
arUseExpandedReplacement(void)
{
        arUseExpanded = true;
}

local void
arFilter(Archive ar)
{
        ArEntry         arent0 = arFindEntry(ar, arCurrentFileName);
        List<ArEntry>     alist;
        bool            precede = true;
        MutString tmp;

        if (arent0 == NULL)
                return;

        if (arent0->lib == NULL)
                return;

        tmp = libGetFileId(arent0->lib);

        if (!strEqual(tmp, arCurrentFileId))
                return;

        arDEBUG(fprintf(dbOut, "arFilter:\n"));
        comsgWarning(NULL, ALDOR_W_OverRideLibraryFile, arToString(ar));

        arent0->mark = false;
        for (alist = ar->members; alist; alist = cdr(alist)) {
                ArEntry         arent = car(alist);
                libLibrarySyme(arEntryLib(ar, arent));
                if (!arUseExpanded)
                        arent->mark = false;
                else if (arent == arent0)
                        precede = false;
                else if (precede)
                        arent->mark = true;
                else
                        arFilterMarkExpanded(ar, arent);
        }

        if (!arUseExpanded) arFilterScanMember(ar, arent0);
}

local void
arFilterScanMember(Archive ar, ArEntry arent0)
{
        List<Syme>        libs, libs0;
        List<ArEntry>     alist;
        bool            marked = true;

        arDEBUG(fprintf(dbOut, "    scanning %s\n", arent0->name));

        libs0 = libGetLibrarySymes(arEntryLib(ar, arent0));

        for (libs = libs0; libs; libs = cdr(libs)) {
                Lib     lib = symeLibrary(car(libs));
                ArEntry arent = arFindLibEntry(ar, lib);
                if (arent) arent->mark = true;
        }

        for (alist = ar->members; marked && alist; alist = cdr(alist)) {
                ArEntry arent = car(alist);
                if (arent == arent0) break;
                marked = arent->mark;
        }

        if (marked) return;

        for (; alist; alist = cdr(alist)) {
                ArEntry arent = car(alist);
                if (arent == arent0) break;
                arent->mark = false;
        }

        arFilterMarkMember(ar, arent0);
}

local void
arFilterMarkMember(Archive ar, ArEntry arent)
{
        List<Syme>        libs = libGetLibrarySymes(arEntryLib(ar, arent));

        arDEBUG(fprintf(dbOut, "    marking %s\n", arent->name));

        for (; libs; libs = cdr(libs)) {
                Lib     lib = symeLibrary(car(libs));
                ArEntry narent = arFindLibEntry(ar, lib);
                if (narent && !narent->mark) {
                        narent->mark = true;
                        arFilterMarkMember(ar, narent);
                }
        }
}

local void
arFilterMarkExpanded(Archive ar, ArEntry arent)
{
        List<Syme>        libs = libGetLibrarySymes(arEntryLib(ar, arent));

        arent->mark = true;
        for (; libs; libs = cdr(libs)) {
                Lib     lib = symeLibrary(car(libs));
                ArEntry narent = arFindLibEntry(ar, lib);
                if (narent && !narent->mark) {
                        arDEBUG(fprintf(dbOut, "    unmarking %s\n",
                                        arent->name));
                        arent->mark = false;
                        return;
                }
        }
}

