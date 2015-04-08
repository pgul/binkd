/*
 *  win9x.h -- Windows 95/98/ME support for binkd definition file
 *
 *  win9x.h is a part of binkd project
 *
 *  Copyright (C) 2002 Alexander Reznikov, homebrewer@yandex.ru (Fido 2:4600/220)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#ifndef _win9x_h
#define _win9x_h
#ifdef BINKD9X
int win9x_process(int argc, char **argv);
int win9x_check_name_all(void);

void CreateWin9xThread(PHANDLER_ROUTINE phandler);

/* TempConsole */
void AllocTempConsole(void);
void FreeTempConsole(void);
#endif
#endif

