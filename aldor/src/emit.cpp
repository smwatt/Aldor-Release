///////////////////////////////////////////////////////////////////////////////
//
// emit.cpp: Compiler output.
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

# include "axlphase.h"
# include "compcfg.h"

/****************************************************************************
 *
 * :: Controlling options
 *
 **************************************************************************/

static String emitCName     = NULL;

static MutString   emitEntryFile   = NULL;
static MutString   emitOutputDir   = NULL;
static FileName emitOutputFileName[FTYPENO_LIMIT];
static MutString   emitFileIdName   = NULL;
static MutString   emitFileIdPrefix = NULL;

static bool     emitSolo        = false;
static bool     emitDoLineNos   = false;
static bool     emitDoRun       = false;
static bool     emitDoInterp    = false;

static bool     emitDo  [FTYPENO_LIMIT];
static bool     emitDone[FTYPENO_LIMIT];
static bool     emitNeed[FTYPENO_LIMIT];
static bool     emitKeep[FTYPENO_LIMIT];

static bool     emitWantDep;

#define emitSxIoMode            (emitDoLineNos ? SXRW_SrcPos : SXRW_NoSrcPos)

void
emitSetDependsWanted(bool flag)
{
        emitWantDep = flag;
}

bool
emitDependsWanted()
{
        return emitWantDep;
}

/*
 * Select the file types to be emitted.
 * Return 0 on success, -1 on failure.
 */
int
emitSelect(MutString arg)
{
        static bool     isInit = false;
        FTypeNo         ftn;
        int             rc;
        MutString          argbuf, ft, fn;

        if (!isInit) {
                int i;
                for (i = 0; i < FTYPENO_LIMIT; i++) {
                        emitDo  [i]       = false;
                        emitDone[i]       = false;
                        emitOutputFileName[i] = 0;
                }
                isInit = true;
        }

        /* Parse <ft>=<fn> */
        argbuf = strCopy(arg);
        for (fn = ft = argbuf; *fn; fn++)
                if (*fn == '=' || *fn == ':') {
                        *fn++ = 0;
                        break;
                }

        if      (ftypeIs(ft,FTYPENO_INCLUDED)) ftn = FTYPENO_INCLUDED;
        else if (ftypeIs(ft,FTYPENO_ABSYN))    ftn = FTYPENO_ABSYN;
        else if (ftypeIs(ft,FTYPENO_OLDABSYN)) {
                comsgWarning(NULL, ALDOR_W_OldOptAbsyn);
                ftn = FTYPENO_OLDABSYN;
        }
        else if (ftypeIs(ft,FTYPENO_INTERMED)) ftn = FTYPENO_INTERMED;
        else if (ftypeIs(ft,FTYPENO_FOAMEXPR)) ftn = FTYPENO_FOAMEXPR;
        else if (ftypeIs(ft,FTYPENO_SYMEEXPR)) ftn = FTYPENO_SYMEEXPR;
        else if (ftypeIs(ft,FTYPENO_SYMEEXPREXTENDED))
                                               ftn = FTYPENO_SYMEEXPREXTENDED;

        else if (ftypeEqual(ft, "lsp"))        ftn = FTYPENO_LISP;
        else if (ftypeEqual(ft, "c"))          ftn = FTYPENO_C;
        else if (ftypeEqual(ft, "c++"))        ftn = FTYPENO_CPP;
        else if (ftypeEqual(ft, "o"))          ftn = FTYPENO_OBJECT;
        else if (ftypeEqual(ft, "x"))          ftn = FTYPENO_EXEC;
        else if (ftypeEqual(ft, "axlmain")) {
                comsgWarning(NULL, ALDOR_W_OldOptMain);
                ftn = FTYPENO_AXLMAINC;
        }
        else if (ftypeEqual(ft, "main"))        ftn = FTYPENO_AXLMAINC;
        else                                   ftn = FTYPENO_LIMIT;


        if (ftn == FTYPENO_LIMIT) {
                rc = -1;
        }
        else {
                rc = 0;
                emitDo[ftn] = true;
                if (*fn) emitSetOutputFile(ftn, fn);
        }

        strFree(argbuf);

        return rc;
}

void
emitAllDone()
{
        int     i;
        for (i = 0; i < FTYPENO_LIMIT; i += 1)
                emitDone[i] = true;
}

void
emitSetDone(int i)
{
        emitDone[i] = true;
}

int
emitSetOutputDir(MutString dir)
{
        if (!osDirIsThere(dir)) return -1;
        emitOutputDir = dir;
        return 0;
}

void
emitSetCName(String str)
{
        emitCName = str;
}

int
emitSetOutputFile(FTypeNo ftno, MutString fname)
{
        if (emitOutputFileName[ftno]) emitOutputFileName[ftno].free();
        emitOutputFileName[ftno] = FileName::parse(fname);
        return 0;
}

void
emitSetEntryFile(MutString fname)
{
        emitEntryFile = fname;
}

MutString
emitGetEntryFile(void)
{
        return emitEntryFile;
}

void
emitSetDebug(bool wantDebug)
{
        emitDoLineNos = wantDebug;
        ccSetDebug(wantDebug);
}

void
emitSetProfile(bool wantProfile)
{
        ccSetProfile(wantProfile);
}

void
emitSetRun(bool flag)
{
        emitDoRun = flag;
}

void
emitSetInterp(bool flag)
{
        emitDoInterp = flag;
}

