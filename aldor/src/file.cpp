///////////////////////////////////////////////////////////////////////////////
//
// file.cpp: File system interactions.
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

bool    fileDebug       = false;
#define fileDEBUG(s)    DEBUG_IF(fileDebug, s)

local FILE *
fileDefaultHandler(FileName fn, IOMode mode)
{
        MutString  fstr = fn.unparseStatic();

        fprintf(stderr, "Cannot use file \"%s\" with mode %s.\n", fstr, mode);
        exitFailure();
        NotReached(return 0);
}

static FileErrorFun     fileError = fileDefaultHandler;

FileErrorFun
fileSetHandler(FileErrorFun f)
{
        FileErrorFun ofun = fileError;
        fileError = f ? f : (FileErrorFun) fileDefaultHandler;
        return ofun;
}


Hash
fileHash(FileName fn)
{
        MutString name = fn.unparseStatic();
        return osFileHash(name);
}

FILE *
fileTryOpen(FileName fn, IOMode mode)
{
        MutString name = fn.unparseStatic();
        bool   isdir = osDirIsThere(name);
        FILE  *file = 0;

        if (!isdir) file = fopen(name, mode);
        fileDEBUG(fprintf(dbOut, "Trying to open \"%s\" (mode \"%s\"):  %s\n",
                          name, mode, file == NULL ? "NO" : "Yes!"));
        return file;
}

FILE *
fileMustOpen(FileName fn, IOMode mode)
{
        FILE    *stream;

        stream = fileTryOpen(fn, mode);
        if (!stream) stream = (*fileError)(fn, mode);
        return stream;
}

bool
fileIsOpenable(FileName fn, IOMode mode)
{
        FILE *file = fileTryOpen(fn, mode);
        if (file) {
                fclose(file);
                return true;
        }
        return false;
}

bool
fileIsThere(FileName fn)
{
        MutString name = fn.unparseStatic();
        bool   isit = osFileIsThere(name);

        fileDEBUG(fprintf(dbOut, "Trying to find \"%s\" (is it there?): %s\n",
                          name, isit ? "Yes!" : "NO"));
        return isit;
}

Length
fileSize(FileName fn)
{
        MutString name = fn.unparseStatic();
        return osFileSize(name);
}

MutString
fileContentsString(FileName fn)
{
        FILE    *file;
        MutString  s, t;
        int     c;
        Length  l;

        file = fileMustOpen(fn, osIoRdMode);

        fseek(file, long0, SEEK_END);
        l = ftell(file);
        fseek(file, long0, SEEK_SET);

        s = strAlloc(l);
        for (t = s; (c = fgetc(file)) != EOF; t++) *t = c;
        *t = 0;
        fclose(file);

        return s;
}

void
fileFreeContentsString(MutString s)
{
        strFree(s);
}

void
fileRemove(FileName fn)
{
        MutString name = fn.unparseStatic();
        osFileRemove(name);
}

void
fileRename(FileName from, FileName to)
{
        MutString name1 = from.unparse();
        MutString name2 = to.unparseStatic();
        osFileRename(name1, name2);
        strFree(name1);
}

/*****************************************************************************
 *
 * Save integers in standard byte order.
 *
 ****************************************************************************/

UByte
fileRdUByte(FILE *file)
{
        UByte result;

        FILE_GET_BYTE(file, result);

        return result;
}

UShort
fileRdUShort(FILE *file)
{
        UShort result;

        FILE_GET_HINT(file, result);

        return result;
}

ULong
fileRdULong(FILE *file)
{
        ULong result;

        FILE_GET_SINT(file, result);

        return result;
}

int
fileWrUByte(FILE *file, UByte b)
{
        FILE_PUT_BYTE(file, b);

        return BYTE_BYTES;
}

int
fileWrUShort(FILE *file, UShort s)
{
        FILE_PUT_HINT(file, s);

        return HINT_BYTES;
}

int
fileWrULong(FILE *file, ULong l)
{
        FILE_PUT_BYTE(file, l);

        return SINT_BYTES;
}

/*****************************************************************************
 *
 * Save floating-point numbers in XFloat format.
 *
 ****************************************************************************/

SFloat
fileRdSFloat(FILE *file)
{
        XSFloat         xs;
        SFloat          s;

        FILE_GET_CHARS(file, (char *) &xs, XSFLOAT_BYTES);
        xsfToNative(&xs, &s);

        return s;
}

DFloat
fileRdDFloat(FILE *file)
{
        XDFloat         xd;
        DFloat          d;

        FILE_GET_CHARS(file, (char *) &xd, XDFLOAT_BYTES);
        xdfToNative(&xd, &d);

        return d;
}

int
fileWrSFloat(FILE *file, SFloat s)
{
        XSFloat xs;
        SFloat  bs = s; /* Avoid problems when float is passed as double. */

        xsfFrNative(&xs, &bs);
        FILE_PUT_CHARS(file, (char *) &xs, XSFLOAT_BYTES);

        return XSFLOAT_BYTES;
}

int
fileWrDFloat(FILE *file, DFloat d)
{
        XDFloat xd;

        xdfFrNative(&xd, &d);
        FILE_PUT_CHARS(file, (char *) &xd, XDFLOAT_BYTES);

        return XDFLOAT_BYTES;
}

/*****************************************************************************
 *
 * Save a given number of characters in ASCII format.
 *
 ****************************************************************************/

/* Read a specified number of characters from the file.
 * Return the result as a null-terminated string.
 */
MutString
fileRdChars(FILE *file, int cc)
{
        MutString  s;

        s = strAlloc(cc);
        FILE_GET_CHARS(file, s, cc);
        s = strnFrAscii(s,cc);

        return s;
}

int
fileWrChars(FILE *file, int cc, String s)
{
        FILE_PUT_CHARS(file, strnToAsciiStatic(s,cc), cc);

        return cc;
}

/* Read a null-terminated string of unknown length from the file.
 * Return no more than ssize chars in the preallocated MutString s.
 */
MutString
fileGetChars(FILE *file, MutString s, int ssize)
{
        int     c, k;

        /* Convert from ASCII to the native char. set. */
        c = fgetc(file);
        for( k = 0; (k < ssize) && (c != EOF) && (c != 0); k += 1 ) {
                s[k] = charFrAscii(c);
                c = fgetc(file);
        }

        /* Add a trailing null character. */
        s[ (k < ssize) ? k : (ssize - 1) ] = 0;

        return s;
}

/*****************************************************************************
 *
 * Save strings in ASCII format, length included.
 *
 ****************************************************************************/

MutString
fileRdString(FILE *file)
{
        int     cc;

        FILE_GET_SINT(file, cc);
        return fileRdChars(file, cc);
}

int
fileWrString(FILE *file, String s)
{
        int cc = strLength(s);

        FILE_PUT_SINT(file, cc);
        return SINT_BYTES + fileWrChars(file, cc, s);
}


/*****************************************************************************
 *
 * Save data in binary format.
 *
 ****************************************************************************/

Buffer
fileRdBuffer(FILE *file)
{
        MutString  s;
        int     cc;

        FILE_GET_SINT(file, cc);
        s = strAlloc(cc);
        FILE_GET_CHARS(file, s, cc);

        return Buffer::capture(s, cc);
}

int
fileWrBuffer(FILE *file, Buffer b)
{
        int cc = b.position();

        FILE_PUT_SINT(file, cc);
        FILE_PUT_CHARS(file, b.chars(), cc);

        return SINT_BYTES + cc;
}
