///////////////////////////////////////////////////////////////////////////////
//
// simpl.cpp: Symbol Implementations.
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


#include "axlobs.h"
#include "simpl.h"

bool implDebug = false;

#define implDEBUG(s)            DEBUG_IF(implDebug, s)

struct SImplInfo {
        SImplTag tag;
        String name;
};

#define implName(x)     (implInfo[implTag(x)].name)
extern struct SImplInfo implInfo[];

local SImpl implNew             (SImplTag, Length);
local bool  implSetConstNum0    (SImpl, int, int);
local int   implFindDefIdx0(SImpl impl, int constId);

local SImpl
implNew(SImplTag tag, Length sz)
{
        SImpl impl;

        impl = (SImpl) stoAlloc(OB_Other, fullsizeof(*impl, sz, AInt));
        impl->implGen.hdr.tag   = tag;
        impl->implGen.hdr.flags = 0;

        return impl;
}


SImpl
implNone()
{
        SImpl impl = implNew(SIMPL_None, int0);

        return impl;
}

SImpl
implNewLocal(bool isdef, int idx)
{
        SImpl impl = implNew(SIMPL_Local, 3);
        impl->implLocal.defineIdx = idx;
        impl->implLocal.constNum  = -1;
        impl->implLocal.constLib  = NULL;

        if (isdef)
                implSetDefault(impl);

        return impl;
}

SImpl
implNewImport(bool isdef, Lib lib, int idx)
{
        SImpl impl = implNew(SIMPL_Local, 3);
        impl->implLocal.defineIdx = -1;
        impl->implLocal.constNum  = idx;
        impl->implLocal.constLib  = lib;

        if (isdef)
                implSetDefault(impl);

        return impl;
}

SImpl
implNewInherit(TForm base)
{
        SImpl impl;

        impl = implNew(SIMPL_Inherit, 1);
        impl->implInherit.base = base;

        return impl;
}

SImpl
implNewBranch(AbLogic cond, SImpl implTrue, SImpl implFalse)
{
        SImpl impl = implNew(SIMPL_Branch, 3);
        impl->implBranch.cond      = cond;
        impl->implBranch.implTrue  = implTrue;
        impl->implBranch.implFalse = implFalse;

        return impl;
}

SImpl
implNewCond(AbLogic cond, SImpl implTrue)
{
        SImpl impl = implNew(SIMPL_Cond, 2);
        impl->implCond.cond = cond;
        impl->implCond.impl = implTrue;

        return impl;
}

SImpl
implNewDefault(Syme syme)
{
        SImpl impl;

        impl = implNew(SIMPL_Default, 1);
        impl->implDefault.def = syme;

        return impl;
}

void
implSetConstNum(SImpl impl, int defId, int constId)
{
        bool ret;

        ret = implSetConstNum0(impl, defId, constId);

        if (!ret)
                bug("Could not find impl");
}

local bool
implSetConstNum0(SImpl impl, int defId, int constId)
{
        bool ret = false;
        switch (implTag(impl)) {
          case SIMPL_Local:
                if (defId == -1 ||
                    impl->implLocal.defineIdx == defId) {
                        impl->implLocal.constNum = constId;
                        ret = true;
                }
                break;
          case SIMPL_Branch:
                ret = implSetConstNum0(impl->implBranch.implTrue,
                                      defId, constId);
                if (!ret)
                        ret = implSetConstNum0(impl->implBranch.implFalse,
                                              defId, constId);
                break;
          case SIMPL_Cond:
                ret = implSetConstNum0(impl->implCond.impl, defId, constId);
                break;
          case SIMPL_Default:
                break;
          case SIMPL_Inherit:
                break;
          default:
                bug("strange SImpl");
                break;
        }
        return ret;
}

int
implFindDefIdx(SImpl impl, int constId)
{
        int ret;

        ret = implFindDefIdx0(impl, constId);

        if (ret == -1)
                bug("Could not find impl");

        return ret;
}

