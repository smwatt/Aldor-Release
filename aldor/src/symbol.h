/*****************************************************************************
 *
 * symbol.h: Pooled symbols.
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

/*
 * Symbols compare by interned identity.  Asking for the string does not
 * allocate storage, so the returned string must not be modified.
 */

#ifndef _SYMBOL_H_
#define _SYMBOL_H_

# include "axlport.h"

# define        SYM_LOOK        0
# define        SYM_ALLOC       1       /* Alloc if not there. Else 0. */
# define        SYM_STRCOPY     2       /* Copy string. Else use original. */

#ifdef __cplusplus

# include <cstddef>
# include <type_traits>

class Symbol {
public:
        Symbol() = default;
        constexpr Symbol(std::nullptr_t) noexcept : rep_(nullptr) {}

        static Symbol generate();
        static Symbol fromPointer(void *p) noexcept
                { return Symbol(reinterpret_cast<Rep *>(p)); }
        void *asPointer() const noexcept { return rep_; }
        static Symbol probe(String, int options);
        static Symbol intern(String s)
                { return probe(s, SYM_ALLOC | SYM_STRCOPY); }
        static Symbol internConst(String s)
                { return probe(s, SYM_ALLOC); }
        static void clear();
        static void map(void (*)(Symbol));

        bool isNull() const noexcept { return rep_ == nullptr; }
        explicit operator bool() const noexcept { return !isNull(); }

        Hash hash() const noexcept;
        String string() const noexcept;
        MostAlignedType *info() const noexcept;
        void setInfo(MostAlignedType *) noexcept;
        int print(FILE *) const;

        friend bool operator==(Symbol a, Symbol b) noexcept
                { return a.rep_ == b.rep_; }
        friend bool operator!=(Symbol a, Symbol b) noexcept
                { return !(a == b); }
        friend bool operator==(Symbol a, std::nullptr_t) noexcept
                { return a.isNull(); }
        friend bool operator!=(Symbol a, std::nullptr_t) noexcept
                { return !a.isNull(); }

private:
        struct Rep;
        explicit constexpr Symbol(Rep *rep) noexcept : rep_(rep) {}
        static void freeStorage(Symbol);
        Rep *rep_;
};

static_assert(sizeof(Symbol) == sizeof(void *));
static_assert(std::is_trivially_copyable_v<Symbol>);

/* Typed callback adapter for the few generic print APIs. */
inline int symPrint(FILE *fout, Symbol sym) { return sym.print(fout); }

#else /* !__cplusplus */

struct symbol {
        MostAlignedType *info;
        String str;
};

typedef struct symbol *Symbol;

# define        symIntern(str)       symProbe(str, SYM_ALLOC | SYM_STRCOPY)
# define        symInternConst(str)  symProbe(str, SYM_ALLOC)
# define        symHash(sym)         ((Hash) ptrCanon(sym))
# define        symString(sym)       ((sym)->str)
# define        symInfo(sym)         ((sym)->info)

extern Symbol   symGen(void);
extern Symbol   symProbe(String, int options);
extern void     symClear(void);
extern int      symPrint(FILE *, Symbol);
extern void     symMap(void (*symfun)(Symbol));

#endif /* __cplusplus */

#endif /* !_SYMBOL_H_ */
