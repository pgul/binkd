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

/*
 *  $Id$
 *
 *  $Log$
 *  Revision 1.1  2001/01/10 11:34:57  gul
 *  Initial revision
 *
 * Revision 1.3  1997/10/23  04:18:08  mff
 * exitfunc() moved to exitproc.c, minor changes
 *
 * Revision 1.2  1997/03/09  07:17:16  mff
 * Support for pid_file
 *
 * Revision 1.1  1996/12/14  07:02:20  mff
 * Initial revision
 *
 */

#include <stdlib.h>
#include <signal.h>
#include "Config.h"
#include "sys.h"
#include "bsy.h"
#include "tools.h"
#include "iphdr.h"
#include "readcfg.h"
#include "binlog.h"

extern int pidcmgr;		/* pid for clientmgr */

static void exitsig (int arg)
{
  /* Log (0, ...) will call exit(), exit() will call exitlist */
#ifdef HAVE_FORK
  if (pidcmgr)
    Log (0, "got signal #%i. Killing %i and quitting...", arg, (int) pidcmgr);
  else
#endif
    Log (0, "got signal #%i.", arg);
}

/* Set up break handler, set up exit list if needed */
int set_break_handlers ()
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
