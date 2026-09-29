/*****************************************************************************
 *
 * cxxexcept.h: Compiler-internal C++ non-local control transfers.
 *
 * These types never cross the generated-code/runtime C ABI.
 *
 ****************************************************************************/

#ifndef _CXXEXCEPT_H_
#define _CXXEXCEPT_H_

#ifdef __cplusplus

struct FintAbort final { };
struct WinAldorExit final { };

struct SExprReadAbort final {
        int error;
        explicit SExprReadAbort(int n) : error(n) {}
};

#endif

#endif
