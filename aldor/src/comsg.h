/*****************************************************************************
 *
 * comsg.h: Compiler message reporting.
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

#ifndef _COMSG_H_
#define _COMSG_H_

# include "axlobs.h"

enum comsgTag {
        COMSG_REMARK,
        COMSG_WARNING,
        COMSG_ERROR,
        COMSG_FATAL,
        COMSG_NOTE
};

typedef union {
        CoMsgList noteList;
        int       noteNumber;
} NoteInfo;

typedef enum comsgTag  CoMsgTag;

struct comsg {
        CoMsgTag        tag;            /* Kind of message */
        Length          serial;         /* Serial number with a compilation. */
        SrcPos          pos;            /* Position where it originated */
        AbSyn           node;           /* .. or node where it originated */
        Msg             msg;            /* The message template. */
        MutString          text;           /* The text of the message */
        NoteInfo        notes;          /* list of eventual notes */
};


/*
 * :: Functions to control behaviour.
 */
extern int      comsgSetOption    (MutString);            /* 0 => OK, -1 => err */
extern void     comsgSetInteractiveOption (void);
extern int      comsgSelectByName (MutString, bool isOn); /* 0 => OK, -1 => err */

extern bool     comsgOkAbbrev     (void); /* Is it OK to abbrev types?   */
extern bool     comsgOkMacText    (void); /* Pt to macro text (not use)? */
extern bool     comsgOkDetails    (void); /* Is it OK to give details?   */
extern bool     comsgOkBreakLoop  (void); /* Is it OK to talk to break loop? */

extern bool     comsgOkRemark     (Msg);  /* Is this remark OK?          */
extern bool     comsgOkWarning    (Msg);  /* Is this warning OK?         */
extern bool     comsgOkRelease    (void); /* Show "Release: Aldor(... " */

/*
 * :: Functions to access message database.
 */
extern void     comsgOpen         (void);
extern void     comsgClose        (void);
extern String comsgString     (Msg);                /* Shared. */

/*
 * :: Functions to collect a list of messages, then print them.
 */
extern void     comsgInit         (void);
extern void     comsgFini         (void);
extern int      comsgErrorCount   (void);
                        /*
                         * comsgInit initializes the list.
                         * comsgFini finalizes message structures
                         *     and prints the messages on stdout.
                         */

/*
 * Add messages.
 */
extern void     comsgFatal        (AbSyn,  Msg fmt, ...);
extern void     comsgError        (AbSyn,  Msg fmt, ...);
extern void     comsgWarning      (AbSyn,  Msg fmt, ...);
extern void     comsgRemark       (AbSyn,  Msg fmt, ...);
extern void     comsgWarnPos      (SrcPos, Msg fmt, ...);
/*
 * Varargs versions
 */
extern void     comsgVFatal       (AbSyn,  Msg fmt, va_list);
extern CoMsg    comsgVError       (AbSyn,  Msg fmt, va_list);
extern CoMsg    comsgVWarning     (AbSyn,  Msg fmt, va_list);
extern CoMsg    comsgVRemark      (AbSyn,  Msg fmt, va_list);
extern CoMsg    comsgVWarnPos     (SrcPos, Msg fmt, va_list);

/*
 * Add messages with notes attached.
 */
extern void     comsgNError       (AbSyn, Msg fmt, ...);
extern void     comsgNWarning     (AbSyn, Msg fmt, ...);
extern void     comsgNRemark      (AbSyn, Msg fmt, ...);

/* Add note to previous comsgNXxxx */
extern void     comsgNote         (AbSyn, Msg fmt, ...);

/*
 * :: Functions for program-controlled display in "comsg" format.
 */
extern void     comsgReportOne    (FILE *, CoMsg);

extern int      comsgFPrintf      (FILE *, Msg fmt, ...);
extern int      comsgVFPrintf     (FILE *, Msg fmt, va_list);

extern void     comsgPromptPrint  (FILE *, FILE *, String, ...);

/*
 * :: Strictly local but quite useful.
 */
extern  int     comsgPrintLine  (FILE *, SrcPos);

#endif /* !_COMSG_H_ */
