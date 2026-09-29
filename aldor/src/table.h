/*****************************************************************************
 *
 * table.h: Hash table data type.
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

#ifndef _TABLE_H_
#define _TABLE_H_

# include "axlport.h"

#ifdef __cplusplus

# include <bit>
# include <cstdint>
# include <type_traits>
# include <utility>

/*
 * The hashing engine remains the C-compatible substrate in table.cpp.  RawTable and the
 * raw callback types describe that ABI.  Compiler clients should use the
 * typed Table<K,V> facade below.
 */
using TblKey = void *;
using TblElt = void *;

using TblHashFun    = Hash  (*)(TblKey);
using TblEqFun      = CBool (*)(TblKey, TblKey);
using TblTestEltFun = CBool (*)(TblElt);
using TblMapEltFun  = TblElt(*)(TblElt);
using TblPrKeyFun   = int   (*)(FILE *, TblKey);
using TblPrEltFun   = int   (*)(FILE *, TblElt);
using TblFreeKeyFun = void  (*)(TblKey);
using TblFreeEltFun = void  (*)(TblElt);

struct TblSlot {
        TblKey          key;
        TblElt          elt;
        Hash            hash;
        TblSlot         *next;
};

struct TableStruct {
        TblHashFun      hashFun;
        TblEqFun        eqFun;
        void            *info;
        Length          count;
        Length          buckc;
        TblSlot         **buckv;
};
using RawTable = TableStruct *;

struct TableIteratorStruct {
        TblSlot         **curr;
        TblSlot         **last;
        TblSlot         *link;
};
using RawTableIterator = TableIteratorStruct;

ALDOR_C_BEGIN
RawTable tblNew         (TblHashFun hash, TblEqFun eq);
void     tblFree        (RawTable);
void     tblFreeDeeply  (RawTable, TblFreeKeyFun, TblFreeEltFun);
RawTable tblCopy        (RawTable);
RawTable tblRemoveIf    (RawTable, TblFreeEltFun, TblTestEltFun);
RawTable tblNMap        (TblMapEltFun, RawTable);
Length   tblSize        (RawTable);
TblElt   tblElt         (RawTable, TblKey, TblElt dflt);
TblElt   tblSetElt      (RawTable, TblKey, TblElt);
RawTable tblDrop        (RawTable, TblKey);
int      tblPrint       (FILE *, RawTable, TblPrKeyFun, TblPrEltFun);
int      tblColumnPrint (FILE *, RawTable, TblPrKeyFun, TblPrEltFun);
int      _tblITER       (RawTableIterator *, RawTable);
int      _tblSTEP       (RawTableIterator *);
ALDOR_C_END

namespace table_detail {

template <class T>
inline void *erase(T value) noexcept
{
        using U = std::remove_cv_t<T>;
        if constexpr (std::is_same_v<U, std::nullptr_t>) {
                return nullptr;
        }
        else if constexpr (std::is_pointer_v<U>) {
                return const_cast<void *>(static_cast<const void *>(value));
        }
        else if constexpr (std::is_enum_v<U>) {
                using I = std::underlying_type_t<U>;
                using UI = std::make_unsigned_t<I>;
                return reinterpret_cast<void *>(
                        static_cast<std::uintptr_t>(static_cast<UI>(value)));
        }
        else if constexpr (std::is_integral_v<U>) {
                using UI = std::make_unsigned_t<U>;
                return reinterpret_cast<void *>(
                        static_cast<std::uintptr_t>(static_cast<UI>(value)));
        }
        else {
                static_assert(sizeof(U) == sizeof(void *),
                              "Table key/value handle must be pointer-sized");
                static_assert(std::is_trivially_copyable_v<U>,
                              "Table key/value handle must be trivially copyable");
                return std::bit_cast<void *>(value);
        }
}

template <class T>
inline T unerase(void *p) noexcept
{
        using U = std::remove_cv_t<T>;
        if constexpr (std::is_pointer_v<U>) {
                return reinterpret_cast<U>(p);
        }
        else if constexpr (std::is_enum_v<U>) {
                using I = std::underlying_type_t<U>;
                return static_cast<U>(static_cast<I>(
                        reinterpret_cast<std::uintptr_t>(p)));
        }
        else if constexpr (std::is_integral_v<U>) {
                return static_cast<U>(reinterpret_cast<std::uintptr_t>(p));
        }
        else {
                static_assert(sizeof(U) == sizeof(void *),
                              "Table key/value handle must be pointer-sized");
                static_assert(std::is_trivially_copyable_v<U>,
                              "Table key/value handle must be trivially copyable");
                return std::bit_cast<U>(p);
        }
}

template <class K, auto HashFn>
Hash hashBridge(void *p)
{
        return HashFn(unerase<K>(p));
}

template <class K, auto EqFn>
CBool eqBridge(void *a, void *b)
{
        return EqFn(unerase<K>(a), unerase<K>(b));
}

template <class T, auto FreeFn>
void freeBridge(void *p)
{
        FreeFn(unerase<T>(p));
}

template <class V, auto TestFn>
CBool testBridge(void *p)
{
        return TestFn(unerase<V>(p));
}

template <class V, auto MapFn>
void *mapBridge(void *p)
{
        return erase<V>(MapFn(unerase<V>(p)));
}

template <class T, auto PrintFn>
int printBridge(FILE *fout, void *p)
{
        return PrintFn(fout, unerase<T>(p));
}

} // namespace table_detail

