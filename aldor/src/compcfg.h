/*****************************************************************************
 *
 * compcfg.h: Compiler configuration
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

#ifndef _COMPCFG_H_
#define _COMPCFG_H_

extern void compCfgInit(String);
extern void compCfgFini(void);

extern MutString compCfgLookupString(String);
extern bool   compCfgLookupBoolean(String);

extern void compCfgSetConfigFile(String);
extern void compCfgSetSysName(String);

extern String compCfgGetConfigFile (void);
extern String compCfgGetSysName    (void);

#endif /*!_COMPCFG_H_*/
