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
 * Revision 2.63  2003/07/12 18:06:45  gul
 * Auto increase patchlevel, set 0.9.6a-52
 * Fixed node output on debugcfg
 *
 * Revision 2.62  2003/07/11 15:06:44  gul
 * Auto increase patchlevel, set 0.9.6a-51
 * Fix building with libc5
 *
 * Revision 2.61  2003/07/08 06:48:05  gul
 * Auto increase patchlevel, set 0.9.6a-50
 * cosmetics
 *
 * Revision 2.60  2003/07/07 18:38:58  hbrew
 * Auto increase patchlevel, set 0.9.6a-49
 * Fix gcc(mingw) warnings:
 *
 * getopt.c:   suggest explicit braces to avoid ambiguous `else'
 * nt/win9x.c: Avoid gcc warnings about non-handled enumeration values
 *
 * Revision 2.59  2003/07/07 18:38:27  hbrew
 * Auto increase patchlevel, set 0.9.6a-48
 * Fix gcc(mingw) warnings:
 *
 * getopt.c:   suggest explicit braces to avoid ambiguous `else'
 * nt/win9x.c: Avoid gcc warnings about non-handled enumeration values
 *
 * Revision 2.58  2003/07/07 10:39:27  gul
 * Auto increase patchlevel, set 0.9.6a-47
 * getopt() usage fix
 *
 * Revision 2.57  2003/07/07 10:14:22  gul
 * Auto increase patchlevel, set 0.9.6a-46
 * Use getopt() for commandline parse
 *
 * Revision 2.56  2003/07/07 10:13:56  gul
 * Auto increase patchlevel, set 0.9.6a-45
 * Use getopt() for commandline parse
 *
 * Revision 2.55  2003/07/07 08:42:06  val
 * Auto increase patchlevel, set 0.9.6a-44
 * check real length of SvPV() when importing queue element from perl
 *
 * Revision 2.54  2003/07/07 08:38:20  val
 * Auto increase patchlevel, set 0.9.6a-43
 * safe pkthdr-reading function (to byte order and struct alignment)
 *
 * Revision 2.53  2003/07/07 08:34:29  val
 * Auto increase patchlevel, set 0.9.6a-42
 * pmatch() replaced by define to xpmatch()
 *
 * Revision 2.52  2003/07/07 08:33:27  val
 * Auto increase patchlevel, set 0.9.6a-41
 * `perl-hooks' config keyword to specify perl script
 *
 * Revision 2.51  2003/07/06 10:34:29  gul
 * Auto increase patchlevel, set 0.9.6a-40
 * Migrate workaround of 100% CPU load with winsock from stable branch
 *
 * Revision 2.50  2003/07/06 10:18:59  gul
 * Auto increase patchlevel, set 0.9.6a-39
 * Increase loglevel for "Watinig for M_GOT" message
 *
 * Revision 2.49  2003/07/06 08:32:33  gul
 * Auto increase patchlevel, set 0.9.6a-38
 * Decrease logging about link status changes
 *
 * Revision 2.48  2003/07/06 06:49:45  gul
 * Auto increase patchlevel
 *
 * Revision 2.47  2003/07/06 06:48:28  gul
 * Auto increase patchlevel
 *
 * Revision 2.46  2003/07/04 08:13:20  val
 * Auto increase patchlevel
 *
 * Revision 2.45  2003/07/03 05:43:45  gul
 * Auto increase patchlevel
 *
 * Revision 2.44  2003/07/02 18:16:46  gul
 * Auto increase patchlevel
 *
 * Revision 2.43  2003/06/30 22:48:37  hbrew
 * Auto increase patchlevel
 *
 * Revision 2.42  2003/06/30 22:42:30  hbrew
 * Auto increase patchlevel
 *
 * Revision 2.41  2003/06/27 07:45:39  val
 * Auto increase patchlevel
 *
 * Revision 2.40  2003/06/26 13:22:26  gul
 * Auto increase patchlevel
 *
 * Revision 2.39  2003/06/26 13:21:33  gul
 * Auto increase patchlevel
 *
 * Revision 2.38  2003/06/26 12:53:35  gul
 * Auto increase patchlevel
 *
 * Revision 2.37  2003/06/26 10:34:05  val
 * Auto increase patchlevel
 *
 * Revision 2.36  2003/06/25 08:22:19  gul
 * Auto increase patchlevel
 *
 * Revision 2.35  2003/06/25 07:25:05  stas
 * Auto increase patchlevel
 *
 * Revision 2.34  2003/06/24 13:46:35  stas
 * Auto increase patchlevel
 *
 * Revision 2.33  2003/06/24 08:08:49  stas
 * Auto increase patchlevel
 *
 * Revision 2.32  2003/06/24 06:33:43  gul
 * Auto increase patchlevel
 *
 * Revision 2.31  2003/06/24 06:28:24  gul
 * Auto increase patchlevel
 *
 * Revision 2.30  2003/06/21 19:35:49  gul
 * Auto increase patchlevel
 *
 * Revision 2.29  2003/06/21 15:31:51  hbrew
 * Auto increase patchlevel
 *
 * Revision 2.28  2003/06/21 08:41:32  gul
 * Auto increase patchlevel
 *
 * Revision 2.27  2003/06/20 10:37:37  val
 * Auto increase patchlevel
 *
 * Revision 2.26  2003/06/13 03:10:09  hbrew
 * Auto increase patchlevel
 *
 * Revision 2.25  2003/06/12 12:04:14  gul
 * Auto increase patchlevel
 *
 * Revision 2.24  2003/06/12 08:31:00  val
 * Auto increase patchlevel
 *
 * Revision 2.23  2003/06/12 08:21:46  val
 * Auto increase patchlevel
 *
 * Revision 2.22  2003/06/12 08:21:43  val
 * 'skipmask' is replaced with 'skip', which allows more skipping features
 *
 * Revision 2.21  2003/06/11 13:10:37  gul
 * Auto increase patchlevel
 *
 * Revision 2.20  2003/06/11 09:01:16  stas
 * Auto increase patchlevel
 *
 * Revision 2.19  2003/06/11 07:44:26  gul
 * Auto increase patchlevel
 *
 * Revision 2.18  2003/06/10 19:15:28  gul
 * Auto increase patchlevel
 *
 * Revision 2.17  2003/06/10 12:29:16  stas
 * Auto increase patchlevel
 *
 * Revision 2.16  2003/06/10 07:43:37  gul
 * Auto increase patchlevel
 *
 * Revision 2.15  2003/06/10 07:28:28  gul
 * Auto increase patchlevel
 *
 * Revision 2.14  2003/06/09 13:27:31  stas
 * Auto increase patchlevel
 *
 * Revision 2.13  2003/06/08 13:41:07  cvs
 * Auto increase patchlevel
 *
 * Revision 2.12  2003/06/08 13:18:58  gul
 * Added patchlevel
 *
 * Revision 2.11  2003/06/06 16:16:34  gul
 * Change version
 *
 * Revision 2.10  2003/03/11 09:21:29  gul
 * Fixed OS/2 Watcom compilation
 *
 * Revision 2.9  2003/03/11 00:04:25  gul
 * Use patches for compile under MSDOS by MSC 6.0 with IBMTCPIP
 *
 * Revision 2.8  2003/03/10 12:16:53  gul
 * Use HAVE_DOS_H macro
 *
 * Revision 2.7  2003/03/10 10:39:23  gul
 * New include file common.h
 *
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
#define MYVER "0.9.6a-52"

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
#define CHECKCFG_INTERVAL rescan_delay
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

/* Pragmas */
#ifdef __WATCOMC__
  #pragma off (unreferenced);
#endif

#endif
