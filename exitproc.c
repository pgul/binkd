/*
 *  exitproc.c -- Actions to perform on exit()
 *
 *  exitproc.c is a part of binkd project
 *
 *  Copyright (C) 1997  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#include <signal.h>

#include "sys.h"
#include "readcfg.h"
#include "common.h"
#include "ftnnode.h"
#include "bsy.h"
#include "tools.h"
#include "sem.h"
#include "server.h"
#ifdef WITH_PERL
#include "perlhooks.h"
#endif
#ifdef BINKD9X
#include "nt/win9x.h"
#endif
#if defined(WIN32) && !defined(BINKD9X)
#include "nt/service.h"
#include "nt/w32tools.h"
#endif

int binkd_exit;

#ifdef HAVE_THREADS

static fd_set sockets;
static SOCKET max_socket;

int add_socket(SOCKET sockfd)
{
  threadsafe(
    FD_SET (sockfd, &sockets);
    if (sockfd >= max_socket)
      max_socket = sockfd + 1;
  );
  return 0;
}

int del_socket(SOCKET sockfd)
{
  threadsafe(FD_CLR (sockfd, &sockets));
  return 0;
}

#endif

void close_srvmgr_socket(void)
{
  int curfd;

  for (curfd=0; curfd<sockfd_used; curfd++)
  { Log (5, "Closing server socket # %i", sockfd[curfd]);
    soclose (sockfd[curfd]);
  }
  sockfd_used = 0;
}

void exitfunc (void)
{
  BINKD_CONFIG *config;
#if defined(WIN32) && !defined(BINKD9X)
  static int exitfunc_called_flag=0;

  if (IsNT() && isService()) {
    LockSem(&exitsem);
    if(exitfunc_called_flag)
    { /* prevent double call exitfunc() at NT service stop sequence */
      ReleaseSem(&exitsem);
      Log(10, "exitfunc() repeated call, return from exitfunc()");
      return;
    }
    exitfunc_called_flag=1;
    ReleaseSem(&exitsem);
  }
#endif

  Log(7, "exitfunc()");

#if defined(HAVE_THREADS)
  /* exit all threads */
  { SOCKET h;
    int timeout = 0;
    /* wait for threads exit */
    binkd_exit = 1;
    for (;;)
      if (n_servers || n_clients || pidcmgr || pidsmgr)
      {
	close_srvmgr_socket();
	if (pidcmgr)
	  PostSem(&wakecmgr);
	/* close active sockets */
	for (h=0; h < max_socket; h++)
	  if (FD_ISSET(h, &sockets))
	    soclose (h);

	if (WaitSem (&eothread, 1))
	{
	  timeout++;
	  if (timeout == 4) /* 4 sec */
	  {
	    Log(5, "exitfunc(): warning, threads exit timeout (%i sec), n_servers %i, n_clients %i pidcmgr %i pidsmgr %i!",
			    timeout, n_servers, n_clients, (int)pidcmgr, (int)pidsmgr);
	    break;
	  }
	}
	else
	{
	  Log(9, "Thread finished");
	  timeout = 0;
	}
      }
      else
      {
	Log(8, "exitfunc(): all threads finished");
	break;
      }
  }
#elif defined(HAVE_FORK)
  if (pidcmgr)
  { int i;
    i=pidcmgr, pidcmgr=0; /* prevent abort when cmgr exits */
    kill (i, SIGTERM);
    /* sleep (1); */
  }
  close_srvmgr_socket();
#endif

  config = lock_current_config();
  if (config)
    bsy_remove_all (config);
  sock_deinit ();
  nodes_deinit ();
  if (config)
  {
    if (*config->pid_file && pidsmgr == (int) getpid ())
      delete (config->pid_file);
    /* completely unload config */
#if defined(HAVE_FORK) && !defined(HAVE_THREADS)
    unlock_config_structure(config, inetd_flag || (!pidsmgr && pidCmgr == (int) getpid()) || (pidsmgr == (int) getpid()));
#else
    unlock_config_structure(config, 1);
#endif
  }
  CleanSem (&config_sem);
  CleanSem (&hostsem);
  CleanSem (&resolvsem);
  CleanSem (&lsem);
  CleanSem (&blsem);
  CleanSem (&varsem);
  CleanEventSem (&eothread);
  CleanEventSem (&wakecmgr);
#ifdef OS2
  CleanSem (&fhsem);
#endif
#if defined(WITH_PERL) && defined(HAVE_THREADS) && defined(PERL_MULTITHREAD)
  CleanSem (&perlsem);
#endif
  ReleaseErrorList();
}
