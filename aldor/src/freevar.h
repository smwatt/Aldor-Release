/*****************************************************************************
 *
 * freevar.h: Free variable sets for type forms.
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

#ifndef _FREEVAR_H_
#define _FREEVAR_H_

#include <type_traits>

class FreeVar {
public:
        FreeVar() = default;

        static FreeVar fromSymes(List<Syme>);
        static FreeVar empty();
        static FreeVar singleton(Syme);

        bool            isNull() const noexcept { return rep_ == nullptr; }
        explicit        operator bool() const noexcept { return !isNull(); }

        List<Syme>      symes() const noexcept;
        bool            skipParam() const noexcept;
        void            setSkipParam() noexcept;
        Length          count() const;

        FreeVar         combinedWith(FreeVar) const;
        bool            hasSyme(Syme) const;
        bool            hasAbSub(AbSub) const;
        int             print(FILE *) const;

        friend bool operator==(FreeVar a, FreeVar b) noexcept
                { return a.rep_ == b.rep_; }
        friend bool operator!=(FreeVar a, FreeVar b) noexcept
                { return !(a == b); }

private:
        struct Rep;
        explicit FreeVar(Rep *rep) noexcept : rep_(rep) {}
        static FreeVar fromTheSymes(List<Syme>);
        Rep *rep_;
};

static_assert(sizeof(FreeVar) == sizeof(void *));
static_assert(std::is_trivially_copyable_v<FreeVar>);

#endif /* !_FREEVAR_H_ */
