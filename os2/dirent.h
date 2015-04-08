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

#define opendir   opendir_
#define readdir   readdir_
#define closedir  closedir_
#define rewinddir rewinddir_

DIR            * opendir(const char * __dirname);
struct dirent  * readdir(DIR * __dir);
int              closedir(DIR * __dir);
void             rewinddir(DIR * __dir);

#if defined(__WATCOMC__)
extern int	mkdir( const char *__path );
extern int	rmdir( const char *__path );
#endif

#ifdef __cplusplus
}
#endif


#endif
