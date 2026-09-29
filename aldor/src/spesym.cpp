///////////////////////////////////////////////////////////////////////////////
//
// spesym.cpp: Special symbols, used throughout compiler.
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

# include "symbol.h"

/*
 * Symbols for Foam types.
 */
Symbol  ssymArr,
        ssymBInt,
        ssymBool,
        ssymByte,
        ssymChar,
        ssymDFlo,
        ssymHInt,
        ssymNil,
        ssymPtr,
        ssymSFlo,
        ssymSInt;

/*
 * Symbols for Aldor type constructors.
 */
Symbol  ssymBoolean,
        ssymCategory,
        ssymCross,
        ssymDelayed,
        ssymEnum,
        ssymExit,
        ssymGenerator,
        ssymJoin,
        ssymLiteral,
        ssymMap,
        ssymMeet,
        ssymPackedMap,
        ssymPointer,
        ssymRaw,
        ssymRawRecord,
        ssymRecord,
        ssymReference,
        ssymSelf,
        ssymSelfSelf,
        ssymSingleInteger,
        ssymTest,
        ssymTextWriter,
        ssymTrailingArray,
        ssymThird,
        ssymTuple,
        ssymType,
        ssymUnion,
        ssymVariable;

/*
 * Symbols for operation names we care about.
 */
Symbol  ssymArrow,
        ssymApply,
        ssymBrace,
        ssymBracket,
        ssymCoerce,
        ssymEquals,
        ssymNotEquals,
        ssymPackedArrow,
        ssymPrint,
        ssymSetBang,
        ssymTheCase,
        ssymTheDispose,
        ssymTheExplode,
        ssymTheFloat,
        ssymTheInteger,
        ssymTheGenerator,
        ssymThePrint,
        ssymTheRawRecord,
        ssymTheRecord,
        ssymTheString,
        ssymTheTest,
        ssymTheTrailingArray,
        ssymTheUnion;

/*
 * Symbols naming function interfaces.
 */
Symbol  ssymBasic,
        ssymBuiltin,
        ssymForeign,
        ssymC,
        ssymFortran,
        ssymLisp,
        ssymMachine;

/*
 * Symbols of attributes we like to know about. This is
 * mainly used for Fortran-specific types.
 */

Symbol  ssymFtnSInt,
        ssymFtnSFlo,
        ssymFtnDFlo,
        ssymFtnSCpx,
        ssymFtnDCpx,
        ssymFtnBool,
        ssymFtnXStr,
        ssymFtnFStr,
        ssymFtnFSA,
        ssymFtnChar,
        ssymFtnArry;

/*
 * Symbols for implicit category stuff
 */

Symbol ssymImplPAOps;

/*
 * Symbols for Foam arrays and records.
 */
Symbol   ssymArrElt,
         ssymArrNew,
         ssymArrSet,
         ssymArrDispose,
         ssymRawRecSet,
         ssymRawRecElt,
         ssymRawRecNew,
         ssymRawRecDispose,
         ssymRecElt,
         ssymRecNew,
         ssymRecSet,
         ssymRecDispose,
         ssymTRNew,
         ssymTRElt,
         ssymIRElt,
         ssymBIntDispose;


