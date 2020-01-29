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

  #ifdef HAVE_NETINET_IN_H
    #include <netinet/in.h>
  #endif
  #ifdef HAVE_NETDB_H
    #include <netdb.h>			    /* One of these two should have
					     * MAXHOSTNAMELEN */
  #endif
#endif

#ifdef HAVE_ARPA_INET_H
  #include <arpa/inet.h>
#endif

#if !defined(WIN32)
  #include <sys/socket.h>
#endif

/* Some systems have MAXHOSTNAMELEN = 64 */
#ifdef NI_MAXHOST
  #define BINKD_FQDNLEN NI_MAXHOST	    /* max length for getnameinfo */
#else
  #define BINKD_FQDNLEN 255		    /* max FQDN size */
#endif

#ifdef NI_MAXSERV
  #define MAXSERVNAME NI_MAXSERV	    /* max length for getnameinfo */
#else
  #define MAXSERVNAME 80                    /* max id len in /etc/services */
#endif

#define MAXPORTSTRLEN 32

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
  #include <sys/errno.h>
  #undef ENAMETOOLONG
  #undef ENOTEMPTY
  #define TCPERR_WOULDBLOCK EWOULDBLOCK
  #define TCPERR_AGAIN EAGAIN
  #define sock_deinit()
#elif defined(WIN32)
const char *w32err (int);
void ReleaseErrorList(void);

  #include <errno.h>
  #define TCPERR() w32err(h_errno)
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

/* OS/2 doesn't support IPv6, we are trying to detect availability of
 * some structures based on this define
 */
#ifdef OS2
  #undef AF_INET6
#endif

#endif
