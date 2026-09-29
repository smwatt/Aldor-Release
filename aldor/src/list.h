/*****************************************************************************
 *
 * list.h: Parameterized list type.
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

#ifndef _LIST_H_
#define _LIST_H_

# include "axlport.h"

/*****************************************************************************
 *
 * Public part.
 *
 ****************************************************************************/

#ifdef __cplusplus

/*
 * Compiler-side C++ list representation.
 *
 * List<T> deliberately remains a nullable pointer to a Store-allocated cons
 * cell.  It is not an owning container: tails may be shared and destructive
 * operations remain part of the API.  This preserves the historical layout
 * and semantics while removing the macro-generated type/function-table layer.
 */
template <class T>
struct ListCons {
        T               first;
        ListCons<T>     *rest;
};

template <class T>
using List = ListCons<T> *;

/* Implemented by list.cpp. */
extern void *listAllocCons(ULong);
extern void  listFreeConsStorage(void *);

template <class T>
inline constexpr List<T> listNil = nullptr;

template <class T>
inline List<T>
listCons(T x, List<T> l)
{
        auto t = static_cast<List<T>>(listAllocCons(sizeof(ListCons<T>)));
        t->first = x;
        t->rest  = l;
        return t;
}

template <class T>
inline List<T>
listSingleton(T x)
{
        return listCons<T>(x, listNil<T>);
}

template <class T>
inline bool
listEqual(List<T> l1, List<T> l2, bool (*eq)(T, T))
{
        for (; l1 && l2; l1 = l1->rest, l2 = l2->rest)
                if (!(*eq)(l1->first, l2->first))
                        return false;
        return l1 == l2;
}

template <class T>
inline T
listFind(List<T> l, T x, bool (*eq)(T, T), int *pos)
{
        int i;
        for (i = 0; l; l = l->rest, ++i)
                if ((*eq)(l->first, x)) {
                        *pos = i;
                        return l->first;
                }
        *pos = -1;
        return T{};
}

template <class T>
inline List<T>
listFreeCons(List<T> l)
{
        List<T> t;
        if (l) {
                t = l->rest;
                listFreeConsStorage(l);
        }
        else
                t = listNil<T>;
        return t;
}

template <class T>
inline List<T>
listFreeDeeplyTo(List<T> l, List<T> tail, void (*eltfree)(T))
{
        while (l && l != tail) {
                List<T> t = l->rest;
                if (eltfree) (*eltfree)(l->first);
                listFreeConsStorage(l);
                l = t;
        }
        return l;
}

template <class T>
inline void
listFreeDeeply(List<T> l, void (*eltfree)(T))
{
        (void) listFreeDeeplyTo<T>(l, listNil<T>, eltfree);
}

template <class T>
inline List<T>
listFreeTo(List<T> l, List<T> tail)
{
        return listFreeDeeplyTo<T>(l, tail, nullptr);
}

template <class T>
inline void
listFree(List<T> l)
{
        (void) listFreeDeeplyTo<T>(l, listNil<T>, nullptr);
}

template <class T>
inline List<T>
listFreeIfSat(List<T> l, void (*eltFree)(T), bool (*eltSat)(T))
{
        List<T> l0 = l;
        List<T> *pred = &l;

        while (l0) {
                if ((*eltSat)(l0->first)) {
                        if (eltFree) (*eltFree)(l0->first);
                        l0 = listFreeCons<T>(l0);
                        *pred = l0;
                }
                else {
                        pred = &l0->rest;
                        l0 = l0->rest;
                }
        }
        return l;
}

template <class T>
inline List<T>
listDrop(List<T> l, Length n)
{
        for (; l && n > 0; l = l->rest, --n)
                ;
        assert(n == 0);
        return l;
}

template <class T>
inline T
listElt(List<T> l, Length n)
{
        l = listDrop<T>(l, n);
        assert(l != nullptr);
        return l->first;
}

template <class T>
inline List<T>
listLastCons(List<T> l)
{
        if (!l) return listNil<T>;
        while (l->rest) l = l->rest;
        return l;
}

template <class T>
inline Length
listLength(List<T> l)
{
        Length i;
        for (i = 0; l; l = l->rest, ++i)
                ;
        return i;
}

template <class T>
inline bool
listIsLength(List<T> l, Length n)
{
        for (; l && n > 0; l = l->rest, --n)
                ;
        return !l && n == 0;
}

template <class T>
inline bool
listIsLonger(List<T> l, Length n)
{
        for (; l && n > 0; l = l->rest, --n)
                ;
        return l && n == 0;
}