bool
emitIsEntry(EmitInfo finfo)
{
        FileName srcfn = emitSrcFile(finfo);

        if (finfo->isAXLmain) return false;

        return emitEntryFile && strEqual(srcfn.name(), emitEntryFile);
}

MutString
emitEntryFileName(void)
{
        return emitEntryFile;
}

void
emitSetFileIdName(MutString name)
{
        emitFileIdName = name;
}

void
emitSetFileIdPrefix(MutString name)
{
        emitFileIdPrefix = name;
}

void
emitDoneOptions(int argc, MutString *argv)
{
        int     i;
        FileName        fn;

        for (i = 0; i < FTYPENO_LIMIT; i++) {
                emitKeep[i] = emitDo[i];
                emitNeed[i] = emitDo[i];
        }

        /* If none of -[gF] are given, assume -Fao. */
        emitNeed[FTYPENO_INTERMED] = emitKeep[FTYPENO_INTERMED] |=
                !(emitDo[FTYPENO_INCLUDED] || emitDo[FTYPENO_CPP] ||
                  emitDo[FTYPENO_ABSYN]    || emitDo[FTYPENO_OLDABSYN] ||
                  emitDo[FTYPENO_FOAMEXPR] || emitDo[FTYPENO_SYMEEXPR] ||
                  emitDo[FTYPENO_LISP]     || emitDo[FTYPENO_C]        ||
                  emitDo[FTYPENO_OBJECT]   || emitDo[FTYPENO_EXEC]     ||
                  emitDo[FTYPENO_AXLMAINC]  || emitDoRun || emitDoInterp);

        emitNeed[FTYPENO_INTERMED] |= emitDoInterp;

        if (emitDoInterp) {
                int             i;

                for (i = 0; i < argc; i++) {
                        fn = FileName::parse(argv[i]);

                        if (ftypeHas(fn,FTYPENO_INTERMED)) {
                                emitKeep[FTYPENO_INTERMED] = true;
                                emitNeed[FTYPENO_INTERMED] = false;
                        }

                        fn.free();
                }
        }

        /* Certain file types imply prerequisites. */
        emitNeed[FTYPENO_C] |=
                  emitDo[FTYPENO_OBJECT]   || emitDo[FTYPENO_EXEC]     ||
                  emitDoRun;

        emitNeed[FTYPENO_OBJECT] |=
                  emitDo[FTYPENO_EXEC]     || emitDoRun;

        emitNeed[FTYPENO_AXLMAINC] |=
                  emitDo[FTYPENO_EXEC]     || emitDoRun;

        emitNeed[FTYPENO_EXEC] |=
                  emitDoRun;


        /* Set entry file and output directory. */
        if (!emitEntryFile && argc > 0) {
                FileName firstFn = FileName::parse(argv[0]);
                emitSetEntryFile(strCopy(firstFn.name()));
                firstFn.free();
        }
        if (!emitOutputDir)
                emitOutputDir = osCurDirName();

        emitSolo = argc == 1;
}


/******************************************************************************
 *
 * :: File information structure
 *
 *****************************************************************************/

# define        emitInfoFnameTempV(fi)  ((fi)->fnameTempV)
# define        emitInfoFnameTemp(fi,i) ((fi)->fnameTempV[i])
# define        emitInfoFname(fi, ft)   ((fi)->fname[ft])
# define        emitInfoInUse(fi, ft)   ((fi)->inUse[ft])
# define        emitInfoIsAXLmain(fi)   ((fi)->isAXLmain)
# define        emitInfoIsStdIn(fi)     ((fi)->isStdIn)

EmitInfo
emitInfoNew(FileName srcfn)
{
        static int      ftv[] = {FTYPENO_INCLUDED, FTYPENO_ABSYN,
                                 FTYPENO_OLDABSYN, FTYPENO_INTERMED,
                                 FTYPENO_FOAMEXPR, FTYPENO_SYMEEXPR,
                                 FTYPENO_SYMEEXPREXTENDED,
                                 FTYPENO_LISP, -1};
        EmitInfo        finfo;
        MutString          dir;
        String             name, type;
        int             i;

        finfo = (EmitInfo) stoAlloc((unsigned) OB_Other, sizeof(*finfo));
        finfo->flist = listNil<FileName>;
        emitInfoFnameTempV(finfo) = NULL;

        for (i = 0; i < FTYPENO_LIMIT; i++) {
                emitInfoFname(finfo, i) = NULL;
                emitInfoInUse(finfo, i) = false;
        }
        emitInfoIsAXLmain(finfo) = false;
        emitInfoFname(finfo, FTYPENO_SRC) = srcfn.copy();

        dir  = emitOutputDir;
        name = srcfn.name();
        for (i = 0; ftv[i] >= 0; i++) {
                type = ftypeString((FTypeNo)ftv[i]);
                emitInfoFname(finfo, ftv[i]) = FileName::create(dir, name, type);
        }

        finfo->isStdIn =  (srcfn.isStdin());

        /*
         * The temp file name vector and the other file names are filled in
         * on demand by emitFileName since we want a test whether a file
         * already exists to be as close to generating the file as possible.
         * Also, we don't want a lock file to be generated when we don't
         * need temp files since the lock files would be left over when
         * running axiomxl inside a debugger and aborting the debugger.
         */

        return finfo;
}

