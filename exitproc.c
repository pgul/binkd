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
#include "bsy.h"
#include "tools.h"
#include "iphdr.h"
#include "readcfg.h"
#include "binlog.h"

extern int pidcmgr;		       /* pid for clientmgr */
extern int pidsmgr;		       /* pid for server */
extern int server_flag;

int binkd_exit;

#ifdef HAVE_THREADS

static fd_set sockets;

void add_socket(int sockfd)
{
  FD_SET (sockfd, &sockets);
}

void del_socket(int sockfd)
{
  FD_CLR (sockfd, &sockets);
}

#endif

void exitfunc (void)
{
#ifdef HAVE_FORK
  if (pidcmgr)
  { int i;
    i=pidcmgr, pidcmgr=0; /* prevent abort when cmgr exits */
    kill (i, SIGTERM);
    sleep (1);
  }
#elif HAVE_THREADS
  /* exit all threads */
  { int i, h;
    /* wait for threads exit */
    binkd_exit = 1;
    for (i=0; i<5; i++)
      if (nservers || nclients || (pidcmgr && server_flag))
      {
	/* close active sockets */
	for (h=0; h<sizeof(sockets)*8; h++)
	  if (FD_ISSET(h, &sockets))
	    soclose(h);
	sleep (1);
      }
      else
	break;
  }
#endif
  bsy_remove_all ();
  sock_deinit ();
  BinLogDeInit ();
  if (*pid_file && pidsmgr == (int) getpid ())
    delete (pid_file);
}
