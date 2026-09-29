#ifndef _STORECONCEPT_HPP_
#define _STORECONCEPT_HPP_

#include <concepts>
#include "store.h"

template <class S>
concept Store = requires(unsigned code, ULong size, void *p) {
        { S::allocate(code, size) } -> std::same_as<MostAlignedType *>;
        { S::deallocate(p) } -> std::same_as<void>;
        { S::size(p) } -> std::convertible_to<ULong>;
        { S::resize(p, size) } -> std::same_as<MostAlignedType *>;
};

template <class M>
concept StoreManager = requires(Length oldSize, Length newSize, void *p) {
        { M::allocate(newSize) } -> std::same_as<void *>;
        { M::deallocate(p) } -> std::same_as<void>;
        { M::resize(p, oldSize, newSize) } -> std::same_as<void *>;
};

#endif