template <class T>
inline bool
listIsShorter(List<T> l, Length n)
{
        for (; l && n > 0; l = l->rest, --n)
                ;
        return !l && n > 0;
}

template <class T>
inline List<T>
listNReverse(List<T> l)
{
        List<T> r = listNil<T>;
        while (l) {
                List<T> t = l->rest;
                l->rest = r;
                r = l;
                l = t;
        }
        return r;
}

template <class T>
inline List<T>
listCopyDeeplyTo(List<T> l, List<T> tail, T (*eltcopy)(T))
{
        List<T> r = listNil<T>;
        for (; l && l != tail; l = l->rest)
                r = listCons<T>(eltcopy ? (*eltcopy)(l->first) : l->first, r);
        return listNReverse<T>(r);
}

template <class T>
inline List<T>
listCopyDeeply(List<T> l, T (*eltcopy)(T))
{
        return listCopyDeeplyTo<T>(l, listNil<T>, eltcopy);
}

template <class T>
inline List<T>
listCopyTo(List<T> l, List<T> tail)
{
        return listCopyDeeplyTo<T>(l, tail, nullptr);
}

template <class T>
inline List<T>
listCopy(List<T> l)
{
        return listCopyDeeplyTo<T>(l, listNil<T>, nullptr);
}

template <class T>
inline List<T>
listNMap(T (*f)(T), List<T> l)
{
        for (List<T> t = l; t; t = t->rest)
                t->first = (*f)(t->first);
        return l;
}

template <class T>
inline List<T>
listMap(T (*f)(T), List<T> l)
{
        return listNMap<T>(f, listCopy<T>(l));
}

template <class T>
inline List<T>
listReverse(List<T> l)
{
        return listNReverse<T>(listCopy<T>(l));
}

template <class T>
inline List<T>
listNConcat(List<T> l1, List<T> l2)
{
        if (!l1) return l2;
        listLastCons<T>(l1)->rest = l2;
        return l1;
}

template <class T>
inline List<T>
listConcat(List<T> l1, List<T> l2)
{
        return listNConcat<T>(listCopy<T>(l1), l2);
}

template <class T>
inline int
listPosq(List<T> l, T x)
{
        Length i;
        for (i = 0; l; l = l->rest, ++i)
                if (l->first == x) return (int) i;
        return -1;
}

template <class T>
inline int
listPosition(List<T> l, T x, bool (*eq)(T, T))
{
        Length i;
        for (i = 0; l; l = l->rest, ++i)
                if ((*eq)(l->first, x)) return (int) i;
        return -1;
}

template <class T>
inline bool
listMemq(List<T> l, T x)
{
        return listPosq<T>(l, x) != -1;
}

template <class T>
inline bool
listMember(List<T> l, T x, bool (*eq)(T, T))
{
        return listPosition<T>(l, x, eq) != -1;
}

template <class T>
inline List<T>
listNRemove(List<T> l, T x, bool (*eq)(T, T))
{
        if (!l) return l;
        if ((*eq)(l->first, x)) return listFreeCons<T>(l);

        for (List<T> p = l, t = l->rest; t; p = t, t = t->rest)
                if ((*eq)(t->first, x)) {
                        p->rest = listFreeCons<T>(t);
                        break;
                }
        return l;
}

template <class T>
inline void
listFillVector(T *v, List<T> l)
{
        for (; l; l = l->rest, ++v) *v = l->first;
}

template <class T>
inline int
listGPrint(FILE *fout, List<T> l, int (*prf)(FILE *, T),
           String open, String sep, String close)
{
        if (!l)
                return fprintf(fout, "%s%s", open, close);

        int cc = fprintf(fout, "%s", open);
        cc += (*prf)(fout, l->first);
        for (l = l->rest; l; l = l->rest) {
                cc += fprintf(fout, "%s", sep);
                cc += (*prf)(fout, l->first);
        }
        cc += fprintf(fout, "%s", close);
        return cc;
}

template <class T>
inline int
listPrint(FILE *fout, List<T> l, int (*prf)(FILE *, T))
{
        return listGPrint<T>(fout, l, prf, "[", ", ", "]");
}

/*
 * Compatibility for generated/shared .c sources which are compiled both as C
 * and as C++20.  Compiler .cpp sources use List<T> and the templates directly.
 */
# define DECLARE_LIST(Type)       using Abut(Type,List) = List<Type>
# define CREATE_LIST(Type)

