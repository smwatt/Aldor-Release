/*****************************************************************************
 *
 * of_killp.h: Pointer crushing
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

#ifndef _OF_KILLP_H_
#define _OF_KILLP_H_

void    kpSetCutOff     (int);
void    killPointers    (Foam);
void    killProgPointers(Foam);

#endif /*!_OF_KILLP_H_*/
