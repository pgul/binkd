/*
 *  Config.h -- misc defines
 *
 *  Config.h is a part of binkd project
 *
 *  Copyright (C) 1996-1998  Dima Maloff, 5047/13
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
 * Revision 2.6  2003/03/01 20:16:27  gul
 * OS/2 IBM C support
 *
 * Revision 2.5  2003/02/28 19:13:55  gul
 * Added prodcode
 *
 * Revision 2.4  2003/02/22 21:53:39  gul
 * Typo in comment
 *
 * Revision 2.3  2002/11/12 16:55:58  gul
 * Run as service under win9x
 *
 * Revision 2.2  2001/12/25 17:13:15  gul
 * mingw _sleep() fix
 *
 * Revision 2.1  2001/09/24 10:31:39  gul
 * Build under mingw32
 *
 * Revision 2.0  2001/01/10 12:12:37  gul
 * Binkd is under CVS again
 *
 * Revision 1.6  1997/10/23  04:22:28  mff
 * Important -- MAX_BLKSIZE is now 0x7FFF (32*1024 was error)
 */

#ifndef _Config_h
#define _Config_h

#ifdef __WATCOMC__
  #include <direct.h>
  #ifndef MAXPATHLEN
    #define MAXPATHLEN NAME_MAX
  #endif
#elif defined (VISUALCPP)
  #include <direct.h>
  #ifndef MAXPATHLEN
    #define MAXPATHLEN _MAX_PATH
  #endif
#elif defined (__MINGW32__)
  #include <limits.h>
  #ifndef MAXPATHLEN
    #define MAXPATHLEN PATH_MAX
  #endif
#elif defined (__IBMC__)
  #include <stdlib.h>
  #ifndef MAXPATHLEN
    #define MAXPATHLEN _MAX_PATH
  #endif
#else
  #include <sys/param.h>
#endif

/* Please, no spaces here! */
#define PRTCLNAME "binkp"
#define PRTCLVER "1.1"
#define MYNAME "binkd"
#define MYVER "0.9.5a"

#define PRODCODE 0x13FF  /* by FTSCPROD */

/* Environment */
#define MAX_ENV_VAR_NAME 256
#if defined(OS2) || defined(WIN32)
  #define PATH_SEPARATOR "\\"
#else
  #define PATH_SEPARATOR "/"
#endif

/* Protocol */
#define DEF_PORT 24554
#define DEF_TIMEOUT (5*60)
#define MIN_BLKSIZE 128
#define MAX_BLKSIZE 0x7fff                  /* Don't change! */
#define DEF_BLKSIZE (4*1024u)
#define MAX_NETNAME 255

#ifndef CHECKCFG_INTERVAL		    /* Can be defined in Makefile */
#define CHECKCFG_INTERVAL rescan_delay
#endif

#define MAILBOX                             /* fileboxes suport */

/* System... */
#define STACKSIZE (256*1024)
#define MKTMPFILE_TRYES 20

#ifndef O_BINARY
  #define O_BINARY 0
#endif
#if defined(__WATCOMC__) || defined(VISUALCPP) || defined(__MINGW32__) || defined(__IBMC__)
  #define MKDIR(s) mkdir(s)
#else
  #define MKDIR(s) mkdir(s, 0755)
#endif
#ifndef OS
  #if defined(BINKDW9X)
    #define OS "Win9x"
  #elif defined(WIN32)
    #define OS "Win32"
  #elif defined(OS2)
    #define OS "OS2"
  #endif
#endif

#ifdef __MINGW32__
#define open  _open
#define close _close
#define read  _read
#define write _write
#define stat  _stat
#define fstat _fstat
#define sleep(sec) _sleep((sec)*1000ul)
#define snprintf  _snprintf
#define vsnprintf _vsnprintf
#endif

/* Pragmas */
#ifdef __WATCOMC__
  #pragma off (unreferenced);
#endif

#endif