EmitInfo
emitInfoNewAXLmain(void)
{
        FileName srcfn = FileName::create(emitOutputDir, "aldormain", FTYPE_SRC);
        EmitInfo finfo = emitInfoNew(srcfn);

        emitInfoIsAXLmain(finfo) = true;

/*
        emitFileName(finfo, FTYPENO_C);
        emitFileName(finfo, FTYPENO_OBJECT);
*/

        srcfn.free();

        return finfo;
}

void
emitInfoShow(EmitInfo finfo)
{
        int i;
        FILE    *fout = osStdout;

        fprintf(fout, "<<");
        if (emitInfoIsAXLmain(finfo)) fprintf(fout, "[aldormain]");
        for (i = 0; i < FTYPENO_LIMIT; i++) {
                if (!finfo->fname[i] && !finfo->inUse[i]) continue;

                fprintf(fout, "| .%s ", ftypeString((FTypeNo)i));
                if (finfo->inUse[i])
                        fprintf(fout, " ** IN USE ** ");
                if (finfo->fname[i])
                        fprintf(fout, "\"%s\"",
                                (finfo->fname[i]).unparseStatic());
        }
        if (finfo->fnameTempV) {
                fprintf(fout, "| TEMP => ");
                for (i = 0; finfo->fnameTempV[i]; i++)
                        fprintf(fout, " \"%s\"",
                                (finfo->fnameTempV[i]).unparseStatic());
        }
        fprintf(fout, ">>\n");
}

void
emitInfoFree(EmitInfo finfo)
{
        int i;

        if (!finfo) return;

        if (emitInfoFnameTempV(finfo)) {
                for (i = 0; emitInfoFnameTemp(finfo, i); i++) {
                        FileName fn = emitInfoFnameTemp(finfo, i);
                        if (ftypeNo(fn.type()) == FTYPENO_LOCK)
                                fileRemove(fn);
                        fn.free();
                }
                stoFree((void *) emitInfoFnameTempV(finfo));
        }
        for (i = 0; i < FTYPENO_LIMIT; i++) {
                FileName        fn = emitInfoFname(finfo, i);

                if (!fn) continue;

                /* Warn if we couldn't do what was needed. */
                if (emitIsOutputNeeded(finfo, (FTypeNo)i) && !emitDone[i])
                        comsgWarning(NULL, ALDOR_W_NotCreatingFile,
                                     fn.unparse());

                fn.free();
        }
        listFree<FileName>(finfo->flist);
        stoFree((void *) finfo);
}

/*****************************************************************************
 *
 * :: File names to use.
 *
 *****************************************************************************/


FileName
emitSrcFile(EmitInfo finfo)
{
        return finfo->fname[FTYPENO_SRC];
}

MutString
emitGetFileIdName(EmitInfo finfo)
{
        String name;

        if (emitFileIdName)
                return emitFileIdName;

        name = emitSrcFile(finfo).name();
        if (emitFileIdPrefix)
                name = strConcat(emitFileIdPrefix, name);

        return strCopy(name);
}

local void
emitFnameTempAndLock(EmitInfo finfo, MutString dir, String name)
{
        FileName *      fntv;
        int             i;

        static String *emitTFTypes = 0;        /* Vector of temp file types. */

        if (!emitTFTypes) {
                emitTFTypes    = (String *) stoAlloc((unsigned) OB_Other,
                                                     5 * sizeof(MutString));
                emitTFTypes[0] = FTYPE_LOCK;
                emitTFTypes[1] = FTYPE_C;
                emitTFTypes[2] = FTYPE_OBJECT;
                emitTFTypes[3] = FTYPE_EXEC;
                emitTFTypes[4] = NULL;
        }

        /*
         * Let's hope nobody grabs the same temp file names between
         * calling fnameTempVector and creating the lock file.
         */
        fntv = FileName::tempVector(dir, name, emitTFTypes);
        if (!fntv)
                comsgFatal(NULL, ALDOR_F_CantFindTemp);
        else {
                for (i = 0; fntv[i]; i++)
                        if (ftypeNo(fntv[i].type()) == FTYPENO_LOCK) break;
                fclose(fileWrOpen(fntv[i]));
        }

        emitInfoFnameTempV(finfo) = fntv;
}

