///////////////////////////////////////////////////////////////////////////////
//
// comsg.cpp: Compiler message reporting.
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
#if EDIT_1_0_n1_04
# include "textcolour.h"
#endif

/****************************************************************************
 *
 * :: Declarations
 *
 ****************************************************************************/

# define  comsgStream           osStdout
# define  LINE_LENGTH           80
# define  EMAX_DEFAULT          10
# define  EMAX_NONE             30000

# define  SelectDontCare        0
# define  SelectOn              1
# define  SelectOff             2

/* Defaults: */
static String      flineFmt        = "\"%s\", line %d: ";
static String      lcharFmt        = "[L%d C%d] ";
static String      remarkTag       = "[Remark]  ";
static String      warningTag      = "[Warning] ";
static String      errorTag        = "[Error]   ";
static String      fatalTag        = "[Fatal]   ";
static String      noteTag         = "[Note]    ";
static String      preview         = "[Message Preview] ";
static String      afterMacEx      = "[After Macro Expansion] ";
static String      expandedExpr    = "Expanded expression was: ";

/* Msg control: */
static int              comsgErrorMax   = EMAX_DEFAULT;

/*These are controlled by -M<n> */
static bool             comsgDoWarnings = true;
static bool             comsgDoNumber   = true;
static bool             comsgDoSource   = true;
static bool             comsgDoDetails  = true;
static bool             comsgDoNotes    = true;
static bool             comsgDoRemarks  = false;
/* These aren't */
static bool             comsgDoSort     = true;
static bool             comsgDoMacText  = true;
static bool             comsgDoAbbrev   = true;
static bool             comsgDoHuman    = true;
static bool             comsgDoInspect  = false;
static bool             comsgDoPreview  = false;
static bool             comsgDoName     = false;
static bool             comsgDoRelease  = true;

/* Database: */
static bool             comsgIsInit     = 0;
static bool             comsgIsOpen     = 0;
static FileName         comsgFile       = 0;

/* The messages: */
static List<CoMsg>        messages        = 0;
static int              nmessages       = 0;

static int              nErrors         = 0;
static int              nWarnings       = 0;
static int              nRemarks        = 0;
static int              nNotes          = 0;

static int              comsgPromptSize = 0;

/*
 * Keep track of the last error position for appending messages at the end.
 * comsgInit allocates abMaxPos to avoid allocating in comsgFini.
 */
static AbSyn            abMaxPos        = NULL;

static CoMsg            lastCoMsgSeeNote = NULL;
static FileName         fnameRef         = NULL;

/* Forward declarations: */
local  MutString           comsgOpt        (MutString prefix, MutString whole);
extern int              comsgSetOption  (MutString opt);
local  void             comsgSetStripDir(MutString base);

local  void             comsgInitSelect (void);
local  int              comsgSelectState(Msg msg);

extern bool             comsgOkRemark   (Msg);
extern bool             comsgOkWarning  (Msg);

extern bool             comsgOkAbbrev   (void);
extern bool             comsgOkDetails  (void);

extern void             comsgOpen       (void);
extern void             comsgClose      (void);
extern String      comsgString     (Msg);
extern String      comsgName       (Msg);

extern void             comsgInit       (void);
extern void             comsgFini       (void);
extern int              comsgErrorCount (void);

local  MutString           comsgText       (Msg, va_list);
local  CoMsg            comsgNew        (CoMsgTag tag, Length serial,
                                         AbSyn, Msg, MutString text);
local  void             comsgFree       (CoMsg msg);
local  int              comsgCmpPtr     (CoMsg *pmsg1, CoMsg *pmsg2);

extern void             comsgRemark     (AbSyn, Msg fmt, ...);
extern void             comsgWarning    (AbSyn, Msg fmt, ...);
extern void             comsgError      (AbSyn, Msg fmt, ...);
extern void             comsgFatal      (AbSyn, Msg fmt, ...);

extern void             comsgNRemark    (AbSyn, Msg fmt, ...);
extern void             comsgNWarning   (AbSyn, Msg fmt, ...);
extern void             comsgNError     (AbSyn, Msg fmt, ...);
extern void             comsgNote       (AbSyn, Msg fmt, ...);

extern CoMsg            comsgVRemark    (AbSyn, Msg fmt, va_list);
extern CoMsg            comsgVWarning   (AbSyn, Msg fmt, va_list);
extern CoMsg            comsgVError     (AbSyn, Msg fmt, va_list);
extern void             comsgVFatal     (AbSyn, Msg fmt, va_list);

