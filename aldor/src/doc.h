/*****************************************************************************
 *
 * doc.h: ++ Documentation handling.
 *
 ****************************************************************************/
#ifndef _DOC_H_
#define _DOC_H_
# include "axlobs.h"
#ifdef __cplusplus
# include <cstddef>
# include <type_traits>
class Doc {
public:
        Doc() = default;
        constexpr Doc(std::nullptr_t) noexcept : rep_(nullptr) {}
        static Doc fromString(String);
        static Doc fromTokens(List<Token>);
        static Doc fromPointer(void * p) noexcept { return Doc(reinterpret_cast<Rep *>(p)); }
        static Doc fromPacked(AInt p) noexcept { return Doc(reinterpret_cast<Rep *>(p)); }
        static const Doc none;
        bool isNull() const noexcept { return rep_ == nullptr; }
        explicit operator bool() const noexcept { return !isNull(); }
        bool done() const noexcept;
        List<MutString> lines() const noexcept;
        String string() const noexcept;
        Length length() const noexcept;
        Hash hash() const noexcept;
        void * asPointer() const noexcept { return reinterpret_cast<void *>(rep_); }
        AInt packed() const noexcept { return reinterpret_cast<AInt>(rep_); }
        Doc copy() const;
        Doc mergedWith(Doc) const;
        bool equals(Doc) const;
        void free();
        int print(FILE *) const;
        static Hash tableHash(Doc);
        static bool tableEqual(Doc, Doc);
        friend bool operator==(Doc a, Doc b) noexcept { return a.rep_ == b.rep_; }
        friend bool operator!=(Doc a, Doc b) noexcept { return !(a == b); }
        friend bool operator==(Doc a, std::nullptr_t) noexcept { return a.isNull(); }
        friend bool operator!=(Doc a, std::nullptr_t) noexcept { return !a.isNull(); }
private:
        struct Rep;
        explicit constexpr Doc(Rep *rep) noexcept : rep_(rep) {}
        static Doc createEmpty(Length);
        static Doc makeNone();
        Rep *rep_;
};
static_assert(sizeof(Doc) == sizeof(void *));
static_assert(std::is_trivially_copyable_v<Doc>);
#else
struct doc { bool hasCorpus; AInt hash; MutStringList lines; MutString corpus; Length cc; };
typedef struct doc *Doc;
#define docDone(doc) ((doc)->hasCorpus)
#define docLines(doc) ((doc)->lines)
#define docString(doc) ((doc)->corpus)
#define docLength(doc) ((doc)->cc)
extern Doc docNone;
extern Doc docNewFrString(MutString);
extern Doc docNewFrList(TokenList);
extern Doc docCopy(Doc);
extern void docFree(Doc);
extern Doc docMerge(Doc,Doc);
extern bool docEqual(Doc,Doc);
extern int docPrint(FILE*,Doc);
extern Hash docHash(Doc);
#endif
#endif