FileName
emitFileName(EmitInfo finfo, FTypeNo ft)
{
        FileName                srcfn, fn, fnt;
        MutString                  dir;
        String                     name, stype, type;
        bool                    tempFileNeeded, chk;
        FTypeNo                 fto = ft;
        int                     i;

        if (emitOutputFileName[ft])
                return emitOutputFileName[ft];

        if (emitInfoFname(finfo, ft))
                return emitInfoFname(finfo, ft);

        srcfn = emitSrcFile(finfo);
        dir   = emitOutputDir;
        name  = srcfn.name();
        type  = ftypeString(ft);
        stype = srcfn.type();

        fn    = FileName::create(dir, name, type);

        /*
         * Since some C compilers don't honor the `-o' flag when
         * generating an object file, we need to have the following
         * complicated logic to find out whether we need a temp file.
         *
         * An object file that's created by somebody else while we
         * work on the C file might get clobbered.
         */

        tempFileNeeded = emitIsOutputNeeded(finfo, ft) &&
                         !emitKeep[ft] && fileIsThere(fn);

        chk = tempFileNeeded;
        if (ft == FTYPENO_AXLMAINC)
                ft = FTYPENO_C;
        if (ft == FTYPENO_C) {
                fnt = FileName::create(dir, name, ftypeString(FTYPENO_OBJECT));
                tempFileNeeded |= (emitNeed[FTYPENO_OBJECT]
                                   && !emitKeep[FTYPENO_OBJECT]
                                   && fileIsThere(fnt));
                fnt.free();
        }
        else if (ft == FTYPENO_OBJECT) {
                /*
                 * Compute the C file name recursively if we don't have
                 * one yet so we know whether we need a temp object file.
                 */
                fnt = emitFileName(finfo, FTYPENO_C);
                tempFileNeeded = !strEqual(name, fnt.name());
        }

        if (tempFileNeeded) {
                if (chk && !srcfn.equals(fn) && emitIsGeneratedFile(fn)) {
                        MutString file = fn.unparse();
                        comsgWarning(NULL, ALDOR_W_WillObsolete, file);
                        strFree(file);
                }
                fn.free();
                if (!emitInfoFnameTempV(finfo))
                        emitFnameTempAndLock(finfo, dir, name);
                /* Find the temporary file name for the given file type.  */
                for (i = 0; emitInfoFnameTemp(finfo, i); i++)
                        if (ftypeEqual(emitInfoFnameTemp(finfo, i).type(),
                                     ftypeString(ft)))
                                break;
                assert(emitInfoFnameTemp(finfo,i));
                fn = emitInfoFnameTemp(finfo, i).copy();
        }
        emitInfoFname(finfo, ft) = fn;
        if (ft != fto) emitInfoFname(finfo, fto) = fn.copy();

        return fn;
}


bool
emitIsOutputNeeded(EmitInfo finfo, FTypeNo ft)
{
        bool need = emitNeed[ft];

        if (finfo && emitInfoIsStdIn(finfo))
                need = false;

        if (finfo && emitInfoIsAXLmain(finfo))
                need = need && emitNeed[FTYPENO_AXLMAINC];

        return need;
}

bool
emitIsOutputNeededOrWarn(EmitInfo finfo, FTypeNo ft)
{
        FileName        srcfn, fn;
        MutString          name;
        bool            needed, keep;

        srcfn  = emitSrcFile       (finfo);
        fn     = emitFileName      (finfo, ft);
        needed = emitIsOutputNeeded(finfo, ft);
        keep   = emitKeep[ft];

        if (!needed && fileIsThere(fn) && !srcfn.equals(fn)
            && emitIsGeneratedFile(fn)) {
                name = fn.unparse();
                comsgWarning(NULL, ALDOR_W_WillObsolete, name);
                strFree(name);
        }
        if (needed && keep && fileIsThere(fn)
            && (!emitIsGeneratedFile(fn) || emitInfoIsAXLmain(finfo) /* this could be wrong TTT */ )) {
                name = fn.unparse();
                comsgFatal(NULL, ALDOR_F_WdClobberFile, name);
                strFree(name);
        }
        if (needed && keep && srcfn.equals(fn)) {
                if (emitSolo) {
                        name = fn.unparse();
                        comsgFatal(NULL, ALDOR_F_WdClobberIn, name);
                        strFree(name);
                }
                else
                        needed = false;
        }

        return needed;
}

EmitInfo
emitExecutableNameOrWarn(int numFiles, EmitInfo *finfov)
{
        EmitInfo        finfo;
        FileName        fn;
        int             i;

        /* Let's check for errors first and find the entry finfo. */
        if (emitEntryFile) {
                for (i = 0; i < numFiles; i++)
                        if (finfov[i] && emitIsEntry(finfov[i])) break;
                if (i == numFiles) {
                        if (emitIsOutputNeeded(finfov[0], FTYPENO_EXEC))
                                comsgFatal(NULL,ALDOR_F_CmdCantUseEntry,
                                             emitEntryFile);
                        else
                                comsgWarning(NULL,ALDOR_W_CmdFunnyEntry,
                                             emitEntryFile);
                        return NULL;
                }
                finfo = finfov[i];
        }
        else
                finfo = finfov[0];

        /* We might see a `file out of date' warning here. */
        emitIsOutputNeededOrWarn(finfo, FTYPENO_EXEC);
        fn = emitFileName(finfo, FTYPENO_EXEC);

        if (strEqual(fn.dir(), "")) fn.setDir(osCurDirName());

        return finfo;
}


/*
 * Test whether a file was generated by Aldor.
 */
bool
emitIsGeneratedFile(FileName fn)
{
        int     tno;
        String hd1;
        FILE    *infile;
        bool    result;

        extern String emitCHdFmt1, emitLispHdFmt1;

        if (!fileIsThere(fn)) return false;

        tno = ftypeNo(fn.type());
        switch (tno) {
        case FTYPENO_C:    hd1 = emitCHdFmt1;    break;
        case FTYPENO_LISP: hd1 = emitLispHdFmt1; break;

        case FTYPENO_INCLUDED:
        case FTYPENO_ABSYN:
        case FTYPENO_OLDABSYN:
        case FTYPENO_INTERMED:
        case FTYPENO_FOAMEXPR:
        case FTYPENO_SYMEEXPR:
        case FTYPENO_SYMEEXPREXTENDED:
        case FTYPENO_OBJECT:
        case FTYPENO_EXEC:
                return true;
        default:
                return false;
        }

        infile    = fileRdOpen(fn);
        result = true;

        while (*hd1 && result) if (fgetc(infile)!=*hd1++) result = false;

        fclose(infile);
        return result;
}