local  CoMsg            comsgVDo        (CoMsgTag tag, AbSyn, Msg, va_list);

extern void             comsgReportOne  (FILE *, CoMsg comsg);
local  void             comsgReportFile (FILE *, int comsgc, CoMsg *comsgv);
local  void             comsgReportLine (FILE *, int comsgc, CoMsg *comsgv);

local  void             comsgPrintDots  (FILE *, int comsgc, CoMsg *);
local  void             comsgPrintLead  (FILE *, Length serial, Msg msg,
                                         CoMsgTag tag, SrcPos, int noteNumber);
local  void             comsgPrintMacEx (FILE *, SrcPos, AbSyn);
local  void             comsgPrintSeeNotes (FILE *fout, List<CoMsg> notes);
extern int              comsgFPrintf    (FILE *, Msg msg, ...);
extern int              comsgVFPrintf   (FILE *, Msg msg, va_list);

local void              comsgSetPromptSize      (int);

/*****************************************************************************
 *
 * :: Option Control
 *
 ****************************************************************************/

local MutString
comsgOpt(String prefix, MutString whole)
{
        if (strAIsPrefix(prefix, whole))
                return whole + strlen(prefix);
        else
                return 0;
}

int
comsgSetOption(MutString opt)
{
        bool    isOn;
        MutString  arg;

        if (!opt) return -1;

        /*
         * Handle specific options.
         */
        if ((arg = comsgOpt("emax=", opt)) != 0) {
                if (!isdigit(arg[0])) return -1;
                comsgErrorMax = atoi(arg);
                return 0;
        }
        if ((arg = comsgOpt("no-emax", opt)) != 0) {
                if (arg[0] != 0) return -1;
                comsgErrorMax = EMAX_NONE;
                return 0;
        }
        if ((arg = comsgOpt("0", opt)) != 0) {
                if (arg[0] != 0) return -1;
                comsgDoWarnings = false;
                comsgDoNumber   = false;
                comsgDoSource   = false;
                comsgDoDetails  = false;
                comsgDoNotes    = false;
                comsgDoRemarks  = false;
                return 0;
        }
        if ((arg = comsgOpt("1", opt)) != 0) {
                if (arg[0] != 0) return -1;
                comsgDoWarnings = true;
                comsgDoNumber   = true;
                comsgDoSource   = false;
                comsgDoDetails  = false;
                comsgDoNotes    = false;
                comsgDoRemarks  = false;
                return 0;
        }
        if ((arg = comsgOpt("2", opt)) != 0) {
                if (arg[0] != 0) return -1;
                comsgDoWarnings = true;
                comsgDoNumber   = true;
                comsgDoSource   = true;
                comsgDoDetails  = true;
                comsgDoNotes    = true;
                comsgDoRemarks  = false;
                return 0;
        }
        if ((arg = comsgOpt("3", opt)) != 0) {
                if (arg[0] != 0) return -1;
                comsgDoWarnings = true;
                comsgDoNumber   = true;
                comsgDoSource   = true;
                comsgDoDetails  = true;
                comsgDoNotes    = true;
                comsgDoRemarks  = true;
                return 0;
        }
        if ((arg = comsgOpt("base=", opt)) != 0) {
                if (arg[0] == 0) return -1;
                comsgSetStripDir((MutString)arg);
                return 0;
        }
        if ((arg = comsgOpt("no-base", opt)) != 0) {
                if (arg[0] == 0) return -1;
                comsgSetStripDir((MutString) NULL);
                return 0;
        }
        if ((arg = comsgOpt("db=", opt)) != 0) {
                if (arg[0] == 0) return -1;
                comsgClose();
                comsgFile = fileRdFind(libSearchPath(), arg, FTYPE_MSG);
                if (!comsgFile) return -1;
                comsgOpen();
                return 0;
        }
        if ((arg = comsgOpt("no-db", opt)) != 0) {
                if (arg[0] != 0) return -1;
                comsgClose();
                comsgFile = 0;
                comsgOpen();
                return 0;
        }

        /*
         * Things which are either on or off.
         */
        isOn = true;
        while ((arg = comsgOpt("no-", opt)) != 0) {
                isOn = !isOn;
                opt  = arg;
        }
        if (opt[0] == 0) return -1;

        if (strAEqual(opt, "inspect")) {
                comsgDoInspect = isOn;
                return 0;
        }
        if (strAEqual(opt, "sort")) {
                comsgDoSort = isOn;
                return 0;
        }
        if (strAEqual(opt, "source")) {
                comsgDoSource = isOn;
                return 0;
        }
        if (strAEqual(opt, "name")) {
                comsgDoName = isOn;
                return 0;
        }
        if (strAEqual(opt, "abbrev")) {
                comsgDoAbbrev = isOn;
                return 0;
        }
        if (strAEqual(opt, "mactext")) {
                comsgDoMacText = isOn;
                return 0;
        }
        if (strAEqual(opt, "details")) {
                comsgDoDetails = isOn;
                return 0;
        }
        if (strAEqual(opt, "preview")) {
                comsgDoPreview = isOn;
                return 0;
        }
        if (strAEqual(opt, "human")) {
                comsgDoHuman = isOn;
                return 0;
        }
        if (strAEqual(opt, "remarks")) {
                comsgDoRemarks  = isOn;
                return 0;
        }
        if (strAEqual(opt, "warnings")) {
                comsgDoWarnings = isOn;
                return 0;
        }
        if (strAEqual(opt, "number")) {
                comsgDoNumber = isOn;
                return 0;
        }
        if (strAEqual(opt, "release")) {
                comsgDoRelease = isOn;
                return 0;
        }

        if (strAEqual(opt, "notes")) {
                comsgDoNotes = isOn;
                return 0;
        }

        /* Specific named messages.  */

        return comsgSelectByName(opt, isOn);
}

