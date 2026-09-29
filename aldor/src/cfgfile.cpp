///////////////////////////////////////////////////////////////////////////////
//
// cfgfile.cpp: Configuration file handling
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

#include "cfgfile.h"
#include <stdio.h>

bool    cfgDebug                = false;

#define cfgDEBUG(s)             DEBUG_IF(cfgDebug, s)

static MutString cfgGetLine(FILE *file, bool *atEof);
static bool   cfgIsSection(char *line, String name);
static void cfgReadAddError(String);
static void cfgReadDupItemError(ConfigItem, String);
static bool cfgEqual(ConfigItem, ConfigItem);
static ConfigItemList cfgGetKeys(FILE *file, String section);
static ConfigItem cfgParseLine(MutString line);

CREATE_LIST(ConfigItem);

/*********************************************************************************
 *
 * :: Extracting config information
 *
 *********************************************************************************/
#define IsWhiteSpace(c) ((c)==' ' || (c)=='\n' || (c) == '\t')

static MutStringList cfgExpandList (MutStringList *plst,
                                 MutStringList **end,
                                 ConfigItemList lst,
                                 bool keep);
static bool       cfgCheckCondition(MutString name, ConfigItemList cfg);
static ConfigItem cfgExpandVar(ConfigItem, ConfigItemList);

static MutString cfgKeyNameValue(MutString str);
static MutString cfgStringValue(MutString str);
static MutStringList cfgStringListValue(MutString str);
static bool cfgBooleanValue(MutString str);
static MutString cfgKeyNameValue(MutString str);

static ConfigItem
cfgExpandVar(ConfigItem item, ConfigItemList lst)
{
        char c;
        int i;

        if (!item) return NULL;
        i = 0;
        while ( (c = cfgVal(item)[i]) == ' ' || c == '\t') i++;
        if (cfgVal(item)[i] == '$') {
                item = cfgLookup(cfgKeyNameValue(&cfgVal(item)[i+1]), lst);
                return cfgExpandVar(item, lst);
        }
        return item;
}

ConfigItemList
cfgLookupList(String string, ConfigItemList lst)
{
        ConfigItemList  result = listNil(ConfigItem);

        cfgDEBUG(printf("Getting key list: %s\n", string));

        while (lst != listNil(ConfigItem)) {
                if (strEqual(string, cfgName(car(lst))))
                        result = listCons(ConfigItem)(car(lst), result);
                lst = cdr(lst);
        }
        return listNReverse(ConfigItem)(result);
}

ConfigItem
cfgLookup(String string, ConfigItemList lst)
{
        cfgDEBUG(printf("Getting key: %s\n", string));

        while (lst != listNil(ConfigItem)) {
                if (strEqual(string, cfgName(car(lst))))
                        return car(lst);
                lst = cdr(lst);
        }
        return NULL;
}

static MutStringList
cfgLookupUnexpandedStringList(String str, ConfigItemList lst)
{
        ConfigItem item = cfgLookup(str, lst);

        /*
         * Ought to store this for the client to report just as we
         * do for cfgRead().
         */
        if (!item) {
                printf("config file: failed to find key `%s'\n", str);
                return NULL;
        }
        return cfgStringListValue(cfgVal(item));
}


MutStringList
cfgLookupStringList(String str, ConfigItemList lst)
{
        MutStringList ll, *foo;
        ll = cfgLookupUnexpandedStringList(str, lst);

        if (ll == listNil(MutString))
                return NULL;
        ll = cfgExpandList(&ll, &foo, lst, true);
        return ll;
}

#define cfgIfTerminator(x) ((x)[1] == '\0' && ((x)[0] == ':' || (x)[0] == ';'))

