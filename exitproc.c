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

/*
 * $Id$
 *
 * $Log$
 * Revision 2.19  2003/08/16 06:21:12  gul
 * Log() semaphoring removed
 *
 * Revision 2.18  2003/08/14 12:56:29  gul
 * Make Log() thread-safe
 *
 * Revision 2.17  2003/08/14 11:43:19  val
 * free allocated log buffer in exitfunc()
 *
 * Revision 2.16  2003/06/20 10:37:02  val
 * Perl hooks for binkd - initial revision
 *
 * Revision 2.15  2003/06/04 10:36:58  stas
 * Thread-safety tcperr() implementation on Win32
 *
 * Revision 2.14  2003/06/02 08:26:00  gul
 * Fix hang on exit with big loglevel
 *
 * Revision 2.13  2003/05/04 08:45:30  gul
 * Lock semaphores more safely for resolve and IP-addr print
 *
 * Revision 2.12  2003/03/31 19:53:08  gul
 * Close socket before exit
 *
 * Revision 2.11  2003/03/31 19:35:16  gul
 * Clean semaphores usage
 *
 * Revision 2.10  2003/03/11 11:42:23  gul
 * Use event semaphores for exit threads
 *
 * Revision 2.9  2003/03/11 09:21:30  gul
 * Fixed OS/2 Watcom compilation
 *
 * Revision 2.8  2003/03/10 10:39:23  gul
 * New include file common.h
 *
 * Revision 2.7  2003/03/10 08:38:07  gul
 * Make n_servers/n_clients changes thread-safe
 *
 * Revision 2.6  2003/03/09 18:19:32  gul
 * Bugfix
 *
 * Revision 2.5  2003/03/06 18:30:28  gul
 * A bit optimize
 *
 * Revision 2.4  2003/03/05 11:43:56  gul
 * Fix win32 compilation
 *
 * Revision 2.3  2003/03/05 11:40:12  gul
 * Fix win32 compilation
 *
 * Revision 2.2  2003/03/05 09:00:45  gul
 * Fix win32 compilation
 *
 * Revision 2.1  2003/03/03 23:41:20  gul
 * Try to resolve problem with active threads while exitproc running
 *
 * Revision 2.0  2001/01/10 12:12:37  gul
 * Binkd is under CVS again
 *
 * Revision 1.2  1997/10/23  04:13:35  mff
 * pidfiles are now killed only by servmgrs, misc
 *
 * Revision 1.1  1997/08/12  21:42:54  mff
 * Initial revision
 */

#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include "Config.h"
#include "sys.h"
#include "common.h"
#include "bsy.h"
#include "iphdr.h"
#include "tools.h"
#include "readcfg.h"
#include "binlog.h"
#include "sem.h"
#include "server.h"
#ifdef WITH_PERL
#include "perlhooks.h"
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

void exitfunc (void)
{
  Log(7, "exitfunc()");
#ifdef HAVE_FORK
  if (pidcmgr)
  { int i;
    i=pidcmgr, pidcmgr=0; /* prevent abort when cmgr exits */
    kill (i, SIGTERM);
    /* sleep (1); */
  }
#elif HAVE_THREADS
  /* exit all threads */
  { SOCKET h;
    /* wait for threads exit */
    binkd_exit = 1;
    for (;;)
      if (n_servers || n_clients || (pidcmgr && server_flag))
      {
	if (pidcmgr)
	  PostSem(&exitcmgr);
	/* close active sockets */
	for (h=0; h < max_socket; h++)
	  if (FD_ISSET(h, &sockets))
	    soclose (h);
	if (WaitSem (&eothread, 1))
	  break; /* timeout */
      }
      else
	break;
  }
#endif
  if (sockfd != INVALID_SOCKET)
  { Log (5, "Closing socket # %i", sockfd);
    soclose (sockfd);
    sockfd = INVALID_SOCKET;
  }
  bsy_remove_all ();
  sock_deinit ();
  BinLogDeInit ();
  nodes_deinit ();
#ifdef WITH_PERL
#  ifdef HAVE_FORK
Log(7, "exitproc(): pid=%d, cmgr=%d, smgr=%d, inetd=%d", getpid(), pidCmgr, pidsmgr, inetd_flag);
  if (inetd_flag) perl_done(1);
  else if (!pidsmgr && pidCmgr == (int) getpid()) perl_done(1);
  else if (pidsmgr == (int) getpid()) perl_done(1);
  else perl_done(0);
#  else
  perl_done(1);
#  endif
#endif
  if (*pid_file && pidsmgr == (int) getpid ())
    delete (pid_file);
  CleanSem (&hostsem);
  CleanSem (&resolvsem);
  CleanSem (&varsem);
  CleanEventSem (&eothread);
  CleanEventSem (&exitcmgr);
#ifdef OS2
  CleanSem (&fhsem);
#endif
  ReleaseErrorList();
}