void
comsgSetInteractiveOption()
{
        comsgDoDetails  = true;
        comsgDoWarnings = true;
        comsgDoNotes    = false;
        comsgErrorMax   = EMAX_NONE;
}

bool
comsgOkAbbrev(void)
{
        return comsgDoAbbrev;
}

bool
comsgOkDetails(void)
{
        return comsgDoDetails;
}

bool
comsgOkMacText(void)
{
        return comsgDoMacText;
}

bool
comsgOkBreakLoop(void)
{
        return !comsgDoHuman && comsgDoInspect;
}

/* Set by -M strip=<dir> */
static MutString comsgStripDirName = (MutString) NULL;

local void
comsgSetStripDir(MutString str)
{
        /* Release any storage associated with a previous value */
        if (comsgStripDirName != (MutString) NULL) strFree(comsgStripDirName);

        /* Duplicate the argument if not NULL */
        comsgStripDirName = (str != (MutString) NULL) ? strCopy(str) : str;
}

local MutString
comsgStripDir(void)
{
        return comsgStripDirName;
}

bool
comsgOkRelease(void)
{
        return comsgDoRelease;
}

/*****************************************************************************
 *
 * :: Message Filter
 *
 ****************************************************************************/

static UByte    *comsgFilterV =  0;
static int      comsgFilterC  = -1;

local void
comsgInitSelect(void)
{
        int     i;

        if (comsgFilterV) return;

        msgDefaults(comsgdb_msgs);

        comsgFilterC = 1 + msgMax(ALDOR_GROUP);
        comsgFilterV = (UByte *) stoAlloc(OB_Other,comsgFilterC*sizeof(UByte));

        for (i = 0; i < comsgFilterC; i++)
                comsgFilterV[i] = SelectDontCare;
}

int
comsgSelectByName(MutString name, bool isOn)
{
        Msg     msg;
        int     mno;
#if EDIT_1_0_n2_03
        MutString  s = (MutString) NULL;

        /* Allow AXL_ as well as ALDOR_ */
        if (strAIsPrefix("AXL_", name)) {
                s = strConcat("ALDOR_", name + 4);
                comsgWarning(NULL, ALDOR_W_OldMsgPrefix);
                name = s;
        }
#endif

        comsgInitSelect();

        msg = msgByAName(ALDOR_GROUP, name);
#if EDIT_1_0_n2_03
        if (s) strFree(s);
#endif
        if (msg == MSG_NOT_FOUND) return -1;

        mno = msgNumber(ALDOR_GROUP, msg);
        comsgFilterV[mno] = isOn ? SelectOn : SelectOff;

        return 0;
}

local int
comsgSelectState(Msg msg)
{
        int     n;

        comsgInitSelect();

        n = msgNumber(ALDOR_GROUP, msg);
        if (n == MSG_NOT_FOUND || n >= comsgFilterC) return SelectDontCare;

        return comsgFilterV[n];
}

