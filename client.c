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
 * Revision 2.29  2003/03/28 14:01:10  gul
 * Do not call _endthread() without _beginthread() in client only mode
 *
 * Revision 2.28  2003/03/26 10:44:40  gul
 * Code cleanup
 *
 * Revision 2.27  2003/03/25 20:37:46  gul
 * free_hostent() function
 *
 * Revision 2.26  2003/03/19 14:36:03  gul
 * Fix typo
 *
 * Revision 2.25  2003/03/14 21:58:33  gul
 * Changed function SLEEP() to define for multithread version
 *
 * Revision 2.24  2003/03/11 14:11:01  gul
 * Bugfix
 *
 * Revision 2.23  2003/03/11 11:42:23  gul
 * Use event semaphores for exit threads
 *
 * Revision 2.22  2003/03/11 00:04:25  gul
 * Use patches for compile under MSDOS by MSC 6.0 with IBMTCPIP
 *
 * Revision 2.21  2003/03/10 15:57:52  gul
 * Fixed segfault on unresolvable host
 *
 * Revision 2.20  2003/03/10 12:16:53  gul
 * Use HAVE_DOS_H macro
 *
 * Revision 2.19  2003/03/10 10:39:23  gul
 * New include file common.h
 *
 * Revision 2.18  2003/03/10 08:38:07  gul
 * Make n_servers/n_clients changes thread-safe
 *
 * Revision 2.17  2003/03/06 18:24:00  gul
 * Fix exitfunc with threads
 *
 * Revision 2.16  2003/03/05 13:21:50  gul
 * Fix warnings
 *
 * Revision 2.15  2003/03/05 11:40:12  gul
 * Fix win32 compilation
 *
 * Revision 2.14  2003/03/04 09:56:00  gul
 * Fix threads compilation
 *
 * Revision 2.13  2003/03/03 23:41:20  gul
 * Try to resolve problem with active threads while exitproc running
 *
 * Revision 2.12  2003/03/01 20:16:27  gul
 * OS/2 IBM C support
 *
 * Revision 2.11  2003/03/01 15:55:02  gul
 * Current outgoing address is now attibute of session, but not node
 *
 * Revision 2.10  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.9  2003/02/28 08:53:38  gul
 * Fixed proxy usage
 *
 * Revision 2.8  2003/02/27 20:34:37  gul
 * Fix proxy usage
 *
 * Revision 2.7  2003/02/27 18:52:37  gul
 * bugfix in proxy using
 *
 * Revision 2.6  2003/02/23 07:20:11  gul
 * Restore lost comment
 *
 * Revision 2.5  2003/02/22 19:30:22  gul
 * Fix compiler warning
 *
 * Revision 2.4  2003/02/22 12:12:33  gul
 * Cleanup sources
 *
 * Revision 2.3  2003/02/22 11:45:41  gul
 * Do not resolve hosts if proxy or socks5 using
 *
 * Revision 2.2  2002/04/02 13:10:32  gul
 * Put real remote addr to log "session with ..." if connect via socks or proxy
 *
 * Revision 2.1  2001/09/14 07:24:20  gul
 * bindaddr works on outgoing connections now
 *
 * Revision 2.0  2001/01/10 12:12:37  gul
 * Binkd is under CVS again
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
#include "client.h"
#include "readcfg.h"
#include "common.h"
#include "iptools.h"
#include "ftnq.h"
#include "tools.h"
#include "protocol.h"
#include "bsy.h"
#include "assert.h"
#include "setpttl.h"
#include "sem.h"

#ifdef HTTPS
#include "https.h"
#endif

static void call (void *arg);

int n_clients = 0;

#ifdef HAVE_FORK

static jmp_buf jmpbuf;

static void chld (int signo)
{
  if (signo == SIGALRM)
    longjmp(jmpbuf, 1);
#define CHILDCOUNT n_clients
#include "reapchld.inc"
}

#endif

#if defined(HAVE_THREADS)
#define SLEEP(x) WaitSem(&exitcmgr, x)
#elif defined(__MSC__)
void SLEEP (time_t s)
{
  struct timeval tv;
  tv.tv_sec = s;
  tv.tv_usec = 0;
  select(0, NULL, NULL, NULL, &tv);
}
#elif defined(VOID_SLEEP) || !defined(HAVE_FORK)
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
#ifdef __WATCOMC__
  if ((addfh=_grow_handles((int)(curmaxfh += nh))) < curmaxfh)
#else
  addfh=nh;
  if (DosSetRelMaxFH(&addfh, &curmaxfh))
#endif
    Log(1, "Cannot grow handles to %ld (now %ld): %s", curmaxfh, addfh, strerror(errno));
  else
    Log(6, "Set MaxFH to %ld (res %ld)", curmaxfh, addfh);
  ReleaseSem(&fhsem);
}
#endif

