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
 * Revision 2.71  2012/01/03 17:25:31  green
 * Implemented IPv6 support
 * - replace (almost) all getXbyY function calls with getaddrinfo/getnameinfo (RFC2553) calls
 * - Add compatibility layer for target systems not supporting RFC2553 calls in rfc2553.[ch]
 * - Add support for multiple listen sockets -- one for IPv4 and one for IPv6 (use V6ONLY)
 * - For WIN32 platform add configuration parameter IPV6 (mutually exclusive with BINKD9X)
 * - On WIN32 platform use Winsock2 API if IPV6 support is requested
 * - config: node IP address literal + port supported: [<ipv6 address>]:<port>
 *
 * Revision 2.70  2010/05/24 14:36:57  gul
 * Fix previous patch
 *
 * Revision 2.69  2010/05/24 14:24:32  gul
 * Exit immediately after all jobs done in "-p" mode
 *
 * Revision 2.68  2010/01/24 16:12:43  stas
 * Log message changed: "unable to connect" -> "connection to smth. failed". Patch from Alexey Vissarionov 2:5020/545
 *
 * Revision 2.67  2009/06/02 17:09:35  gul
 * Build binkd for OS/2 with perl support
 *
 * Revision 2.66  2009/05/31 07:16:16  gul
 * Warning: many changes, may be unstable.
 * Perl interpreter is now part of config and rerun on config reload.
 * Perl 5.10 compatibility.
 * Changes in outbound queue managing and sorting.
 *
 * Revision 2.65  2007/10/13 05:35:15  gul
 * play around checkcfg()
 *
 * Revision 2.64  2007/10/06 10:20:04  gul
 * more accurate checkcfg()
 *
 * Revision 2.63  2007/10/04 17:30:28  gul
 * SIGHUP handler (thx to Sergey Babitch)
 *
 * Revision 2.62  2005/09/23 13:32:46  gul
 * Bugfix in work via proxy with authorization
 *
 * Revision 2.61  2005/09/23 12:24:33  gul
 * define $hosts variable for on_call() perl hook (can be changed).
 * Changes for $proxy and $socks are now local for the single outgoing call.
 *
 * Revision 2.60  2005/03/30 17:35:28  stream
 * Finally implemented '-noproxy' node option.
 *
 * Revision 2.59  2005/03/28 10:15:13  val
 * manage proxy/socks via perl-hook on_call()
 *
 * Revision 2.58  2004/11/07 13:52:40  stream
 * Automatically rescan outbound after reload of configuration
 *
 * Revision 2.57  2004/11/05 12:44:24  gul
 * Client manager did not reload config on change
 * in fork versions (unix, os2-emx)
 *
 * Revision 2.56  2004/11/01 13:05:31  gul
 * Bugfix in connect-timeout
 *
 * Revision 2.55  2004/10/18 15:22:19  gul
 * Change handle perl errors method
 *
 * Revision 2.54  2004/08/04 19:51:40  gul
 * Change SIGCHLD handling, make signal handler more clean,
 * prevent occasional hanging (mutex deadlock) under linux kernel 2.6.
 *
 * Revision 2.53  2004/08/03 20:06:08  gul
 * Remove unneeded longjump from signal handler
 *
 * Revision 2.52  2003/12/06 00:27:23  gul
 * Coredump on exit cmgr with DEBUGCHILD
 *
 * Revision 2.51  2003/10/29 21:08:38  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.50  2003/10/07 20:54:47  gul
 * End clientmgr by _endthread() on break
 * (patch by Alexander Reznikov).
 *
 * Revision 2.49  2003/10/07 20:50:07  gul
 * Wait for servmanager exit from exitproc()
 * (Patch from Alexander Reznikov)
 *
 * Revision 2.48  2003/10/07 17:57:09  gul
 * Some small changes in close threads function.
 * Inhibit errors "socket operation on non-socket" on break.
 *
 * Revision 2.47  2003/09/22 09:54:41  gul
 * Screen output semaphoring, prevent mixing output from threads
 *
 * Revision 2.46  2003/09/21 17:51:08  gul
 * Fixed PID in logfile for perl stderr handled messages in fork version.
 *
 * Revision 2.45  2003/09/21 17:34:26  gul
 * Change perl stderr handling for thread vertions,
 * some small changes.
 *
 * Revision 2.44  2003/09/05 06:49:06  val
 * Perl support restored after config reloading patch
 *
 * Revision 2.43  2003/08/26 21:01:09  gul
 * Fix compilation under unix
 *
 * Revision 2.42  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.41  2003/07/28 10:23:33  val
 * Perl DLL dynamic load for Win32, config keyword perl-dll, nmake PERLDL=1
 *
 * Revision 2.40  2003/06/20 10:37:02  val
 * Perl hooks for binkd - initial revision
 *
 * Revision 2.39  2003/06/02 14:10:17  gul
 * write domain and IP to logfile on outgoing connections
 *
 * Revision 2.38  2003/04/30 13:38:17  gul
 * Avoid warnings
 *
 * Revision 2.37  2003/04/28 07:30:16  gul
 * Bugfix: Log() changes TCPERRNO
 *
 * Revision 2.36  2003/04/25 12:51:18  gul
 * Fix diagnostics on exit
 *
 * Revision 2.35  2003/04/22 22:26:17  gul
 * Fix previous patch
 *
 * Revision 2.34  2003/04/22 20:13:49  gul
 * Fixed possible premature exit in -p mode
 *
 * Revision 2.33  2003/04/18 08:30:33  hbrew
 * Fix memory fault when use proxy. Path from Stas Degteff 2:5080/102
 *
 * Revision 2.32  2003/04/06 13:50:11  gul
 * dos sleep() realization
 *
 * Revision 2.31  2003/04/06 08:38:47  gul
 * Log port number for outgoing connections to non-standard port
 *
 * Revision 2.30  2003/03/31 16:28:09  gul
 * Fix previous patch
 *
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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_FORK
#include <signal.h>
#include <sys/wait.h>
#endif

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
#if defined(WITH_PERL)
#include "perlhooks.h"
#endif