bool
comsgOkRemark(Msg msg)
{
        int msgstate = comsgSelectState(msg);
        if (comsgDoRemarks  && msgstate != SelectOff) return true;
        if (!comsgDoRemarks && msgstate == SelectOn)  return true;
        return false;
}

bool
comsgOkWarning(Msg msg)
{
        int msgstate = comsgSelectState(msg);
        if (comsgDoWarnings  && msgstate != SelectOff) return true;
        if (!comsgDoWarnings && msgstate == SelectOn)  return true;
        return false;
}

/*****************************************************************************
 *
 * :: Initialize-Finalize
 *
 ****************************************************************************/

/*
 * comsgOpen();
 * comsgInit(); comsgError...; comsgError...; comsgFini();
 * ...
 * comsgClose();
 */

void
comsgOpen(void)
{
        if (!comsgIsOpen) {
                comsgIsOpen = 1;
                msgDefaults(comsgdb_msgs);
                if (comsgFile) msgOpen(comsgFile);
        }

        flineFmt        = comsgString(ALDOR_P_MsgSposFileLine);
        lcharFmt        = comsgString(ALDOR_P_MsgSposLineChar);
        remarkTag       = comsgString(ALDOR_P_MsgTagRemark);
        warningTag      = comsgString(ALDOR_P_MsgTagWarning);
        errorTag        = comsgString(ALDOR_P_MsgTagError);
        fatalTag        = comsgString(ALDOR_P_MsgTagFatal);
        noteTag         = comsgString(ALDOR_P_MsgTagNote);
        preview         = comsgString(ALDOR_P_MsgPreview);
        afterMacEx      = comsgString(ALDOR_P_MsgAfterMacEx);
        expandedExpr    = comsgString(ALDOR_P_MsgExpandedExpr);

}

void
comsgClose(void)
{
        if (comsgIsOpen) {
                msgClose();
                comsgIsOpen = 0;
                comsgFile   = 0;
        }
}

/* Returns a pointer to the shared msg. */
String
comsgString(Msg msg)
{
        return msgGet(ALDOR_GROUP, msg);
}


String
comsgName(Msg msg)
{
        return msgName(ALDOR_GROUP, msg);
}

void
comsgInit(void)
{
        if (!comsgIsInit) {
                comsgOpen();

                messages        = 0;
                nmessages       = 0;
                nErrors         = 0;
                nWarnings       = 0;
                nRemarks        = 0;
                comsgIsInit     = 1;

                abMaxPos        = abNewNothing(SrcPos::none);
        }
}

void
comsgFini(void)
{
        static bool     inFini = false;

        if (comsgIsInit && inFini == false) {
                List<CoMsg> msgl;
                int       msgc;
                CoMsg     *msgv;

                inFini = true;

                abSetPos(abMaxPos, SrcPos::end());

                /* Add message summary, if advised. */
                if (nErrors + nWarnings + nRemarks > 0) {
                        Msg whatsUp = (nErrors==0)? ALDOR_R_MsgCongratulations
                                                  : ALDOR_R_MsgCondolences;
                        comsgRemark(abMaxPos, ALDOR_R_MsgCountMessages,
                                            nErrors, nWarnings, nRemarks + 2);
                        comsgRemark(abMaxPos, whatsUp);
                }

                /* Stash the list. */
                msgl        = messages;
                messages    = 0;
                comsgIsInit = 0;

                /* Convert message list to vector. */
                msgc = listLength<CoMsg>(msgl);
                msgv = (CoMsg *) stoAlloc(OB_Other, msgc*sizeof(CoMsg));
                listFillVector<CoMsg>(msgv, msgl);

                /* Do the job and clean up. */
                comsgReportFile(comsgStream, msgc, msgv);
                if (comsgDoInspect) breakLoop(comsgDoHuman, msgc, msgv);
                stoFree((void *) msgv);
                listFreeDeeply<CoMsg>(msgl, comsgFree);

                inFini = false;
        }
}


int
comsgErrorCount(void)
{
        return nErrors;
}


/*****************************************************************************
 *
 * :: CoMsg Objects
 *
 ****************************************************************************/

local MutString
comsgText(Msg msg, va_list argp)
{
        String fmt = comsgString(msg);
        MutString s   = strVPrintf(fmt, argp);
        return s;
}