/******************************************************************************
 *
 * :: File utilities: Rename and Remove
 *
 *****************************************************************************/

local void
emitFileRename(EmitInfo finfo, FTypeNo ft)
{
        FileName srcfn = emitSrcFile(finfo);
        String      name  = srcfn.name();
        FileName ofn   = emitFileName(finfo, ft);
        FileName nfn;

        if (emitInfoIsAXLmain(finfo)) return;

        if (!ftypeIs(srcfn.type(), ft) && !strEqual(name, ofn.name())
            && emitKeep[ft]) {
                nfn = FileName::create(ofn.dir(), name, ofn.type());
                emitInfoFname(finfo, ft) = nfn;
                fileRename(ofn, nfn);
                ofn.free();
        }
}

local void
emitFileRemove(EmitInfo finfo, FTypeNo ft)
{
        FileName srcfn = emitSrcFile(finfo);
        String      name  = srcfn.name();
        FileName ofn   = emitFileName(finfo, ft);
        FileName nfn;

        if (emitInfoIsAXLmain(finfo)) return;

        /* Remove an existing C/object file if it's going to be overwritten
           later in emitFileRename.  */
        if (!ftypeIs(srcfn.type(), ft) && !strEqual(name, ofn.name())
            && emitKeep[ft]) {
                nfn = FileName::create(ofn.dir(), name, ofn.type());
                fileRemove(nfn);
                nfn.free();
        }
}


/*****************************************************************************
 *
 * :: Syme list collectors
 *
 ****************************************************************************/

/*!!*/
extern List<Syme>          foamSymeList           (Foam);

List<Syme>
emitCollectIntermedSymes(Stab stab, Foam foam)
{
        List<Syme>        xsymes, fsymes, symes;

        xsymes = stabGetExportedSymes(stab);
        fsymes = foamSymeList(foam);

        symes = listNil<Syme>;
        symes = listNConcat<Syme>(symes, xsymes);
        symes = listNConcat<Syme>(symes, fsymes);

        return symes;
}

/*****************************************************************************
 *
 * :: Dumb emitters -- just do what they're told.
 *
 ****************************************************************************/

/*
 * Emit the .ai file of included source.
 */
void
emitTheIncluded(EmitInfo finfo, List<SrcLine> sll)
{
        FILE    *fout;
        FileName fn;

        fn = emitFileName(finfo, FTYPENO_INCLUDED);
        emitInfoInUse(finfo, FTYPENO_INCLUDED) = true;
        fout = fileWrOpen(fn);
        inclWrite(fout, sll);
        fclose(fout);
        emitInfoInUse(finfo, FTYPENO_INCLUDED) = false;
        emitSetDone(FTYPENO_INCLUDED);
}

/*
 * Emit the .ap abstract syntax file.
 */
void
emitTheAbSyn(EmitInfo finfo, AbSyn absyn)
{
        FILE    *fout;
        FileName fn;

        fn = emitFileName(finfo, FTYPENO_ABSYN);
        emitInfoInUse(finfo, FTYPENO_ABSYN) = true;
        fout = fileWrOpen(fn);
        abWrSExpr(fout, absyn, emitSxIoMode);
        fclose(fout);
        emitInfoInUse(finfo, FTYPENO_ABSYN) = false;
        emitSetDone(FTYPENO_ABSYN);
}

/*
 * Emit the .ax abstract syntax file.
 */
void
emitTheOldAbSyn(EmitInfo finfo, AbSyn absyn)
{
        FILE    *fout;
        FileName fn;

        fn = emitFileName(finfo, FTYPENO_OLDABSYN);
        emitInfoInUse(finfo, FTYPENO_OLDABSYN) = true;
        fout = fileWrOpen(fn);
        abWrSExpr(fout, absyn, emitSxIoMode);
        fclose(fout);
        emitInfoInUse(finfo, FTYPENO_OLDABSYN) = false;
        emitSetDone(FTYPENO_OLDABSYN);
}

/*
 * Emit the parts of a .ao file.
 */
void
emitTheIntermed(EmitInfo finfo, List<Syme> sl, Foam foam, AbSyn macs)
{
        Lib      lib;
        FileName fn;

        fn  = emitFileName(finfo, FTYPENO_INTERMED);
        emitInfoInUse(finfo, FTYPENO_INTERMED) = true;
        lib = libWrite(fn);
        libPutSymes(lib, sl, foam);
        libPutFoamSymes(lib, foam);
        libPutMacros(lib, macs);
        foam = libPutFoam(lib, foam);

        libPutFileId(lib, emitGetFileIdName(finfo));

        if (emitDoLineNos)
                libPutPos(lib, foam);

        libClose(lib);
        emitInfoInUse(finfo, FTYPENO_INTERMED) = false;
        emitSetDone(FTYPENO_INTERMED);
}

/*
 * Emit the files needed in order to compile against the .ao file
 */
extern void
emitTheDependencies     (EmitInfo finfo)
{
        FileName        fn;
        List<MutString>      lst;

        fn = emitFileName(finfo, FTYPENO_INTERMED);
        lst = libDependencies(fn);
        printf("Dependencies(%s):", emitGetFileIdName(finfo));

        while (lst) {
                printf(" %s", car(lst));
                lst = cdr(lst);
        }
        printf("\n");
}

