/*
 *  client.c -- Outbound calls
 *
 *  client.c is a part of binkd project
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
 * Revision 1.1  2001/01/10 11:34:57  gul
 * Initial revision
 *
 * Revision 1.12  1998/06/19  05:21:12  mff
 * Oport was broken
 *
 * Revision 1.11  1997/11/04  23:33:55  mff
 * Fixed a minor bug in walking through hostlists
 *
 * Revision 1.10  1997/10/23  04:16:36  mff
 * host lists, more?
 *
 * Revision 1.9  1997/03/09  07:17:59  mff
 * Support for -p key
 *
 * Revision 1.8  1997/02/07  07:11:53  mff
 * We now restart sleep() after signals
 *
 * Revision 1.7  1997/02/01  05:55:24  mff
 * Changed SIGCHLD support
 *
 * Revision 1.6  1997/02/01  05:37:22  mff
 * Support for new queue scanning
 *
 * Revision 1.4  1996/12/14  07:03:26  mff
 * Now we use branch()
 *
 * Revision 1.3  1996/12/09  03:37:21  mff
 * Changed call-out logic
 *
 * Revision 1.2  1996/12/07  12:03:02  mff
 * Now fork()'s or _beginthread()'s
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

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
#include "sys.h"
#include "client.h"
#include "readcfg.h"
#include "iphdr.h"
#include "iptools.h"
#include "ftnq.h"
#include "tools.h"
#include "protocol.h"
#include "bsy.h"
#include "assert.h"
#include "setpttl.h"

#ifdef HTTPS
#include "https.h"
#endif

#ifdef HAVE_THREADS
#include "sem.h"
extern MUTEXSEM hostsem;
#ifdef OS2
extern MUTEXSEM fhsem;
#endif
#endif

int checkcfg (void);
static void call (void *arg);

int n_clients = 0;

#ifdef HAVE_FORK

static jmp_buf jmpbuf;
extern int connect_timeout;

static void chld (int signo)
{
  if (signo == SIGALRM)
    longjmp(jmpbuf, 1);
#define CHILDCOUNT n_clients
#include "reapchld.inc"
}

#endif

#if defined(VOID_SLEEP) || !defined(HAVE_FORK)
#define SLEEP(x) sleep(x)
#else

void SLEEP (time_t s)
{
  while ((s = sleep (s)) > 0);
}

#endif

#if defined(HAVE_THREADS) && defined(OS2)
void rel_grow_handles(int nh)
{ LONG addfh=0;
  static ULONG curmaxfh=0;

  LockSem(&fhsem);
  if (curmaxfh == 0)
  { if (DosSetRelMaxFH(&addfh, &curmaxfh))
    { Log(1, "Cannot DosSetRelMaxFH");
      return;
    }
  }
  if ((addfh=_grow_handles((int)(curmaxfh += nh))) < curmaxfh)
    Log(1, "Cannot grow handles to %ld (now %ld): %s", curmaxfh, addfh, strerror(errno));
  else
    Log(6, "Set MaxFH to %ld (res %ld)", curmaxfh, addfh);
  ReleaseSem(&fhsem);
}
#endif

void clientmgr (void *arg)
{
  extern int pidcmgr;
  extern int poll_flag;
  extern int server_flag;
  extern int client_flag;
  extern int checkcfg_flag;	       /* exit(3) on config change */

  int pid;
  int q_empty = 1;

#ifdef HAVE_FORK
  signal (SIGCHLD, chld);
#endif

  pidcmgr = 0;

  setproctitle ("client manager");
  Log (4, "clientmgr started");

  while (1)
  {
    FTN_NODE *r;

    if (checkcfg_flag && client_flag && !server_flag && !poll_flag)
      checkcfg();
    if (q_empty)
    {
      q_free (SCAN_LISTED);
      if (printq)
	Log (-1, "scan\r");
      q_scan (SCAN_LISTED);
      q_empty = !q_not_empty (SCAN_LISTED);
      if (printq)
      {
	q_list (stderr, SCAN_LISTED);
	Log (-1, "idle\r");
      }
    }
    if (n_clients < max_clients)
    {
      if ((r = q_next_node ()) != 0 &&
	  bsy_test (&r->fa, F_BSY) &&
	  bsy_test (&r->fa, F_CSY))
      {
#if defined(HAVE_THREADS) && defined(OS2)
        rel_grow_handles (6);
#endif
	++n_clients;
	if ((pid = branch (call, (void *) r, sizeof (*r))) < 0)
	{
#if defined(HAVE_THREADS) && defined(OS2)
          rel_grow_handles (-6);
#endif
	  --n_clients;
	  Log (1, "cannot branch out");
          SLEEP(1);
	}
	else
	{
	  Log (5, "started client #%i, id=%i", n_clients, pid);
	}
      }
      else
      {
	if (poll_flag && n_clients <= 0 && q_not_empty (SCAN_LISTED) == 0)
	{
	  Log (4, "the queue is empty, quitting...");
	  break;
	}
	q_empty = 1;
	SLEEP (rescan_delay);
      }
    }
    else
    {
      SLEEP (call_delay);
    }
  }
  exit (0);
}

