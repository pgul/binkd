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
#include "sys.h"                            /* Get system i/o headers */

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#if !defined(WIN32)
  #include <netinet/in.h>
  #include <netdb.h>			    /* One of these two should have
					     * MAXHOSTNAMELEN */
  #if defined(IBMTCPIP)
    #include <types.h>
  #endif
#endif

#ifdef HAVE_ARPA_INET_H
  #include <arpa/inet.h>
#endif

#ifndef MAXHOSTNAMELEN
  #define MAXHOSTNAMELEN 255		    /* max hostname size */
#endif

#define MAXSERVNAME 80                      /* max id len in /etc/services */

#ifdef IBMTCPIP
  #ifdef __WATCOMC__
    #define __IBMC__ 0
    #define __IBMCPP__ 0
  #endif
  #define BSD_SELECT
  #include <utils.h>
  #include <sys/select.h>
  #ifndef MAXSOCKETS
    #define MAXSOCKETS 2048
  #endif
#endif

#if !defined(WIN32)
  #include <sys/socket.h>
#else
  #include <winsock.h>
#endif

#if defined(IBMTCPIP)
const char *tcperr (void);

  #define TCPERR() tcperr()
  #define TCPERRNO (sock_errno())
  #include <errno.h>
  #undef ENAMETOOLONG
  #undef ENOTEMPTY
  #include <nerrno.h>
  #define TCPERR_WOULDBLOCK EWOULDBLOCK
  #define TCPERR_AGAIN EAGAIN
  #define sock_deinit()
#elif defined(WIN32)
const char *tcperr (void);

  #define TCPERR() tcperr()
  #define TCPERRNO (h_errno)
  #define TCPERR_WOULDBLOCK WSAEWOULDBLOCK
  #define TCPERR_AGAIN WSAEWOULDBLOCK
  #include "nt\wsock.h"
  #define sock_init() WinsockIni()
  #define sock_deinit() WinsockClean()
  #define soclose(h) closesocket(h)
#else
  #include <errno.h>
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
