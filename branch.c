/*
 *  branch.c -- Create co-processes
 *
 *  branch.c is a part of binkd project
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
 * Revision 2.11  2003/10/29 21:08:38  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.10  2003/10/04 20:46:56  gul
 * New configure --with-debug=nofork option, DEBUGCHILD macro
 *
 * Revision 2.9  2003/09/21 17:51:08  gul
 * Fixed PID in logfile for perl stderr handled messages in fork version.
 *
 * Revision 2.8  2003/09/05 08:15:24  gul
 * Make DEBUG-version single-thread
 *
 * Revision 2.7  2003/08/26 21:01:09  gul
 * Fix compilation under unix
 *
 * Revision 2.6  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.5  2003/07/19 06:59:34  hbrew
 * Complex patch:
 * * nt/w32tools.c: Fix warnings
 * * nt/w32tools.c: Fix typo in #ifdef
 * * nt/win9x.c: Fix type in #include
 * * Config.h, sys.h, branch.c, nt/service.c,
 *     nt/win9x.c, : _beginthread()-->BEGINTHREAD()
 * * binkd.c, common.h, mkfls/nt95-msvc/Makefile.dep,
 *     nt/service.c, nt/w32tools.c,nt/win9x.c: cosmitic code cleanup
 *
 * Revision 2.4  2003/03/11 00:04:25  gul
 * Use patches for compile under MSDOS by MSC 6.0 with IBMTCPIP
 *
 * Revision 2.3  2003/03/10 12:16:53  gul
 * Use HAVE_DOS_H macro
 *
 * Revision 2.2  2003/03/01 20:16:27  gul
 * OS/2 IBM C support
 *
 * Revision 2.1  2001/09/24 10:31:39  gul
 * Build under mingw32
 *
 * Revision 2.0  2001/01/10 12:12:37  gul
 * Binkd is under CVS again
 *
 * Revision 1.3  1997/10/23  04:19:38  mff
 * Amiga port, argument copying (DUP() removed)
 *
 * Revision 1.2  1996/12/29  09:37:30  mff
 * Misc changes
 *
 * Revision 1.1  1996/12/14  07:01:58  mff
 * Initial revision
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "tools.h"
#include "sys.h"

#ifdef AMIGA
int ix_vfork (void);
void vfork_setup_child (void);
void ix_vfork_resume (void);
#endif

int branch (register void (*F) (void *), register void *arg, register size_t size)
{
  register int rc;
  char *tmp;

  /* We make our own copy of arg for the child as the parent may destroy it
   * before the child finish to use it. It's not really needed with fork()
   * but we do not want extra checks for HAVE_FORK before free(arg) in the
   * child. */
  if (size > 0)
  {
    if ((tmp = malloc (size)) == NULL)
    {
      Log (1, "malloc failed");
      return -1;
    }
    else
    {
      memcpy (tmp, arg, size);
      arg = tmp;
    }
  }
  else
    arg = 0;

#if defined(HAVE_FORK) && !defined(AMIGA) && !defined(DEBUGCHILD)
again:
  if (!(rc = fork ()))
  {
    /* new process */
    mypid = getpid();
    F (arg);
    exit (0);
  }
  else if (rc < 0)
  {
    if (errno == EINTR) goto again;
    /* parent, error */
    Log (1, "fork: %s", strerror (errno));
  }
  else
  {
    /* parent, free our copy of args */
    xfree (arg);
  }
#endif

#if defined(HAVE_THREADS) && !defined(DEBUGCHILD)
  if ((rc = BEGINTHREAD (F, STACKSIZE, arg)) < 0)
  {
    Log (1, "_beginthread: %s", strerror (errno));
  }
#endif

#ifdef AMIGA
  /* this is rather bizzare. this function pretends to be a fork and behaves
   * like one, but actually it's a kind of a thread. so we'll need semaphores */

  if (!(rc = ix_vfork ()))
  {
    vfork_setup_child ();
    ix_vfork_resume ();
    F (arg);
    exit (0);
  }
  else if (rc < 0)
  {
    Log (1, "ix_vfork: %s", strerror (errno));
  }
#endif

#if defined(DOS) || defined(DEBUGCHILD)
  rc = 0;
  F (arg);
#endif

  return rc;
}
