/*****************************************************************************
 *
 * srcline.h: Structure for source lines.
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

#ifndef _SRCLINE_H_
#define _SRCLINE_H_

# include "axlobs.h"

#ifdef __cplusplus
# include <cstddef>
# include <type_traits>

class SrcLine {
public:
        SrcLine() = default;
        constexpr SrcLine(std::nullptr_t) noexcept : rep_(nullptr) {}

        static SrcLine create(SrcPos, int indent, MutString text);
        static SrcLine createSysCmd(SrcPos, int indent, MutString text,
                                    bool isHandled);
        static SrcLine fromPointer(void * p) noexcept
                { return SrcLine(reinterpret_cast<Rep *>(p)); }
        static const SrcLine nil;

        bool      isNull() const noexcept { return rep_ == nullptr; }
        explicit  operator bool() const noexcept { return !isNull(); }

        SrcPos    position() const noexcept;
        int       indentation() const noexcept;
        bool      isSysCmd() const noexcept;
        bool      sysCmdHandled() const noexcept;
        bool      isEndifLine() const noexcept;
        MutString text() const noexcept;

        void      setSysCmdHandled(bool) noexcept;
        void      setEndifLine(bool value = true) noexcept;

        void      free();
        int       print(FILE *) const;

        friend bool operator==(SrcLine a, SrcLine b) noexcept
                { return a.rep_ == b.rep_; }
        friend bool operator!=(SrcLine a, SrcLine b) noexcept
                { return !(a == b); }
        friend bool operator==(SrcLine a, std::nullptr_t) noexcept
                { return a.isNull(); }
        friend bool operator!=(SrcLine a, std::nullptr_t) noexcept
                { return !a.isNull(); }

private:
        struct Rep;
        explicit constexpr SrcLine(Rep *rep) noexcept : rep_(rep) {}
        Rep *rep_;
};

static_assert(sizeof(SrcLine) == sizeof(void *));
static_assert(std::is_trivially_copyable_v<SrcLine>);

extern int sllPrint(FILE *, List<SrcLine>);

#else
struct srcLine {
        SrcPos spos;
        unsigned short indentation;
        unsigned int isSysCmd:1, sysCmdHandled:1, isEndifLine:1;
        MutString text;
};
typedef struct srcLine *SrcLine;
extern SrcLine slineNew(SrcPos,int,MutString);
extern SrcLine slineNewSysCmd(SrcPos,int,MutString,bool);
extern void slineFree(SrcLine);
extern int slinePrint(FILE *, SrcLine);
extern int sllPrint(FILE *, SrcLineList);
#endif

#endif  /* !_SRCLINE_H_ */