local CoMsg
comsgNew(CoMsgTag tag, Length serial, AbSyn ab, Msg msg, MutString text)
{
        CoMsg comsg;

        comsg = (CoMsg) stoAlloc((int) OB_CoMsg, sizeof(*comsg));
        comsg->tag    = tag;
        comsg->serial = serial;
        comsg->pos    = ab ? abPos(ab) : SrcPos::none;
        comsg->node   = abCopy(ab);
        comsg->msg    = msg;
        comsg->text   = text;
        comsg->notes.noteList = NULL;
        return comsg;
}

local void
comsgFree(CoMsg msg)
{
        abFree(msg->node);
        if (msg->notes.noteList && msg->tag != COMSG_NOTE)
                listFree<CoMsg>(msg->notes.noteList);
        stoFree((void *) msg->text);
        stoFree((void *) msg);
}

local int
comsgCmpPtr(CoMsg *pmsg1, CoMsg *pmsg2)
{
        return ((*pmsg1)->pos).compare((*pmsg2)->pos);
}


/*****************************************************************************
 *
 * :: Functions to collect messages.  Remark/Warning/Error/Fatal.
 *
 *****************************************************************************/

void
comsgRemark(AbSyn ab, Msg msg, ...)
{
        va_list argp;
        va_start(argp, msg);
        comsgVRemark(ab, msg, argp);
        va_end(argp);
}

void
comsgWarning(AbSyn ab, Msg msg, ...)
{
        va_list argp;
        va_start(argp, msg);
        comsgVWarning(ab, msg, argp);
        va_end(argp);
}

void
comsgWarnPos(SrcPos pos, Msg msg, ...)
{
        va_list argp;
        va_start(argp, msg);
        comsgVWarnPos(pos, msg, argp);
        va_end(argp);
}

void
comsgError(AbSyn ab, Msg msg, ...)
{
        va_list argp;

        va_start(argp, msg);
        comsgVError(ab, msg, argp);
        va_end(argp);
}

void
comsgFatal(AbSyn ab, Msg msg, ...)
{
        va_list argp;
        va_start(argp, msg);
        comsgVFatal(ab, msg, argp);
        va_end(argp);
}

/*
 * The first arg must be comsgVError, comsgVWarning, ...
 * Example:
 *      comsgNotePoint(comsgVError, ab, msg, str);
 *      comsgNote(ab, msg, str);
 *      comsgNote(ab, msg, str);
 */
local void
comsgNotePoint(CoMsg (* comsgf)(AbSyn, Msg, va_list), AbSyn ab,
               Msg msg, va_list argp)
{
        lastCoMsgSeeNote = comsgf(ab, msg, argp);
        fnameRef = (lastCoMsgSeeNote->pos).file();
        lastCoMsgSeeNote->notes.noteList = listNil<CoMsg>;
}

void
comsgNRemark(AbSyn ab, Msg msg, ...)
{
        va_list argp;
        va_start(argp, msg);
        comsgNotePoint(comsgVRemark, ab, msg, argp);
        va_end(argp);
}

void
comsgNWarning(AbSyn ab, Msg msg, ...)
{
        va_list argp;
        va_start(argp, msg);

        comsgNotePoint(comsgVWarning, ab, msg, argp);

        va_end(argp);
}

void
comsgNError(AbSyn ab, Msg msg, ...)
{
        va_list argp;
        va_start(argp, msg);

        comsgNotePoint(comsgVError, ab, msg, argp);

        va_end(argp);
}

void
comsgNote(AbSyn ab, Msg msg, ...)
{
        CoMsg comsg;
        va_list argp;
        int lnRef, cnRef;
        SrcPos spos;
        MutString s;
        String fmt;
        Buffer obuf;


        if (!comsgDoNotes)
                return;

        assert(lastCoMsgSeeNote);

        va_start(argp, msg);
        obuf = Buffer::create();
        comsg = comsgVDo(COMSG_NOTE, ab, msg, argp);
        assert(comsg);
        comsg->notes.noteNumber = ++nNotes;
        lastCoMsgSeeNote->notes.noteList =
                listCons<CoMsg>(comsg,lastCoMsgSeeNote->notes.noteList);
        spos  = comsg->pos;

        if ((spos).isSpecial())
                bug("Bad case in comsgNote");

        lnRef = (lastCoMsgSeeNote->pos).line();
        cnRef = (lastCoMsgSeeNote->pos).character();

        s = comsg->text;
        obuf.printf("%s", s);

        if ((comsg->pos).file() == fnameRef) {
                fmt = comsgString(ALDOR_P_MsgCfNote);
                obuf.printf(" ");
                obuf.printf(fmt, lnRef, cnRef);
        }
        else {
                fmt = comsgString(ALDOR_P_MsgCfFarNote);
                obuf.printf("\n");
                obuf.printf(fmt, fnameRef.unparseStatic(), lnRef, cnRef);
        }

        comsg->text = obuf.liberate();
        strFree(s);
        va_end(argp);
}

