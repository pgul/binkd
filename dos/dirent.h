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
   char d_name[13];
};

typedef struct {
   char          _d_reserved[30];
   struct dirent _d_dirent;
   char *        dirname;
   char          _d_first;
} DIR;

DIR            *   opendir(const char * __dirname);
struct dirent  *   readdir(DIR * __dir);
int               closedir(DIR * __dir);
void             rewinddir(DIR * __dir);

#ifdef __cplusplus
}
#endif


#endif
