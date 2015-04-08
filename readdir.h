/*
 *  readdir.h -- The header to get POSIX directory interface out of
 *               misc compilers
 *
 *  ftnq.h is a part of binkd project
 *
 *  Copyright (C) 1996  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#ifndef _readdir_h
#define _readdir_h

#if defined(__WATCOMC__)
#include <sys/utime.h>
#elif defined(VISUALCPP) || defined(IBMC) || defined(__MSC__)
#include <direct.h>
#include <sys/utime.h>
#elif defined(__MINGW32__)
#include <dirent.h>
#include <sys/utime.h>
#else
#include <dirent.h>
#include <utime.h>
#endif

#if defined(VISUALCPP)
#include "nt/dirwin32.h"
#endif

#if defined(IBMC) || defined(__WATCOMC__)
#include "os2/dirent.h"
#endif

#if defined(__MSC__)
#include "dos/dirent.h"
#endif

#endif
