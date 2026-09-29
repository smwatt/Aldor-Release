/*****************************************************************************
 *
 * fluid.h: C++ dynamically scoped bindings.
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

#ifndef _FLUID_H_
#define _FLUID_H_

# include "axlport.h"

#ifdef __cplusplus

# include <type_traits>

/*
 * Fluid<T> gives a global/compiler-state variable one dynamically scoped
 * binding.  The current value is saved on construction and restored by
 * ordinary C++ stack unwinding on every exit from the lexical scope.
 *
 * Fluid deliberately owns no storage and changes no representation.  It is
 * only a binding guard around an existing object.  The bound object remains
 * accessed directly, exactly as in the historical compiler.
 */
template <class T>
class Fluid {
        static_assert(std::is_nothrow_copy_assignable_v<T>,
                      "Fluid<T> restoration must not throw");

        T&       target_;
        T        saved_;

public:
        explicit Fluid(T& target)
                : target_(target), saved_(target)
        {
        }

        Fluid(T& target, const T& value)
                : target_(target), saved_(target)
        {
                target_ = value;
        }

        ~Fluid() noexcept
        {
                target_ = saved_;
        }

        Fluid(const Fluid&) = delete;
        Fluid& operator=(const Fluid&) = delete;
        Fluid(Fluid&&) = delete;
        Fluid& operator=(Fluid&&) = delete;
};

#endif /* __cplusplus */

#endif /* !_FLUID_H_ */
