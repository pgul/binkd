/*
 *  iphdr.h -- TCP/IP interface
 *
 *  iphdr.h is a part of binkd project
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
 * Revision 2.10.2.4  2004/07/13 17:31:10  stas
 * Fix the #include statement
 *
 * Revision 2.10.2.3  2003/08/24 18:58:34  gul
 * Bugfix in timeout check on win32
 *
 * Revision 2.10.2.2  2003/08/24 00:35:46  hbrew
 * Cosmetic change for previous patch
 *
 * Revision 2.10.2.1  2003/08/24 00:29:31  hbrew
 * win9x-select-workaround fix, thanks to Pavel Gulchouck)
 *
 * Revision 2.10  2003/06/04 10:36:58  stas
 * Thread-safety tcperr() implementation on Win32
 *
 * Revision 2.9  2003/03/30 10:14:40  gul
 * Use HAVE_SOCKLEN_T macro
 *
 * Revision 2.8  2003/03/26 13:53:28  gul
 * Fix OS/2 compilation
 *
 * Revision 2.7  2003/03/11 09:21:30  gul
 * Fixed OS/2 Watcom compilation
 *
 * Revision 2.6  2003/03/11 00:04:25  gul
 * Use patches for compile under MSDOS by MSC 6.0 with IBMTCPIP
 *
 * Revision 2.5  2003/03/10 18:16:10  gul
 * Define socklen_t for win32
 *
 * Revision 2.4  2003/03/10 12:16:53  gul
 * Use HAVE_DOS_H macro
 *
 * Revision 2.3  2003/03/01 20:16:27  gul
 * OS/2 IBM C support
 *
 * Revision 2.2  2003/03/01 18:37:08  gul
 * Use HAVE_SYS_PARAM_H macro
 *
 * Revision 2.1  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.0  2001/01/10 12:12:38  gul
 * Binkd is under CVS again
 *
 * Revision 1.4  1997/10/23  04:02:31  mff
 * many, many changes (forget to ci a version or two)
 *
 * Revision 1.2  1996/12/07  11:42:18  mff
 * soclose() for NT was defined as close(). Fixed.
 *
 *
 */
#ifndef _iphdrs_h
#define _iphdrs_h

#include <sys/types.h>

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#include "sys.h"                            /* Get system i/o headers */

#ifdef IBMTCPIP
  #include <errno.h>
  #undef ENAMETOOLONG
  #undef ENOTEMPTY

  #define BSD_SELECT
  #define __off_t
  #define __size_t
  #include <types.h>
  #include <utils.h>
  #include <unistd.h>
  #include <sys/select.h>
#endif

#if !defined(WIN32)
  #if defined(IBMTCPIPDOS)
    #include <sys/tcptypes.h>
  #endif

  #include <netinet/in.h>
  #include <netdb.h>			    /* One of these two should have
					     * MAXHOSTNAMELEN */
#endif

#ifdef HAVE_ARPA_INET_H
  #include <arpa/inet.h>
#endif

#if !defined(WIN32)
  #include <sys/socket.h>
#else
  #include <winsock.h>
#endif

#ifndef MAXHOSTNAMELEN
  #define MAXHOSTNAMELEN 255		    /* max hostname size */
#endif

#define MAXSERVNAME 80                      /* max id len in /etc/services */

#ifndef HAVE_SOCKLEN_T
  typedef int socklen_t;
#endif

#if defined(IBMTCPIP)
const char *tcperr (void);

  #define ReleaseErrorList()
  #define TCPERR() tcperr()
  #define TCPERRNO (sock_errno())
  #include <nerrno.h>
  #define TCPERR_WOULDBLOCK EWOULDBLOCK
  #define TCPERR_AGAIN EAGAIN
  #define sock_deinit()
  #ifndef MAXSOCKETS
    #define MAXSOCKETS 2048
  #endif
#elif defined(IBMTCPIPDOS)
const char *tcperr (void);

  #define ReleaseErrorList()
  #define TCPERR() tcperr()
  #define TCPERRNO (tcperrno)
  #include <stdio.h>
  #include <sys/errno.h>
  #undef ENAMETOOLONG
  #undef ENOTEMPTY
  #define TCPERR_WOULDBLOCK EWOULDBLOCK
  #define TCPERR_AGAIN EAGAIN
  #define sock_deinit()
#elif defined(WIN32)
const char *tcperr (int);
void ReleaseErrorList(void);

  #define TCPERR() tcperr(h_errno)
  #define TCPERRNO (h_errno)
  #define TCPERR_WOULDBLOCK WSAEWOULDBLOCK
  #define TCPERR_AGAIN WSAEWOULDBLOCK
  #include "nt/WSock.h"
  #define sock_init() WinsockIni()
  #define sock_deinit() WinsockClean()
  #define soclose(h) closesocket(h)
/* w9x_workaround_sleep: 1000000 = 1 sec, 10000 = 10 ms */
  #define w9x_workaround_sleep 10000
#else
  #include <errno.h>
  #define ReleaseErrorList()
  #define TCPERR() strerror(errno)
  #define TCPERRNO errno
  #define TCPERR_WOULDBLOCK EWOULDBLOCK
  #define TCPERR_AGAIN EAGAIN
  #define sock_init() 0
  #define sock_deinit()
  #define soclose(h) close(h)
#endif

#if !defined(WIN32)
typedef int SOCKET;

  #define INVALID_SOCKET (-1)
  #define SOCKET_ERROR (-1)
#endif

#ifndef INADDR_NONE
  #define INADDR_NONE -1
#endif

#endif
