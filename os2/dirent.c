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

/*
 * $Id$
 *
 * Revision history:
 * $Log$
 * Revision 2.3  2003/03/26 13:53:28  gul
 * Fix OS/2 compilation
 *
 * Revision 2.2  2003/03/11 09:21:30  gul
 * Fixed OS/2 Watcom compilation
 *
 * Revision 2.1  2003/03/01 20:26:36  gul
 * *** empty log message ***
 *
 *
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "../sys.h"

#include <os2def.h>
#include <bsedos.h>

#include "dirent.h"

#define SEARCH_ATTR (FILE_HIDDEN|FILE_SYSTEM|FILE_DIRECTORY)

DIR * opendir(const char * dirname)
{
   char * name;
   int len;
   DIR * dir;
   ULONG nfiles;
   APIRET apiret;

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

   dir->hdir=HDIR_CREATE;
   nfiles=256;
   if( (apiret=DosFindFirst((PSZ)name,&dir->hdir,SEARCH_ATTR,
                            (PVOID)&dir->buf[0],sizeof(dir->buf),
                            &nfiles,FIL_STANDARD))!=0 ) {
     free(name);
     free(dir);
     return NULL;
   }

   dir->dirname=name;
   dir->nfiles=(unsigned)nfiles;
   dir->bufp=&dir->buf[0];
   return dir;
}

void rewinddir(DIR * dir)
{
   ULONG nfiles;

   nfiles=256;
   if( DosFindFirst((PSZ)dir->dirname,&dir->hdir,SEARCH_ATTR,
                    (PVOID)&dir->buf[0],sizeof(dir->buf),
                    &nfiles,FIL_STANDARD) == 0 ) {
     dir->nfiles=(unsigned)nfiles;
     dir->bufp=&dir->buf[0];
   }
}

struct dirent * readdir(DIR * dir)
{
   ULONG nfiles;
   FILEFINDBUF3 *ff;

   if( dir->nfiles==0 ) {
     nfiles=256;
     if( DosFindNext(dir->hdir,(PFILEFINDBUF)&dir->buf[0],
                     sizeof(dir->buf),&nfiles)!=0 )
       return NULL;
     dir->nfiles=(unsigned)nfiles;
     dir->bufp=&dir->buf[0];
   }

   ff=(FILEFINDBUF3 *)(dir->bufp);
   dir->bufp+=(int)ff->oNextEntryOffset;
   dir->nfiles--;

   return ((struct dirent *)&ff->achName[0]);
}

int closedir(DIR * dir)
{
   if( dir==NULL ) {
     errno = EBADF;
     return -1;
   }

   DosFindClose(dir->hdir);
   free(dir->dirname);
   free(dir);
   return 0;
}
