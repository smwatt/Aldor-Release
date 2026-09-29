/*****************************************************************************
 *
 * msg.h: Message catalog manipulation.
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

#ifndef _MSG_H_
#define _MSG_H_

# include "axlport.h"
# include "fname.h"

typedef int     Msg;
typedef int     MsgSet;

struct msgInfo {
        int     setno;
        int     msgno;
        String name;
        String text;
};

# define        MSG_NOT_FOUND   (-1)

extern void     msgDefaults     (struct msgInfo *);

extern void     msgOpen         (FileName);
extern void     msgClose        (void);

extern String msgGet        (MsgSet, Msg);
extern void     msgClear        (void);           /* Reclaim messages. */

extern int      msgFPrintf      (FILE *, MsgSet, Msg, ...);
extern int      msgVFPrintf     (FILE *, MsgSet, Msg, va_list);

/*
 * These do not deal with message text so drive themselves from msgDefaults.
 */
extern Msg      msgByName       (MsgSet, String); /* May be MSG_NOT_FOUND. */
extern Msg      msgByAName      (MsgSet, String); /* Case-insensitive. */
extern String msgName      (MsgSet, Msg);
extern int      msgNumber       (MsgSet, Msg);
extern int      msgMax          (MsgSet);

#endif /* !_MSG_H_ */
