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
 * Revision 2.24  2003/06/20 10:37:02  val
 * Perl hooks for binkd - initial revision
 *
 * Revision 2.23  2003/05/23 18:10:57  stas
 * Do not report errors when threads exits by exitfunc
 *
 * Revision 2.22  2003/05/04 08:45:30  gul
 * Lock semaphores more safely for resolve and IP-addr print
 *
 * Revision 2.21  2003/04/28 07:30:17  gul
 * Bugfix: Log() changes TCPERRNO
 *
 * Revision 2.20  2003/03/31 19:53:08  gul
 * Close socket before exit
 *
 * Revision 2.19  2003/03/26 13:53:28  gul
 * Fix OS/2 compilation
 *
 * Revision 2.18  2003/03/11 11:42:23  gul
 * Use event semaphores for exit threads
 *
 * Revision 2.17  2003/03/11 00:04:26  gul
 * Use patches for compile under MSDOS by MSC 6.0 with IBMTCPIP
 *
 * Revision 2.16  2003/03/10 17:32:37  gul
 * Use socklen_t
 *
 * Revision 2.15  2003/03/10 12:16:54  gul
 * Use HAVE_DOS_H macro
 *
 * Revision 2.14  2003/03/10 10:39:23  gul
 * New include file common.h
 *
 * Revision 2.13  2003/03/10 08:38:07  gul
 * Make n_servers/n_clients changes thread-safe
 *
 * Revision 2.12  2003/03/05 19:47:11  gul
 * Fix compilation warning
 *
 * Revision 2.11  2003/03/05 13:21:51  gul
 * Fix warnings
 *
 * Revision 2.10  2003/03/03 23:41:20  gul
 * Try to resolve problem with active threads while exitproc running
 *
 * Revision 2.9  2003/03/01 20:16:27  gul
 * OS/2 IBM C support
 *
 * Revision 2.8  2003/03/01 18:16:04  gul
 * Use HAVE_SYS_TIME_H macro
 *
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
#ifdef HAVE_SYS_TIME_H
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
#ifdef HAVE_DOS_H
#include <dos.h>
#endif
#include <process.h>
#elif !defined(DOS)
#error Must define either HAVE_FORK or HAVE_THREADS!
#endif

#include "Config.h"
#include "sys.h"
#include "iphdr.h"
#include "common.h"
#include "iptools.h"
#include "tools.h"
#include "readcfg.h"
#include "protocol.h"
#include "server.h"
#include "assert.h"
#include "setpttl.h"
#include "sem.h"
#if defined(WITH_PERL) && defined(HAVE_THREADS)
#include "perlhooks.h"
#endif

int n_servers = 0;
int ext_rand = 0;

#ifdef HAVE_FORK

static void chld (int signo)
{
#define CHILDCOUNT n_servers
#include "reapchld.inc"
}

#endif

SOCKET sockfd = INVALID_SOCKET;

void serv (void *arg)
{
  int h = *(int *) arg;
#if defined(WITH_PERL) && defined(HAVE_THREADS)
  void *cperl;
#endif

#ifdef HAVE_FORK
  pidcmgr = 0;
  soclose(sockfd);
  sockfd = INVALID_SOCKET;
#endif
#if defined(WITH_PERL) && defined(HAVE_THREADS)
  cperl = perl_init_clone();
#endif
  protocol (h, 0, NULL);
  Log (5, "downing server...");
#if defined(WITH_PERL) && defined(HAVE_THREADS)
  perl_done_clone(cperl);
#endif
  del_socket(h);
  soclose (h);
  free (arg);
  rel_grow_handles (-6);
#ifdef HAVE_THREADS
  threadsafe(--n_servers);
  PostSem(&eothread);
  _endthread();
#elif defined(DOS)
  --n_servers;
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
  socklen_t client_addr_len;
  struct sockaddr_in serv_addr, client_addr;
  int opt = 1;
  int save_errno;

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
	save_errno = TCPERRNO;
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
      save_errno = TCPERRNO;
      if (save_errno != EINVAL && save_errno != EINTR)
      {
	if (!binkd_exit)
	  Log (1, "accept: %s", TCPERR ());
#ifdef UNIX
	if (save_errno == ECONNRESET ||
	    save_errno == ETIMEDOUT ||
	    save_errno == EHOSTUNREACH)
	   continue;
#endif
accepterr:
#ifdef OS2
	if (save_errno == ENOTSOCK)
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

      add_socket(new_sockfd);
      rel_grow_handles (6);
      ext_rand=rand();
      get_hostname(&client_addr, host, sizeof(host));
      lockhostsem();
      Log (3, "incoming from %s (%s)", host,
	   inet_ntoa (client_addr.sin_addr));
      releasehostsem();

      /* Creating a new process for the incoming connection */
      threadsafe(++n_servers);
      if ((pid = branch (serv, (void *) &new_sockfd, sizeof (new_sockfd))) < 0)
      {
        rel_grow_handles (-6);
	threadsafe(--n_servers);
	PostSem(&eothread);
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
