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

/*
 * $Id$
 *
 * $Log$
 * Revision 2.5  2003/03/11 00:04:26  gul
 * Use patches for compile under MSDOS by MSC 6.0 with IBMTCPIP
 *
 * Revision 2.4  2003/03/10 12:16:53  gul
 * Use HAVE_DOS_H macro
 *
 * Revision 2.3  2003/03/10 11:40:09  gul
 * Use self opendir/readdir/closedir functions for watcom
 *
 * Revision 2.2  2003/03/01 20:16:27  gul
 * OS/2 IBM C support
 *
 * Revision 2.1  2001/09/24 10:31:39  gul
 * Build under mingw32
 *
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 * Revision 1.1  1996/12/29  09:46:20  mff
 * Initial revision
 *
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
#include "NT/dirwin32.h"
#endif

#if defined(IBMC) || defined(__WATCOMC__)
#include "os2/dirent.h"
#endif

#if defined(__MSC__)
#include "dos/dirent.h"
#endif

#endif
