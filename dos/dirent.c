/*
 *  dirent.c -- additional functions not found in IBM VisualAge C / CSet RTL
 *
 *  dirent.c is a part of binkd project
 *
 *  Copyright (C) 1997 Victor Pashkevich, 2:451/30@FidoNet
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#include <dos.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "dirent.h"

#define SEARCH_ATTR (_A_HIDDEN|_A_SYSTEM|_A_RDONLY|_A_SUBDIR)

DIR * opendir(const char * dirname)
{
   char * name;
   int len;
   DIR * dir;
   int apiret;

   len=strlen(dirname);
   if( (name=malloc(len+5))==NULL ) {
     errno=ENOMEM;
     return NULL;
   }
   strcpy(name,dirname);
   if( len-- && name[len]!=':' && name[len]!='\\' && name[len]!='/' )
     strcat(name,"\\*.*");
   else
     strcat(name,"*.*");

   if( (dir=malloc(sizeof(DIR)))==NULL ) {
     errno=ENOMEM;
     free(name);
     return NULL;
   }

   if( (apiret=_dos_findfirst(name,SEARCH_ATTR,
                              (struct find_t *)&dir->_d_reserved) )!=0 ) {
     free(name);
     free(dir);
     return NULL;
   }

   dir->dirname=name;
   dir->_d_first = 1;
   return dir;
}

void rewinddir(DIR * dir)
{
   _dos_findfirst(dir->dirname,SEARCH_ATTR,
                  (struct find_t *)&dir->_d_reserved);
   dir->_d_first = 1;
}

struct dirent * readdir(DIR * dir)
{
   if( !dir->_d_first ) {
     if( _dos_findnext((struct find_t *)&dir->_d_reserved)!=0 )
       return NULL;
   }
   dir->_d_first=0;
   return &dir->_d_dirent;
}

int closedir(DIR * dir)
{
   if( dir==NULL ) {
     errno = EBADF;
     return -1;
   }

   free(dir->dirname);
   free(dir);
   return 0;
}
