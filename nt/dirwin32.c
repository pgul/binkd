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

#include "dirwin32.h"

DIR* opendir(const char* mask)
{
    DIR* dir;
    char *ch;

    if ((dir = malloc(sizeof(DIR))) == NULL) return NULL;
    dir->_first_time = 1;
    dir->_handle = -1;
    strcpy(dir->_mask,mask);
    ch = dir->_mask + strlen(dir->_mask) - 1;
    if (*ch=='\\' || *ch=='/') strcat(dir->_mask,"*");
                          else strcat(dir->_mask,"\\*");

    return dir;
}

DIR* readdir(DIR* dir)
{
    if (!dir) return NULL;

    if (dir->_first_time || dir->_handle==-1) {
      dir->_handle=_findfirst(dir->_mask,&(dir->_dt));
      if (dir->_handle==-1) return NULL;
      dir->_first_time=0;
      strcpy(dir->d_name,dir->_dt.name);
      }
    else {
      if (_findnext(dir->_handle,&(dir->_dt))==-1) return NULL;
      strcpy(dir->d_name,dir->_dt.name);
      }

    return dir; 
}


int  closedir(DIR* dir)
{
   int res;

   if (!dir) return 0;

   res = _findclose(dir->_handle);
   free(dir);

   return res==0;
}