CoMsg
comsgVRemark(AbSyn ab, Msg msg, va_list argp)
{
        CoMsg comsg = NULL;

        if (comsgOkRemark(msg)) {
                nRemarks++;
                comsg = comsgVDo(COMSG_REMARK, ab, msg, argp);
        }
        return comsg;
}

CoMsg
comsgVWarning(AbSyn ab, Msg msg, va_list argp)
{
        CoMsg comsg = NULL;

        if (comsgOkWarning(msg)) {
                nWarnings++;
                comsg = comsgVDo(COMSG_WARNING, ab, msg, argp);
        }
        return comsg;
}

CoMsg
comsgVWarnPos(SrcPos pos, Msg msg, va_list argp)
{
        CoMsg comsg = NULL;
        AbSyn ab;
        ab = abNewNothing(pos);
        if (comsgOkWarning(msg)) {
                nWarnings++;
                comsg = comsgVDo(COMSG_WARNING, ab, msg, argp);
        }
        return comsg;
}

CoMsg
comsgVError(AbSyn ab, Msg msg, va_list argp)
{
        CoMsg comsg;

        nErrors++;
        comsg = comsgVDo(COMSG_ERROR, ab, msg, argp);

        if (nErrors == comsgErrorMax)
                comsgFatal(abMaxPos, ALDOR_F_MsgTooManyErrors);

        return comsg;
}

void
comsgVFatal(AbSyn ab, Msg msg, va_list argp)
{
        nErrors++;
        comsgVDo(COMSG_FATAL, ab, msg, argp);
        comsgFini();
        exitFailure();
}

local CoMsg
comsgVDo(CoMsgTag tag, AbSyn ab, Msg msg, va_list argp)
{
        CoMsg comsg = NULL;

        if (!comsgIsInit || !ab) {
                SrcPos spos = ab ? abPos(ab) : SrcPos::none;
                comsgInit();
#if EDIT_1_0_n1_04
                fprintf(comsgStream, "%s", tcolPrefix(tag));
                comsgPrintLead(comsgStream, ++nmessages, msg, tag, spos, int0);
                comsgVFPrintf (comsgStream, msg, argp);
                fprintf(comsgStream, "%s", tcolPostfix(tag));
#else
                comsgPrintLead(comsgStream, ++nmessages, msg, tag, spos, int0);
                comsgVFPrintf (comsgStream, msg, argp);
#endif
        }
        else {
                MutString text  = comsgText(msg, argp);
                bool   debug = false;
                DO_DEBUG(debug = true);

                if (ab) abSetPos(abMaxPos,
                                 (abPos(abMaxPos)).max(abPos(ab)));

                comsg = comsgNew(tag, ++nmessages, ab, msg, text);
                messages = listCons<CoMsg>(comsg, messages);

                if (comsgDoPreview || debug) {
                        fprintf(comsgStream, "%s\n", preview);
                        comsgReportOne(comsgStream, comsg);
                        if (comsgDoInspect)
                                breakLoop(comsgDoHuman, 1, &comsg);
                }
        }

        return comsg;
}

/*****************************************************************************
 *
 * :: Sorting and display of lines and their messages.
 *
 ****************************************************************************/

/*
 * Print each line and its messages as:
 *
 * "foo.sp", line 10: f(x) == y +-+ 1
 *                    ........^..^
 * [L10 C 9] (Warning) Variable y is undeclared.
 * [L10 C12] Improper syntax.
 */

void
comsgReportOne(FILE *fout, CoMsg comsg)
{
        comsgReportFile(fout, 1, &comsg);
}