/*
 * Emit lisp-readable symbol meaning information to the .asx file.
 */
void
emitTheSymbolExprExtended(EmitInfo finfo, List<Syme> symes, AbSyn macs)
{
        FILE    *fout;
        FileName fn;


        fn = emitFileName(finfo, FTYPENO_SYMEEXPREXTENDED);
        emitInfoInUse(finfo, FTYPENO_SYMEEXPREXTENDED) = true;
        fout = fileWrOpen(fn);
        symeListWrite(fout, emitSrcFile(finfo).name(), symes);
        fclose(fout);
        emitInfoInUse(finfo, FTYPENO_SYMEEXPREXTENDED) = false;
        emitSetDone(FTYPENO_SYMEEXPREXTENDED);
}

/*
 * Emit lisp-readable symbol meaning information to the .asy file.
 */
void
emitTheSymbolExpr(EmitInfo finfo, List<Syme> symes, AbSyn macs)
{
        FILE    *fout;
        FileName fn;


        fn = emitFileName(finfo, FTYPENO_SYMEEXPR);
        emitInfoInUse(finfo, FTYPENO_SYMEEXPR) = true;
        fout = fileWrOpen(fn);
        symeListWrSExpr(fout, emitSrcFile(finfo).name(),
                        symes, SXRW_Default);
        abWrSExpr(fout, macs, SXRW_Default);

        fclose(fout);
        emitInfoInUse(finfo, FTYPENO_SYMEEXPR) = false;
        emitSetDone(FTYPENO_SYMEEXPR);
}

/*
 * Emit lisp-readable Foam codes to the .fm file.
 */
void
emitTheFoamExpr(EmitInfo finfo, Foam foam)
{
        FILE    *fout;
        FileName fn;

        fn   = emitFileName(finfo, FTYPENO_FOAMEXPR);
        emitInfoInUse(finfo, FTYPENO_FOAMEXPR) = true;
        fout = fileWrOpen(fn);
        foamWrSExpr(fout, foam, emitSxIoMode);
        fclose(fout);
        emitInfoInUse(finfo, FTYPENO_FOAMEXPR) = false;
        emitSetDone(FTYPENO_FOAMEXPR);
}

/*
 * Emit the .l Lisp-code file.
 */
String emitLispHdFmt1 = ";;;\n;;; Lisp code generated by Aldor";
String emitLispHdFmt2 = " from file \"%s\"";
String emitLispHdFmt3 = ".\n;;;\n";

void
emitTheCpp() {
        emitSetDone(FTYPENO_CPP);
}

void
emitTheLisp(EmitInfo finfo, SExpr lispCode)
{
        FILE    *fout;
        MutString  fnstring;
        FileName srcfn, fn;

        srcfn    = emitSrcFile(finfo);
        fn       = emitFileName(finfo, FTYPENO_LISP);

        emitInfoInUse(finfo, FTYPENO_LISP) = true;
        fout     = fileWrOpen(fn);

        fnstring = srcfn.unparseStaticWithout();
        fprintf(fout, "%s", emitLispHdFmt1);
        if (!emitInfoIsAXLmain(finfo)) fprintf(fout, emitLispHdFmt2, fnstring);
        fprintf(fout, "%s", emitLispHdFmt3);

        for ( ; !sxiNull(lispCode); lispCode = sxCdr(lispCode)) {
                fprintf(fout, "\n");
                sxiWrite(fout, sxCar(lispCode), glWriteMode | emitSxIoMode);
        }
        fclose(fout);
        emitInfoInUse(finfo, FTYPENO_LISP) = false;
        emitSetDone(FTYPENO_LISP);
}

/*
 * Emit the .c C-code file.
 */
String emitCHdFmt1 = "/*\n * C code generated by Aldor";
String emitCHdFmt2 = " from file \"%s\"";
String emitCHdFmt3 = ".\n */\n";