local int
implFindDefIdx0(SImpl impl, int constId)
{
        int ret = -1;
        switch (implTag(impl)) {
          case SIMPL_Local:
                    if (impl->implLocal.constNum == constId)
                        ret = impl->implLocal.defineIdx;
                break;
          case SIMPL_Branch:
                ret = implFindDefIdx0(impl->implBranch.implTrue, constId);
                if (!ret)
                        ret = implFindDefIdx0(impl->implBranch.implFalse,
                                              constId);
                break;
          case SIMPL_Cond:
                ret = implFindDefIdx0(impl->implCond.impl, constId);
                break;
          case SIMPL_Default:
                break;
          case SIMPL_Inherit:
                break;
          default:
                bug("strange SImpl");
                break;
        }
        return ret;
}

SImpl
implEvaluate(SImpl impl, AbLogic cond)
{

        SImpl newImpl = NULL;

        if (ablogIsTrue(cond))
                return impl;

        if (!impl)
                return impl;

        implDEBUG(printf("(ImplEvaluate:\n");
                  implPrintDb(impl);
                  ablogPrintDb(cond);
                  );

        switch (implTag(impl)) {
          case SIMPL_Local:
          case SIMPL_Default:
          case SIMPL_None:
          case SIMPL_Inherit:
          case SIMPL_Import:
                newImpl = impl;
                break;
          case SIMPL_Branch:
                if (ablogImplies(cond, impl->implBranch.cond))
                        newImpl = implEvaluate(impl->implBranch.implTrue, cond);
                else if (ablogImplies(cond, ablogNot(impl->implBranch.cond)))
                        newImpl = implEvaluate(impl->implBranch.implFalse, cond);
                else
                        newImpl = impl;
                break;
          case SIMPL_Cond:
                if (ablogImplies(cond, impl->implCond.cond))
                        newImpl = implEvaluate(impl->implCond.impl, cond);
                else
                        newImpl = implNone();
                break;
          default:
                bug("implEvaluate: not good");
                return impl;
        }
        implDEBUG(
                  implPrintDb(newImpl);
                  printf(")\n");
                  );
        return newImpl;
}

void
implFree(SImpl impl)
{
        /* FIXME: Should either copy or reference count */
        /*stoFree(impl); */
}

int
implPrintDb(SImpl impl)
{
        int cc = implPrint(dbOut, impl);
        fnewline(dbOut);

        return cc+1;
}

int
implPrint(FILE *file, SImpl impl)
{
        int cc = 0;

        if (impl == NULL)
                return fprintf(file, "(Impl: <null>)");
        cc += fprintf(file, "(Impl %s: ", implName(impl));
        switch (implTag(impl)) {
          case SIMPL_None:
          case SIMPL_Unknown:
                break;
          case SIMPL_Inherit:
                cc += tfPrint(file, impl->implInherit.base);
                break;
          case SIMPL_Default:
                cc += symePrint(file, impl->implDefault.def);
                break;
          case SIMPL_Cond:
                cc += ablogPrint(file, impl->implBranch.cond);
                cc += implPrint(file, impl->implBranch.implTrue);
                break;
          case SIMPL_Branch:
                cc += ablogPrint(file, impl->implBranch.cond);
                cc += implPrint(file, impl->implBranch.implTrue);
                cc += implPrint(file, impl->implBranch.implFalse);
                break;
          case SIMPL_Local:
                cc += fprintf(file, "%d (%s.%d)",
                              (int)impl->implLocal.defineIdx,
                              impl->implLocal.constLib
                                ? libGetFileId(impl->implLocal.constLib)
                                : "Local",
                              (int)impl->implLocal.constNum);

          case SIMPL_Import:
                break;
          default:
                printf("Aaarghh: %d", implTag(impl));
                break;
        }
        cc += printf(")");
        return cc;
}

struct SImplInfo implInfo[] = {
        { SIMPL_None,      "None" },
        { SIMPL_Unknown,   "Unknown" },
        { SIMPL_Inherit,   "Inherit" },
        { SIMPL_Cond,      "Condition" },
        { SIMPL_Branch,    "Branch" },
        { SIMPL_Default,   "Default" },
        { SIMPL_Local,     "Local" },
        { SIMPL_Import,    "Import"}
        };
