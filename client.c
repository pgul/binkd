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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#if defined(HAVE_FORK) || defined(WITH_PTHREADS)
#include <signal.h>
#include <sys/wait.h>
#endif

#include "sys.h"
#include "readcfg.h"
#include "client.h"
#include "ftnnode.h"
#include "ftnaddr.h"
#include "common.h"
#include "iptools.h"
#include "ftnq.h"
#include "tools.h"
#include "protocol.h"
#include "bsy.h"
#include "assert.h"
#include "setpttl.h"
#include "sem.h"
#include "run.h"
#if defined(WITH_PERL)
#include "perlhooks.h"
#endif

#ifdef HTTPS
#include "https.h"
#endif
#include "rfc2553.h"
#include "srv_gai.h"

static void call (void *arg);

int n_clients = 0;

#ifdef AF_INET6
#define NO_INVALID_ADDRESSES 2
#else
#define NO_INVALID_ADDRESSES 1
#endif
static struct sockaddr_storage invalidAddresses[NO_INVALID_ADDRESSES];

#if defined(HAVE_FORK) && !defined(HAVE_THREADS)

static void alrm (int signo)
{
}

#endif

#if defined(HAVE_THREADS)
#define SLEEP(x) WaitSem(&wakecmgr, x)
#else
#define SLEEP(x) sleep(x)
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

struct call_args
{
    FTN_NODE     *node;
    BINKD_CONFIG *config;
};

/*
 * Run one client loop. Return -1 to exit
 */
static int do_client(BINKD_CONFIG *config)
{
  FTN_NODE *r;
  int pid;

  if (!config->q_present)
  {
    q_free (SCAN_LISTED, config);
    if (config->printq)
      Log (-1, "scan\r");
    q_scan (SCAN_LISTED, config);
    config->q_present = 1;
    if (config->printq)
    {
      LockSem (&lsem);
      q_list (stderr, SCAN_LISTED, config);
      ReleaseSem (&lsem);
      Log (-1, "idle\r");
    }
  }
  if (n_clients < config->max_clients)
  {
    if ((r = q_next_node (config)) != 0)
    {
      struct call_args args;

      if (!bsy_test (&r->fa, F_BSY, config) ||
          !bsy_test (&r->fa, F_CSY, config))
      {
        char szDestAddr[FTN_ADDR_SZ + 1];

        ftnaddress_to_str (szDestAddr, &r->fa);
        Log (4, "%s busy, skipping", szDestAddr);
        return 0; /* go to the next node */
      }
      rel_grow_handles (6);
      threadsafe(++n_clients);
      lock_config_structure(config);
      args.node   = r;
      args.config = config;
      if ((pid = branch (call, &args, sizeof (args))) < 0)
      {
        unlock_config_structure(config, 0);
        rel_grow_handles (-6);
        threadsafe(--n_clients);
        PostSem(&eothread);
        Log (1, "cannot branch out");
        SLEEP(1);
      }
#if !defined(DEBUGCHILD)
      else
      {
        Log (5, "started client #%i, id=%i", n_clients, pid);
#if defined(HAVE_FORK) && !defined(HAVE_THREADS) && !defined(AMIGA)
        unlock_config_structure(config, 0); /* Forked child has own copy */
#endif
      }
#endif
    }
    else
    {
      int need_sleep = config->rescan_delay;
      time_t start_sleep = time(NULL), end_sleep;

      unblocksig();
      while (need_sleep > 0 && !binkd_exit
#if defined(HAVE_FORK)
             && !got_sighup
#endif
            )
      {
        check_child(&n_clients);
        if (poll_flag && n_clients <= 0)
        {
          blocksig();
          if (q_not_empty(config) == 0)
          {
            Log (4, "the queue is empty, quitting...");
            return -1;
          }
          unblocksig();
        }
        SLEEP (need_sleep);
        end_sleep = time(NULL);
        if (end_sleep > start_sleep)
          need_sleep -= (int)(end_sleep - start_sleep);
        start_sleep = end_sleep;
      }
      check_child(&n_clients);
      blocksig();
      if (!poll_flag)
        config->q_present = 0;
    }
  }
  else
  {
    /* This sleep can be interrupted by signal, it's OK */
    unblocksig();
    check_child(&n_clients);
    SLEEP (config->call_delay);
    check_child(&n_clients);
    blocksig();
  }

  return 0;
}

