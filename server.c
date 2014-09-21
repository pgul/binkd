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
 * Revision 2.62  2014/09/21 08:44:51  gul
 * Write configured remote hostname and port in log line "outgoing session with ..."
 *
 * Revision 2.61  2014/08/13 20:50:54  gul
 * Fixed IPv6 support with MSVC build
 *
 * Revision 2.60  2014/08/09 13:58:04  gul
 * Fix servmgr exit after incoming session (broken in 1.1a-50)
 *
 * Revision 2.59  2014/08/02 09:54:04  gul
 * Fix in signal handling
 *
 * Revision 2.58  2014/02/02 07:46:47  gul
 * Set FD_CLOEXEC on listening socket
 *
 * Revision 2.57  2014/01/12 13:25:31  gul
 * unix (linux) pthread version
 *
 * Revision 2.56  2013/02/04 12:47:12  gul
 * New config option "listen"
 *
 * Revision 2.55  2012/09/20 12:16:54  gul
 * Added "call via external pipe" (for example ssh) functionality.
 * Added "-a", "-f" options, removed obsoleted "-u" and "-i" (for win32).
 *
 * Revision 2.54  2012/05/14 06:14:59  gul
 * More safe signal handling
 *
 * Revision 2.53  2012/01/08 19:18:03  green
 * Improved hostname lookup and logging
 *
 * Revision 2.52  2012/01/08 17:34:58  green
 * Avoid using MAXHOSTNAMELEN
 *
 * Revision 2.51  2012/01/08 14:09:04  green
 * Corrected initialization of getaddrinfo hints
 *
 * Revision 2.50  2012/01/07 23:38:45  green
 * Improved getnameinfo handling, retry without name resolution
 *
 * Revision 2.49  2012/01/07 16:56:28  green
 * Improved getnameinfo error handling
 *
 * Revision 2.48  2012/01/07 16:34:00  green
 * Add error id where gai_strerror() is used
 *
 * Revision 2.47  2012/01/07 11:54:04  green
 * Fix MSVC6 compilation errors
 *
 * Revision 2.46  2012/01/06 11:33:31  gul
 * Format error
 *
 * Revision 2.45  2012/01/03 17:52:32  green
 * Implement FSP-1035 (SRV record usage)
 * - add SRV enabled getaddrinfo() wrapper (srv_gai.[ch])
 * - Unix (libresolv, autodetected) and Win32 support implemented
 * - Port information is stored as string now, i.e. may be service name
 *
 * Revision 2.44  2012/01/03 17:25:32  green
 * Implemented IPv6 support
 * - replace (almost) all getXbyY function calls with getaddrinfo/getnameinfo (RFC2553) calls
 * - Add compatibility layer for target systems not supporting RFC2553 calls in rfc2553.[ch]
 * - Add support for multiple listen sockets -- one for IPv4 and one for IPv6 (use V6ONLY)
 * - For WIN32 platform add configuration parameter IPV6 (mutually exclusive with BINKD9X)
 * - On WIN32 platform use Winsock2 API if IPV6 support is requested
 * - config: node IP address literal + port supported: [<ipv6 address>]:<port>
 *
 * Revision 2.43  2009/05/31 07:16:17  gul
 * Warning: many changes, may be unstable.
 * Perl interpreter is now part of config and rerun on config reload.
 * Perl 5.10 compatibility.
 * Changes in outbound queue managing and sorting.
 *
 * Revision 2.42  2009/02/04 20:13:47  gul
 * Possible remote DoS (thx to Konstantin Kuzov 2:5019/40)
 *
 * Revision 2.41  2007/10/06 10:20:05  gul
 * more accurate checkcfg()
 *
 * Revision 2.40  2007/10/04 17:30:28  gul
 * SIGHUP handler (thx to Sergey Babitch)
 *
 * Revision 2.39  2004/11/07 13:20:13  stream
 * Lock config of server manager so it can be safely reloaded in SIGHUP
 *
 * Revision 2.38  2004/08/04 19:51:40  gul
 * Change SIGCHLD handling, make signal handler more clean,
 * prevent occasional hanging (mutex deadlock) under linux kernel 2.6.
 *
 * Revision 2.37  2003/10/29 21:08:39  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.36  2003/10/19 10:28:10  gul
 * Minor DEBUGCHILD fix
 *
 * Revision 2.35  2003/10/07 20:50:07  gul
 * Wait for servmanager exit from exitproc()
 * (Patch from Alexander Reznikov)
 *
 * Revision 2.34  2003/10/07 17:57:09  gul
 * Some small changes in close threads function.
 * Inhibit errors "socket operation on non-socket" on break.
 *
 * Revision 2.33  2003/09/21 17:51:08  gul
 * Fixed PID in logfile for perl stderr handled messages in fork version.
 *
 * Revision 2.32  2003/09/05 06:49:07  val
 * Perl support restored after config reloading patch
 *
 * Revision 2.31  2003/08/26 22:18:48  gul
 * Fix compilation under w32-mingw and os2-emx
 *
 * Revision 2.30  2003/08/26 21:01:10  gul
 * Fix compilation under unix
 *
 * Revision 2.29  2003/08/26 16:06:27  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.28  2003/08/25 06:11:06  gul
 * Fix compilation with HAVE_FORK
 *
 * Revision 2.27  2003/08/24 13:30:33  stream
 * Socket wasn't closed if branch() failed
 *
 * Revision 2.26  2003/08/23 15:51:51  stream
 * Implemented common list routines for all linked records in configuration
 *
 * Revision 2.25  2003/08/18 09:41:00  gul
 * Little cleanup in handle perl errors
 *
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

