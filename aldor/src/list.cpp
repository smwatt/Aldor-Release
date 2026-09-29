///////////////////////////////////////////////////////////////////////////////
//
// list.cpp: List operations.
//
///////////////////////////////////////////////////////////////////////////////

// This file is part of Aldor.
//
// Aldor is licensed under the Apache License, Version 2.0.
//
//
// See legal/LICENSE in the Aldor distribution for details.
//
// Copyright (C) 1990-2026 Stephen M. Watt.

# include "axlgen.h"

#ifdef __cplusplus

void *
listAllocCons(ULong size)
{
        return (void *) stoAlloc((unsigned) OB_List, size);
}

void
listFreeConsStorage(void *p)
{
        stoFree(p);
}

#endif /* __cplusplus */