static MutStringList
cfgExpandList(MutStringList *plst, MutStringList **end, ConfigItemList cfg, bool keep)
{
        MutStringList *pp;
        String error = NULL;
        pp = plst;
        while (*pp != listNil(MutString) && !cfgIfTerminator(car(*pp))) {
                MutString name;
                if (car(*pp)[0] != '$') {
                        if (keep) pp = &cdr(*pp);
                        else *pp = cdr(*pp);
                        continue;
                }
                name = &(car(*pp)[1]);
                if (name[0] == '?') {
                        bool flg = cfgCheckCondition(&name[1], cfg);
                        MutStringList *iend;
                        *pp = cdr(*pp);
                        *pp = cfgExpandList(pp, &iend, cfg, flg && keep);
                        if (car(*iend)[0] != ':')
                                error = "Missing `:'";
                        if (cdr(*iend)) *iend = cdr(*iend);
                        else
                                error = "Nothing after `:'.";
                        cfgExpandList(iend, &iend, cfg, !flg && keep);
                        /* Should check for ';' */
                        *iend = cdr(*iend);
                        pp = iend;
                }
                else {
                        MutStringList ll;
                        if (!keep) *pp = cdr(*pp);
                        else {
                                ll = cfgLookupUnexpandedStringList(name, cfg);
                                *pp = listNConcat(MutString)(ll, cdr(*pp));
                        }
                }
        }
        if (error)
                printf("Error in cfg: %s\n", error);
        *end = pp;
        return *plst;
}


MutString
cfgLookupKeyName(String str, ConfigItemList lst)
{
        ConfigItem item = cfgLookup(str, lst);
        item = cfgExpandVar(item, lst);

        if (!item) {
                return NULL;
        }
        return cfgKeyNameValue(cfgVal(item));
}

MutStringList
cfgLookupKeyNameList(String str, ConfigItemList lst)
{
        ConfigItemList  items = cfgLookupList(str, lst);
        MutStringList      result = listNil(MutString);


        /* Process each item in the list */
        for (; items; items = cdr(items)) {
                MutString          value;
                ConfigItem      item;


                /* Get the next item */
                item = car(items);


                /* Expand variables */
                item = cfgExpandVar(item, lst);


                /* Try the next one if no value found */
                if (!item) continue;


                /* Convert key into a value */
                value = cfgKeyNameValue(cfgVal(item));


                /* Add it to the result list */
                result = listCons(MutString)(value, result);
        }

        return listNReverse(MutString)(result);
}

MutString
cfgLookupString(String str, ConfigItemList lst)
{
        ConfigItem item = cfgLookup(str, lst);
        item = cfgExpandVar(item, lst);

        if (!item) {
                return NULL;
        }
        return cfgStringValue(cfgVal(item));
}

bool
cfgLookupBoolean(String str, ConfigItemList lst)
{
        ConfigItem item = cfgLookup(str, lst);
        item = cfgExpandVar(item, lst);

        if (!item) {
                return 0; /* Hmmm */
        }
        return cfgBooleanValue(cfgVal(item));
}

static MutStringList
cfgStringListValue(MutString str)
{
        int argc;
        char **argv;
        MutStringList lst;
        int i;

        if (str[0] == ',')
                cstrParseCommaified(str+1, &argc, &argv);
        else
                cstrParseUnquoted(str, &argc, &argv);

        lst = listNil(MutString);
        for (i=0; i<argc; i++) lst = listCons(MutString)(argv[argc - i - 1], lst);

        return lst;
}

static bool
cfgBooleanValue(MutString str)
{
        MutString s = cfgKeyNameValue(str);

        if (strEqual(s, "yes")) return true;
        if (strEqual(s, "no")) return false;
        if (strEqual(s, "true")) return true;
        if (strEqual(s, "false")) return false;

        printf("Illegal value for boolean: %s\n", s);
        return false;
}

static MutString
cfgKeyNameValue(MutString str)
{
        char **argv;
        int argc;
        cstrParseUnquoted(str, &argc, &argv);
        if (argc != 1) {
                printf("Bad identifier format: `%s'\n", str);
                { static char empty[] = ""; return empty; }
        }

        str = argv[0];
        stoFree(argv);
        return str;
}

static MutString
cfgStringValue(MutString str)
{
        char *p, *start, *new_;
        /* Strip spaces front and rear, ignore leading and trailing quotes */
        p = str;
        while (IsWhiteSpace(*p) && *p != '\0') p++;
        if (*p == '"') p++;
        if (*p == '\0') return strCopy("");
        start = p;
        p = start + strlen(start) - 1;
        while (IsWhiteSpace(*p) && p != start) p--;
        if (*p == '"') p--;
        new_ = strCopy(start);
        new_[p - start +1] = '\0';
        return new_;
}

