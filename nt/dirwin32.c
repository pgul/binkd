/*
 *  dirwin32.c -- additional functions not found in Visual C++ RTL
 *
 *  dirwin32.c is a part of binkd project
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
 * Revision 2.1  2003/03/22 08:59:58  gul
 * opendir() return NULL if directori does not exist
 *
 * Revision 2.0  2001/01/10 12:12:40  gul
 * Binkd is under CVS again
 *
 *
 */

#include <string.h>
#include <stdio.h>
#include <io.h>

#include "dirwin32.h"

DIR* opendir(const char* mask)
{
    DIR* dir;
    char *ch;
    int  h;
    char fmask[_MAX_PATH+1];
    struct _finddata_t dt;

    if (!mask || !*mask) return NULL;
    strncpy(fmask, mask, sizeof(fmask)-2);
    fmask[sizeof(fmask) - 3] = '\0';
    ch = fmask + strlen(fmask) - 1;
    if (*ch != '/' && *ch != '\\') *++ch = '\\';
    *++ch = '*';
    *++ch = '\0';
    if ((h = _findfirst(fmask, &dt)) == -1)
	return NULL;
    if ((dir = malloc(sizeof(DIR))) == NULL) return NULL;
    dir->handle = h;
    strncpy(dir->de.d_name, dt.name, sizeof(dir->de.d_name));
    dir->first_time = 1;
    return dir;
}

struct dirent *readdir(DIR* dir)
{
    struct _finddata_t dt;

    if (!dir || dir->handle==-1) return NULL;

    if (!dir->first_time) {
	if (_findnext(dir->handle, &dt)==-1) return NULL;
	strncpy(dir->de.d_name, dt.name, sizeof(dir->de.d_name));
    }
    else
	dir->first_time = 0;
    return &(dir->de);
}

int closedir(DIR* dir)
{
   int res;

   if (!dir) return 0;

   res = _findclose(dir->handle);
   free(dir);

   return res==0;
}
