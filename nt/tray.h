/*
 *  tray.h -- Declarations of functions used to implement "minimize to tray"
 *            feature
 *
 *  This file is a part of binkd project
 *
 *  Copyright (C) 2003 Stas Degteff 2:5080/102@fidonet, g@grumbler.org
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#ifndef __TRAY_H_
#define __TRAY_H_

#define BINKD_ICON_FILE "binkd.ico" /* place this file into binkd directory and binkd loads it for tray icon */

/* Function used for "minimize to tray" feature
 */
void wndthread(void *par);
#endif
