///////////////////////////////////////////////////////////////////////////////
//
// fortran.cpp: Fortran interactions and utilities.
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

/*
 * This file contains routines for helping to bridge the gap between
 * Aldor and Fortran.
 */

#include "fortran.h"

bool    fortranTypesDebug       = false;
#define fortranTypesDEBUG(s)    DEBUG_IF(fortranTypesDebug, s)


/* This is a local macro just to make the if tests more readable */
#define XtfIsValid(tf)  (tf && !tfIsUnknown(tf))


/* Some interesting TForms which we want to keep a lookout for */
TForm tfFtnBool = (TForm) NULL; /* Boolean/LOGICAL */
TForm tfFtnChar = (TForm) NULL; /* Character/CHARACTER or CHARACTER*1 */
TForm tfFtnXStr = (TForm) NULL; /* MutString/CHARACTER(*) */
TForm tfFtnFStr = (TForm) NULL; /* MutString/CHARACTER*N for some N */
TForm tfFtnFSA  = (TForm) NULL; /* Array FixedString N for some N */
TForm tfFtnArry = (TForm) NULL; /* FSingleArray/REAL(*) etc. */
TForm tfFtnSInt = (TForm) NULL; /* SingleInteger/INTEGER */
TForm tfFtnSFlo = (TForm) NULL; /* SingleFloat/REAL */
TForm tfFtnDFlo = (TForm) NULL; /* DoubleFloat/DOUBLE PRECISION */
TForm tfFtnSCpx = (TForm) NULL; /* FSComplex/COMPLEX */
TForm tfFtnDCpx = (TForm) NULL; /* FDComplex/COMPLEX(KIND=KIND(0.D0)) */


/*
 * We lazily initialise the TForms for Fortran category
 * attributes. We ought to put them in tform.c to allow
 * them to be picked up by tfFrSymbol() properly by other
 * clients. That would make ftnTypeFrString() seem a bit
 * cleaner but there isn't much point.
 */
local void
ftnTypeInitTForms()
{
        if (!tfFtnSInt) tfFtnSInt = tfFrSymbol(ssymFtnSInt);
        if (!tfFtnSFlo) tfFtnSFlo = tfFrSymbol(ssymFtnSFlo);
        if (!tfFtnDFlo) tfFtnDFlo = tfFrSymbol(ssymFtnDFlo);
        if (!tfFtnSCpx) tfFtnSCpx = tfFrSymbol(ssymFtnSCpx);
        if (!tfFtnDCpx) tfFtnDCpx = tfFrSymbol(ssymFtnDCpx);
        if (!tfFtnBool) tfFtnBool = tfFrSymbol(ssymFtnBool);
        if (!tfFtnChar) tfFtnChar = tfFrSymbol(ssymFtnChar);
        if (!tfFtnXStr) tfFtnXStr = tfFrSymbol(ssymFtnXStr);
        if (!tfFtnFStr) tfFtnFStr = tfFrSymbol(ssymFtnFStr);
        if (!tfFtnFSA)  tfFtnFSA  = tfFrSymbol(ssymFtnFSA);
        if (!tfFtnArry) tfFtnArry = tfFrSymbol(ssymFtnArry);
}


/*
 * Given a domain, check to see if it has any of the Fortran
 * attributes such as FortranLogical. If it does then we return
 * the corresponding Fortran type, if not we return 0.
 * Fortran type indicates both input type and output type.
 *
 * Essentially this routine evaluates `dom has FortranLogical'
 * etc for each of the Fortran* categories.
 */
FortranType
ftnTypeFrDomTForm(TForm tf)
{
        Sefo    sefo;
        TForm   tfcat;


        /* We hope that non-word-sized values don't need converting */
        if (gen0Type(tf, NULL) != FOAM_Word)
                return FTN_None;


        /* Walk past the declaration to get the type */
        if (tfIsDeclare(tf))
                tf = tfDeclareType(tf);


        /* We use used to only consider leaf nodes ... */
        sefo  = tfGetExpr(tf);
        tfcat = abGetCategory((AbSyn)sefo);

        if (tfcat)
                return ftnTypeFrCatTForm(sefo, tfcat);
        else
                return FTN_None;
}


/*
 * Given a category, check to see if it satisfies any of the
 * Fortran attribute categories such as FortranLogical and
 * return 0 if not. The sefo is only used for debugging.
 */
