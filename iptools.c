/*
 *  iptools.c -- Some useful TCP/IP utils
 *
 *  iptools.c is a part of binkd project
 *
 *  Copyright (C) 1997-1998  Dima Maloff, 5047/13
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
 * Revision 2.18  2012/01/08 14:09:04  green
 * Corrected initialization of getaddrinfo hints
 *
 * Revision 2.17  2012/01/07 11:54:04  green
 * Fix MSVC6 compilation errors
 *
 * Revision 2.16  2012/01/03 17:52:32  green
 * Implement FSP-1035 (SRV record usage)
 * - add SRV enabled getaddrinfo() wrapper (srv_gai.[ch])
 * - Unix (libresolv, autodetected) and Win32 support implemented
 * - Port information is stored as string now, i.e. may be service name
 *
 * Revision 2.15  2012/01/03 17:25:32  green
 * Implemented IPv6 support
 * - replace (almost) all getXbyY function calls with getaddrinfo/getnameinfo (RFC2553) calls
 * - Add compatibility layer for target systems not supporting RFC2553 calls in rfc2553.[ch]
 * - Add support for multiple listen sockets -- one for IPv4 and one for IPv6 (use V6ONLY)
 * - For WIN32 platform add configuration parameter IPV6 (mutually exclusive with BINKD9X)
 * - On WIN32 platform use Winsock2 API if IPV6 support is requested
 * - config: node IP address literal + port supported: [<ipv6 address>]:<port>
 *
 * Revision 2.14  2003/10/29 21:08:39  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.13  2003/10/07 17:57:09  gul
 * Some small changes in close threads function.
 * Inhibit errors "socket operation on non-socket" on break.
 *
 * Revision 2.12  2003/08/26 21:01:10  gul
 * Fix compilation under unix
 *
 * Revision 2.11  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.10  2003/05/26 20:37:59  gul
 * typo in previous patch
 *
 * Revision 2.9  2003/05/26 20:34:38  gul
 * Bugfix on resolving raw IP when HAVE_FORK
 *
 * Revision 2.8  2003/05/04 08:45:30  gul
 * Lock semaphores more safely for resolve and IP-addr print
 *
 * Revision 2.7  2003/03/26 12:59:16  gul
 * Fix previous patch
 *
 * Revision 2.6  2003/03/26 10:44:40  gul
 * Code cleanup
 *
 * Revision 2.5  2003/03/25 20:37:46  gul
 * free_hostent() function
 *
 * Revision 2.4  2003/03/01 18:46:05  gul
 * Use HAVE_SYS_IOCTL_H macro
 *
 * Revision 2.3  2003/02/28 08:53:38  gul
 * Fixed proxy usage
 *
 * Revision 2.2  2003/02/22 12:12:33  gul
 * Cleanup sources
 *
 * Revision 2.1  2003/02/22 11:45:41  gul
 * Do not resolve hosts if proxy or socks5 using
 *
 * Revision 2.0  2001/01/10 12:12:38  gul
 * Binkd is under CVS again
 *
 * Revision 1.3  1998/06/19  05:19:33  mff
 * changes in get_hostname()
 *
 * Revision 1.2  1997/10/23  04:01:29  mff
 * +find_port(), minor changes for Amiga port
 *
 * Revision 1.1  1997/03/28  06:52:14  mff
 * Initial revision
 */

#include "iphdr.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#if defined(HAVE_SYS_IOCTL_H)
#include <sys/ioctl.h>
#endif

#include "Config.h"
#include "common.h"
#include "iptools.h"
#include "tools.h"
#include "sem.h"
#include "rfc2553.h"

/*
 * Sets non-blocking mode for a given socket
 */
void setsockopts (SOCKET s)
{

#if defined(FIONBIO)
#if defined(UNIX) || defined(IBMTCPIP) || defined(AMIGA)
  int arg;

  arg = 1;
  if (ioctl (s, FIONBIO, (char *) &arg, sizeof arg) < 0)
    Log (1, "ioctl (FIONBIO): %s", TCPERR ());

#elif defined(WIN32)
  u_long arg;

  arg = 1;
  if (ioctlsocket (s, FIONBIO, &arg) < 0)
    if (!binkd_exit)
      Log (1, "ioctlsocket (FIONBIO): %s", TCPERR ());
#endif
#endif

#if defined(UNIX) || defined(EMX) || defined(AMIGA)
  if (fcntl (s, F_SETFL, O_NONBLOCK) == -1)
    Log (1, "fcntl: %s", strerror (errno));
#endif
}

/*
 * Find the appropriate port string to be used.
 * Find_port ("") will return binkp's port from /etc/services or even 
 * (if there is no binkp entry) 24554.
 * Returns NULL on error.
 */
char * find_port (char *s)
{
  char *ps = NULL;
  struct addrinfo *aiHead, hints;
  int aiErr;

  /* setup hints for getaddrinfo */
  memset((void *)&hints, 0, sizeof(hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  aiErr = getaddrinfo(NULL, (s && *s) ? s : PRTCLNAME, &hints, &aiHead);
  if (aiErr == 0)
  {
    ps = (s && *s) ? s : PRTCLNAME;
    freeaddrinfo(aiHead);
  }
  else
  if (s == NULL || *s == 0)
    ps = DEF_PORT;

  if (ps == NULL)
    Log (1, "%s: incorrect port (getaddrinfo: %s)", s, gai_strerror(aiErr));

  return ps;
}

int sockaddr_cmp_addr(const struct sockaddr *a, const struct sockaddr *b)
{
  if (a->sa_family != b->sa_family)
    return a->sa_family - b->sa_family;
  
  if (a->sa_family == AF_INET)
    return (((struct sockaddr_in*)a)->sin_addr.s_addr - ((struct sockaddr_in*)b)->sin_addr.s_addr);
#ifdef AF_INET6
  else if (a->sa_family == AF_INET6)
    return memcmp((char *) &(((struct sockaddr_in6*)a)->sin6_addr), 
		  (char *) &(((struct sockaddr_in6*)b)->sin6_addr),
		  sizeof(((struct sockaddr_in6*)a)->sin6_addr));
#endif
  else
  {
    Log(2, "Unsupported address family: %d", a->sa_family);
    return -1;
  }
}

int sockaddr_cmp_port(const struct sockaddr *a, const struct sockaddr *b)
{
  if (a->sa_family != b->sa_family)
    return a->sa_family - b->sa_family;
  
  if (a->sa_family == AF_INET)
    return (((struct sockaddr_in*)a)->sin_port - ((struct sockaddr_in*)b)->sin_port);
#ifdef AF_INET6
  else if (a->sa_family == AF_INET6)
    return (((struct sockaddr_in6*)a)->sin6_port - ((struct sockaddr_in6*)b)->sin6_port);
#endif
  else
  {
    Log(2, "Unsupported address family: %d", a->sa_family);
    return -1;
  }
}