# define listSingleton(Type)      listSingleton<Type>
# define listCons(Type)           listCons<Type>
# define listEqual(Type)          listEqual<Type>
# define listFind(Type)           listFind<Type>
# define listFreeCons(Type)       listFreeCons<Type>
# define listFree(Type)           listFree<Type>
# define listFreeTo(Type)         listFreeTo<Type>
# define listFreeDeeply(Type)     listFreeDeeply<Type>
# define listFreeDeeplyTo(Type)   listFreeDeeplyTo<Type>
# define listFreeIfSat(Type)      listFreeIfSat<Type>
# define listElt(Type)            listElt<Type>
# define listDrop(Type)           listDrop<Type>
# define listLastCons(Type)       listLastCons<Type>
# define listLength(Type)         listLength<Type>
# define listIsLength(Type)       listIsLength<Type>
# define listIsLonger(Type)       listIsLonger<Type>
# define listIsShorter(Type)      listIsShorter<Type>
# define listCopy(Type)           listCopy<Type>
# define listCopyTo(Type)         listCopyTo<Type>
# define listCopyDeeply(Type)     listCopyDeeply<Type>
# define listCopyDeeplyTo(Type)   listCopyDeeplyTo<Type>
# define listMap(Type)            listMap<Type>
# define listNMap(Type)           listNMap<Type>
# define listReverse(Type)        listReverse<Type>
# define listNReverse(Type)       listNReverse<Type>
# define listConcat(Type)         listConcat<Type>
# define listNConcat(Type)        listNConcat<Type>
# define listMemq(Type)           listMemq<Type>
# define listMember(Type)         listMember<Type>
# define listPosq(Type)           listPosq<Type>
# define listPosition(Type)       listPosition<Type>
# define listNRemove(Type)        listNRemove<Type>
# define listFillVector(Type)     listFillVector<Type>
# define listPrint(Type)          listPrint<Type>
# define listGPrint(Type)         listGPrint<Type>
# define listNil(Type)            listNil<Type>

#else /* __cplusplus */

# define List(Type)               Abut(Type,List)

# define DECLARE_LIST(Type)                             \
        typedef struct Abut(Type,ListCons)      {       \
                  Type                          first;  \
                  struct Abut(Type,ListCons)    *rest;  \
        } * List(Type);                                 \
        ListOpsStruct(Type);                            \
        extern const struct ListOpsStructName(Type)  *ListOps(Type)

# define CREATE_LIST(Type)  \
        const struct ListOpsStructName(Type) * ListOps(Type) =  \
                (struct ListOpsStructName(Type) *) &ptrlistOps

# define listSingleton(Type)           (*(ListOps(Type)->Singleton))
# define listCons(Type)                (*(ListOps(Type)->Cons))
# define listEqual(Type)               (*(ListOps(Type)->Equal))
# define listFind(Type)                (*(ListOps(Type)->Find))
# define listFreeCons(Type)            (*(ListOps(Type)->FreeCons))
# define listFree(Type)                (*(ListOps(Type)->Free))
# define listFreeTo(Type)              (*(ListOps(Type)->FreeTo))
# define listFreeDeeply(Type)          (*(ListOps(Type)->FreeDeeply))
# define listFreeDeeplyTo(Type)        (*(ListOps(Type)->FreeDeeplyTo))
# define listFreeIfSat(Type)           (*(ListOps(Type)->FreeIfSat))
# define listElt(Type)                 (*(ListOps(Type)->Elt))
# define listDrop(Type)                (*(ListOps(Type)->Drop))
# define listLastCons(Type)            (*(ListOps(Type)->LastCons))
# define listLength(Type)              (*(ListOps(Type)->_Length))
# define listIsLength(Type)            (*(ListOps(Type)->IsLength))
# define listIsLonger(Type)            (*(ListOps(Type)->IsLonger))
# define listIsShorter(Type)           (*(ListOps(Type)->IsShorter))
# define listCopy(Type)                (*(ListOps(Type)->Copy))
# define listCopyTo(Type)              (*(ListOps(Type)->CopyTo))
# define listCopyDeeply(Type)          (*(ListOps(Type)->CopyDeeply))
# define listCopyDeeplyTo(Type)        (*(ListOps(Type)->CopyDeeplyTo))
# define listMap(Type)                 (*(ListOps(Type)->Map))
# define listNMap(Type)                (*(ListOps(Type)->NMap))
# define listReverse(Type)             (*(ListOps(Type)->Reverse))
# define listNReverse(Type)            (*(ListOps(Type)->NReverse))
# define listConcat(Type)              (*(ListOps(Type)->Concat))
# define listNConcat(Type)             (*(ListOps(Type)->NConcat))
# define listMemq(Type)                (*(ListOps(Type)->Memq))
# define listMember(Type)              (*(ListOps(Type)->Member))
# define listPosq(Type)                (*(ListOps(Type)->Posq))
# define listPosition(Type)            (*(ListOps(Type)->Position))
# define listNRemove(Type)             (*(ListOps(Type)->NRemove))
# define listFillVector(Type)          (*(ListOps(Type)->FillVector))
# define listPrint(Type)               (*(ListOps(Type)->Print))
# define listGPrint(Type)              (*(ListOps(Type)->GPrint))