/*************************************************************************************
 *
 * :: Parsing input strings
 *
 *************************************************************************************/



void
cstrParseUnquoted(char *str, int *pargc, char ***pargv)
{
        char *p = str;
        char *wstart;
        char **argv;
        char c;
        int  n, lim;

        n = 0;
        lim = 0;
        argv = NULL;
        while (1) {
                while (IsWhiteSpace(*p) && *p != '\0') p++;
                if (*p == '\0') break;
                wstart = p;
                /* May want to be clever here wrt to '"' */
                while (!IsWhiteSpace(*p) && *p != '\0') p++;
                c = *p;
                *p = '\0';
                if (n==lim) {
                        char **targv = argv;
                        int i;
                        argv = (char **) stoAlloc(OB_Other, (n + 5)*sizeof(char *));
                        lim += 5;
                        for (i=0; i<n; i++) argv[i] = targv[i];
                        if (targv) stoFree(targv);
                }
                argv[n++] = strCopy(wstart);
                *p = c;
                if (c == '\0') break;
        }

        *pargc = n;
        *pargv = argv;
}


void
cstrParseCommaified(String opts, int *pargc, char ***pargv)
{
        MutString str = strCopy(opts);
        char **argv;
        char *p, *q;
        int  argc, i;

        p = str;
        q = p;
        argc = 1;
        while (*p != '\0') {
                if (*p == '\\')
                        p++;
                else if (*p == ',') {
                        *p = '\0';
                        argc++;
                }
                *q = *p;
                p++; q++;
        }
        *q = '\0';
        p = str;
        argv = (char **) stoAlloc(OB_Other, argc * sizeof(char *));
        for (i=0; i<argc; i++) {
                argv[i] = p;
                while (*p != '\0') p++;
                p++;
        }

        *pargc = argc;
        *pargv = argv;
}

/**********************************************************************************
 *
 * :: Conditions
 *
 **********************************************************************************/

/*
 * Function should return 1 for true, 0 for false, and -1 if
 * not set
 */

static int (*cfgCondFn)(MutString);

void
cfgSetCondFunc(int (*check)(MutString))
{
        cfgCondFn = check;
}

static bool
cfgCheckCondition(MutString name, ConfigItemList cfg)
{
        int val;
        if ( (val = (*cfgCondFn)(name)) != -1)
                return val;

        return cfgLookupBoolean(name, cfg);
}

/**********************************************************************************
 *
 * :: Reading information
 *
 **********************************************************************************/

static MutStringList cfgPath;

ConfigItem
cfgNew(String key, String val)
{
        ConfigItem item;
        int nkey, nval;
        nkey = strlen(key);
        nval = strlen(val);

        item = (ConfigItem) stoAlloc(OB_Other, fullsizeof(struct _ConfigItem, nkey + nval + 2, char));
        item->optName = item->content;
        strcpy(item->optName, key);
        item->optVal = item->content + nkey + 1;
        strcpy(item->optVal, val);

        return item;
}

void cfgFree(ConfigItem item)
{
        stoFree(item);
}

void cfgSetConfPath(char *path)
{
        bool done;
        MutString s = strCopy(path);
        char *start, *p;
        p = s;
        cfgPath = listNil(MutString);
        done = false;
        while (!done) {
                char c;
                start = p;
                while (*p != '\0' && *p != osPathSeparator())
                        p++;
                c = *p;
                *p = '\0';
                cfgPath = listCons(MutString)(start, cfgPath);
                if (c == '\0') done = true;
                p++;
        }
}

void cfgFreeConfPath()
{
        if (cfgPath) strFree(car(cfgPath));
        listFree(MutString)(cfgPath);
}

FileName
cfgFindFile(String basename, String ext)
{
        MutStringList lst;

        lst = cfgPath;
        while (lst != listNil(MutString)) {
                FileName name = FileName::create(car(lst), basename, ext);
                if (fileIsOpenable(name, osIoRdMode))
                        return name;
                lst = cdr(lst);
        }

        return NULL;
}