#include <stdlib.h>
#include <string.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif
#ifdef HAVE_FORK
#include <signal.h>
#include <sys/wait.h>
#endif

#include "sys.h"
#include "iphdr.h"
#include "readcfg.h"
#include "common.h"
#include "server.h"
#include "iptools.h"
#include "tools.h"
#include "protocol.h"
#include "assert.h"
#include "setpttl.h"
#include "sem.h"
#if defined(WITH_PERL)
#include "perlhooks.h"
#endif
#include "rfc2553.h"

int n_servers = 0;
int ext_rand = 0;

SOCKET sockfd[MAX_LISTENSOCK];
int sockfd_used = 0;

static void serv (void *arg)
{
  int h = *(int *) arg;
  BINKD_CONFIG *config;
#if defined(WITH_PERL) && defined(HAVE_THREADS)
  void *cperl;
#endif

#if defined(HAVE_FORK) && !defined(HAVE_THREADS) && !defined(DEBUGCHILD)
  int curfd;
  pidcmgr = 0;
  for (curfd=0; curfd<sockfd_used; curfd++)
    soclose(sockfd[curfd]);
#endif

  config = lock_current_config();
#if defined(WITH_PERL) && defined(HAVE_THREADS)
  cperl = perl_init_clone(config);
#endif
  protocol (h, h, NULL, NULL, NULL, NULL, NULL, config);
  Log (5, "downing server...");
#if defined(WITH_PERL) && defined(HAVE_THREADS)
  perl_done_clone(cperl);
#endif
  del_socket(h);
  soclose (h);
  free (arg);
  unlock_config_structure(config, 0);
  rel_grow_handles (-6);
#ifdef HAVE_THREADS
  threadsafe(--n_servers);
  PostSem(&eothread);
  ENDTHREAD();
#elif defined(DOS) || defined(DEBUGCHILD)
  --n_servers;
#endif
}

/*
 * Server manager.
 */

