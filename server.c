/*
 *  server.c -- Handles inbound connections
 *
 *  server.c is a part of binkd project
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
 * Revision 2.7  2003/03/01 15:55:02  gul
 * Current outgoing address is now attibute of session, but not node
 *
 * Revision 2.6  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.5  2003/02/22 12:12:34  gul
 * Cleanup sources
 *
 * Revision 2.4  2002/11/12 16:55:58  gul
 * Run as service under win9x
 *
 * Revision 2.3  2001/09/14 07:23:06  gul
 * bindaddr bugfix, did not work on freebsd
 *
 * Revision 2.2  2001/08/24 13:23:28  da
 * binkd/binkd.c
 * binkd/readcfg.c
 * binkd/readcfg.h
 * binkd/server.c
 * binkd/nt/service.c
 *
 * Revision 2.1  2001/05/23 16:48:04  gul
 * msvc warnings fixed
 *
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 * Revision 1.8  1997/10/23  03:39:42  mff
 * DUP() removed
 *
 * Revision 1.7  1997/06/16  05:41:14  mff
 * Added exit(3) on config change.
 *
 * Revision 1.6  1997/03/09  07:14:49  mff
 * Now we use HOSTNAME()
 *
 * Revision 1.5  1997/02/07  06:42:59  mff
 * Under UNIXs SIGHUP forces binkd to restart
 *
 * Revision 1.4  1997/02/01  05:55:24  mff
 * Changed SIGCHLD support
 *
 * Revision 1.2  1996/12/14  07:10:52  mff
 * We now use branch(). Listening changed.
 */

#include <sys/types.h>
#include <time.h>
#ifndef VISUALCPP
#include <sys/time.h>
#endif
#include <sys/stat.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef HAVE_FORK
#include <signal.h>
#include <setjmp.h>
#elif defined(HAVE_THREADS)
#include <dos.h>
#include <process.h>
#else
#error Must define either HAVE_FORK or HAVE_THREADS!
#endif

#include "Config.h"
#include "iphdr.h"
#include "iptools.h"
#include "tools.h"
#include "readcfg.h"
#include "protocol.h"
#include "server.h"
#include "sys.h"
#include "assert.h"
#include "setpttl.h"
#include "sem.h"

int n_servers = 0;
int ext_rand = 0;

#ifdef HAVE_FORK

static void chld (int signo)
{
#define CHILDCOUNT n_servers
#include "reapchld.inc"
}

#endif

SOCKET sockfd = (SOCKET)-1;
extern int checkcfg_flag;	       /* exit(3) on config change */

void serv (void *arg)
{
  int h = *(int *) arg;
  extern int pidcmgr;

  pidcmgr = 0;
#ifdef HAVE_FORK
  soclose(sockfd);
#endif
  protocol (h, 0, NULL);
  Log (5, "downing server...");
  soclose (h);
  free (arg);
  rel_grow_handles (-6);
#ifdef HAVE_THREADS
  --n_servers;
  _endthread();
#endif
}

int checkcfg (void)
{
  struct stat sb;
  struct conflist_type *pc;
#ifdef HAVE_FORK
  extern jmp_buf jb;
#endif

  for (pc = config_list; pc; pc = pc->next)
  {
    if (pc->path == NULL || stat (pc->path, &sb))
      continue;
    if (pc->mtime == 0)
    {
      pc->mtime = (unsigned long)sb.st_mtime;
    }
    else if ((time_t)pc->mtime != sb.st_mtime)
    {
      soclose (sockfd);
#if defined(HAVE_FORK)
      Log (2, "%s changed! Restart binkd...", pc->path);
      longjmp(jb, 1);
#else
#if defined(BINKDW9X)
      Log (2, "%s changed! Restart binkd...", pc->path);
#else
      Log (2, "%s changed! exit(3)...", pc->path);
#endif
      checkcfg_flag=2;
#endif
      exit (3);
    }
  }
  return 0;
}

void servmgr (void *arg)
{
  SOCKET new_sockfd;
  int pid;
  int client_addr_len;
  struct sockaddr_in serv_addr, client_addr;
  int opt = 1;

  srand(time(0));
  setproctitle ("server manager");
  Log (4, "servmgr started");

  /* Store initial value for Binkd config's mtime */
  checkcfg ();

#ifdef HAVE_FORK
  signal (SIGCHLD, chld);
#endif

  if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    Log (0, "socket: %s", TCPERR ());

  if (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR,
		  (char *) &opt, sizeof opt) == SOCKET_ERROR)
    Log (1, "setsockopt (SO_REUSEADDR): %s", TCPERR ());

  memset(&serv_addr, 0, sizeof serv_addr);
  serv_addr.sin_family = AF_INET;
  if (bindaddr[0])
    serv_addr.sin_addr.s_addr = inet_addr (bindaddr);
  else
    serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  serv_addr.sin_port = htons ((unsigned short) iport);

  if (bind (sockfd, (struct sockaddr *) & serv_addr, sizeof (serv_addr)) != 0)
    Log (0, "bind: %s", TCPERR ());

  listen (sockfd, 5);
  setproctitle ("server manager (listen %u)", (unsigned) iport);

  for (;;)
  {
    struct timeval tv;
    fd_set r;

    FD_ZERO (&r);
    FD_SET (sockfd, &r);
    tv.tv_usec = 0;
    tv.tv_sec  = CHECKCFG_INTERVAL;
    switch (select(sockfd+1, &r, NULL, NULL, &tv))
    { case 0: /* timeout */
        /* Test config mtime */
        if (checkcfg_flag)
          checkcfg();
        continue;
      case -1:
        if (TCPERRNO == EINTR)
          continue;
	Log (1, "select: %s", TCPERR ());
        goto accepterr;
    }

    /* Test config mtime */
    if (checkcfg_flag)
      checkcfg ();

    client_addr_len = sizeof (client_addr);
    if ((new_sockfd = accept (sockfd, (struct sockaddr *) & client_addr,
			      &client_addr_len)) == INVALID_SOCKET)
    {
      if (TCPERRNO != EINVAL && TCPERRNO != EINTR)
      {
	Log (1, "accept: %s", TCPERR ());
#ifdef UNIX
	if (TCPERRNO == ECONNRESET ||
	    TCPERRNO == ETIMEDOUT ||
	    TCPERRNO == EHOSTUNREACH)
	   continue;
#endif
accepterr:
#ifdef OS2
	if (TCPERRNO == ENOTSOCK)
	{ /* os/2 ugly hack */
	  if (config_list)
	  { config_list->mtime--;
	    checkcfg();
	  }
	}
#endif
        exit(1);
      }
    }
    else
    { char host[MAXHOSTNAMELEN + 1];
      rel_grow_handles (6);

      ext_rand=rand();
      lockhostsem();
      Log (3, "incoming from %s (%s)",
	   get_hostname(&client_addr, host, sizeof(host)),
	   inet_ntoa (client_addr.sin_addr));
      releasehostsem();

      /* Creating a new process for the incoming connection */
      ++n_servers;
      if ((pid = branch (serv, (void *) &new_sockfd, sizeof (new_sockfd))) < 0)
      {
        rel_grow_handles (-6);
	--n_servers;
	Log (1, "cannot branch out");
        sleep(1);
      }
      else
      {
	Log (5, "started server #%i, id=%i", n_servers, pid);
#ifdef HAVE_FORK
	soclose (new_sockfd);
#endif
      }
    }
  }
}
