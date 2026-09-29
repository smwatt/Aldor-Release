///////////////////////////////////////////////////////////////////////////////
//
// symbol.cpp: Pooled symbols.
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

# include "axlgen.h"

struct Symbol::Rep {
        MostAlignedType *info;
        String str;
};

static Table<String, Symbol> symbolPool = nullptr;
static long genSymCount = 0;

void
Symbol::freeStorage(Symbol sym)
{
        stoFree((void *) sym.rep_);
}

void
Symbol::clear()
{
        if (symbolPool)
                symbolPool.freeDeeply<nullptr, freeStorage>();
        symbolPool = nullptr;
}

Symbol
Symbol::generate()
{
        static char gs[40];
        sprintf(gs, "GenSym #%ld", (long) ++genSymCount);
        return intern(gs);
}

Symbol
Symbol::probe(String str, int options)
{
        if (!symbolPool)
                symbolPool = Table<String, Symbol>::create<strHash, strEqual>();

        Symbol sym = symbolPool.get(str, nullptr);
        if (sym || !(options & SYM_ALLOC)) return sym;

        Rep *rep = (Rep *) stoAlloc((unsigned) OB_Symbol, sizeof(*rep));
        rep->info = nullptr;
        rep->str = (options & SYM_STRCOPY) ? strCopy(str) : str;
        sym = Symbol(rep);

        symbolPool.set(sym.string(), sym);
        return sym;
}

Hash
Symbol::hash() const noexcept
{
        return (Hash) reinterpret_cast<UAInt>(rep_);
}

String
Symbol::string() const noexcept
{
        return rep_->str;
}

MostAlignedType *
Symbol::info() const noexcept
{
        return rep_->info;
}

void
Symbol::setInfo(MostAlignedType *info) noexcept
{
        rep_->info = info;
}

int
Symbol::print(FILE *fout) const
{
        return fprintf(fout, "%s", *this ? string() : "<NULL>");
}

void
Symbol::map(void (*symfun)(Symbol))
{
        for (Table<String, Symbol>::Iterator it(symbolPool);
             it.more(); it.step())
                symfun(it.value());
}
