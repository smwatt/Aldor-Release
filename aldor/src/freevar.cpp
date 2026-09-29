///////////////////////////////////////////////////////////////////////////////
//
// freevar.cpp: Free variable sets for type forms.
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

# include "axlobs.h"

bool    fvDebug = false;
#define fvDEBUG(s) DEBUG_IF(fvDebug, s)

struct FreeVar::Rep {
        List<Syme>      symes;
        bool            skip;
};

FreeVar
FreeVar::fromTheSymes(List<Syme> symes)
{
        Rep *rep = reinterpret_cast<Rep *>(stoAlloc(OB_Other, sizeof(Rep)));
        rep->symes = symes;
        rep->skip  = false;
        return FreeVar(rep);
}

FreeVar
FreeVar::fromSymes(List<Syme> symes)
{
        if (symes == listNil<Syme>)
                return empty();
        if (cdr(symes) == listNil<Syme>) {
                FreeVar fv = singleton(car(symes));
                listFree<Syme>(symes);
                return fv;
        }
        return fromTheSymes(symes);
}

FreeVar
FreeVar::empty()
{
        static FreeVar fv;
        if (fv.isNull())
                fv = fromTheSymes(listNil<Syme>);
        return fv;
}

FreeVar
FreeVar::singleton(Syme syme)
{
        static Table<Syme, Rep *> tbl = nullptr;

        if (tbl == 0)
                tbl = Table<Syme, Rep *>::create();

        Rep *rep = tbl.get(syme, nullptr);
        if (!rep) {
                FreeVar fv = fromTheSymes(listCons<Syme>(syme, listNil<Syme>));
                rep = fv.rep_;
                tbl.set(syme, rep);
        }
        return FreeVar(rep);
}

List<Syme>
FreeVar::symes() const noexcept
{
        return rep_->symes;
}

bool
FreeVar::skipParam() const noexcept
{
        return rep_->skip;
}

void
FreeVar::setSkipParam() noexcept
{
        rep_->skip = true;
}

Length
FreeVar::count() const
{
        return listLength<Syme>(rep_->symes);
}

FreeVar
FreeVar::combinedWith(FreeVar other) const
{
        if (*this == empty()) return other;
        if (other == empty()) return *this;
        return fromTheSymes(symeListUnion(rep_->symes, other.rep_->symes,
                                          symeEq));
}

int
FreeVar::print(FILE *fout) const
{
        return listPrint<Syme>(fout, rep_->symes, symePrint);
}

bool
FreeVar::hasSyme(Syme syme) const
{
        return listMemq<Syme>(rep_->symes, syme);
}

bool
FreeVar::hasAbSub(AbSub sigma) const
{
        return absHasSymes(sigma, rep_->symes);
}
