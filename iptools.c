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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#if defined(HAVE_SYS_IOCTL_H)
#include <sys/ioctl.h>
#endif

#include "sys.h"
#include "Config.h"
#include "iphdr.h"
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
    if (!binkd_exit && TCPERRNO != WSAENOTSOCK)
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
  hints.ai_flags = AI_PASSIVE;

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
