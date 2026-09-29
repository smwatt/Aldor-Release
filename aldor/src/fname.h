#ifndef _FNAME_H_
#define _FNAME_H_
# include "axlport.h"
#ifdef __cplusplus
# include <cstddef>
# include <type_traits>

class FileName {
public:
        FileName() = default;
        constexpr FileName(std::nullptr_t) noexcept : rep_(nullptr) {}

        static FileName create(String dir, String name, String type);
        static FileName standardInput();
        static FileName standardOutput();
        static FileName parse(String);
        static FileName parseStatic(String);
        static FileName parseStaticWithin(String, String dir);
        static FileName temp(String, String, String);
        static FileName *tempVector(String, String, String *);

        bool      isNull() const noexcept { return rep_ == nullptr; }
        explicit  operator bool() const noexcept { return !isNull(); }
        bool      isStdin() const;
        bool      isStdout() const;
        bool      equals(FileName) const;
        bool      hasDir() const;
        bool      hasType() const;

        MutString dir() const noexcept;
        MutString name() const noexcept;
        MutString type() const noexcept;
        void      setDir(String);
        void      setName(String);
        void      setType(String);

        FileName  copy() const;
        FileName  withType(String) const;
        void      free();

        MutString unparse() const;
        MutString unparseStatic() const;
        MutString unparseStaticWith() const;
        MutString unparseStaticWithout() const;

        friend bool operator==(FileName, FileName);
        friend bool operator!=(FileName a, FileName b) { return !a.equals(b); }
        friend bool operator==(FileName a, std::nullptr_t) noexcept { return a.isNull(); }
        friend bool operator!=(FileName a, std::nullptr_t) noexcept { return !a.isNull(); }

private:
        struct Rep;
        explicit constexpr FileName(Rep *rep) noexcept : rep_(rep) {}
        static FileName templateName();
        static FileName tempVectorStatic(String, String, String *);
        MutString *parts() const noexcept;
        void setPartRaw(int, MutString) noexcept;
        Rep *rep_;
};

static_assert(sizeof(FileName) == sizeof(void *));
static_assert(std::is_trivially_copyable_v<FileName>);

#define FNAME_DIR  0
#define FNAME_NAME 1
#define FNAME_TYPE 2

#else
typedef struct fileName {
        MutString  partv[NARY];
} *FileName;

#define         FNAME_DIR                       0
#define         FNAME_NAME                      1
#define         FNAME_TYPE                      2

#define         fnameDir(fn)                    ((fn)->partv[FNAME_DIR])
#define         fnameName(fn)                   ((fn)->partv[FNAME_NAME])
#define         fnameType(fn)                   ((fn)->partv[FNAME_TYPE])

#define         fnameTSetDir(fn,s)              ((fn)->partv[FNAME_DIR]  = (s))
#define         fnameTSetName(fn,s)             ((fn)->partv[FNAME_NAME] = (s))
#define         fnameTSetType(fn,s)             ((fn)->partv[FNAME_TYPE] = (s))

extern FileName fnameNew                        (String d, String n, String t);
extern FileName fnameStdin                      (void);
extern FileName fnameStdout                     (void);

extern bool     fnameIsStdin                    (FileName);
extern bool     fnameIsStdout                   (FileName);

extern FileName fnameCopy                       (FileName);
extern FileName fnameWithType                   (FileName, String);
extern void     fnameFree                       (FileName);
extern bool     fnameEqual                      (FileName, FileName);

extern FileName fnameParse                      (String);
extern FileName fnameParseStatic                (String);
extern FileName fnameParseStaticWithin          (String, String dir);

extern MutString   fnameUnparse                    (FileName);
extern MutString   fnameUnparseStatic              (FileName);
extern MutString   fnameUnparseStaticWith          (FileName);
extern MutString   fnameUnparseStaticWithout       (FileName);

extern bool     fnameHasDir                     (FileName);
extern bool     fnameHasType                    (FileName);

extern void     fnameSetDir                     (FileName, String);
extern void     fnameSetName                    (FileName, String);
extern void     fnameSetType                    (FileName, String);

extern FileName fnameTemp                       (String, String, String);
extern FileName*fnameTempVector                 (String, String, String *);

#endif
#endif