void clientmgr (void *arg)
{
  int status;
  BINKD_CONFIG *config = NULL;
#if defined(WITH_PERL) && defined(HAVE_THREADS)
  void *cperl = NULL;
#endif

  UNUSED_ARG(arg);

 /*
  * This strange casting dance is to work around systems that are
  * supported by binkd (OS/2 and Windows 9x),
  * but do not support `struct sockaddr_storage`.
  * This is not needed for IPv6, since we assume that if the target
  * platform is new enough to understand IPv6, it probably also
  * understands `sockaddr_storage`.
  * Some compilers (MSVC6 as example) have rudimental AF_INET6,
  * but don't have `struct sockaddr_storage`.
  * So we have to check not only AF_INET6, but more defines.
  */

  /* Initialize invalid addresses. static variables are guaranteed to be initialized to 0, so no need to specify all members */
  ((struct sockaddr *)&invalidAddresses[0])->sa_family = AF_INET;
#if defined(AF_INET6) && defined(_SS_MAXSIZE)
  invalidAddresses[1].ss_family = AF_INET6;
#endif

#ifdef HAVE_THREADS
  pidcmgr = PID();
#elif defined(HAVE_FORK)
  pidcmgr = 0;
  pidCmgr = (int) getpid();
  blocksig();
  signal (SIGCHLD, sighandler);
#endif

  config = lock_current_config();
#if defined(WITH_PERL) && defined(HAVE_THREADS)
  if (server_flag)
    cperl = perl_init_clone(config);
#endif

#ifndef HAVE_THREADS
  setproctitle ("client manager");
#endif
  Log (4, "clientmgr started");

  for (;;)
  {
    if (config != current_config)
    {
#if defined(WITH_PERL) && defined(HAVE_THREADS)
      if (server_flag && cperl)
        perl_done_clone(cperl);
#endif
      if (config)
        unlock_config_structure(config, 0);
      config = lock_current_config();
#if defined(WITH_PERL) && defined(HAVE_THREADS)
      if (server_flag)
        cperl = perl_init_clone(config);
#endif
    }
    status = do_client(config);

    if (status != 0 || binkd_exit) break;

    if (
#ifdef HAVE_THREADS
        !server_flag &&
#endif
        !poll_flag)
      checkcfg();
  }

  Log (5, "downing clientmgr...");

#if defined(WITH_PERL) && defined(HAVE_THREADS)
  if (server_flag && cperl)
    perl_done_clone(cperl);
#endif
  unlock_config_structure(config, 0);

  unblocksig();
#ifdef HAVE_THREADS
  pidcmgr = 0;
  if (server_flag) {
    PostSem(&eothread);
    if (binkd_exit)
      ENDTHREAD();
  }
#endif
  exit (0);
}

