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
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#if defined(HAVE_FORK) && defined(HAVE_THREADS)
#error You cannot define both HAVE_FORK and HAVE_THREADS!
#endif

#if !defined(HAVE_FORK) && !defined(HAVE_THREADS)
#error Must define either HAVE_FORK or HAVE_THREADS!
#endif

#ifdef HAVE_THREADS
#ifdef HAVE_DOS_H
#include <dos.h>
#endif
#include <process.h>
#endif

#include "Config.h"
#include "sys.h"
#include "tools.h"

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

#if defined(HAVE_FORK) && !defined(AMIGA)
again:
  if (!(rc = fork ()))
  {
    /* new process */
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
    if (arg)
      free (arg);
  }
#endif

#ifdef HAVE_THREADS
#ifdef __MINGW32__
  if ((rc = _beginthread (F, STACKSIZE, arg)) < 0)
#else
  if ((rc = _beginthread (F, 0, STACKSIZE, arg)) < 0)
#endif
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

  return rc;
}
