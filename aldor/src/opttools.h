/*****************************************************************************
 *
 * opttools.h: Generic Optimization Tools
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

#ifndef _OPTTOOLS_H
#define  _OPTTOOLS_H

#include "axlobs.h"

#include "of_comex.h"
#include "of_cprop.h"
#include "usedef.h"




# if 0
union VarInfoUnion {
        ExpInfo         expInfo;        /* used by: comex */
        Foam            foam;           /* used by: cprop, usedef */

} ;

typedef union  VarInfoUnion * VarInfo;

#endif

typedef void * VarInfo;

DECLARE_LIST(VarInfo);

typedef int (* VarInfoPrintFn)(VarInfo) ;

#if 0

union AssociationTypeUnion {
        VarInfoList     list;
        VarInfo         single;
};

typedef union AssociationTypeUnion * AssociationType;

#endif

typedef void * AssociationType;

DECLARE_LIST(AssociationType);

/*****************************************************************************
 *
 * :: Exports
 *
 ****************************************************************************/

#define otSymeConstSetEnvIndep(s)  symeSetConstFlag(s, 0)
#define otSymeConstClrEnvIndep(s)  symeClrConstFlag(s, 0)
#define otSymeConstEnvIndep(s)     symeConstFlag(s, 0)

extern VarInfoList      otGetVarInfoList        (Foam);

/* NOTE: you may use the otSetVarInfo macro*/
extern void             otSetVarInfo0           (VarInfo, Foam);

/* NOTE: you may use the otAddVarInfo macro*/
extern void             otAddVarInfo0           (VarInfo, Foam);

extern void             otProgInfoInit          (UShort, int, int, Foam);
extern void             otProgInfoFini          (void);
extern void             otPrintVarAssociations  (VarInfoPrintFn);

extern bool             otIsMovableData         (Foam);
extern bool             otIsConstSyme           (Syme);
extern bool             otSymeIsFoamConst       (Syme);
extern bool             otIsForcer              (Foam);
extern void             otTransferFoamInfoToSyme(Syme, Foam);
extern void             otTransferFoamInfo      (SymeList, Foam);

/****************************************************************************
 *
 * :: Macros
 *
 ****************************************************************************/

# define OT_ASSOCIATION_SINGLE          0x0001
# define OT_ASSOCIATION_LIST            0x0002
# define OT_ASSOCIATION_VECTOR          0x0004

# define otAddVarInfo(info, var)   otAddVarInfo0((info).asPointer(), var)
# define otSetVarInfo(info, var)   otSetVarInfo0((VarInfo) (info), var)

# define otIsDef(foam)          (foamTag(foam) == FOAM_Set ||   \
                                 foamTag(foam) == FOAM_Def)

# define otIsVar(foam)          (foamTag(foam) == FOAM_Loc ||   \
                                 foamTag(foam) == FOAM_Par ||   \
                                 foamTag(foam) == FOAM_Lex ||   \
                                 foamTag(foam) == FOAM_Glo)

# define otIsVar(foam)          (foamTag(foam) == FOAM_Loc ||   \
                                 foamTag(foam) == FOAM_Par ||   \
                                 foamTag(foam) == FOAM_Lex ||   \
                                 foamTag(foam) == FOAM_Glo)

# define otIsLocalVar(foam)     (foamTag(foam) == FOAM_Loc ||   \
                                 foamTag(foam) == FOAM_Par)

# define otIsNonLocalVar(foam)  (foamTag(foam) == FOAM_Lex ||   \
                                 foamTag(foam) == FOAM_Glo)

/* Note that foamTag(foam) is always >= FOAM_DATA_START */
# define otIsFoamConst(foam)    (foamTag(foam) < FOAM_DATA_LIMIT)

# define otDereferenceCast(foam)   \
        while (foamTag(foam) == FOAM_Cast) foam = foam->foamCast.expr;

#endif /* _OPTTOOLS_H */