void clientmgr (void *arg)
{
  int pid;
  int q_empty = 1;

#ifdef HAVE_FORK
  pidcmgr = 0;
  signal (SIGCHLD, chld);
#elif HAVE_THREADS
  pidcmgr = PID();
#endif

  setproctitle ("client manager");
  Log (4, "clientmgr started");

  while (!binkd_exit)
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
      q_empty = !q_not_empty ();
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
        rel_grow_handles (6);
	threadsafe(++n_clients);
	if ((pid = branch (call, (void *) r, sizeof (*r))) < 0)
	{
          rel_grow_handles (-6);
	  threadsafe(--n_clients);
	  PostSem(&eothread);
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
	if (poll_flag && n_clients <= 0 && q_not_empty () == 0)
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
#ifdef HAVE_THREADS
  pidcmgr = 0;
  PostSem(&eothread);
  _endthread();
#else
  exit (0);
#endif
}

static int call0 (FTN_NODE *node)
{
  int sockfd = INVALID_SOCKET;
  struct hostent he;
  struct hostent *hp = NULL; /* prevent compiler warning */
  struct sockaddr_in sin;
  char **cp;
  char szDestAddr[FTN_ADDR_SZ + 1];
  struct in_addr defaddr;
  int i, rc;
  char host[MAXHOSTNAMELEN + 1];       /* current host/port */
  unsigned short port;

  ftnaddress_to_str (szDestAddr, &node->fa);
  Log (2, "call to %s", szDestAddr);
  setproctitle ("call to %s", szDestAddr);
  memset(&sin, 0, sizeof(sin));

#ifdef HTTPS
  if (proxy[0] || socks[0])
  {
    char *sp, *sport;
    strncpy(host, proxy[0] ? proxy: socks, sizeof(host));
    if ((sp=strchr(host, ':')) != NULL)
    {
      *sp++ = '\0';
      sport = sp;
      if ((sp=strchr(sp, '/')) != NULL)
	*sp++ = '\0';
    }
    else
    {
      if((sp=strchr(host, '/')) != NULL)
	*sp++ = '\0';
      sport = proxy[0] ? "squid" : "socks"; /* default port */
    }
    if (!isdigit(*sport))
    { struct servent *se;
      lockhostsem();
      if ((se = getservbyname(sport, "tcp")) == NULL)
      {
	Log(2, "Port %s not found, try default %d", sp, proxy[0] ? 3128 : 1080);
	sin.sin_port = htons((unsigned short)(proxy[0] ? 3128 : 1080));
      } else
	sin.sin_port = se->s_port;
      releasehostsem();
    }
    else
      sin.sin_port = htons((unsigned short)atoi(sport));
    /* resolve proxy host */
    if ((hp = find_host(host, &he, &defaddr)) == NULL)
    {
      Log(1, "%s host %s not found", proxy[0] ? "Proxy" : "Socks", host);
      return 0;
    }
  }
#endif

  for (i = 1; sockfd == INVALID_SOCKET
       && (rc = get_host_and_port
	   (i, host, &port, node->hosts, &node->fa)) != -1; ++i)
  {
    if (rc == 0)
    {
      Log (1, "%s: %i: error parsing host list", node->hosts, i);
      continue;
    }
#ifdef HTTPS
    if (!proxy[0] && !socks[0]) /* don't resolve if proxy or socks specified */
#endif
    {
      if ((hp = find_host(host, &he, &defaddr)) == NULL)
      {
        bad_try(&node->fa, "Cannot gethostbyname");
        continue;
      }
      sin.sin_port = htons(port);
    }

    /* Trying... */

    for (cp = hp->h_addr_list; cp && *cp; cp++)
    {
      if ((sockfd = socket (hp->h_addrtype, SOCK_STREAM, 0)) == INVALID_SOCKET)
      {
	Log (1, "socket: %s", TCPERR ());
	return 0;
      }
      add_socket(sockfd);
      sin.sin_addr = *((struct in_addr *) * cp);
      sin.sin_family = hp->h_addrtype;
      lockhostsem();
#ifdef HTTPS
      if (proxy[0] || socks[0])
      {
	char *sp = strchr(host, ':');
	if (sp) *sp = '\0';
	if (port == oport)
	  Log (4, "trying %s via %s %s:%u...", host,
	       proxy[0] ? "proxy" : "socks", inet_ntoa (sin.sin_addr), 
	       ntohs(sin.sin_port));
	else
	  Log (4, "trying %s:%u via %s %s:%u...", host, port,
	       proxy[0] ? "proxy" : "socks", inet_ntoa (sin.sin_addr), 
	       ntohs(sin.sin_port));
	sprintf(host+strlen(host), ":%u", port);
      }
      else
#endif
	Log (4, port == oport ? "trying %s..." : "trying %s:%u...",
	     inet_ntoa (sin.sin_addr), (unsigned) port);
      releasehostsem();
      if (bindaddr[0])
      {
        struct sockaddr_in src_sin;
        memset(&src_sin, 0, sizeof(src_sin));
        src_sin.sin_addr.s_addr = inet_addr(bindaddr);
        src_sin.sin_family = AF_INET;
        if (bind(sockfd, (struct sockaddr *)&src_sin, sizeof(src_sin)))
          Log(4, "bind: %s", TCPERR());
      }
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
      if (connect (sockfd, (struct sockaddr *) & sin, sizeof (sin)) == 0)
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
      del_socket(sockfd);
      soclose (sockfd);
      sockfd = INVALID_SOCKET;
    }
#ifdef HAVE_THREADS
#ifdef HTTPS
    if (!proxy[0] && !socks[0])
#endif
      free_hostent(hp);
#endif
#ifdef HTTPS
    if (sockfd != INVALID_SOCKET && (proxy[0] || socks[0]) &&
        h_connect(sockfd, host) != 0)
    {
      bad_try (&node->fa, TCPERR ());
      del_socket(sockfd);
      soclose (sockfd);
      sockfd = INVALID_SOCKET;
    }
#endif
  }
#if defined(HAVE_THREADS) && defined(HTTPS)
  if (proxy[0] || socks[0])
    free_hostent(hp);
#endif

  if (sockfd == INVALID_SOCKET)
    return 0;

  protocol (sockfd, node, host);
  del_socket(sockfd);
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
  rel_grow_handles(-6);
#ifdef HAVE_THREADS
  threadsafe(--n_clients);
  if (server_flag) {
    PostSem(&eothread);
    _endthread();
  }
#elif defined(DOS)
  --n_clients;
#endif
}
