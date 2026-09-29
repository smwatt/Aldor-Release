/*****************************************************************************
 *
 * priq.h: Priority Queue data structure.
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

#ifndef _PRIQ_H
#define _PRIQ_H

# include "axlport.h"

#ifdef __cplusplus

# include <type_traits>

class PriQ {
public:
        using Key            = double;
        using Element        = void *;
        using ElementFreeFn  = void (*)(Element);
        using MapFn          = void (*)(Key, Element);

        PriQ() = default;

        static PriQ create(Length argcGuess);

        bool    isNull() const noexcept { return rep_ == nullptr; }
        Length  count() const noexcept;

        void    free();
        void    freeDeeply(ElementFreeFn);
        void    insert(Key, Element);
        Element extractMin(Key * = nullptr);
        Element peekMin(Key * = nullptr) const;
        int     check() const;
        int     print(FILE *) const;
        void    map(MapFn) const;

        static constexpr Key minKey = 0.0;

        friend bool operator==(PriQ a, PriQ b) noexcept
                { return a.rep_ == b.rep_; }
        friend bool operator!=(PriQ a, PriQ b) noexcept
                { return !(a == b); }

private:
        struct Rep;
        explicit PriQ(Rep *rep) noexcept : rep_(rep) {}
        Rep *rep_;
};

static_assert(sizeof(PriQ) == sizeof(void *));
static_assert(std::is_trivially_copyable_v<PriQ>);

#else

/* C sources include axlgen.h, which in turn includes this header.  There is
 * no C priority-queue client in the current build; keep only the opaque type
 * so those headers remain C-compatible. */
typedef struct priq *PriQ;

#endif

#endif /* _PRIQ_H_ */