void
ssymInit(void)
{
        static bool isInit = false;

        if (isInit) return;

        /*
         * Symbols for Foam types.
         */
        ssymArr          = Symbol::intern("Arr");
        ssymBInt         = Symbol::intern("BInt");
        ssymBool         = Symbol::intern("Bool");
        ssymByte         = Symbol::intern("XByte");
        ssymChar         = Symbol::intern("Char");
        ssymDFlo         = Symbol::intern("DFlo");
        ssymHInt         = Symbol::intern("HInt");
        ssymNil          = Symbol::intern("Nil");
        ssymPtr          = Symbol::intern("Ptr");
        ssymSFlo         = Symbol::intern("SFlo");
        ssymSInt         = Symbol::intern("SInt");

        /*
         * Symbols for Aldor types we care about.
         */
        ssymBoolean      = Symbol::intern("Boolean");
        ssymCategory     = Symbol::intern("Category");
        ssymCross        = Symbol::intern("Cross");
        ssymDelayed      = Symbol::intern("Delayed");
        ssymEnum         = Symbol::intern("Enumeration");
        ssymExit         = Symbol::intern("Exit");
        ssymGenerator    = Symbol::intern("Generator");
        ssymJoin         = Symbol::intern("Join");
        ssymLiteral      = Symbol::intern("Literal");
        ssymMap          = Symbol::intern("Map");
        ssymMeet         = Symbol::intern("Meet");
        ssymPackedMap    = Symbol::intern("PackedMap");
        ssymPointer      = Symbol::intern("Pointer");
        ssymRaw          = Symbol::intern("Raw");
        ssymRawRecord    = Symbol::intern("RawRecord");
        ssymRecord       = Symbol::intern("Record");
        ssymReference    = Symbol::intern("Ref");
        ssymSelf         = Symbol::intern("%");
        ssymSelfSelf     = Symbol::intern("%%");
        ssymSingleInteger= Symbol::intern("SingleInteger");
        ssymTest         = Symbol::intern("Test");
        ssymTextWriter   = Symbol::intern("TextWriter");
        ssymTrailingArray= Symbol::intern("TrailingArray");
        ssymThird        = Symbol::intern("Third");
        ssymTuple        = Symbol::intern("Tuple");
        ssymType         = Symbol::intern("Type");
        ssymUnion        = Symbol::intern("Union");
        ssymVariable     = Symbol::intern("?");

        /*
         * Symbols for operation names we care about.
         */
        ssymArrow        = Symbol::intern("->");
        ssymApply        = Symbol::intern("apply");
        ssymBrace        = Symbol::intern("brace");
        ssymBracket      = Symbol::intern("bracket");
        ssymCoerce       = Symbol::intern("coerce");
        ssymEquals       = Symbol::intern("=");
        ssymNotEquals    = Symbol::intern("~=");
        ssymPackedArrow  = Symbol::intern("->*");
        ssymPrint        = Symbol::intern("<<");
        ssymSetBang      = Symbol::intern("set!");
        ssymTheCase      = Symbol::intern("case");
        ssymTheDispose   = Symbol::intern("dispose!");
        ssymTheExplode   = Symbol::intern("explode");
        ssymTheFloat     = Symbol::intern("float");
        ssymTheInteger   = Symbol::intern("integer");
        ssymTheGenerator = Symbol::intern("generator");
        ssymThePrint     = Symbol::intern("print");
        ssymTheRawRecord = Symbol::intern("rawrecord");
        ssymTheRecord    = Symbol::intern("record");
        ssymTheString    = Symbol::intern("string");
        ssymTheTest      = Symbol::intern("test");
        ssymTheTrailingArray = Symbol::intern("trailing");
        ssymTheUnion     = Symbol::intern("union");

        /*
         * Symbols naming function interfaces.
         */
        ssymBasic        = Symbol::intern("Basic");
        ssymBuiltin      = Symbol::intern("Builtin");
        ssymForeign      = Symbol::intern("Foreign");
        ssymC            = Symbol::intern("C");
        ssymFortran      = Symbol::intern("Fortran");
        ssymLisp         = Symbol::intern("Lisp");
        ssymMachine      = Symbol::intern("Machine");

        /*
         * Symbols of attributes we like to know about. This is
         * mainly used for Fortran-specific types.
         */
        ssymFtnSInt      = Symbol::intern("FortranInteger");
        ssymFtnSFlo      = Symbol::intern("FortranReal");
        ssymFtnDFlo      = Symbol::intern("FortranDouble");
        ssymFtnSCpx      = Symbol::intern("FortranComplexReal");
        ssymFtnDCpx      = Symbol::intern("FortranComplexDouble");
        ssymFtnBool      = Symbol::intern("FortranLogical");
        ssymFtnXStr      = Symbol::intern("FortranString");
        ssymFtnFSA       = Symbol::intern("FortranFStringArray");
        ssymFtnFStr      = Symbol::intern("FortranFString");
        ssymFtnChar      = Symbol::intern("FortranCharacter");
        ssymFtnArry      = Symbol::intern("FortranArray");

        /*
         * Symbols for implicit category stuff
         */
        ssymImplPAOps   = Symbol::intern("DenseStorageCategory");

        /*
         * Symbols for Foam arrays and records.
         */
        ssymArrNew       = Symbol::intern("ArrNew");
        ssymArrElt       = Symbol::intern("ArrElt");
        ssymArrSet       = Symbol::intern("ArrSet");
        ssymArrDispose   = Symbol::intern("ArrDispose");
        ssymRawRecNew    = Symbol::intern("RawRecNew");
        ssymRawRecSet    = Symbol::intern("RawRecSet");
        ssymRawRecElt    = Symbol::intern("RawRecElt");
        ssymRecNew       = Symbol::intern("RecNew");
        ssymRecElt       = Symbol::intern("RecElt");
        ssymRecSet       = Symbol::intern("RecSet");
        ssymTRNew        = Symbol::intern("TRNew");
        ssymTRElt        = Symbol::intern("TRElt");
        ssymIRElt        = Symbol::intern("IRElt");
        ssymRawRecDispose= Symbol::intern("RawRecDispose");
        ssymRecDispose   = Symbol::intern("RecDispose");
        ssymBIntDispose  = Symbol::intern("BIntDispose");

        isInit           = true;
}