static int call0 (FTN_NODE *node, BINKD_CONFIG *config)
{
  int sockfd = INVALID_SOCKET;
  int sock_out;
  char szDestAddr[FTN_ADDR_SZ + 1];
  int i, j, rc, pid = -1;
  char host[BINKD_FQDNLEN + 5 + 1];       /* current host/port */
  char addrbuf[BINKD_FQDNLEN + 1];
  char servbuf[MAXSERVNAME + 1];
  char *hosts;
  char port[MAXPORTSTRLEN + 1] = { 0 };
  char *dst_ip = NULL;
  const char *save_err;
#ifdef HTTPS
  int use_proxy;
  char *proxy, *socks;
  struct addrinfo *aiProxyHead;
#endif
  struct addrinfo *ai, *aiNodeHead, *aiHead, hints;
#ifdef AF_FORCE
  struct addrinfo *aiNewHead;
#endif
  int aiErr;

  /* setup hints for getaddrinfo */
  memset((void *)&hints, 0, sizeof(hints));
  hints.ai_family = node->IP_afamily;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

#ifdef WITH_PERL
  hosts = xstrdup(node->hosts);
#ifdef HTTPS
  proxy = xstrdup(config->proxy);
  socks = xstrdup(config->socks);
#endif
  if (!perl_on_call(node, config, &hosts
#ifdef HTTPS
                    , &proxy, &socks
#endif
                    )) {
    Log(1, "call aborted by Perl on_call()");
    return 0;
  }
#else
  hosts = node->hosts;
#ifdef HTTPS
  proxy = config->proxy;
  socks = config->socks;
#endif
#endif

  ftnaddress_to_str (szDestAddr, &node->fa);
  Log (2, "call to %s", szDestAddr);
#ifndef HAVE_THREADS
  setproctitle ("call to %s", szDestAddr);
#endif

#ifdef HTTPS
  use_proxy = (node->NP_flag != NP_ON) && (!node->pipe || !node->pipe[0]) && (proxy[0] || socks[0]);
  if (use_proxy)
  {
    char *sp, *sport;
    strncpy(host, proxy[0] ? proxy : socks, sizeof(host));
    if ((sp=strchr(host, ':')) != NULL)
    {
      *sp++ = '\0';
      sport = sp;
      if ((sp=strchr(sp, '/')) != NULL)
        *sp++ = '\0';
    }
    else
    {
      if ((sp=strchr(host, '/')) != NULL)
        *sp++ = '\0';
      sport = proxy[0] ? "squid" : "socks"; /* default port */
    }
    /* resolve proxy host */
    if ( (aiErr = srv_getaddrinfo(host, sport, &hints, &aiProxyHead)) != 0)
    {
        Log(2, "Port %s not found, try default %d", sp, proxy[0] ? 3128 : 1080);
        aiErr = getaddrinfo(host, proxy[0] ? "3128" : "1080", &hints, &aiProxyHead);
    }
    if (aiErr != 0)
    {
      Log(1, "%s host %s not found", proxy[0] ? "Proxy" : "Socks", host);
#ifdef WITH_PERL
      xfree(hosts);
      xfree(proxy);
      xfree(socks);
#endif
      return 0;
    }
  }
#endif

  for (i = 1; sockfd == INVALID_SOCKET
       && (rc = get_host_and_port
           (i, host, port, hosts, &node->fa, config)) != -1; ++i)
  {
    if (rc == 0)
    {
      Log (1, "%s: %i: error parsing host list", hosts, i);
      continue;
    }

    pid = -1;
    if (node->pipe && node->pipe[0])
    {
      char *cmdline = strdup(node->pipe);
      cmdline = ed(cmdline, "*H", host, NULL);
      cmdline = ed(cmdline, "*I", port, NULL);
      pid = run3(cmdline, &sock_out, &sockfd, NULL);
      free(cmdline);
      if (pid != -1)
      {
        Log (4, "connected");
        add_socket(sock_out);
        break;
      }
      if (!binkd_exit)
      {
        Log (1, "connection to %s failed");
        /* bad_try (&node->fa, "exec error", BAD_CALL, config); */
      }
      sockfd = INVALID_SOCKET;
      continue;
    }

#ifdef HTTPS
    if (use_proxy)
      aiHead = aiProxyHead;
    else /* don't resolve if proxy or socks specified */
#endif
    {
      aiErr = srv_getaddrinfo(host, port, &hints, &aiNodeHead);

      if (aiErr != 0)
      {
        Log(2, "getaddrinfo failed: %s (%d)", gai_strerror(aiErr), aiErr);
        bad_try(&node->fa, "Cannot getaddrinfo", BAD_CALL, config);
        continue;
      }
      aiHead = aiNodeHead;
    }
#ifdef AF_INET6
#ifdef AF_FORCE
    /* Soft address family force ai list reorder */
    /* Soft IPv6 force */
    if (aiHead->ai_family == AF_INET && node->AFF_flag == 6)
    {
       for (ai = aiHead; ai != NULL; ai = ai->ai_next)
       {
          if (ai->ai_family == AF_INET && ai->ai_next != NULL && ai->ai_next->ai_family == AF_INET6)
          {
             aiNewHead = ai->ai_next;
	     ai->ai_next = aiNewHead->ai_next;
	     aiNewHead->ai_next = aiHead;
	     aiHead = aiNewHead;
	     break;
          }
       }
    }
    /* Soft IPv4 force */
    else if (aiHead->ai_family == AF_INET6 && node->AFF_flag == 4)
    {
       for (ai = aiHead; ai != NULL; ai = ai->ai_next)
       {
          if (ai->ai_family == AF_INET6 && ai->ai_next != NULL && ai->ai_next->ai_family == AF_INET)
          {
             aiNewHead = ai->ai_next;
	     ai->ai_next = aiNewHead->ai_next;
	     aiNewHead->ai_next = aiHead;
	     aiHead = aiNewHead;
	     break;
          }
       }
    }
#endif
#endif
    /* Trying... */

    for (ai = aiHead; ai != NULL && sockfd == INVALID_SOCKET; ai = ai->ai_next)
    {
      for (j = 0; j < NO_INVALID_ADDRESSES; j++)
        if (0 == sockaddr_cmp_addr(ai->ai_addr, (struct sockaddr *)&invalidAddresses[j]))
        {
#if defined(AF_INET6) && defined(_SS_MAXSIZE)
          const int l = invalidAddresses[j].ss_family == AF_INET6
                      ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);
#else
          const int l = sizeof(struct sockaddr_in);
#endif
          rc = getnameinfo( (struct sockaddr *)&invalidAddresses[j], l, addrbuf, sizeof(addrbuf)
                          , NULL, 0, NI_NUMERICHOST );
          if (rc != 0)
            Log(2, "Error in getnameinfo(): %s (%d)", gai_strerror(rc), rc);
          else
            Log(1, "Invalid address: %s", addrbuf);

          break;
        }
      if (j < NO_INVALID_ADDRESSES)
        /* try possible next address */
        continue;

      if ((sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) == INVALID_SOCKET)
      {
        Log (1, "socket: %s", TCPERR ());

        /* try possible next address */
        continue;
      }
      add_socket(sockfd);
      /* Was the socket created after close_sockets loop in exitfunc()? */
      if (binkd_exit)
      {
#ifdef WITH_PERL
        xfree(hosts);
#ifdef HTTPS
        xfree(proxy);
        xfree(socks);
#endif
#endif
        freeaddrinfo(aiHead);
        return 0;
      }
      rc = getnameinfo(ai->ai_addr, ai->ai_addrlen, addrbuf, sizeof(addrbuf),
                       servbuf, sizeof(servbuf), NI_NUMERICHOST | NI_NUMERICSERV);
      if (rc != 0) {
        Log (2, "Error in getnameinfo(): %s (%d)", gai_strerror(rc), rc);
        snprintf(addrbuf, BINKD_FQDNLEN, "invalid");
        *servbuf = '\0';
      }

#ifdef HTTPS
      if (use_proxy)
      {
        char *sp = strchr(host, ':');
        if (sp) *sp = '\0';
        if (strcmp (port, config->oport) == 0)
          Log (4, "trying %s via %s %s:%s...", host,
               proxy[0] ? "proxy" : "socks", addrbuf, servbuf);
        else
          Log (4, "trying %s:%s via %s %s:%s...", host, port,
               proxy[0] ? "proxy" : "socks", addrbuf, servbuf);
      }
      else
#endif
      {
        if (strcmp (port, config->oport) == 0)
          Log (4, "trying %s [%s]...", host, addrbuf);
        else
          Log (4, "trying %s [%s]:%s...", host, addrbuf, servbuf);
        dst_ip = addrbuf;
        strnzcpy (port, servbuf, MAXPORTSTRLEN);
      }
      /* find bind addr with matching address family */
      if (config->bindaddr[0])
      {
        struct addrinfo *src_ai, src_hints;

        memset((void *)&src_hints, 0, sizeof(src_hints));
        src_hints.ai_socktype = SOCK_STREAM;
        src_hints.ai_family = ai->ai_family;
        src_hints.ai_protocol = IPPROTO_TCP;
        if ((aiErr = getaddrinfo(config->bindaddr, NULL, &src_hints, &src_ai)) == 0)
        {
          if (bind(sockfd, src_ai->ai_addr, src_ai->ai_addrlen))
            Log(4, "bind: %s", TCPERR());
          freeaddrinfo(src_ai);
        }
        else
          if (aiErr == EAI_FAMILY)
            /* address family of target and bind address don't match */
            continue;
          else
            /* otherwise just warn and don't bind() */
            Log(2, "bind -- getaddrinfo: %s (%d)", gai_strerror(aiErr), aiErr);
      }
#if defined(HAVE_FORK) && !defined(HAVE_THREADS)
      if (config->connect_timeout)
      {
        signal(SIGALRM, alrm);
        alarm(config->connect_timeout);
      }
#endif
      if (connect (sockfd, ai->ai_addr, ai->ai_addrlen) == 0)
      {
#if defined(HAVE_FORK) && !defined(HAVE_THREADS)
        alarm(0);
#endif
        Log (4, "connected");
        sock_out = sockfd;
        break;
      }

#if defined(HAVE_FORK) && !defined(HAVE_THREADS)
      if (errno == EINTR && config->connect_timeout)
        save_err = strerror (ETIMEDOUT);
      else
        save_err = TCPERR ();
      alarm(0);
#else
      save_err = TCPERR ();
#endif
      if (!binkd_exit)
      {
        Log (1, "connection to %s failed: %s", szDestAddr, save_err);
        bad_try (&node->fa, save_err, BAD_CALL, config);
      }
      del_socket(sockfd);
      soclose (sockfd);
      sockfd = INVALID_SOCKET;
    }
#ifdef HTTPS
    if (!use_proxy)
#endif
      freeaddrinfo(aiNodeHead);
#ifdef HTTPS
    if (sockfd != INVALID_SOCKET && use_proxy) {
      if (h_connect(sockfd, host, port, config, proxy, socks) != 0) {
        if (!binkd_exit)
          bad_try (&node->fa, TCPERR (), BAD_CALL, config);
        del_socket(sockfd);
        soclose (sockfd);
        sockfd = INVALID_SOCKET;
      }
    }
#endif
  }
