/*****************************************************************************
 *
 * of_deada.h: Dead Assignment Elimination
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

#ifndef _OF_DEADA_H_
#define _OF_DEADA_H_

typedef struct liveInfo {
        int  stmtId;
        Bitv live;
} *LiveInfo;

DECLARE_LIST(LiveInfo);

typedef struct {
        /* Should be a vector so binary search is possible */
        LiveInfoList *vars;
        BitvClass    bvClass;
} *LiveVars;


void    daSetCutOff     (int);
void    deadAssign      (Foam);

#endif /*!_OF_DEADA_H_*/