template <class K, class V>
class Table {
public:
        Table() = default;
        constexpr Table(std::nullptr_t) noexcept : rep_(nullptr) {}

        static Table create()
                { return Table(tblNew(nullptr, nullptr)); }

        template <auto HashFn, auto EqFn>
        static Table create()
                { return Table(tblNew(&table_detail::hashBridge<K, HashFn>,
                                      &table_detail::eqBridge<K, EqFn>)); }

        template <auto HashFn>
        static Table createWithHash()
                { return Table(tblNew(&table_detail::hashBridge<K, HashFn>,
                                      nullptr)); }

        bool isNull() const noexcept { return rep_ == nullptr; }
        explicit operator bool() const noexcept { return !isNull(); }

        Length size() const { return tblSize(rep_); }
        V get(K key, V notFound) const
                { return table_detail::unerase<V>(
                          tblElt(rep_, table_detail::erase(key),
                                table_detail::erase(notFound))); }
        V get(K key, std::nullptr_t) const
                { return table_detail::unerase<V>(
                          tblElt(rep_, table_detail::erase(key), nullptr)); }
        V set(K key, V value)
                { return table_detail::unerase<V>(
                          tblSetElt(rep_, table_detail::erase(key),
                                   table_detail::erase(value))); }
        void drop(K key) { (void) tblDrop(rep_, table_detail::erase(key)); }

        void free() { tblFree(rep_); }

        template <auto FreeKey = nullptr, auto FreeValue = nullptr>
        void freeDeeply()
        {
                TblFreeKeyFun fk = nullptr;
                TblFreeEltFun fv = nullptr;
                if constexpr (FreeKey != nullptr)
                        fk = &table_detail::freeBridge<K, FreeKey>;
                if constexpr (FreeValue != nullptr)
                        fv = &table_detail::freeBridge<V, FreeValue>;
                tblFreeDeeply(rep_, fk, fv);
        }

        Table copy() const { return Table(tblCopy(rep_)); }

        template <auto FreeValue, auto Test>
        void removeIf()
        {
                (void) tblRemoveIf(rep_,
                        &table_detail::freeBridge<V, FreeValue>,
                        &table_detail::testBridge<V, Test>);
        }

        template <auto Map>
        void nmap()
        {
                (void) tblNMap(&table_detail::mapBridge<V, Map>, rep_);
        }

        template <auto PrintKey = nullptr, auto PrintValue = nullptr>
        int print(FILE *fout) const
        {
                TblPrKeyFun pk = nullptr;
                TblPrEltFun pv = nullptr;
                if constexpr (PrintKey != nullptr)
                        pk = &table_detail::printBridge<K, PrintKey>;
                if constexpr (PrintValue != nullptr)
                        pv = &table_detail::printBridge<V, PrintValue>;
                return tblPrint(fout, rep_, pk, pv);
        }

        template <auto PrintKey = nullptr, auto PrintValue = nullptr>
        int columnPrint(FILE *fout) const
        {
                TblPrKeyFun pk = nullptr;
                TblPrEltFun pv = nullptr;
                if constexpr (PrintKey != nullptr)
                        pk = &table_detail::printBridge<K, PrintKey>;
                if constexpr (PrintValue != nullptr)
                        pv = &table_detail::printBridge<V, PrintValue>;
                return tblColumnPrint(fout, rep_, pk, pv);
        }

