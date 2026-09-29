/*****************************************************************************
 *
 * srcpos.h: Source position operations
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

#ifndef _SRCPOS_H_
#define _SRCPOS_H_

# include "axlport.h"
# include "fname.h"
# include "buffer.h"

#ifdef __cplusplus
# include <type_traits>

struct SrcPosRawAccess;

class SrcPos {
public:
        SrcPos() = default;

        static const SrcPos none;
        static SrcPos top() noexcept;
        static SrcPos end() noexcept;

        static void   tableToBuffer(Buffer);
        static void   tableFromBuffer(Buffer);
        static void   show();
        static void   init();
        static void   fini();
        static SrcPos create(FileName, Length fileLine, Length globalLine,
                             Length character);
        static SrcPos fromGlobal(Length globalLine, Length character) noexcept;
        static Length globalLine(FileName, Length fileLine);
        static void   registerFileLine(FileName, Length fileLine,
                                       Length globalLine);

        static SrcPos fromPacked(ULong bits) noexcept;
        ULong         packed() const noexcept;

        Length   globalLine() const noexcept;
        FileName file() const;
        Length   line() const;
        Length   character() const noexcept;

        SrcPos   offset(int) const noexcept;
        bool     equals(SrcPos) const noexcept;
        SrcPos   min(SrcPos) const noexcept;
        SrcPos   max(SrcPos) const noexcept;
        int      compare(SrcPos) const noexcept;
        bool     isNone() const noexcept { return equals(none); }
        bool     isSpecial() const noexcept;

        int      lineText(Buffer) const;
        int      print(FILE *) const;

        bool     isMacroExpanded() const noexcept;
        SrcPos   macroExpanded() const noexcept;

        friend bool operator==(SrcPos a, SrcPos b) noexcept
                { return a.equals(b); }
        friend bool operator!=(SrcPos a, SrcPos b) noexcept
                { return !a.equals(b); }

private:
        struct RawTag {};
        explicit constexpr SrcPos(ULong bits, RawTag) noexcept : bits_(bits) {}
        ULong bits_;
        friend struct SrcPosRawAccess;
};

static_assert(sizeof(SrcPos) == sizeof(ULong));
static_assert(std::is_trivially_copyable_v<SrcPos>);

#else

typedef ULong SrcPos;

#endif

typedef struct sposCell *SrcPosCell;
typedef union sposStack SrcPosStack;

union sposStack {
        SrcPos          spos;
        SrcPosCell      stack;
};

struct sposCell {
        SrcPos          spos;
        SrcPosStack     rest;
};

extern SrcPosStack      spstackEmpty;
extern SrcPosStack      spstackPush             (SrcPos, SrcPosStack);
extern SrcPosStack      spstackCopy             (SrcPosStack);
extern void             spstackFree             (SrcPosStack);
extern SrcPos           spstackFirst            (SrcPosStack);
extern SrcPosStack      spstackRest             (SrcPosStack);
extern SrcPosStack      spstackSetFirst         (SrcPosStack, SrcPos);
extern SrcPosStack      spstackSetSecond        (SrcPosStack, SrcPos);
extern void             spstackPrintDb          (SrcPosStack);

#endif /* !_SRCPOS_H_ */
