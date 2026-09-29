/*****************************************************************************
 *
 * depdag.h: Dependency DAGs
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

#ifndef _DEPDAG_H_
#define _DEPDAG_H_

# include "axlobs.h"

#ifdef __cplusplus

# include <type_traits>

class DepDag {
public:
        DepDag() = default;

        static DepDag leaf(void *label);
        static DepDag fromPointer(void * p) noexcept
                { return DepDag(reinterpret_cast<Rep *>(p)); }

        bool            isNull() const noexcept { return rep_ == nullptr; }
        explicit        operator bool() const noexcept { return !isNull(); }
        void *         asPointer() const noexcept
                { return reinterpret_cast<void *>(rep_); }

        void *          label() const noexcept;
        List<DepDag>    dependencies() const noexcept;
        bool            isPending() const noexcept;
        bool            isFinished() const noexcept;
        bool            isNode() const noexcept;
        bool            isLeaf() const noexcept { return !isNode(); }

        void            setLabel(void *) noexcept;
        void            setDependencies(List<DepDag>) noexcept;
        void            setPending(bool) noexcept;
        void            setFinished(bool) noexcept;

        DepDag          addDependency(DepDag);
        void            free();
        void            freeDeeply();

        friend bool operator==(DepDag a, DepDag b) noexcept
                { return a.rep_ == b.rep_; }
        friend bool operator!=(DepDag a, DepDag b) noexcept
                { return !(a == b); }

private:
        struct Rep;
        explicit DepDag(Rep *rep) noexcept : rep_(rep) {}
        Rep *rep_;
};

static_assert(sizeof(DepDag) == sizeof(void *));
static_assert(std::is_trivially_copyable_v<DepDag>);

#else

/* C compatibility for headers which only need the historical opaque type. */
typedef struct depDag *DepDag;

#endif

#endif /* !_DEPDAG_H_ */
