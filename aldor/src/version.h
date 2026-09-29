/*****************************************************************************
 *
 * version.h: Compiler version.
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

#ifndef _VERSION_H_
#define _VERSION_H_

/* Public release identity. */
#define ALDOR_VERSION_MAJOR 1
#define ALDOR_VERSION_MINOR 5
#define ALDOR_VERSION_MICRO 0
#define ALDOR_VERSION_SUFFIX ""
#define ALDOR_VERSION_STRING "1.5.0"

/*
 * Version of the program. The edit number is for internal purposes
 * and allows more precise release checking. There are so many changes
 * that are being made to the compiler between patch levels that we
 * need a more high-resolution version counter, hence axiomxlEditNumber.
 */
#if EDIT_1_0_n2_07
extern const char *     axiomxlName;
#else
extern char *   axiomxlName;
#endif
extern int      axiomxlMajorVersion;
extern int      axiomxlMinorVersion;
extern int      axiomxlMinorFreeze;
extern int      axiomxlMicroFreeze;
extern int      axiomxlEditNumber; /* martin@nag.co.uk */
#if EDIT_1_0_n2_07
extern const char *     axiomxlPatchLevel;
#else
extern char *   axiomxlPatchLevel;
#endif
extern bool     verBannerWanted         (void);
extern void     verPrint                (void);

#endif /* !_VERSION_H_ */
