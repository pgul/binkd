/*
 *  service.h -- Windows NT services support for binkd definition file
 *
 *  service.h is a part of binkd project
 *
 *  Copyright (C) 2000 Dima Afanasiev, da@4u.net (Fido 2:5020/463)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */
/*
 * $Id$
 *
 * $Log$
 * Revision 2.2.2.1  2003/08/27 10:22:28  stas
 * Make binkd 0.9.5 command line compatible with binkd 0.9.6
 *
 * Revision 2.2  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.1  2003/02/13 19:44:45  gul
 * Change \r\n -> \n
 *
 * Revision 2.0  2001/01/10 12:12:40  gul
 * Binkd is under CVS again
 *
 *
 */

#define DEFAULT_SRVNAME "binkd-service"

int checkservice(void);
int service(int argc, char **argv, char **envp);

/* For debugging */
#define AlertWin(msg) MessageBox(NULL, msg, MYNAME, MB_OK|MB_ICONSTOP|0x00200000L|MB_SYSTEMMODAL|MB_SETFOREGROUND)
