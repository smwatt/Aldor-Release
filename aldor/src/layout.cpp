/*****************************************************************************
 *
 * layout.cpp: Representation guards for C-to-C++ modernization.
 *
 * These assertions intentionally describe the v0.04 representations.  A
 * future representation change must therefore be explicit in the same
 * version that changes these guards.
 *
 ****************************************************************************/

#include "axlgen.h"
#include "buffer.h"
#include "bitv.h"
#include "table.h"
#include "symbol.h"
#include "btree.h"
#include "list.h"

#include <cstddef>

namespace {
constexpr std::size_t
alignUp(std::size_t n, std::size_t a)
{
        return (n + a - 1) / a * a;
}
}

/* Cons lists: exactly two fields, preserving tail-sharing representation. */
static_assert(offsetof(ListCons<void *>, first) == 0);
static_assert(offsetof(ListCons<void *>, rest) ==
              alignUp(sizeof(void *), alignof(ListCons<void *> *)));
static_assert(sizeof(ListCons<void *>) ==
              offsetof(ListCons<void *>, rest) + sizeof(ListCons<void *> *));

/* Buffer: pos, capacity, byte pointer, in that order. */
/* Buffer representation assertions live with private Buffer::Rep. */

/* Bit-vector shape descriptor remains two Length values. */
static_assert(offsetof(_BitvClass, nbits) == 0);
static_assert(offsetof(_BitvClass, nwords) == sizeof(Length));
static_assert(sizeof(_BitvClass) == 2 * sizeof(Length));

/* Symbol is now an opaque pointer-sized value handle. */
static_assert(sizeof(Symbol) == sizeof(void *));
static_assert(std::is_trivially_copyable_v<Symbol>);

/* Raw hash table representation: guard field order and final extent. */
static_assert(offsetof(TblSlot, key) == 0);
static_assert(offsetof(TblSlot, elt) > offsetof(TblSlot, key));
static_assert(offsetof(TblSlot, hash) > offsetof(TblSlot, elt));
static_assert(offsetof(TblSlot, next) > offsetof(TblSlot, hash));
static_assert(sizeof(TblSlot) == offsetof(TblSlot, next) + sizeof(TblSlot *));

static_assert(offsetof(TableStruct, hashFun) == 0);
static_assert(offsetof(TableStruct, eqFun) > offsetof(TableStruct, hashFun));
static_assert(offsetof(TableStruct, info) > offsetof(TableStruct, eqFun));
static_assert(offsetof(TableStruct, count) > offsetof(TableStruct, info));
static_assert(offsetof(TableStruct, buckc) > offsetof(TableStruct, count));
static_assert(offsetof(TableStruct, buckv) > offsetof(TableStruct, buckc));
static_assert(sizeof(TableStruct) ==
              offsetof(TableStruct, buckv) + sizeof(TblSlot **));

static_assert(offsetof(TableIteratorStruct, curr) == 0);
static_assert(offsetof(TableIteratorStruct, last) == sizeof(TblSlot **));
static_assert(offsetof(TableIteratorStruct, link) == 2 * sizeof(TblSlot **));
static_assert(sizeof(TableIteratorStruct) == 3 * sizeof(TblSlot **));

/* Store's B-tree stays untouched in v0.05. */
static_assert(offsetof(btreePart, branch) == 0);
static_assert(offsetof(btreePart, key) > offsetof(btreePart, branch));
static_assert(offsetof(btreePart, entry) > offsetof(btreePart, key));
static_assert(sizeof(btreePart) ==
              offsetof(btreePart, entry) + sizeof(BTreeElt));

static_assert(offsetof(btree, isLeaf) == 0);
static_assert(offsetof(btree, t) == sizeof(CBool));
static_assert(offsetof(btree, nKeys) == sizeof(CBool) + sizeof(unsigned short));
static_assert(offsetof(btree, part) ==
              sizeof(CBool) + 2 * sizeof(unsigned short));
static_assert(sizeof(btree) ==
              offsetof(btree, part) + NARY * sizeof(btreePart));
