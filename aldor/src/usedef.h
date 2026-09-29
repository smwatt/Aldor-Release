/*****************************************************************************
 *
 * usedef.h: Usage definition chains
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

#ifndef _USEDEF_H_
#define _USEDEF_H_

# include "axlobs.h"

# define        udReachingDefs(foam)    ((foam)->foamGen.hdr.info.defList)
# define        udInfoDef(udinfo)       ((udinfo)->foam)
# define        udInfoBlock(udinfo)     ((udinfo)->block)


/****************************************************************************
 *
 * :: Type Definitions
 *
 ****************************************************************************/

struct  _UdInfo {
        Foam            foam;   /* definition */
        BBlock          block;  /* block containing the definition */

};

typedef enum { 
        UD_OUTPUT_UdList,               
        UD_OUTPUT_SinglePointer
} UdOutputKind;

/****************************************************************************
 *
 * :: External entry points
 *
 ****************************************************************************/

extern void     usedefChainsFreeFrProg  (Foam);

extern bool     usedefChainsFrFlog      (FlowGraph, UdOutputKind);
extern void     usedefChainsFreeFrFlog  (FlowGraph);
extern void     udSetFlogCutOff         (int);

#endif /* _USEDEF_H_ */