#ifdef HTTPS
#include "https.h"
#endif
#include "rfc2553.h"

static void call (void *arg);

int n_clients = 0;

#ifdef HAVE_FORK

static void chld (int signo)
{
#define CHILDCOUNT n_clients
#include "reapchld.inc"
}

static void alrm (int signo)
{
}

#endif

#if defined(HAVE_THREADS)
#define SLEEP(x) WaitSem(&wakecmgr, x)
#elif defined(VOID_SLEEP) || !defined(HAVE_FORK)
#define SLEEP(x) sleep(x)
#else
void SLEEP (time_t s)
{
  while ((s = sleep (s)) > 0 && !binkd_exit && !poll_flag
#ifdef HAVE_FORK
         && !got_sighup
#endif
         );
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
        unblockchld();
        SLEEP(1);
        blockchld();
      }
#if !defined(DEBUGCHILD)
      else
      {
        Log (5, "started client #%i, id=%i", n_clients, pid);
#if defined(HAVE_FORK) && !defined(AMIGA)
        unlock_config_structure(config, 0); /* Forked child has own copy */
#endif
      }
#endif
    }
    else
    {
      if (poll_flag)
      {
        if (n_clients <= 0 && q_not_empty (config) == 0)
        {
          Log (4, "the queue is empty, quitting...");
          return -1;
        }
      } else
        config->q_present = 0;
      unblockchld();
      SLEEP (config->rescan_delay);
      blockchld();
    }
  }
  else
  {
    unblockchld();
    SLEEP (config->call_delay);
    blockchld();
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

#ifdef HAVE_FORK
  pidcmgr = 0;
  pidCmgr = (int) getpid();
  blockchld();
  signal (SIGCHLD, chld);
#elif HAVE_THREADS
  pidcmgr = PID();
#endif

  config = lock_current_config();
#if defined(WITH_PERL) && defined(HAVE_THREADS)
  if (server_flag)
    cperl = perl_init_clone(config);
#endif

  setproctitle ("client manager");
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

  unblockchld();
#ifdef HAVE_THREADS
  pidcmgr = 0;
  PostSem(&eothread);
  if (binkd_exit)
    _endthread();
#endif
  exit (0);
}

static int call0 (FTN_NODE *node, BINKD_CONFIG *config)
{
  int sockfd = INVALID_SOCKET;
  char szDestAddr[FTN_ADDR_SZ + 1];
  int i, rc;
  char host[MAXHOSTNAMELEN + 5 + 1];       /* current host/port */
  char addrbuf[MAXHOSTNAMELEN + 1];
  char servbuf[MAXSERVNAME + 1];
  char *hosts;
  unsigned short port;
  const char *save_err;
#ifdef HTTPS
  int use_proxy;
  char *proxy, *socks;
  struct addrinfo *aiProxyHead;
#endif
  struct addrinfo *ai, *aiNodeHead, *aiHead;
  struct addrinfo hints = { .ai_family = PF_UNSPEC,
			    .ai_socktype = SOCK_STREAM,
			    .ai_protocol = IPPROTO_TCP };
  int aiErr;

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
  setproctitle ("call to %s", szDestAddr);

#ifdef HTTPS
  use_proxy = (node->NP_flag != NP_ON) && (proxy[0] || socks[0]);
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
      if((sp=strchr(host, '/')) != NULL)
	*sp++ = '\0';
      sport = proxy[0] ? "squid" : "socks"; /* default port */
    }
    if ( (aiErr = getaddrinfo(host, sport, &hints, &aiProxyHead)) != 0)
    {
	Log(2, "Port %s not found, try default %d", sp, proxy[0] ? 3128 : 1080);
	aiErr = getaddrinfo(host, proxy[0] ? "3128" : "1080", &hints, &aiProxyHead);
    }
    /* resolve proxy host */
    if (aiErr != 0)
    {
      Log(1, "%s host %s not found", proxy[0] ? "Proxy" : "Socks", host);
#ifdef WITH_PERL
      xfree(hosts);
#ifdef HTTPS
      xfree(proxy);
      xfree(socks);
#endif
#endif
      return 0;
    }
  }
