///////////////////////////////////////////////////////////////////////////////
//
// file_t.cpp: Test file system interaction.
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

#if !defined(TEST_FILE) && !defined(TEST_ALL)

void testFile(void) { }

#else

# include "axlgen.h"

void
testFile(void)
{
        FileName         testFile;
        FILE             *fout;

        printf("fileSetHandler:\n");
        fileSetHandler(fileSetHandler((FileErrorFun) 0));

        printf("pathInit:\n");
        pathInit();

        printf("fileRdFind: yabba -- ");
        testFile = fileRdFind(binSearchPath(), "yabba", "");
        if (testFile) 
                printf("dir \"%s\", name \"%s\", type \"%s\"\n",
                        testFile.dir(),
                        testFile.name(),
                        testFile.type());
        else
                printf("Not found\n");

        printf("fileRdFind: cat -- ");
        testFile = fileRdFind(binSearchPath(), "cat", osExecFileType);
        if (testFile) {
                /* The directory is host-layout dependent (/bin vs /usr/bin). */
                printf("found name \"%s\", type \"%s\"\n",
                        testFile.name(), testFile.type());
                
                printf("fileRdOpen: ");
                fout = fileRdOpen(testFile);
                printf("Opened \"%s\"\n",testFile.name());

                fclose(fout);
        }
        else
                printf("Not found\n");

        printf("DONE.\n");

        testFile.free();
/*      fileRemove(testFile); */

}

#endif