static int do_server(BINKD_CONFIG *config)
{
  struct addrinfo *ai, *aiHead, hints;
  int aiErr;
  SOCKET new_sockfd;
  int pid;
  socklen_t client_addr_len;
  struct sockaddr_storage client_addr;
  int opt = 1;
  int save_errno;
  struct listenchain *listen_list;

  /* setup hints for getaddrinfo */
  memset((void *)&hints, 0, sizeof(hints));
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  for (listen_list = config->listen.first; listen_list; listen_list = listen_list->next)
  {
    if ((aiErr = getaddrinfo(listen_list->addr[0] ? listen_list->addr : NULL, 
                             listen_list->port, &hints, &aiHead)) != 0)
    {
      Log(0, "servmgr getaddrinfo: %s (%d)", gai_strerror(aiErr), aiErr);
      return -1;
    }

    for (ai = aiHead; ai != NULL && sockfd_used < MAX_LISTENSOCK; ai = ai->ai_next)
    {
      sockfd[sockfd_used] = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
      if (sockfd[sockfd_used] < 0)
      {
        Log (0, "servmgr socket(): %s", TCPERR ());
        continue;
      }
#ifdef UNIX /* Not sure how to set NOINHERIT flag for socket on Windows and OS/2 */
      if (fcntl(sockfd[sockfd_used], F_SETFD, FD_CLOEXEC) != 0)
        Log(1, "servmgr fcntl set FD_CLOEXEC error: %s", strerror(errno));
#endif
#ifdef IPV6_V6ONLY
      if (ai->ai_family == PF_INET6)
      {
        int v6only = 1;
        if (setsockopt(sockfd[sockfd_used], IPPROTO_IPV6, IPV6_V6ONLY, 
                 (char *) &v6only, sizeof(v6only)) == SOCKET_ERROR)
          Log(1, "servmgr setsockopt (IPV6_V6ONLY): %s", TCPERR());
      }
#endif
      if (setsockopt (sockfd[sockfd_used], SOL_SOCKET, SO_REUSEADDR,
                    (char *) &opt, sizeof opt) == SOCKET_ERROR)
        Log (1, "servmgr setsockopt (SO_REUSEADDR): %s", TCPERR ());
    
      if (bind (sockfd[sockfd_used], ai->ai_addr, ai->ai_addrlen) != 0)
      {
        Log (0, "servmgr bind(): %s", TCPERR ());
        soclose(sockfd[sockfd_used]);
        continue;
      }
      if (listen (sockfd[sockfd_used], 5) != 0)
      {
        Log(0, "servmgr listen(): %s", TCPERR ());
        soclose(sockfd[sockfd_used]);
        continue;
      }

      sockfd_used++;
    }

    Log (3, "servmgr listen on %s:%s", listen_list->addr[0] ? listen_list->addr : "*", listen_list->port);
  
    freeaddrinfo(aiHead);
  }

  if (sockfd_used == 0) {
    Log(0, "servmgr: No listen socket open");
    return -1;
  }

  setproctitle ("server manager (listen %s)", config->listen.first->port);

  for (;;)
  {
    struct timeval tv;
    int n;
    int curfd, maxfd = 0;
    fd_set r;

    FD_ZERO (&r);
    for (curfd=0; curfd<sockfd_used; curfd++)
    {
      FD_SET (sockfd[curfd], &r);
      if (sockfd[curfd] > maxfd)
        maxfd = sockfd[curfd];
    }
    tv.tv_usec = 0;
    tv.tv_sec  = CHECKCFG_INTERVAL;
    unblocksig();
    check_child(&n_servers);
    n = select(maxfd+1, &r, NULL, NULL, &tv);
    blocksig();
    switch (n)
    { case 0: /* timeout */
        if (checkcfg()) 
        {
          for (curfd=0; curfd<sockfd_used; curfd++)
            soclose(sockfd[curfd]);
          sockfd_used = 0;
          return 0;
        }
        unblocksig();
        check_child(&n_servers);
        blocksig();
        continue;
      case -1:
        if (TCPERRNO == EINTR)
        {
          unblocksig();
          check_child(&n_servers);
          blocksig();
          if (checkcfg())
          {
            for (curfd=0; curfd<sockfd_used; curfd++)
              soclose(sockfd[curfd]);
            sockfd_used = 0;
            return 0;
          }
          continue;
        }
        save_errno = TCPERRNO;
        if (!binkd_exit) /* Suppress servmgr socket error at binkd exit */
          Log (1, "servmgr select(): %s", TCPERR ());
        goto accepterr;
    }
 
    for (curfd=0; curfd<sockfd_used; curfd++)
    {
      if (!FD_ISSET(sockfd[curfd], &r))
        continue;

      client_addr_len = sizeof (client_addr);
      if ((new_sockfd = accept (sockfd[curfd], (struct sockaddr *)&client_addr,
                                &client_addr_len)) == INVALID_SOCKET)
      {
        save_errno = TCPERRNO;
        if (save_errno != EINVAL && save_errno != EINTR)
        {
          if (!binkd_exit)
            Log (1, "servmgr accept(): %s", TCPERR ());
#ifdef UNIX
          if (save_errno == ECONNRESET ||
              save_errno == ETIMEDOUT ||
              save_errno == ECONNABORTED ||
              save_errno == EHOSTUNREACH)
            continue;
#endif
        accepterr:
#ifdef OS2
          /* Buggy external process closed our socket? Or OS/2 bug? */
          if (save_errno == ENOTSOCK)
            return 0;  /* will force socket re-creation */
#endif
          return -1;
        }
      }
      else
      {
        char host[BINKD_FQDNLEN + 1];
        char service[MAXSERVNAME + 1];
        int aiErr;
  
        add_socket(new_sockfd);
        /* Was the socket created after close_sockets loop in exitfunc()? */
        if (binkd_exit)
        {
          del_socket(new_sockfd);
          soclose(new_sockfd);
          continue;
        }
        rel_grow_handles (6);
        ext_rand=rand();
        /* never resolve name in here, will be done during session */
        aiErr = getnameinfo((struct sockaddr *)&client_addr, client_addr_len,
            host, sizeof(host), service, sizeof(service),
            NI_NUMERICHOST | NI_NUMERICSERV);
        if (aiErr == 0) 
          Log (3, "incoming from %s (%s)", host, service);
        else
        {
          Log(2, "Error in getnameinfo(): %s (%d)", gai_strerror(aiErr), aiErr);
          Log(3, "incoming from unknown");
        }
  
        /* Creating a new process for the incoming connection */
        threadsafe(++n_servers);
        if ((pid = branch (serv, (void *) &new_sockfd, sizeof (new_sockfd))) < 0)
        {
          del_socket(new_sockfd);
          soclose(new_sockfd);
          rel_grow_handles (-6);
          threadsafe(--n_servers);
          PostSem(&eothread);
          Log (1, "servmgr branch(): cannot branch out");
          sleep(1);
        }
        else
        {
          Log (5, "started server #%i, id=%i", n_servers, pid);
#if defined(HAVE_FORK) && !defined(HAVE_THREADS)
          soclose (new_sockfd);
#endif
        }
      }
    }
  }
}

void servmgr (void)
{
  int status;
  BINKD_CONFIG *config;

  srand(time(0));
  setproctitle ("server manager");
  Log (4, "servmgr started");

#if defined(HAVE_FORK) && !defined(HAVE_THREADS)
  blocksig();
  signal (SIGCHLD, sighandler);
#endif

  /* Loop on socket (listen can be changed by reload)
   * do_server() will return 0 to restart and -1 to terminate
   */
  do
  {
    config = lock_current_config();
    status = do_server(config);
    unlock_config_structure(config, 0);
  } while (status == 0 && !binkd_exit);
  Log(4, "downing servmgr...");
  pidsmgr = 0;
  PostSem(&eothread);
}
