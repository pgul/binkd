/*
 *  dirwin32.h -- additional functions not found in Visual C++ RTL
 *
 *  dirwin32.h is a part of binkd project
 *
 *  Copyright (C) 1996-97 Mike Malakhov, ww@pobox.com (FiDO 2:5030/280)
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
 * Revision 2.3  2003/03/22 08:59:58  gul
 * opendir() return NULL if directori does not exist
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

#ifdef VISUALCPP

#ifndef _DIRWIN32_H_
#define _DIRWIN32_H_

#include <stdlib.h>

struct dirent {
   char        d_name[_MAX_PATH+1];  /* file's name */
};

typedef struct {
   struct dirent	de;
   long			handle;
   char			first_time;
} DIR;

DIR* opendir(const char*);
struct dirent *readdir(DIR*);
int  closedir(DIR*);

#endif /* _DIRWIN32_H_ */

#endif /* VISUALCPP */
