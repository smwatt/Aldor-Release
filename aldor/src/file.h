/*****************************************************************************
 *
 * file.h: File system interactions.
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

#ifndef _FILE_H_
#define _FILE_H_

# include "axlport.h"
# include "fname.h"

#ifndef SEEK_SET
#  define SEEK_SET 0
#  define SEEK_CUR 1
#  define SEEK_END 2
#endif

typedef FILE *          (*FileErrorFun)         (FileName, IOMode);
extern  FileErrorFun    fileSetHandler          (FileErrorFun);

extern Hash             fileHash                (FileName);
extern FILE *           fileTryOpen             (FileName, IOMode);
extern FILE *           fileMustOpen            (FileName, IOMode);
extern bool             fileIsOpenable          (FileName, IOMode);
extern bool             fileIsThere             (FileName);
extern Length           fileSize                (FileName);
extern MutString           fileContentsString      (FileName);
extern void             fileFreeContentsString  (MutString);
extern void             fileRemove              (FileName);
extern void             fileRename              (FileName, FileName);

# define                fileRdOpen(fn)          fileMustOpen(fn,osIoRdMode)
# define                fileWrOpen(fn)          fileMustOpen(fn,osIoWrMode)
# define                fileRbOpen(fn)          fileMustOpen(fn,osIoRbMode)
# define                fileWubOpen(fn)         fileMustOpen(fn,osIoWubMode)

# define                fileIsReadable(fn)      fileIsOpenable(fn,osIoRdMode)
# define                fileIsWritable(fn)      fileIsOpenable(fn,osIoWrMode)

/*
 * Macros for putting and getting characters to a file.
 */

#define FILE_PUT_CHARS(fout, s, cc)                     \
        fwrite(s, BYTE_BYTES, cc, fout)

#define FILE_GET_CHARS(fin, s, cc)                      \
        IgnoreResult(fread(s, BYTE_BYTES, cc, fin))

/*
 * Macros for putting and getting integers as byte sequences.
 */

#define FILE_PUT_BYTE(fout, i) {                        \
        UByte _s[BYTE_BYTES];                           \
        UByte _i = (i);                                 \
        _s[0] = UBYTE0(_i);                             \
        FILE_PUT_CHARS(fout, _s, sizeof(_s));           \
}

#define FILE_PUT_HINT(fout, i) {                        \
        UByte _s[HINT_BYTES];                           \
        ULong _i = (i);                                 \
        _s[0] = HBYTE0(_i);                             \
        _s[1] = HBYTE1(_i);                             \
        FILE_PUT_CHARS(fout, _s, sizeof(_s));           \
}

#define FILE_PUT_SINT(fout, i) {                        \
        UByte _s[SINT_BYTES];                           \
        ULong _i = (i);                                 \
        _s[0] = BYTE0(_i);                              \
        _s[1] = BYTE1(_i);                              \
        _s[2] = BYTE2(_i);                              \
        _s[3] = BYTE3(_i);                              \
        FILE_PUT_CHARS(fout, _s, sizeof(_s));           \
}

#define FILE_GET_BYTE(fin, i) {                         \
        UByte _s[BYTE_BYTES];                           \
        FILE_GET_CHARS(fin, _s, sizeof(_s));            \
        (i) = UNBYTE1(_s[0]);                           \
}

#define FILE_GET_HINT(fin, i) {                         \
        UByte _s[HINT_BYTES];                           \
        FILE_GET_CHARS(fin, _s, sizeof(_s));            \
        (i) = UNBYTE2(_s[0],_s[1]);                     \
}

#define FILE_GET_SINT(fin, i) {                         \
        UByte _s[SINT_BYTES];                           \
        FILE_GET_CHARS(fin, _s, sizeof(_s));            \
        (i) = UNBYTE4(_s[0],_s[1],_s[2],_s[3]);         \
}

/* Save integers in standard byte order. */
extern UByte    fileRdUByte     (FILE *file);
extern UShort   fileRdUShort    (FILE *file);
extern ULong    fileRdULong     (FILE *file);

extern int      fileWrUByte     (FILE *file, UByte b);
extern int      fileWrUShort    (FILE *file, UShort s);
extern int      fileWrULong     (FILE *file, ULong l);

/* Save floating-point numbers in IEEE format. */
extern SFloat   fileRdSFloat    (FILE *file);
extern DFloat   fileRdDFloat    (FILE *file);

extern int      fileWrSFloat    (FILE *file, SFloat s);
extern int      fileWrDFloat    (FILE *file, DFloat d);

/* Save a given number of characters in ASCII format. */
extern MutString   fileRdChars     (FILE *file, int cc);
extern int      fileWrChars     (FILE *file, int cc, String s);

/* Read an unknown number of ASCII characters into a preallocated MutString. */
extern MutString   fileGetChars    (FILE *file, MutString s, int ssize);

/* Save strings in ASCII format, length included. */
extern MutString   fileRdString    (FILE *file);
extern int      fileWrString    (FILE *file, String s);

/* Save data in binary format. */
extern Buffer   fileRdBuffer    (FILE *file);
extern int      fileWrBuffer    (FILE *file, Buffer buf);

#if 0
   typedef ULong        FilePos;

   local FilePos fileRdFilePos(FILE *);
   local int     fileWrFilePos(FILE *, FilePos);
#endif

#endif /* !_FILE_H_ */
