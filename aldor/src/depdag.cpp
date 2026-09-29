///////////////////////////////////////////////////////////////////////////////
//
// depdag.cpp: Dependency DAGs
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

# include "depdag.h"

struct DepDag::Rep {
        void            *label;
        List<DepDag>    dependsOn;
        bool            pending;
        bool            finished;
};

DepDag
DepDag::leaf(void *label)
{
        Rep *rep = reinterpret_cast<Rep *>(stoAlloc(OB_Other, sizeof(Rep)));
        rep->label     = label;
        rep->dependsOn = listNil<DepDag>;
        rep->pending   = false;
        rep->finished  = false;
        return DepDag(rep);
}

void *
DepDag::label() const noexcept
{
        return rep_->label;
}

List<DepDag>
DepDag::dependencies() const noexcept
{
        return rep_->dependsOn;
}

bool
DepDag::isPending() const noexcept
{
        return rep_->pending;
}

bool
DepDag::isFinished() const noexcept
{
        return rep_->finished;
}

bool
DepDag::isNode() const noexcept
{
        return rep_->dependsOn != listNil<DepDag>;
}

void
DepDag::setLabel(void *label) noexcept
{
        rep_->label = label;
}

void
DepDag::setDependencies(List<DepDag> dependencies) noexcept
{
        rep_->dependsOn = dependencies;
}

void
DepDag::setPending(bool pending) noexcept
{
        rep_->pending = pending;
}

void
DepDag::setFinished(bool finished) noexcept
{
        rep_->finished = finished;
}

DepDag
DepDag::addDependency(DepDag dependency)
{
        List<DepDag> dags = dependencies();
        if (!listMemq<DepDag>(dags, dependency))
                setDependencies(listCons<DepDag>(dependency, dags));
        return *this;
}

void
DepDag::free()
{
        stoFree((void *) rep_);
        rep_ = nullptr;
}

void
DepDag::freeDeeply()
{
        List<DepDag> dags = dependencies();
        listIter(DepDag, d, dags, d.freeDeeply());
        free();
}
