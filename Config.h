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
 */

#ifndef _Config_h
#define _Config_h

#if defined(HAVE_FORK) + defined(HAVE_THREADS) != 1
#error You must defined either HAVE_FORK either HAVE_THREADS!
#endif

#ifdef __WATCOMC__
  #include <stdlib.h>
  #ifndef MAXPATHLEN
    #define MAXPATHLEN _MAX_PATH
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
#elif defined (IBMC) || defined(__MSC__)
  #ifndef _MAX_PATH
    #include <stdlib.h>
  #endif
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
#define MYVER "1.0a-236"

#define PRODCODE 0x13FF  /* by FTSCPROD */

/* Environment */
#define MAX_ENV_VAR_NAME 256
#if defined(OS2) || defined(WIN32) || defined(DOS)
  #define PATH_SEPARATOR "\\"
#else
  #define PATH_SEPARATOR "/"
#endif

/* Protocol */
#define DEF_PORT 24554
#define DEF_TIMEOUT (5*60)
#define MIN_BLKSIZE 128
#define MAX_BLKSIZE 0x7fffu                 /* Don't change! */
#define DEF_BLKSIZE (4*1024u)
#define MAX_NETNAME 255

#ifndef CHECKCFG_INTERVAL		    /* Can be defined in Makefile */
#define CHECKCFG_INTERVAL config->rescan_delay
#endif

#define MAILBOX                             /* fileboxes suport */

/* System... */
#define STACKSIZE (256*1024)
#define MKTMPFILE_TRYES 20

#ifndef O_BINARY
  #define O_BINARY 0
#endif
#if defined(__WATCOMC__) || defined(VISUALCPP) || defined(__MINGW32__) || defined(IBMC) || defined(__MSC__)
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
  #elif defined(DOS)
    #define OS "DOS"
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

#if defined(WIN32) && defined(_MSC_VER)
#define snprintf  _snprintf
#define vsnprintf _vsnprintf
#endif

#if defined(__WATCOMC__) || defined(__EMX__) /* expand list if necessary */
#define BEGINTHREAD(a, b, c)   _beginthread(a, NULL, b, c)
#else
#define BEGINTHREAD _beginthread
#endif

#define UNUSED_ARG(s)  (void)(s)

#endif