#endif

  for (i = 1; sockfd == INVALID_SOCKET
       && (rc = get_host_and_port
	   (i, host, &port, hosts, &node->fa, config)) != -1; ++i)
  {
    if (rc == 0)
    {
      Log (1, "%s: %i: error parsing host list", hosts, i);
      continue;
    }
#ifdef HTTPS
    if (use_proxy)
      aiHead = aiProxyHead;
    else /* don't resolve if proxy or socks specified */
#endif
    {
      char pstr[11];

      snprintf(pstr, sizeof(pstr), "%d", port);
      aiErr = getaddrinfo(host, pstr, &hints, &aiNodeHead);
     
      if (aiErr != 0)
      {
        bad_try(&node->fa, "Cannot getaddrinfo", BAD_CALL, config);
        continue;
      }
      aiHead = aiNodeHead;
    }

    /* Trying... */

    for (ai = aiHead; ai != NULL && sockfd == INVALID_SOCKET; ai = ai->ai_next)
    {
      if ((sockfd = socket (ai->ai_family, ai->ai_socktype, ai->ai_protocol)) == INVALID_SOCKET)
      {
	Log (1, "socket: %s", TCPERR ());
#ifdef WITH_PERL
	xfree(hosts);
#ifdef HTTPS
	xfree(proxy);
	xfree(socks);
#endif
#endif
#ifdef HAVE_THREADS
	freeaddrinfo(ai);
#endif
	return 0;
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
        freeaddrinfo(aiProxyHead);
#endif
#endif
#ifdef HAVE_THREADS
	freeaddrinfo(aiNodeHead);
#endif
	return 0;
      }
      rc = getnameinfo(ai->ai_addr, ai->ai_addrlen, addrbuf, sizeof(addrbuf),
		       servbuf, sizeof(servbuf), NI_NUMERICHOST | NI_NUMERICSERV);
      if (rc != 0)
	Log (2, "Error in getnameinfo(): %s", gai_strerror(rc));
#ifdef HTTPS
      if (use_proxy)
      {
	char *sp = strchr(host, ':');
	if (sp) *sp = '\0';
	if (port == config->oport)
	  Log (4, "trying %s via %s %s:%u...", host,
	       proxy[0] ? "proxy" : "socks", addrbuf, servbuf);
	else
	  Log (4, "trying %s:%u via %s %s:%u...", host, port,
	       proxy[0] ? "proxy" : "socks", addrbuf, servbuf);
	sprintf(host+strlen(host), ":%u", port);
      }
      else
#endif
      {
	if (port == config->oport)
          Log (4, "trying %s...", addrbuf);
	else
          Log (4, "trying %s:%s...", addrbuf, servbuf);
      }
      /* find bind addr with matching address family */
      if (config->bindaddr[0])
      {
	struct addrinfo *src_ai;
	struct addrinfo src_hints = { .ai_socktype = SOCK_STREAM,
				      .ai_protocol = IPPROTO_TCP };
	
	src_hints.ai_family = ai->ai_family;
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
	    Log(2, "bind -- getaddrinfo: %s", gai_strerror(aiErr));
      }
#ifdef HAVE_FORK
      if (config->connect_timeout)
      {
	signal(SIGALRM, alrm);
	alarm(config->connect_timeout);
      }
#endif
      if (connect (sockfd, ai->ai_addr, ai->ai_addrlen) == 0)
      {
#ifdef HAVE_FORK
	alarm(0);
#endif
	Log (4, "connected");
	break;
      }

#ifdef HAVE_FORK
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
#ifdef HAVE_THREADS
#ifdef HTTPS
    if (!use_proxy)
#endif
      freeaddrinfo(aiNodeHead);
#endif
#ifdef HTTPS
    if (sockfd != INVALID_SOCKET && use_proxy) {
      if (h_connect(sockfd, host, config, proxy, socks) != 0) {
        if (!binkd_exit)
          bad_try (&node->fa, TCPERR (), BAD_CALL, config);
        del_socket(sockfd);
        soclose (sockfd);
        sockfd = INVALID_SOCKET;
      }
      else if (port == config->oport) {
        char *pp;
        if( (pp = strchr(host, ':')) ){
          *pp = '\0';
        }
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

  protocol (sockfd, node, host, config);
  del_socket(sockfd);
  soclose (sockfd);
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
  _endthread();
#elif defined(DOS)
  --n_clients;
#endif
}
