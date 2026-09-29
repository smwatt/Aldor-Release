/*****************************************************************************
 *
 * cfgfile.h: Configuration file handling
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

#ifndef _CFGFILE_H_
#define _CFGFILE_H_

#include "axlgen.h"

typedef struct _ConfigItem {
        char *optName;
        char *optVal;
        char  content[NARY];
} *ConfigItem;

DECLARE_LIST(ConfigItem);

#define cfgName(item) ((item)->optName)
#define cfgVal(item)  ((item)->optVal)

extern void           cfgSetConfPath    (MutString path);
extern FileName       cfgFindFile       (String name, String ext);
extern ConfigItemList cfgRead           (FILE *file, String name);
extern MutStringList     cfgReadGetErrors  (void);
extern void           cfgReadClearErrors(void);
extern void           cfgFree           (ConfigItem item);
extern ConfigItem     cfgNew            (String key, String val);

void cfgSetCondFunc(int (*check)(MutString));

extern ConfigItem cfgLookup(String, ConfigItemList);
extern ConfigItemList cfgLookupList(String, ConfigItemList);
extern MutStringList cfgLookupStringList(String, ConfigItemList);
extern MutString   cfgLookupKeyName(String, ConfigItemList);
extern MutStringList cfgLookupKeyNameList(String, ConfigItemList);
extern MutString   cfgLookupString(String, ConfigItemList);
extern bool     cfgLookupBoolean(String, ConfigItemList);

extern void   cstrParseUnquoted(char *str, int *pargc, char ***pargv);
extern void   cstrParseCommaified(String opts, int *pargc, char ***pargv);

#endif /*!_CFGFILE_H_*/
