///////////////////////////////////////////////////////////////////////////////
//
// srcline.cpp: SrcLine data structure.
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

struct SrcLine::Rep {
        SrcPos          spos;
        unsigned short  indentation;
        unsigned int    isSysCmd:       1;
        unsigned int    sysCmdHandled:  1;
        unsigned int    isEndifLine:    1;
        MutString       text;
};

const SrcLine SrcLine::nil{nullptr};

SrcLine
SrcLine::create(SrcPos spos, int indent, MutString text)
{
        Rep *rep = (Rep *) stoAlloc((unsigned) OB_SrcLine, sizeof(*rep));
        rep->spos          = spos;
        rep->indentation   = indent;
        rep->isSysCmd      = false;
        rep->sysCmdHandled = false;
        rep->isEndifLine   = false;
        rep->text          = strCopy(text);
        return SrcLine(rep);
}

SrcLine
SrcLine::createSysCmd(SrcPos spos, int indent, MutString text, bool isHandled)
{
        SrcLine sl = create(spos, indent, text);
        sl.rep_->isSysCmd = true;
        sl.rep_->sysCmdHandled = isHandled;
        return sl;
}

SrcPos SrcLine::position() const noexcept { return rep_->spos; }
int SrcLine::indentation() const noexcept { return rep_->indentation; }
bool SrcLine::isSysCmd() const noexcept { return rep_->isSysCmd; }
bool SrcLine::sysCmdHandled() const noexcept { return rep_->sysCmdHandled; }
bool SrcLine::isEndifLine() const noexcept { return rep_->isEndifLine; }
MutString SrcLine::text() const noexcept { return rep_->text; }
void SrcLine::setSysCmdHandled(bool value) noexcept { rep_->sysCmdHandled = value; }
void SrcLine::setEndifLine(bool value) noexcept { rep_->isEndifLine = value; }

void
SrcLine::free()
{
        strFree(rep_->text);
        stoFree((void *) rep_);
}

int
SrcLine::print(FILE *fout) const
{
        int cc;
        cc  = fprintf(fout, "[%d] %s, line %d: ",
#if EDIT_1_0_n1_07
                      (int) position().globalLine(),
                      position().file().unparseStatic(),
                      (int) position().line());
#else
                      position().globalLine(),
                      position().file().unparseStatic(),
                      position().line());
#endif
        cc += fprintf(fout, "%*s", indentation(), "");
        cc += fprintf(fout, "%s", text());
        return cc;
}

int
sllPrint(FILE *fout, List<SrcLine> sll)
{
        return listPrint<SrcLine>(
                fout, sll,
                [](FILE *f, SrcLine sl) -> int { return sl.print(f); });
}
