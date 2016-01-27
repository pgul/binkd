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
      Log(1, "servmgr getaddrinfo: %s (%d)", gai_strerror(aiErr), aiErr);
      return -1;
    }

    for (ai = aiHead; ai != NULL && sockfd_used < MAX_LISTENSOCK; ai = ai->ai_next)
    {
      sockfd[sockfd_used] = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
      if (sockfd[sockfd_used] < 0)
      {
        Log(1, "servmgr socket(): %s", TCPERR ());
        return -1;
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
        Log(1, "servmgr bind(): %s", TCPERR ());
        soclose(sockfd[sockfd_used]);
        return -1;
      }
      if (listen (sockfd[sockfd_used], 5) != 0)
      {
        Log(1, "servmgr listen(): %s", TCPERR ());
        soclose(sockfd[sockfd_used]);
        return -1;
      }

      sockfd_used++;
    }

    Log (3, "servmgr listen on %s:%s", listen_list->addr[0] ? listen_list->addr : "*", listen_list->port);
  
    freeaddrinfo(aiHead);
  }

  if (sockfd_used == 0) {
    Log(1, "servmgr: No listen socket open");
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
        save_errno = TCPERRNO;
        if (binkd_exit)
          goto accepterr;
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