# define listNil(Type)                 ((Abut(Type,List)) 0)

# define ListOps(Type)           Abut(Type,_listPointer)
# define ListOpsStructName(Type) Abut(Type,_listOpsStruct)

# define ListOpsStruct(Type) \
        struct ListOpsStructName(Type) { \
                List(Type)      (*Cons)         (Type, List(Type)); \
                List(Type)      (*Singleton)    (Type); \
                bool            (*Equal)        (List(Type), List(Type), \
                                                 bool (*f) (Type, Type)); \
                Type            (*Find)         (List(Type), Type, \
                                                 bool(*eq)(Type,Type) , int *);\
                List(Type)      (*FreeCons)     (List(Type)); \
                void            (*Free)         (List(Type)); \
                List(Type)      (*FreeTo)       (List(Type), List(Type)); \
                void            (*FreeDeeply)   (List(Type), void (*f)(Type)); \
                List(Type)      (*FreeDeeplyTo) (List(Type), List(Type), \
                                                 void (*f) (Type) ); \
                List(Type)      (*FreeIfSat)    (List(Type), void (*f)(Type),\
                                                 bool (*s)(Type)); \
                Type            (*Elt)          (List(Type), Length); \
                List(Type)      (*Drop)         (List(Type), Length); \
                List(Type)      (*LastCons)     (List(Type)); \
                Length          (*_Length)      (List(Type)); \
                bool            (*IsLength)     (List(Type), Length); \
                bool            (*IsShorter)    (List(Type), Length); \
                bool            (*IsLonger)     (List(Type), Length); \
                List(Type)      (*Copy)         (List(Type)); \
                List(Type)      (*CopyTo)       (List(Type), List(Type)); \
                List(Type)      (*CopyDeeply)   (List(Type), Type (*f)(Type)); \
                List(Type)      (*CopyDeeplyTo) (List(Type), List(Type), \
                                                 Type (*f) (Type) ); \
                List(Type)      (*Map)          (Type (*f)(Type), List(Type)); \
                List(Type)      (*NMap)         (Type (*f)(Type), List(Type)); \
                List(Type)      (*Reverse)      (List(Type)); \
                List(Type)      (*NReverse)     (List(Type)); \
                List(Type)      (*Concat)       (List(Type), List(Type)); \
                List(Type)      (*NConcat)      (List(Type), List(Type)); \
                bool            (*Memq)         (List(Type), Type); \
                bool            (*Member)       (List(Type), Type, \
                                                 bool(*eq)(Type,Type) );\
                int             (*Posq)         (List(Type), Type); \
                int             (*Position)     (List(Type), Type, \
                                                 bool(*eq)(Type,Type) );\
                List(Type)      (*NRemove)      (List(Type), Type, \
                                                 bool(*eq)(Type,Type) ); \
                void            (*FillVector)   (Type *, List(Type)); \
                int             (*Print)        (FILE *, List(Type), \
                                                 int (*pr)(FILE *, Type) );  \
                int             (*GPrint)       (FILE *, List(Type), \
                                                 int (*pr)(FILE *, Type), \
                                                 String l,String m,String r);  \
        }

#endif /* __cplusplus */

# define car(l)                        ((l)->first)
# define cdr(l)                        ((l)->rest)
# define setcar(l,a)                   ((l)->first = (a))
# define setcdr(l,d)                   ((l)->rest = (d))

# define listIsSingleton(l)            ((l) && !cdr(l))

# define listPush(T, X, L)             (L = listCons(T)(X, L))
# define listPop(T, X, L, F)           (L = listNRemove(T)(L, X, F))

#ifdef __cplusplus
# define listIter(T, arg, list, action)                         \
Statement({                                                     \
           List<T> _l0;                                         \
           T arg;                                               \
           for (_l0 = (list); _l0; _l0 = cdr(_l0)) {           \
                   arg = car(_l0);                              \
                   Statement(action);                           \
           }                                                    \
})
#else
# define listIter(T, arg, list, action)                         \
Statement({                                                     \
           Abut(T,List) _l0;                                    \
           T arg;                                               \
           for (_l0 = (list); _l0; _l0 = cdr(_l0)) {           \
                   arg = car(_l0);                              \
                   Statement(action);                           \
           }                                                    \
})
#endif


#endif /* !_LIST_H_ */
