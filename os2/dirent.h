/*
 *  dirent.h -- additional functions not found in IBM VisualAge C / CSet RTL
 *
 *  dirent.h is a part of binkd project
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
 * Revision 2.3  2003/03/10 11:40:10  gul
 * Use self opendir/readdir/closedir functions for watcom
 *
 * Revision 2.2  2003/03/01 20:31:59  gul
 * dos2unix EOL
 *
 * Revision 2.1  2003/03/01 20:26:36  gul
 * *** empty log message ***
 *
 */

#ifndef __DIRENT_H__
#define __DIRENT_H__

#ifdef __cplusplus
extern "C" {
#endif

struct dirent {
   char d_name[256];
};

typedef struct {
   unsigned long hdir;
   char *        dirname;
   unsigned      nfiles;
   char *        bufp;
   char          buf[512];
} DIR;

DIR            * _Optlink   opendir(const char * __dirname);
struct dirent  * _Optlink   readdir(DIR * __dir);
int              _Optlink  closedir(DIR * __dir);
void             _Optlink rewinddir(DIR * __dir);

#if defined(__WATCOMC__)
extern int	mkdir( const char *__path );
extern int	rmdir( const char *__path );
#endif

#ifdef __cplusplus
}
#endif


#endif
