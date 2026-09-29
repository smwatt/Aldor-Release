/*****************************************************************************
 *
 * tfsat.h: Type form satisfaction.
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

#ifndef _TFSAT_H_
#define _TFSAT_H_

#include "axlobs.h"

/******************************************************************************
 *
 * :: Type form satisfaction flags.
 *
 *****************************************************************************/

typedef ULong           SatMask;

extern SatMask          tfSatHasMask            (void);
extern SatMask          tfSatBupMask            (void);
extern SatMask          tfSatTdnMask            (void);
extern SatMask          tfSatTdnInfoMask        (void);
extern SatMask          tfSatSefMask            (void);
extern SatMask          tfSatTErrorMask         (void);

extern bool             tfSatSucceed            (SatMask);
extern bool             tfSatPending            (SatMask);
extern TForm            tfSatGetPendingFail     (void);

extern bool             tfSatFailedExportsMissing(SatMask);
extern bool             tfSatFailedEmbedFail     (SatMask);
extern bool             tfSatFailedArgMissing    (SatMask);
extern bool             tfSatFailedBadArgType    (SatMask);
extern bool             tfSatFailedDifferentArity(SatMask);

extern Length           tfSatParN               (SatMask);
extern Length           tfSatArgN               (AbSyn, Length, AbSynGetter,
                                                 Length, TForm);

extern AbEmbed          tfSatAbEmbed            (SatMask);
extern AbEmbed          tfSatEmbedType          (TForm, TForm);
extern TForm            tfsEmbedResult          (TForm, AbEmbed);
/******************************************************************************
 *
 * :: tfSatisfies
 *
 *****************************************************************************/

/*
 * Return true if any object of type S is valid in any context
 * which requires an object of type T.
 */
extern bool             tfSatisfies     (TForm S, TForm T);

/*
 * Return true if any object of type S is valid in a value context which
 * requires an object of type T.  The embedding from S -> () is not used.
 */
extern bool             tfSatValues     (TForm S, TForm T);

/*
 * Return true if any object of type S is valid in a return context which
 * requires an object of type T.  The embedding from S -> () is used if needed.
 */
extern bool             tfSatReturn     (TForm S, TForm T);
 
/******************************************************************************
 *
 * :: Type orders
 *
 *****************************************************************************/

/*
 * Return true if S is a subtype of Domain.
 * Symbols whose type is S represent Domains.
 */
extern bool             tfSatDom        (TForm S);

/*
 * Return true if S is a subtype of Category.
 * Symbols whose type is S represent Categories.
 */
extern bool             tfSatCat        (TForm S);

/*
 * Return true if S is a subtype of Type.
 * Symbols whose type is S represent Types.
 */
extern bool             tfSatType       (TForm S);

/******************************************************************************
 *
 * :: tfSatMap
 *
 *****************************************************************************/

extern SatMask          tfSatMap        (SatMask, Stab, TForm, TForm,
                                         AbSyn, Length, AbSynGetter);
extern SatMask          tfSatArg        (SatMask, AbSyn, TForm);

extern bool             tfSatBit        (SatMask, TForm S, TForm T);
extern SatMask          tfSat           (SatMask, TForm S, TForm T);

extern AbSub            tfSatSubList    (AbSyn);


/******************************************************************************
 *
 * :: tfSatMulti
 *
 *****************************************************************************/

extern SatMask          tfSatMapArgs    (SatMask, AbSub, TForm,
                                         AbSyn, Length, AbSynGetter);
extern SatMask          tfSatAsMulti    (SatMask, AbSub, TForm, TForm,
                                         AbSyn, Length, AbSynGetter);

#endif /* !_TFSAT_H_ */
