/*****************************************************************************
 *
 * dnf.h: Disjunctive normal form for boolean expressions.
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

#ifndef _DNF_H_
#define _DNF_H_

#include "axlport.h"

#include <cstddef>
#include <type_traits>

/*
 * The literals are numbers:
 * A positive number means the variable is taken to be true.
 * A negative number means the variable is taken to be false.
 *
 * false         => []          -- 0 term "or"  = "or"'s  identity, "false"
 * true          => [[]]        -- 0 term "and" = "and"'s identity, "true"
 * X1            => [[1]]
 * X1/\~X2 \/ X3 => [[1,-2], [3]]
 *
 * DNF is a non-owning, one-word handle.  Storage remains explicitly managed
 * with copy() and free(), as it was for the historical pointer typedef.
 */
using DNF_Atom = int;

struct DNF_Rep;

class DNF {
public:
        DNF() = default;
        constexpr DNF(std::nullptr_t) noexcept : rep_(nullptr) {}

        static DNF trueForm() noexcept;
        static DNF falseForm() noexcept;
        static DNF atom(DNF_Atom);
        static DNF notAtom(DNF_Atom atom) { return DNF::atom(-atom); }

        static DNF fromPointer(void *p) noexcept
                { return DNF(reinterpret_cast<DNF_Rep *>(p)); }
        void *asPointer() const noexcept { return rep_; }

        bool isNull() const noexcept { return rep_ == nullptr; }
        explicit operator bool() const noexcept { return !isNull(); }

        bool isTrue() const noexcept;
        bool isFalse() const noexcept;

        DNF negate() const;
        DNF conjoin(DNF) const;
        DNF disjoin(DNF) const;

        DNF copy() const;
        void free() const;
        int print(FILE *) const;

        bool equals(DNF) const;
        bool implies(DNF) const;

        DNF follow() const noexcept;
        void alias(DNF newValue) const;

        void map(bool (*mapFn)(void *, DNF_Atom), void *clos) const;
        bool expandImplies(bool (*testFn)(void *, DNF_Atom, DNF_Atom),
                           void *clos, DNF yy) const;

        /* Read-only structural access for AbLogic's presentation layer. */
        int termCount() const noexcept;
        int atomCount(int term) const noexcept;
        DNF_Atom atomAt(int term, int atom) const noexcept;

        friend bool operator==(DNF a, DNF b) noexcept
                { return a.rep_ == b.rep_; }
        friend bool operator!=(DNF a, DNF b) noexcept
                { return !(a == b); }
        friend bool operator==(DNF a, std::nullptr_t) noexcept
                { return a.isNull(); }
        friend bool operator!=(DNF a, std::nullptr_t) noexcept
                { return !a.isNull(); }

private:
        explicit constexpr DNF(DNF_Rep *rep) noexcept : rep_(rep) {}
        DNF_Rep *rep_;

        friend struct DNF_Access;
};

static_assert(sizeof(DNF) == sizeof(void *));
static_assert(std::is_trivially_copyable_v<DNF>);

#endif /* !_DNF_H_ */
