///////////////////////////////////////////////////////////////////////////////
//
// fname.cpp: File name type
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

struct FileName::Rep { MutString partv[NARY]; };

MutString *
FileName::parts() const noexcept
{
        return rep_->partv;
}

void
FileName::setPartRaw(int part, MutString value) noexcept
{
        rep_->partv[part] = value;
}

FileName
FileName::templateName()
{
        FileName fn;
        Length   i, sz;

        sz = osFnameNParts * sizeof(MutString);
        fn = FileName((Rep *) stoAlloc((unsigned) OB_Other, sz));
        for (i = 0; i < osFnameNParts; i++) fn.rep_->partv[i] = 0;

        return fn;
}

MutString FileName::dir() const noexcept { return rep_ ? rep_->partv[FNAME_DIR] : nullptr; }
MutString FileName::name() const noexcept { return rep_ ? rep_->partv[FNAME_NAME] : nullptr; }
MutString FileName::type() const noexcept { return rep_ ? rep_->partv[FNAME_TYPE] : nullptr; }
void FileName::setName(String s) { if (rep_->partv[FNAME_NAME]) strFree(rep_->partv[FNAME_NAME]); rep_->partv[FNAME_NAME] = strCopy(s); }
void FileName::setType(String s) { if (rep_->partv[FNAME_TYPE]) strFree(rep_->partv[FNAME_TYPE]); rep_->partv[FNAME_TYPE] = strCopy(s); }

FileName
FileName::create(String dir, String name, String type)
{
        FileName nfn = templateName();
        nfn.rep_->partv[FNAME_DIR] = strCopyIf(dir);
        nfn.rep_->partv[FNAME_NAME] = strCopyIf(name);
        nfn.rep_->partv[FNAME_TYPE] = strCopyIf(type);
        return nfn;
}

FileName
FileName::standardInput()
{
        return create(NULL, "-", NULL);
}

FileName
FileName::standardOutput()
{
        return create(NULL, "-", NULL);
}

bool
FileName::isStdin() const
{
        return strEqual(name(), "-");
}

bool
FileName::isStdout() const
{
        return strEqual(name(), "-");
}

FileName
FileName::copy() const
{
        FileName nfn = templateName();
        Length   i;
        for (i = 0; i < osFnameNParts; i++)
                nfn.rep_->partv[i] = strCopyIf(rep_->partv[i]);
        return nfn;
}

FileName
FileName::withType(String type) const
{
        FileName nfn = templateName();
        Length   i;

        for (i = 0; i < osFnameNParts; i++)
                nfn.rep_->partv[i] = strCopyIf(i == FNAME_TYPE
                                          ? type : rep_->partv[i]);
        return nfn;
}

bool
FileName::equals(FileName g) const
{
        Length  i;

        for (i = 0; i < osFnameNParts; i++) {
                String fi = rep_->partv[i] ? rep_->partv[i] : "";
                String gi = g.rep_->partv[i] ? g.rep_->partv[i] : "";

                if (i == FNAME_DIR) {
                        if (!osFnameDirEqual(fi, gi)) return false;
                }
                else {
                        if (!strEqual(fi, gi)) return false;
                }
        }
        return true;
}

bool
operator==(FileName f, FileName g)
{
        return f.equals(g);
}

bool
FileName::hasDir() const
{
        return dir()  && *dir();
}

bool
FileName::hasType() const
{
        return type() && *type();
}

void
FileName::setDir(String dir)
{
        if (rep_->partv[FNAME_DIR]) strFree(rep_->partv[FNAME_DIR]);
        rep_->partv[FNAME_DIR] = strCopy(dir);
}

void
FileName::free()
{
        Length  i;

        if (!rep_) return;

        for (i = 0; i < osFnameNParts; i++)
                if (rep_->partv[i]) strFree(rep_->partv[i]);

        stoFree((void *) rep_);
        rep_ = nullptr;
}


/*
 * Parse a string into a static file name according to OS-specific rules.
 */
FileName
FileName::parse(String file)
{
        return parseStatic(file).copy();
}

FileName
FileName::parseStatic(String file)
{
        return parseStaticWithin(file, NULL);
}

FileName
FileName::parseStaticWithin(String file, String indir)
{
        static FileName fn  = 0;
        static Buffer buf = 0;

        if (!buf) buf = Buffer::create();
        if (!fn)  fn  = templateName();

        buf.need(osFnameParseSize(file, indir));

        osFnameParse(fn.rep_->partv, buf.chars(), file, indir);

        return fn;
}

