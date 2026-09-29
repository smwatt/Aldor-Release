///////////////////////////////////////////////////////////////////////////////
//
// msg_t.cpp: Test message handling.
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

#if !defined(TEST_MSG) && !defined(TEST_ALL)

void testMsg(void) { }

#else

# include "axlgen.h"

struct msgInfo testmsgdb_msgs[] = {
        {1, 1,  "RedMsg",   "This is the first  default message of set 1. A."},
        {1, 2,  "BlueMsg",  "This is the second default message of set 1. B."},
        {2, 1,  "BlackMsg", "This is the first  default message of set 2. C."},
        {2, 2,  "BlueMsg",  "This is the second default message of set 2. D."},
        {-1, 0, 0, 0}
};

void
testMsg(void)
{
        FileName msgfile = 0;

        printf("msgDefaults:\n");
        msgDefaults(testmsgdb_msgs);
        printf("%s\n%s\n%s\n%s\n",
               msgGet(1,2), msgGet(2,1),
               msgGet(2, msgByName(2,"Blue")),
               msgGet(2, msgByAName(2, "bLue")));
        printf("This message is not there: %s\n", msgGet(3,3));

        printf("msgOpen:\n");
        msgfile = fileRdFind(libSearchPath(), "comsgpig", "cat");
        if (!msgfile) return;

        msgOpen(msgfile);
        printf("The seventh message of the first set is: %s\n", msgGet(1,7));
        printf("msgClear:\n");

        msgClear();
        printf("The seventh message of the first set is: %s\n", msgGet(1,7));

        printf("msgClose:\n");
        msgClose();
        msgfile.free();

}

#endif