FortranType
ftnTypeFrCatTForm(AbSyn sefo, TForm tf)
{
        /*
         * Make sure that we have the category symbols that we
         * are interested in. Initialise them if we don't.
         */
        ftnTypeInitTForms();


        /* Some debugging information. */
        fortranTypesDEBUG({
                (void)printf("- ftnTypeFrCatTForm: %s\n", abPretty(sefo));

                if (XtfIsValid(tfFtnBool) && tfSatisfies(tf,tfFtnBool))
                        (void)printf("     has %s\n", (ssymFtnBool).string());

                if (XtfIsValid(tfFtnChar) && tfSatisfies(tf,tfFtnChar))
                        (void)printf("     has %s\n", (ssymFtnChar).string());

                if (XtfIsValid(tfFtnXStr) && tfSatisfies(tf,tfFtnXStr))
                        (void)printf("     has %s\n", (ssymFtnXStr).string());

                if (XtfIsValid(tfFtnFSA) && tfSatisfies(tf,tfFtnFSA))
                        (void)printf("     has %s\n", (ssymFtnFSA).string());

                if (XtfIsValid(tfFtnFStr) && tfSatisfies(tf,tfFtnFStr))
                        (void)printf("     has %s\n", (ssymFtnFStr).string());

                if (XtfIsValid(tfFtnSInt) && tfSatisfies(tf,tfFtnSInt))
                        (void)printf("     has %s\n", (ssymFtnSInt).string());

                if (XtfIsValid(tfFtnSFlo) && tfSatisfies(tf,tfFtnSFlo))
                        (void)printf("     has %s\n", (ssymFtnSFlo).string());

                if (XtfIsValid(tfFtnDFlo) && tfSatisfies(tf,tfFtnDFlo))
                        (void)printf("     has %s\n", (ssymFtnDFlo).string());

                if (XtfIsValid(tfFtnSCpx) && tfSatisfies(tf,tfFtnSCpx))
                        (void)printf("     has %s\n", (ssymFtnSCpx).string());

                if (XtfIsValid(tfFtnDCpx) && tfSatisfies(tf,tfFtnDCpx))
                        (void)printf("     has %s\n", (ssymFtnDCpx).string());

                if (XtfIsValid(tfFtnArry) && tfSatisfies(tf,tfFtnArry))
                        (void)printf("     has %s\n", (ssymFtnArry).string());

                fnewline(dbOut);
        });


        /*
         * Use type satisfaction to check category attributes. We
         * need to be slightly cautious as some of these categories
         * might not been defined (eg. the library doesn't use them).
         *
         * IMPORTANT: the test for FortranFStringArray MUST occur before
         * the test for FortranArray. This is because anything that
         * has FortranFStringArray will also have FortranArray.
         *
         * IMPORTANT: consider using tfSatBit(tfSatHasMask(), tfcat, ...)
         * as this does not commit the has question.
         */
        if (XtfIsValid(tfFtnBool) && tfSatisfies(tf, tfFtnBool))
                return FTN_Boolean;
        if (XtfIsValid(tfFtnChar) && tfSatisfies(tf, tfFtnChar))
                return FTN_Character;
        if (XtfIsValid(tfFtnSInt) && tfSatisfies(tf, tfFtnSInt))
                return FTN_SingleInteger;
        if (XtfIsValid(tfFtnSFlo) && tfSatisfies(tf, tfFtnSFlo))
                return FTN_FSingle;
        if (XtfIsValid(tfFtnDFlo) && tfSatisfies(tf, tfFtnDFlo))
                return FTN_FDouble;
        if (XtfIsValid(tfFtnSCpx) && tfSatisfies(tf, tfFtnSCpx))
                return FTN_FSComplex;
        if (XtfIsValid(tfFtnDCpx) && tfSatisfies(tf, tfFtnDCpx))
                return FTN_FDComplex;
        if (XtfIsValid(tfFtnXStr) && tfSatisfies(tf, tfFtnXStr))
                return FTN_XLString;
        if (XtfIsValid(tfFtnFStr) && tfSatisfies(tf, tfFtnFStr))
                return FTN_String;
        if (XtfIsValid(tfFtnFSA) && tfSatisfies(tf, tfFtnFSA))
                return FTN_StringArray;
        if (XtfIsValid(tfFtnArry) && tfSatisfies(tf, tfFtnArry))
                return FTN_Array;


        /* Failed to match */
        return FTN_None;
}


/*
 * Convert from FortranType into text strings. Used to insert
 * FortranType values into FOAM declarations etc.
 */