void
emitTheC(EmitInfo finfo, List<CCode> cco)
{
        FILE            *fout=NULL, *hout = NULL;
        MutString          fnstring;
        FileName        srcfn, fn, hfn=NULL;
        CCodeMode       ccmode;
        int             i, l = listLength<CCode>(cco);
        bool            stdc;

        srcfn = emitSrcFile(finfo);
        fn    = emitFileName(finfo, FTYPENO_C);
        emitFileRemove(finfo, FTYPENO_C);
        emitInfoInUse(finfo, FTYPENO_C) = true;
        emitSetDone(FTYPENO_C);

        stdc = ccDoStandardC();

        if (l > 1) {
                hfn = emitFileName(finfo, FTYPENO_H);
                emitFileRemove(finfo, FTYPENO_H);
                emitInfoInUse(finfo, FTYPENO_H) = true;
                hout = fileWrOpen(hfn);

                fnstring = srcfn.unparseStaticWithout();
                fprintf(hout, "%s", emitCHdFmt1);
                fprintf(hout, emitCHdFmt2, fnstring);
                fprintf(hout, "%s", emitCHdFmt3);
                fprintf(hout, "\n");
        }

        ccmode  = stdc ? CCOM_StandardC : CCOM_OldC;
        ccmode = (CCodeMode)(ccmode |
                 (emitDoLineNos && ccLineNos() ? CCOM_LineNo : 0));

        for (i = 0; cco && cco != listNil<CCode>; cco = cdr(cco), i++) {
#define FN_PREF_LEN     5
#define FN_SUFF_LEN     3
                String     fnold;
                char            fnnew[FN_PREF_LEN + FN_SUFF_LEN + 10];
                FileName        fname;
                int             k, nf;
                if ((i || !hout) && i < l) {
                        if (ccoArgc(ccoArgv(car(cco))[0])) {
                                /* Need to check for name conflicts here. */
                                if (i == 1 || !hout)
                                        fout  = fileWrOpen(fn);
                                else {
                                        fnold = emitCName;
                                        if (!fnold) fnold = fn.name();

                                        for (k = 0; k < FN_PREF_LEN; k++) {
                                                if (!fnold[k]) break;
                                                fnnew[k] = fnold[k];
                                        }
                                        nf = (i > 1) ? i-1 : i;

                                        sprintf(fnnew + k, "%.*d",
                                                FN_SUFF_LEN, nf);

                                        fname = FileName::create(fn.dir(), fnnew, fn.type());
                                        /* Add to list of filenames */
                                        finfo->flist = listCons<FileName>(fname, finfo->flist);
                                        fout  = fileWrOpen(fname);
                                }
                                fnstring = srcfn.unparseStaticWithout();
                                fprintf(fout, "%s", emitCHdFmt1);
                                if (!emitInfoIsAXLmain(finfo))
                                        fprintf(fout, emitCHdFmt2, fnstring);
                                fprintf(fout, "%s", emitCHdFmt3);
                                fprintf(fout, "\n");
                                if (emitDoLineNos && ccLineNos())
                                        fprintf(fout, "#line 1 \"%s.as\"\n\n",
                                                fn.name());
                                fprintf(fout, "#include \"foam_c.h\"");
                                if (l > 1)
                                        fprintf(fout, "\n#include \"%s\"",
                                                hfn.unparseStatic());
                                ccoPrint(fout, car(cco), ccmode);
                                fclose(fout);
                        }
                }
                else
                        ccoPrint(hout, car(cco), ccmode);
        }
        if (finfo->flist) finfo->flist = listNReverse<FileName>(finfo->flist);
        emitInfoInUse(finfo, FTYPENO_C) = false;
        emitSetDone(FTYPENO_LISP);
        if (hout) {
                fclose(hout);
                emitInfoInUse(finfo, FTYPENO_H) = false;
                emitSetDone(FTYPENO_H);
        }
}

/*
 * Emit the .o object-code file by compiling C-code.
 */
void
emitTheObject(EmitInfo finfo)
{
        List<FileName>    fl;
        FileName cfile = emitFileName(finfo, FTYPENO_C);

        emitFileRemove(finfo, FTYPENO_OBJECT);

        emitInfoInUse(finfo, FTYPENO_OBJECT) = true;
        ccCompileFile(emitOutputDir, cfile);
        /* Need to emit an object file for each C file in filename list */
        for (fl = finfo->flist; fl; fl = cdr(fl))
                ccCompileFile(emitOutputDir, car(fl));
        emitInfoInUse(finfo, FTYPENO_OBJECT) = false;
        emitSetDone(FTYPENO_OBJECT);

        /* Keep if (-Fc && !aldormain.c) or (-Waldormain && aldormain.c). */
        if (emitKeep[FTYPENO_C] && !emitInfoIsAXLmain(finfo))
                ; /* Keep */
        else if (strEqual(emitSrcFile(finfo).type(), FTYPE_C))
                ; /* Keep */
        else if (emitDo[FTYPENO_AXLMAINC] && emitInfoIsAXLmain(finfo))
                ; /* Keep */
        else {
                fileRemove(cfile);
                if (listLength<FileName>(finfo->flist) > 0)
                        fileRemove(emitFileName(finfo, FTYPENO_H));
                /* Remove each C file in filename list */
                for (fl = finfo->flist; fl; fl = cdr(fl))
                        fileRemove(car(fl));
        }

        emitFileRename(finfo, FTYPENO_C);
}


/*****************************************************************************
 *
 * :: Producing and running executable file
 *
 ****************************************************************************/

local bool
emitIsKindOfObjectFile(FileName fn)
{
        String     type = fn.type();
        return  ftypeIs(type, FTYPENO_OBJECT) ||
                ftypeIs(type, FTYPENO_AR_OBJ) ||
                ftypeIs(type, FTYPENO_AR_INT);
}

static EmitInfo emitExecFinfo;