/*
 * List of errors discovered during cfgRead(). The list is
 * reset to nothing each time cfgRead() is invoked and may
 * be accessed by calling cfgReadGetErrors().
 */
static MutStringList CfgReadErrs = listNil(MutString);

#include "sto_debug.h"
ConfigItemList
cfgRead(FILE *file, String name)
{
        bool done = false;

        /* Discard the previous error list */
        cfgReadClearErrors();


        /* Locate, and then read, the specified section */
        while (!done) {
            MutString line = cfgGetLine(file, &done);
            if (cfgIsSection(line, name)) {
                return cfgGetKeys(file, name);
            }
        }
        return NULL;
}


/* Return the list of errors from the previous cfgRead() call */
MutStringList
cfgReadGetErrors(void)
{
        return CfgReadErrs;
}


/* Reset the error list */
void
cfgReadClearErrors(void)
{
        /* Discard the previous error list */
        listFreeDeeply(MutString)(CfgReadErrs, strFree);


        /* Start a new error list */
        CfgReadErrs = listNil(MutString);
}


/* Add a new error to the list */
static void
cfgReadAddError(String error)
{
        CfgReadErrs = listCons(MutString)(strCopy(error), CfgReadErrs);
}


/* Report that we've seen this item before */
static void
cfgReadDupItemError(ConfigItem item, String section)
{
        MutString  msg;

        /* Compose the error message */
        msg = strlConcat(
                "Duplicate key `", cfgName(item),
                "' found in section [", section, "]",
                " of the compiler configuration file.",
                (String) NULL
        );


        /* Add the error to the list */
        cfgReadAddError(msg);
}


/* Equality based on key name */
static bool
cfgEqual(ConfigItem itemA, ConfigItem itemB)
{
        return strEqual(cfgName(itemA), cfgName(itemB));
}

static bool
cfgIsSection(char *line, String name)
{
        char *p = line;
        while (*p != ']' && *p != '\0') p++;
        *p = 0;
        return (strEqual(line+1, name));
}


static ConfigItemList
cfgGetKeys(FILE *file, String section)
{
        ConfigItemList lst;
        bool atEof;

        atEof = false;
        lst = listNil(ConfigItem);
        while (true) {
                ConfigItem item;
                MutString line = cfgGetLine(file, &atEof);


                /* Stop if we hit the next section */
                if (line[0] == '[') break;


                /* Parse the line to see if contains an item */
                item = cfgParseLine(line);


                /* Check for duplicates (allow `inherit') */
                if (item && !strEqual(cfgName(item), "inherit"))
                {
                        if (listMember(ConfigItem)(lst, item, cfgEqual))
                                cfgReadDupItemError(item, section);
                }


                /*
                 * We always add the item to the list even if one with
                 * the same key exists already. The client may wish to
                 * deal with duplicate keys themselves (e.g. inherit).
                 */
                if (item) lst = listCons(ConfigItem)(item, lst);


                /* Stop if we've reached the end of the file */
                if (atEof) break;


                /* Release temporary storage */
                strFree(line);
        }
        return lst;
}

static ConfigItem
cfgParseLine(MutString line)
{
        MutString key;
        MutString val;
        char *p = line, *keyEnd;
        /* skip whitespace */
        while (*p == ' ' || *p == '\t') p++;

        if (*p == '#') return NULL;
        key = p;

        while (*p != ' ' && *p != '\t' && *p != '=' && *p != '\0') p++;
        keyEnd = p;

        if (*p == '\0') return NULL;

        while (*p != '=') p++;

        p++;
        val = p;
        *keyEnd = '\0';

        return cfgNew(key, val);
}

static MutString
cfgGetLine(FILE *file, bool *atEof)
{
        MutString s;
        int n, lim, c;
        s = strAlloc(50);
        n = 0;
        lim = 50;

        c = fgetc(file);
        while (c != '\n' && c != EOF) {
                if (n == lim) {
                        MutString tmp = s;
                        s = strAlloc(lim + 20);
                        strncpy(s, tmp, n);
                        strFree(tmp);
                        lim += 20;
                }
                s[n] = c;
                c = fgetc(file);
                n++;
        }
        s[n] = '\0';
        if (c == EOF) *atEof = true;
        return s;
}

