/*****************************************************************************
 *
 * loops.h: Loop Optimization Tools
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

#ifndef _LOOPS_H
#define  _LOOPS_H

#include "axlobs.h"

/****************************************************************************
 *
 * :: Type Definitions
 *
 ****************************************************************************/


struct _Dominators {

        FlowGraph       graph;
        BitvClass       bitvClass;
        Bitv            * doms;
        int             nBlocks;

};

typedef struct _Dominators * Dominators;

struct _Loop {
        
        FlowGraph       graph;
        BBlock          header;
        BBlockList      blockList;

        Bitv            blockSet;
        BitvClass       bitvClass;

};
        
typedef struct _Loop    * Loop;

DECLARE_LIST(Loop);

/****************************************************************************
 *
 * :: External Entry Points
 *
 ****************************************************************************/

extern LoopList         lpNaturalLoopsFrFlog    (FlowGraph, Dominators *);
extern void             lpLoopFree              (Loop);

extern BBlockList       lpExitBlocksFrLoop      (Loop);

extern void             lpLoopDepthInfoSetInFlog(FlowGraph);

extern LoopList         lpUnifyCommonHeaders    (LoopList);

extern void             lpDominatorsFree        (Dominators);
/****************************************************************************
 *
 * :: Macros
 *
 ****************************************************************************/


# define lpIsDom(doms, b1,b2)   \
     bitvTest((doms)->bitvClass, (doms)->doms[(b2)->label], (b1)->label)

# define lpSetDom(doms, b1,b2)  \
     bitvSet((doms)->bitvClass, (doms)->doms[(b2)->label], (b1)->label)

# define lpIsBlockInLoop(bb, loop) \
     (bitvTest((loop)->bitvClass, (loop)->blockSet, (bb)->label))

# define lpNumBlocksInLoop(loop) (listLength(BBlock)((loop)->blockList))

#endif /* _LOOPS_H */