static int call0 (FTN_NODE *node)
{
  int sockfd = INVALID_SOCKET;
  struct hostent he;
  struct hostent *hp;
  struct sockaddr_in sin;
  struct in_addr defaddr;
  char **cp;
  char szDestAddr[FTN_ADDR_SZ + 1];
  char *alist[2];
  int i, rc;

  char host[MAXHOSTNAMELEN + 1];       /* current host/port */
  unsigned short port;

  ftnaddress_to_str (szDestAddr, &node->fa);
  Log (2, "call to %s", szDestAddr);
  setproctitle ("call to %s", szDestAddr);

  for (i = 1; sockfd == INVALID_SOCKET
       && (rc = get_host_and_port
	   (i, host, &port, node->hosts, &node->fa)) != -1; ++i)
  {
    if (rc == 0)
    {
      Log (1, "%s: %i: error parsing host list", node->hosts, i);
      continue;
    }

    if (!isdigit (host[0]) ||
	(defaddr.s_addr = inet_addr (host)) == INADDR_NONE)
    {
      /* If not a raw ip address, try nameserver */
      Log (5, "resolving `%s'...", host);
#ifdef HAVE_THREADS
      LockSem(&hostsem);
#endif
      if ((hp = gethostbyname (host)) == NULL)
      {
	Log (1, "%s: unknown host", host);
	bad_try (&node->fa, "Cannot gethostbyname");
#ifdef HAVE_THREADS
        ReleaseSem(&hostsem);
#endif
	continue;
      }
#ifdef HAVE_THREADS
      copy_hostent(&he, hp);
      hp = &he;
      ReleaseSem(&hostsem);
#endif
    }
    else
    {
      /* Raw ip address, fake */
      hp = &he;
      hp->h_name = host;
      hp->h_aliases = 0;
      hp->h_addrtype = AF_INET;
      hp->h_length = sizeof (struct in_addr);
      hp->h_addr_list = alist;
      hp->h_addr_list[0] = (char *) &defaddr;
      hp->h_addr_list[1] = (char *) 0;
    }
    sin.sin_family = hp->h_addrtype;
    sin.sin_port = htons (port);

    /* Trying... */

    for (cp = hp->h_addr_list; cp && *cp; cp++)
    {
      if ((sockfd = socket (hp->h_addrtype, SOCK_STREAM, 0)) == INVALID_SOCKET)
      {
	Log (1, "socket: %s", TCPERR ());
	return 0;
      }
      sin.sin_addr = *((struct in_addr *) * cp);
#ifdef HAVE_THREADS
      LockSem(&hostsem);
#endif
      Log (4, port == DEF_PORT ? "trying %s..." : "trying %s, port %u...",
	   inet_ntoa (sin.sin_addr), (unsigned) port);
#ifdef HAVE_THREADS
      ReleaseSem(&hostsem);
#endif
#ifdef HAVE_FORK
      if (connect_timeout)
      {
	if (setjmp(jmpbuf))
	{
	  errno = ETIMEDOUT;
	  goto badtry;
	}
	signal(SIGALRM, chld);
	alarm(connect_timeout);
      }
#endif
#ifdef HTTPS
      if (h_connect (&sockfd, &sin) == 0)
#else
      if (connect (sockfd, (struct sockaddr *) & sin, sizeof (sin)) == 0)
#endif
      {
#ifdef HAVE_FORK
	alarm(0);
	signal(SIGALRM, SIG_DFL);
#endif
	Log (4, "connected");
	break;
      }

#ifdef HAVE_FORK
badtry:
      alarm(0);
      signal(SIGALRM, SIG_DFL);
#endif
      Log (1, "unable to connect: %s", TCPERR ());
      bad_try (&node->fa, TCPERR ());
      soclose (sockfd);
      sockfd = INVALID_SOCKET;
    }
#ifdef HAVE_THREADS
    if (hp->h_addr_list != alist)
    {
      if (hp->h_addr_list && hp->h_addr_list[0])
        free(hp->h_addr_list[0]);
      if (hp->h_addr_list)
        free(hp->h_addr_list);
    }
#endif
  }

  if (sockfd == INVALID_SOCKET)
    return 0;

  protocol (sockfd, node);
  soclose (sockfd);
  return 1;
}

static void call (void *arg)
{
  FTN_NODE *node = (FTN_NODE *) arg;

  if (bsy_add (&node->fa, F_CSY))
  {
    call0 (node);
    bsy_remove (&node->fa, F_CSY);
  }
  free (arg);
#ifdef HAVE_THREADS
#ifdef OS2
  rel_grow_handles(-6);
#endif
  --n_clients;
  _endthread();
#endif
}