void
emitLink(int numFiles, EmitInfo * finfov)
{
        FileName *      files;
        FileName        srcfn;
        FileName        execfn;
        int             i, ix, tmpFiles = 0;

        /*
         * Compute the exec name first, so we get the `file out of date'
         * warning message even if we don't need an executable.
         */
        emitExecFinfo = emitExecutableNameOrWarn(numFiles, finfov);

        if (!emitExecFinfo
            || !emitIsOutputNeeded(emitExecFinfo, FTYPENO_EXEC)) {
                for (i = 0; i < numFiles - 1; i++)
                        emitFileRename(finfov[i], FTYPENO_OBJECT);
                return;
        }

        for (i = 0; i < numFiles; i++)
                tmpFiles += listLength<FileName>(finfov[i]->flist);

        files = (FileName *) stoAlloc((unsigned) OB_Other,
                                      (tmpFiles+numFiles) * sizeof(FileName));
        for (i = 0, ix = 0; i < numFiles; i++, ix++) {
                List<FileName>    finfo;
                srcfn = emitSrcFile(finfov[i]);
                if (emitIsKindOfObjectFile(srcfn)
                    && !emitInfoIsAXLmain(finfov[i]))
                        files[ix] = srcfn;
                else
                        files[ix] = emitFileName(finfov[i], FTYPENO_OBJECT);
                for (finfo = finfov[i]->flist;
                     finfo && finfo != listNil<FileName>;
                     finfo = cdr(finfo)) {
                        FileName        tmpobj;
                        ix++;
                        tmpobj = car(finfo);
                        tmpobj.setType(FTYPE_OBJECT);
                        files[ix] = tmpobj;
                }
        }

        emitInfoInUse(emitExecFinfo, FTYPENO_EXEC) = true;
        execfn = emitFileName(emitExecFinfo, FTYPENO_EXEC);
        ccSetOutputFile(execfn.unparse());
        ccLinkProgram(emitOutputDir, files, numFiles + tmpFiles);
        emitInfoInUse(emitExecFinfo, FTYPENO_EXEC) = false;
        emitSetDone(FTYPENO_AXLMAINC);
        emitSetDone(FTYPENO_EXEC);

        for (i = 0, ix = 0; i < numFiles - 1; i++, ix++) {
                List<FileName>    fl;
                srcfn = emitSrcFile(finfov[i]);
                if (!emitIsKindOfObjectFile(srcfn) &&
                    !emitKeep[FTYPENO_OBJECT]) {
                        if (!files[ix].equals(execfn))
                                fileRemove(files[ix]);
                        for (fl = finfov[i]->flist; fl; fl = cdr(fl)) {
                                ++ix;
                                if (!files[ix].equals(execfn))
                                        fileRemove(files[ix]);
                        }
                }
                emitFileRename(finfov[i], FTYPENO_OBJECT);
        }
        {
                FileName mainObj = emitFileName(
                        finfov[numFiles - 1], FTYPENO_OBJECT);
                if (!mainObj.equals(execfn))
                        fileRemove(mainObj);
        }

        /* The linked executable must survive object-file cleanup. */
        if (!fileIsThere(execfn)) {
                MutString name = execfn.unparse();
                fprintf(osStderr,
                        "Aldor: linked executable disappeared during cleanup: %s\n",
                        name);
                strFree(name);
                comsgFatal(NULL, ALDOR_F_LinkFailed, "post-link cleanup");
        }
}

int
emitRun(int argc1, MutString *argv1)
{
        FileName emitExecName;
        int      rc;

        if (!emitDoRun) return 0;

        /* If emitDoRun, we are sure that emitExecFinfo != NULL. */
        emitExecName = emitFileName(emitExecFinfo, FTYPENO_EXEC);

        rc = ccGoProgram(emitExecName, argc1, argv1);

        if (!emitKeep[FTYPENO_EXEC])
                fileRemove(emitExecName);

        return rc;
}

void
emitInterp(int argc1, MutString *argv1)
{
        FileName emitExecName;

        if (!emitDoInterp) return;

        /* If emitDoInterp, we are sure that emitExecFinfo != NULL. */
        emitExecName = emitFileName(emitExecFinfo, FTYPENO_INTERMED);

        fintFile(emitExecName);

        if (!emitKeep[FTYPENO_INTERMED])
                fileRemove(emitExecName);
}


/******************************************************************************
 *
 * :: Cleanup
 *
 *****************************************************************************/

void
emitCleanup(int numFiles, EmitInfo *finfov)
{
        EmitInfo        finfo;
        FileName        fn;
        MutString          name;
        int             i, j;

        if (!finfov) return;

        /* For each of the input files */
        for (i = 0; i < numFiles; i++) {
                finfo = finfov[i];
                if (!finfo) continue;
                /* For each file generated from the ith input file */
                for (j = 0; j < FTYPENO_LIMIT; j++) {
                        /* Don't remove files we didn't generate. */
                        if (!emitInfoFname(finfo, j)
                            || !emitIsOutputNeeded(finfo, (FTypeNo)j))
                                continue;

#ifdef AXL_EDIT_1_1_13_37
                        /*
                         * Deal with aldormain.c when j==FTYPENO_AXLMAINC
                         * and aldormain.o when j==FTYPENO_OBJECT.
                         */
                        if (emitInfoIsAXLmain(finfo) &&
                               (j != FTYPENO_AXLMAINC) &&
                               (j != FTYPENO_OBJECT))
                                continue;
#endif

                        fn   = emitInfoFname(finfo, j);
                        name = fn.unparse();

                        /* Need to remove all files in filename list */
                        for (; finfo->flist; finfo->flist = cdr(finfo->flist))
                                fileRemove(car(finfo->flist));
                        if (emitInfoInUse(finfo, j) && fileIsThere(fn)) {
                                comsgWarning(NULL, ALDOR_W_RemovingFile, name);
                                fileRemove(fn);
                        }
                        else if (fileIsThere(fn)) {
                                if (!emitKeep[j]) {
                                        /* We might not want a warning here. */
                                        comsgWarning(NULL,ALDOR_W_RemovingFile,name);
                                        fileRemove(fn);
                                }
                                else
                                        emitFileRename(finfo, (FTypeNo)j);
                        }
                        strFree(name);
                }
                emitAllDone(); /* FIXME: why is this in the inner loop? */
                emitInfoFree(finfo);
                finfov[i] = 0;
        }
}
