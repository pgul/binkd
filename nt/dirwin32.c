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

#include <string.h>
#include <stdio.h>
#include <io.h>

#include "dirwin32.h"

/*
 * Note: still uses 32-bit (2G) file API because returned length is ignored
 */
DIR* opendir(const char* mask)
{
    DIR* dir;
    char *ch;
    intptr_t h;
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
    dir->de.d_attrib = dt.attrib;
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
        dir->de.d_attrib = dt.attrib;
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