local void
comsgReportFile(FILE *fout, int comsgc, CoMsg *comsgv)
{
        int     i0, n, glno;

        if (comsgc <= 0) return;

        comsgInit();

        /* Reverse the messages and sort if requested. */
        for (i0 = 0, n = comsgc - 1; i0 < comsgc/2; i0++, n--) {
                CoMsg m    = comsgv[n];
                comsgv[n]  = comsgv[i0];
                comsgv[i0] = m;
        }

        /* Messages at the same position are reported in the order
         * in which they were generated.
         */
        if (comsgDoSort)
                lisort(comsgv, comsgc, sizeof(CoMsg),
                       (int (*)(ConstPointer, ConstPointer)) comsgCmpPtr);

        for (i0 = 0; i0 < comsgc; i0 += n) {
                glno = (comsgv[i0]->pos).globalLine();
                for (n = 1; i0 + n < comsgc; n++)
                        if ((comsgv[i0+n]->pos).globalLine() != glno) break;
                comsgReportLine(fout, n, comsgv+i0);
        }


        /* Flush the output stream just incase */
        (void)fflush(fout);
}

local void
comsgReportLine(FILE *fout, int comsgc, CoMsg *comsgv)
{
        int     indent;
        String lastText = "";

        if (comsgc <= 0) return;

        if (comsgDoSource) {
                indent = comsgPrintLine(fout, comsgv[0]->pos);
                fputcTimes(' ', indent, fout);
                comsgPrintDots(fout, comsgc, comsgv);
        }

        for ( ; comsgc-- > 0; comsgv++) {
                CoMsg   co = *comsgv;
                if (strcmp(lastText, co->text)) {
#if EDIT_1_0_n1_04
                        fprintf(fout, "%s", tcolPrefix(co->tag));
                        comsgPrintLead(fout, co->serial, co->msg, co->tag,
                                       co->pos, co->notes.noteNumber);
                        fprintf(fout, "%s", co->text);
                        if (co->notes.noteList && co->tag != COMSG_NOTE)
                                comsgPrintSeeNotes(fout, co->notes.noteList);
                        fprintf(fout, "%s\n", tcolPostfix(co->tag));
                        comsgPrintMacEx(fout, co->pos, co->node);
#else
                        comsgPrintLead(fout, co->serial, co->msg, co->tag,
                                       co->pos, co->notes.noteNumber);
                        fprintf(fout, "%s", co->text);
                        if (co->notes.noteList && co->tag != COMSG_NOTE)
                                comsgPrintSeeNotes(fout, co->notes.noteList);
                        fprintf(fout, "\n");
                        comsgPrintMacEx(fout, co->pos, co->node);
#endif
                }
                lastText = co->text;
        }
        if (comsgDoDetails || comsgDoSource)
                fprintf(fout, "\n");
}


/*****************************************************************************
 *
 * :: Output style.
 *
 ****************************************************************************/

/*
 * Print the source position and source line text.
 * Return the indendation for the first (possibly blank) char of the line text.
 * The value -1 indicates an error.
 */
int
comsgPrintLine(FILE *fout, SrcPos spos)
{
        Buffer  buf;
        MutString  s;
        int     rc, cc;
        Length  n, nu;
        bool    splitLine = false;

        /*
         * Get the source line.
         */
        buf = Buffer::create();
        rc = (spos).lineText(buf);
        if (rc == -1) {
                buf.free();
                return (fintMode == FINT_LOOP ? comsgPromptSize : -1);
        }

        s  = buf.liberate();
        n  = strLength(s);
        nu = strUntabLength(s, TABSTOP);

        /*
         * Print the position and source line,  splitting them if necessary.
         */
        if ((spos).isSpecial())
                cc = (spos).print(fout);
        else {
                String     name;
                FileName        fn = (spos).file();

                String     rest = NULL;

                /* If -M strip=<dir>, strip <dir> from front of filename */
                name = fn.unparseStatic();
                if (comsgStripDir()) rest = strIsPrefix(comsgStripDir(), name);
                if (rest) {
                        /* Change the name of the file */
                        name = rest;
                        fn = FileName::parseStatic(name);
                }

                /* Display the line details */
                cc = fprintf(fout, flineFmt, name, (spos).line());

                /* Do we need to split the line? */
                if (fn.hasDir() && !strEqual(fn.dir(), osCurDirName()))
                        splitLine = true;
        }

        if (cc + nu >= LINE_LENGTH) splitLine = true;
        if (splitLine) fputc('\n', fout);

        fputsUntab(s, TABSTOP, fout);
        if (n == 0 || s[n-1] != '\n') fputc('\n', fout);

        return splitLine ? 0 : cc;
}