        RawTable raw() const noexcept { return rep_; }
        static Table fromRaw(RawTable raw) noexcept { return Table(raw); }

        class Iterator {
        public:
                explicit Iterator(Table table)
                        : more_(_tblITER(&it_, table.rep_) != 0) {}

                bool more() const noexcept { return more_; }
                void step() noexcept
                {
                        if (!more_) return;
                        if ((it_.link = it_.link->next) != nullptr)
                                more_ = true;
                        else
                                more_ = _tblSTEP(&it_) != 0;
                }
                K key() const noexcept
                        { return table_detail::unerase<K>(it_.link->key); }
                V value() const noexcept
                        { return table_detail::unerase<V>(it_.link->elt); }
                void setKey(K key) noexcept
                        { it_.link->key = table_detail::erase(key); }
                void setValue(V value) noexcept
                        { it_.link->elt = table_detail::erase(value); }
        private:
                RawTableIterator it_{};
                bool more_ = false;
        };

        friend bool operator==(Table a, Table b) noexcept { return a.rep_ == b.rep_; }
        friend bool operator!=(Table a, Table b) noexcept { return !(a == b); }
        friend bool operator==(Table a, std::nullptr_t) noexcept { return a.isNull(); }
        friend bool operator!=(Table a, std::nullptr_t) noexcept { return !a.isNull(); }

private:
        explicit constexpr Table(RawTable rep) noexcept : rep_(rep) {}
        RawTable rep_;
};


#else /* !__cplusplus */

typedef void            *TblKey;
typedef void            *TblElt;

typedef Hash            (* TblHashFun)          (TblKey);
typedef CBool           (* TblEqFun)            (TblKey, TblKey);
typedef CBool           (* TblTestEltFun)       (TblElt);
typedef TblElt          (* TblMapEltFun)        (TblElt);
typedef int             (* TblPrKeyFun)         (FILE *, TblKey);
typedef int             (* TblPrEltFun)         (FILE *, TblElt);
typedef void            (* TblFreeKeyFun)       (TblKey);
typedef void            (* TblFreeEltFun)       (TblElt);

struct TblSlot {
        TblKey          key;
        TblElt          elt;
        Hash            hash;
        struct TblSlot  *next;
};

typedef struct TableStruct {
        TblHashFun      hashFun;
        TblEqFun        eqFun;
        void            *info;
        Length          count;
        Length          buckc;
        struct TblSlot  **buckv;
} *Table;
typedef Table RawTable;

typedef struct TableIteratorStruct {
        struct TblSlot  **curr;
        struct TblSlot  **last;
        struct TblSlot  *link;
} TableIterator;
typedef TableIterator RawTableIterator;

extern Table    tblNew          (TblHashFun hash, TblEqFun eq);
extern void     tblFree         (Table);
extern void     tblFreeDeeply   (Table, TblFreeKeyFun, TblFreeEltFun);
extern Table    tblCopy         (Table);
extern Table    tblRemoveIf     (Table, TblFreeEltFun, TblTestEltFun);
extern Table    tblNMap         (TblMapEltFun, Table);
extern Length   tblSize         (Table);
extern TblElt   tblElt          (Table, TblKey, TblElt dflt);
extern TblElt   tblSetElt       (Table, TblKey, TblElt);
extern Table    tblDrop         (Table, TblKey);
extern int      tblPrint        (FILE *, Table, TblPrKeyFun, TblPrEltFun);
extern int      tblColumnPrint  (FILE *, Table, TblPrKeyFun, TblPrEltFun);

#define tblITER(it, t)  _tblITER(&(it), t)
#define tblMORE(it)     ((it).curr <= (it).last)
#define tblSTEP(it)     ((((it).link=(it).link->next))==0 ? _tblSTEP(&(it)) : 1)
#define tblKEY(it)      ((it).link->key)
#define tblELT(it)      ((it).link->elt)
#define tblSETKEY(it,k) ((it).link->key = (k))
#define tblSETELT(it,e) ((it).link->elt = (e))

extern int      _tblITER        (TableIterator *, Table);
extern int      _tblSTEP        (TableIterator *);

#endif /* __cplusplus */

#endif
