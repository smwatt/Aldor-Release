///////////////////////////////////////////////////////////////////////////////
//
// buffer_t.cpp: Test grow-on-demand-buffers.
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

#if !defined(TEST_BUFFER) && !defined(TEST_ALL)

void testBuffer(void) { }

#else

# include "axlgen.h"

void
testBuffer(void)
{
        Buffer  b;
        int     i, n;
        MutString  t;
        String s = "The rain in Iberia falls mainly in the cafeteria. ";

#if EDIT_1_0_n1_07
        printf("bufNew: ");
        b = Buffer::create();
        printf("[%d/%d]\n\n", (int) b.position(), (int) b.size());

        printf("BUF_START / BUF_ADD1:\n");
        BUF_START(b);
        for (i = 0, n = strlen(s); i < n; i++)  BUF_ADD1(b, s[i]);
        BUF_ADD1(b, char0);
        t = b.chars();
        printf("\"%s\" [%d]\n", t, n);
        printf("[%d/%d]\n\n", (int) b.position(), (int) b.size());

        printf("bufNeed(%d): ", (int) BUF_INIT_SIZE + 5);
        b.need(BUF_INIT_SIZE + 5);
        printf("[%d/%d]\n", (int) b.position(), (int) b.size());
        printf("bufChars: ");
        printf("\"%s\" [%d]\n\n", b.chars(), (int) strlen(b.chars()));

        printf("bufGrow(%d): ", 5);
        b = b.grow(5);
        printf("[%d/%d]\n", (int) b.position(), (int) b.size());
        printf("bufChars: ");
        printf("\"%s\" [%d]\n\n", b.chars(), (int) strlen(b.chars()));

        printf("bufFree: ");
        b.free();
        printf("OK\n\n");

        printf("bufNew: ");
        b = Buffer::create();
        printf("[%d/%d]\n\n", (int) b.position(), (int) b.size());

        printf("BUF_ADD1 across a boundary: \n");
        n = b.size();
        BUF_START(b);
        for (i = 0; i < n-5; i++)
                BUF_ADD1(b, "1234567890"[i%10]);
        for (i = 0; i < 10; i++) {
                int c = "abcdefghij"[i%10];
                BUF_ADD1(b, c);
                printf(" %c: [%d/%d]", c, (int) b.position(), (int) b.size());
        }
        BUF_ADD1(b, char0);
        t = b.chars();
        printf("\n");

        printf("bufChars: ");
        printf("\"%s\" [%d]\n\n", t, (int) strlen(t));
#else
        printf("bufNew: ");
        b = Buffer::create();
        printf("[%d/%d]\n\n", b.position(), b.size());

        printf("BUF_START / BUF_ADD1:\n");
        BUF_START(b);
        for (i = 0, n = strlen(s); i < n; i++)  BUF_ADD1(b, s[i]);
        BUF_ADD1(b, char0);
        t = b.chars();
        printf("\"%s\" [%d]\n", t, n);
        printf("[%d/%d]\n\n", b.position(), b.size());

        printf("bufNeed(%d): ", BUF_INIT_SIZE + 5);
        b.need(BUF_INIT_SIZE + 5);
        printf("[%d/%d]\n", b.position(), b.size());
        printf("bufChars: ");
        printf("\"%s\" [%d]\n\n", b.chars(), strlen(b.chars()));

        printf("bufGrow(%d): ", 5);
        b = b.grow(5);
        printf("[%d/%d]\n", b.position(), b.size());
        printf("bufChars: ");
        printf("\"%s\" [%d]\n\n", b.chars(), strlen(b.chars()));

        printf("bufFree: ");
        b.free();
        printf("OK\n\n");

        printf("bufNew: ");
        b = Buffer::create();
        printf("[%d/%d]\n\n", b.position(), b.size());

        printf("BUF_ADD1 across a boundary: \n");
        n = b.size();
        BUF_START(b);
        for (i = 0; i < n-5; i++)
                BUF_ADD1(b, "1234567890"[i%10]);
        for (i = 0; i < 10; i++) {
                int c = "abcdefghij"[i%10];
                BUF_ADD1(b, c);
                printf(" %c: [%d/%d]", c, b.position(), b.size());
        }
        BUF_ADD1(b, char0);
        t = b.chars();
        printf("\n");

        printf("bufChars: ");
        printf("\"%s\" [%d]\n\n", t, strlen(t));
#endif


        printf("bufPrint: ");
        b.print(osStdout);
        printf("\n");

        printf("bufLiberate: ");
        t = b.liberate();
        printf("\"%s\"\n\n", t);

        printf("strFree: ");
        strFree(t);
        printf("OK\n\n");

        printf("bufPrintf:\n");
        b = Buffer::create();
        n  = b.printf("This %s a %s command.  ", "is", "format");
        n += b.printf("! marks the %d-th character.\n", n+1);
        printf("%d: %s", n, b.chars());
        b.free();

}

#endif