/*
 * Print a "...^..^" indicator line.
 */
local void
comsgPrintDots(FILE *fout, int comsgc, CoMsg *comsgv)
{
        int cno = 1;
        while (comsgc-- > 0) {
                SrcPos spos = (*comsgv++)->pos;
                if (!(spos).isSpecial()) {
                        int     Dcno = (spos).character() - cno;
                        if (Dcno >= 0) {
                                fputcTimes('.', Dcno, fout);
                                fprintf(fout, "^");
                        }
                        cno = (spos).character() + 1;
                }
        }
        fprintf(fout, "\n");
}

/*
 * Print the position and any prefixes for a message.
 * E.g. "{ALDOR_E_Stupid} [L32 C47] #3 (Error) (After macro expansion)"
 */
local void
comsgPrintLead(FILE *fout, Length serial, Msg msg, CoMsgTag tag,
               SrcPos spos, int noteNumber)
{
        String tagstring;

        if (comsgDoName)
                fprintf(fout, "{%s} ", comsgName(msg));

        if (!(spos).isSpecial()) {
                FileName fn = (spos).file();
                int      ln = (spos).line(), cn = (spos).character();

                if (comsgDoSource)
                        fprintf(fout, lcharFmt, ln, cn);
                else
                        fprintf(fout, flineFmt, fn.unparseStatic(), ln);
        }

        if (comsgDoNumber)
#if EDIT_1_0_n1_07
                fprintf(fout, "#%d ", (int) serial);
#else
                fprintf(fout, "#%d ", serial);
#endif

        switch (tag) {
        case COMSG_REMARK:  tagstring = remarkTag;  break;
        case COMSG_WARNING: tagstring = warningTag; break;
        case COMSG_ERROR:   tagstring = errorTag;   break;
        case COMSG_FATAL:   tagstring = fatalTag;   break;
        case COMSG_NOTE:    tagstring = noteTag;    break;
        default:            tagstring = "";         bugBadCase(tag);
        }

        if (tag != COMSG_NOTE)
                fprintf(fout, "%s", tagstring);
        else
                fprintf(fout, tagstring, noteNumber);

        if ((spos).isMacroExpanded()) fprintf(fout, "%s", afterMacEx);
}

local void
comsgPrintMacEx(FILE *fout, SrcPos spos, AbSyn ab)
{
        if (! (spos).isMacroExpanded() || !ab) return;

        fprintf(fout, "%s", expandedExpr);
        abPrettyPrint(fout, ab);
        fprintf(fout, "\n");
}

local void
comsgPrintSeeNotes(FILE *fout, List<CoMsg> notes)
{
        CoMsg co;
        String fmtNote = comsgString(ALDOR_P_MsgNote);
        String fmtAnd  = comsgString(ALDOR_P_MsgConjunction);
        String fmt     = comsgString(ALDOR_P_MsgSeeNote);
        Buffer obuf    = Buffer::create();
        bool first     = true;


        for (notes = listNReverse<CoMsg>(notes); notes; notes = cdr(notes)) {
                co = car(notes);
                if (first)
                        first = false;
                else
                        obuf.printf(cdr(notes) ? ", " : fmtAnd);
                obuf.printf(fmtNote, (int) co->notes.noteNumber);
        }
        fprintf(fout, "\n");
        fprintf(fout, fmt, obuf.chars());
        obuf.free();
}

int
comsgFPrintf(FILE *fout, Msg msg, ...)
{
        int     cc;
        va_list argp;

        va_start(argp, msg);
        cc = comsgVFPrintf(fout, msg, argp);
        va_end(argp);

        return cc;
}

int
comsgVFPrintf(FILE *fout, Msg msg, va_list argp)
{
        int     cc;
        String fmt;

        fmt = comsgString(msg);

        cc = vfprintf(fout, fmt, argp);
        fputc('\n', fout);
        cc++;

        return cc;
}

local void
comsgSetPromptSize(int size)
{
        comsgPromptSize = size;
}

/*
 * If fin is interactive print the prompt on fout using
 * printf-style formatting.
 */
void
comsgPromptPrint(FILE *fin, FILE *fout, String fmt, ...)
{
        int promptSize;

        va_list argp;

        if (!osIsInteractive(fin)) return;

        va_start(argp, fmt);
        promptSize = vfprintf(fout, fmt, argp);
        va_end(argp);

        comsgSetPromptSize(promptSize);

        fflush(fout);
}

