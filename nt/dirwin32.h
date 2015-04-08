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

#ifdef VISUALCPP

#ifndef _DIRWIN32_H_
#define _DIRWIN32_H_

#include <stdlib.h>

#ifndef _INTPTR_T_DEFINED
#ifdef  _WIN64
typedef __int64             intptr_t;
#else
typedef /* _W64 */ int            intptr_t;
#endif
#define _INTPTR_T_DEFINED
#endif

struct dirent {
   char        d_name[_MAX_PATH+1];  /* file's name */
   unsigned    d_attrib;             /* file's attributes */
};

typedef struct {
   struct dirent	de;
   intptr_t		handle;
   char			first_time;
} DIR;

DIR* opendir(const char*);
struct dirent *readdir(DIR*);
int  closedir(DIR*);

#endif /* _DIRWIN32_H_ */

#endif /* VISUALCPP */
