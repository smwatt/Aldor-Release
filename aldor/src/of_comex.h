/*****************************************************************************
 *
 * of_comex.h: Common Subexpressions Elimination
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

#ifndef OF_COMEX_H
#define OF_COMEX_H

#include "axlobs.h"


struct _ExpInfo {
                /* NOTE: EXP and STMT fields use structure sharing */
        Foam            exp;            /* expression foam code */
        int             expNo;          /* expression index */
        int             phantomNo;      /* number phantom bit */

        AInt            newLoc;         /* local used for the transformation */
        FoamList        copies;         /*     "    "      "        "        */
        bool            evaluated;      /* false -> use newLoc instead of exp*/

};


extern void cseUnit     (Foam);
extern void cseFlog     (FlowGraph, Foam);


#endif
