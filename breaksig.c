/*
 *  breaksig.c -- SIGBREAK, etc. signal handlers
 *
 *  breaksig.c is a part of binkd project
 *
 *  Copyright (C) 1996  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include "sys.h"
#include "common.h"
#include "tools.h"
#include "sem.h"

static void exitsig (int arg)
{
  /* Log (0, ...) will call exit(), exit() will call exitlist */
#if defined(HAVE_FORK) && !defined(HAVE_THREADS)
  if (pidcmgr)
    Log (0, "got signal #%i. Killing %i and quitting...", arg, (int) pidcmgr);
  else
    Log (0, "got signal #%i.", arg);
#else
  Log (1, "got signal #%i.", arg);
  binkd_exit = 1;
#ifdef WITH_PTHREADS
  if (tidsmgr && tidsmgr != (int) PID ())
  {
    Log(6, "Resend signal to servmgr");
    pthread_kill(servmgr_thread, arg);
  } else if (!server_flag)
    PostSem(&wakecmgr);
#endif
#endif
}

/* Set up break handler, set up exit list if needed */
int set_break_handlers (void)
{
  atexit (exitfunc);

#ifdef SIGBREAK
  signal (SIGBREAK, exitsig);
#endif
#ifdef SIGHUP
  signal (SIGHUP, SIG_IGN);
#endif
#ifdef SIGINT
  signal (SIGINT, exitsig);
#endif
#ifdef SIGTERM
  signal (SIGTERM, exitsig);
#endif
  return 1;
}