String
ftnNameFrType(FortranType ftype)
{
        String s;

        switch (ftype)
        {
                case FTN_Boolean        : s = (ssymFtnBool).string();break;
                case FTN_Character      : s = (ssymFtnChar).string();break;
                case FTN_SingleInteger  : s = (ssymFtnSInt).string();break;
                case FTN_FSingle        : s = (ssymFtnSFlo).string();break;
                case FTN_FDouble        : s = (ssymFtnDFlo).string();break;
                case FTN_FSComplex      : s = (ssymFtnSCpx).string();break;
                case FTN_FDComplex      : s = (ssymFtnDCpx).string();break;
                case FTN_XLString       : s = (ssymFtnXStr).string();break;
                case FTN_String         : s = (ssymFtnFStr).string();break;
                case FTN_StringArray    : s = (ssymFtnFSA).string();break;
                case FTN_Array          : s = (ssymFtnArry).string();break;
                default                 : s = "";break;
        }

        return strCopy(s);
}


/*
 * Convert from text strings into FortranType. Used to extract
 * FortranType values from FOAM declarations etc.
 */
FortranType
ftnTypeFrString(String s)
{
        if (!strcmp((ssymFtnChar).string(), s))
                return FTN_Character;
        if (!strcmp((ssymFtnBool).string(), s))
                return FTN_Boolean;
        if (!strcmp((ssymFtnSInt).string(), s))
                return FTN_SingleInteger;
        if (!strcmp((ssymFtnSFlo).string(), s))
                return FTN_FSingle;
        if (!strcmp((ssymFtnDFlo).string(), s))
                return FTN_FDouble;
        if (!strcmp((ssymFtnSCpx).string(), s))
                return FTN_FSComplex;
        if (!strcmp((ssymFtnDCpx).string(), s))
                return FTN_FDComplex;
        if (!strcmp((ssymFtnXStr).string(), s))
                return FTN_XLString;
        if (!strcmp((ssymFtnFSA).string(), s))
                return FTN_StringArray;
        if (!strcmp((ssymFtnFStr).string(), s))
                return FTN_String;
        if (!strcmp((ssymFtnArry).string(), s))
                return FTN_Array;
        return FTN_None;
}


/*
 * Convert an Aldor type into the machine type
 * which will be passed to/from Fortran. For
 * example, SingleInteger is passed as SInt.
 */
FoamTag
gen0FtnMachineType(FortranType ftntype)
{
        switch (ftntype)
        {
                /* FTN_Boolean -> FOAM_SInt really ... */
                case FTN_Character      : return FOAM_Char;
                case FTN_Boolean        : return FOAM_Bool;
                case FTN_SingleInteger  : return FOAM_SInt;
                case FTN_FSingle        : return FOAM_SFlo;
                case FTN_FDouble        : return FOAM_DFlo;
                case FTN_FSComplex      : return FOAM_Word;
                case FTN_FDComplex      : return FOAM_Word;
                case FTN_XLString       : return FOAM_Word;
                case FTN_String         : return FOAM_Word;
                case FTN_StringArray    : return FOAM_Word;
                case FTN_Array          : return FOAM_Word;
                default                 : return FOAM_Word;
        }
}


/*
 * Return a list of all the exports of FortranComplexReal.
 */
List<Syme>
ftnComplexRealExports(void)
{
        /*
         * Make sure that we have the category symbols that we
         * are interested in. Initialise them if we don't.
         */
        ftnTypeInitTForms();


        /* Return the exports of this category */
        return tfGetCatExports(tfFtnSCpx);
}


/*
 * Return a list of all the exports of FortranComplexDouble.
 */
List<Syme>
ftnComplexDoubleExports(void)
{
        /*
         * Make sure that we have the category symbols that we
         * are interested in. Initialise them if we don't.
         */
        ftnTypeInitTForms();


        /* Return the exports of this category */
        return tfGetCatExports(tfFtnDCpx);
}


/*
 * Return a list of all the exports of FortranArray.
 */
List<Syme>
ftnArrayExports(void)
{
        /*
         * Make sure that we have the category symbols that we
         * are interested in. Initialise them if we don't.
         */
        ftnTypeInitTForms();


        /* Return the exports of this category */
        return tfGetCatExports(tfFtnArry);
}


/*
 * Return a list of all the exports of FortranFStringArray.
 */
List<Syme>
ftnFSArrayExports(void)
{
        /*
         * Make sure that we have the category symbols that we
         * are interested in. Initialise them if we don't.
         */
        ftnTypeInitTForms();


        /* Return the exports of this category */
        return tfGetCatExports(tfFtnFSA);
}