#ifdef HTTPS
  if (use_proxy)
    freeaddrinfo(aiProxyHead);
#endif
#ifdef WITH_PERL
  xfree(hosts);
#ifdef HTTPS
  xfree(proxy);
  xfree(socks);
#endif
#endif

  if (sockfd == INVALID_SOCKET)
    return 0;

  protocol (sockfd, sock_out, node, NULL, host, port, dst_ip, config);
  if (pid != -1)
  {
    del_socket(sock_out);
    close(sock_out);
#ifdef HAVE_WAITPID
    if (waitpid (pid, &rc, 0) == -1)
    {
      Log (1, "waitpid(%u) error: %s", pid, strerror(errno));
    }
    else
    {
      if (WIFSIGNALED(rc))
        Log (2, "process %u exited by signal %u", pid, WTERMSIG(rc));
      else
        Log (4, "rc(%u)=%u", pid, WEXITSTATUS(rc));
    }
#endif
    close(sockfd);
  }
  else
  {
    del_socket(sockfd);
    soclose (sockfd);
  }
  return 1;
}

static void call (void *arg)
{
  struct call_args *a = arg;
  char szDestAddr[FTN_ADDR_SZ + 1];
#if defined(WITH_PERL) && defined(HAVE_THREADS)
  void *cperl;
#endif

#if defined(WITH_PERL) && defined(HAVE_THREADS)
  cperl = perl_init_clone(a->config);
#endif
  if (bsy_add (&a->node->fa, F_CSY, a->config))
  {
    call0 (a->node, a->config);
    bsy_remove (&a->node->fa, F_CSY, a->config);
  }
  else
  {
    ftnaddress_to_str (szDestAddr, &a->node->fa);
    Log (4, "%s busy, skipping", szDestAddr);
  }
#if defined(WITH_PERL) && defined(HAVE_THREADS)
  perl_done_clone(cperl);
#endif
  unlock_config_structure(a->config, 0);
  free (arg);
  rel_grow_handles(-6);
#ifdef HAVE_THREADS
  threadsafe(--n_clients);
  PostSem(&eothread);
  if (poll_flag)
    PostSem(&wakecmgr);
  ENDTHREAD();
#elif defined(DOS) || defined(DEBUGCHILD)
  --n_clients;
#endif
}