/*
 * Format a file name according to local OS rules.
 */
MutString
FileName::unparse() const
{
        return strCopy(unparseStatic());
}

MutString
FileName::unparseStatic() const
{
        static Buffer buf = 0;

        if (!buf) buf = Buffer::create();

        buf.need(osFnameUnparseSize(rep_->partv, false));

        return osFnameUnparse(buf.chars(), rep_->partv, false);
}

MutString
FileName::unparseStaticWith() const
{
        static Buffer buf = 0;

        if (!buf) buf = Buffer::create();

        buf.need(osFnameUnparseSize(rep_->partv, true));

        return osFnameUnparse(buf.chars(), rep_->partv, true);
}

MutString
FileName::unparseStaticWithout() const
{
        static char emptyDir[] = "";
        MutString  odir, image;

        odir = (MutString) dir();
        rep_->partv[FNAME_DIR] = emptyDir;

        image   = unparseStatic();

        rep_->partv[FNAME_DIR] = odir;
        return image;
}

/*
 * Find a new temporary file name.
 */
local char      mod36Digits[] =  "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

local void
intToMod36(int n, MutString buf, Length len)
{
        int i;

        for (i = len-1; i >= 0 && n >= 0; i--, n /= 36)
                buf[i] = mod36Digits[n % 36];
        for (         ; i >= 0; i--)
                buf[i] = mod36Digits[0];
}

#define TN_Prefix       "tt"    /* default prefix for temp file names */

#define TN_PrefixLen    2       /* max number of chars copied from file name */
#define TN_SeedLen      4       /* number of chars for random seed */
#define TN_CountLen     2       /* number of chars for counter */
#define TN_Len          (TN_PrefixLen + TN_SeedLen + TN_CountLen)  /* <= 8 */

FileName
FileName::tempVectorStatic(String dir, String name, String * type)
{
        static FileName nfn   = 0;
        static int              count = -1;     /* Last count number used. */
        static MutString           dirbuf = NULL;
        static MutString           typebuf = NULL;
        int                     maxcount, i;
        static char             buf[TN_Len+1];

        if (!nfn) nfn = templateName();

        if (dirbuf) strFree(dirbuf);
        dirbuf = strCopyIf(osFnameTempDir(dir));
        nfn.rep_->partv[FNAME_DIR] = dirbuf;
        nfn.rep_->partv[FNAME_NAME] = buf;

        /* Copy the first TN_PrefixLen chars from fn's name if available.  */
        if (name && *name) {
                buf[0] = name[0];
                buf[1] = name[1] ? name[1] : '0';
        }
        else
                strcpy(buf, TN_Prefix);

        /* Append the process seed.  */
        intToMod36(osFnameTempSeed(), &buf[TN_PrefixLen], TN_SeedLen);
        buf[TN_Len] = '\0';

        /* Calculate the maximum count.  */
        for (maxcount = 1, i = 0; i < TN_CountLen; i++) maxcount *= 36;

        /* Append a counter and test whether the files exist.  */
        while (++count < maxcount) {
                intToMod36(count, &buf[TN_PrefixLen+TN_SeedLen], TN_CountLen);
                for (i = 0; type[i]; i++) {
                        if (typebuf) strFree(typebuf);
                        typebuf = strCopy(type[i]);
                        nfn.rep_->partv[FNAME_TYPE] = typebuf;
                        if (fileIsThere(nfn)) break;
                }
                if (!type[i]) break;
        }

        if (count >= maxcount)
                return 0;       /* Couldn't find unused temp file names. */

        return nfn;
}

FileName
FileName::temp(String dir, String name, String type)
{
        String ftv[2];
        FileName nfn;

        ftv[0] = type;
        ftv[1] = 0;
        nfn    = tempVectorStatic(dir, name, ftv);
        return (nfn == 0) ? 0 : nfn.copy();
}

FileName *
FileName::tempVector(String dir, String name, String * ftv)
{
        FileName        nfn = tempVectorStatic(dir, name, ftv);
        FileName *      buf;
        int             i, n;

        if (nfn == 0) return 0;

        for (n = 0; ftv[n]; n++);
        buf = (FileName *) stoAlloc((unsigned) OB_Other,
                                    sizeof(FileName) * (n+1));
        for (i = 0; i < n; i++)
                buf[i] = nfn.withType(ftv[i]);
        buf[n] = 0;

        return buf;
}
