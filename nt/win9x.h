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

/*
 * $Id$
 *
 * Revision history:
 * $Log$
 * Revision 2.6  2003/09/11 13:04:14  hbrew
 * Undo 'move binkd9x deinit to exitfunc()' patch
 *
 * Revision 2.5  2003/09/07 04:49:42  hbrew
 * Remove binkd9x restart-on-config-change code; move binkd9x deinit to exitfunc()
 *
 * Revision 2.4  2003/07/07 10:13:54  gul
 * Use getopt() for commandline parse
 *
 * Revision 2.3  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.2  2002/11/13 07:58:19  gul
 * Add CVS macros
 *
 *
 */

#ifndef _win9x_h
#define _win9x_h
#ifdef BINKDW9X
int win9x_process(int argc, char **argv);
int win9x_check_name_all(void);

void CreateWin9xThread(PHANDLER_ROUTINE phandler);

/* TempConsole */
void AllocTempConsole(void);
void FreeTempConsole(void);
#endif
#endif

