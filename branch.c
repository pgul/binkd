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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sys.h"
#include "common.h"
#include "tools.h"
#include "sem.h"

#ifdef AMIGA
int ix_vfork (void);
void vfork_setup_child (void);
void ix_vfork_resume (void);
#endif

#ifdef WITH_PTHREADS
typedef struct {
    void (*F) (void *);
    void *args;
    MUTEXSEM mutex;
#ifdef HAVE_GETTID
    pid_t tid;
#endif
  } thread_args_t;

static void *thread_start(void *arg)
{
  void (*F) (void*);
  void *args;

  F = ((thread_args_t *)arg)->F;
  args = ((thread_args_t *)arg)->args;
#ifdef HAVE_GETTID
  ((thread_args_t *)arg)->tid = PID();
#endif
  ReleaseSem(&((thread_args_t *)arg)->mutex);
  pthread_detach(pthread_self());
  F(args);
  return NULL;
}
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

#if defined(HAVE_FORK) && !defined(HAVE_THREADS) && !defined(AMIGA) && !defined(DEBUGCHILD)
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
  #ifdef WITH_PTHREADS
  { thread_args_t args;
    pthread_t tid;

    args.F = F;
    args.args = arg;
    InitSem(&args.mutex);
    LockSem(&args.mutex);
    if ((rc = pthread_create (&tid, NULL, thread_start, &args)) != 0)
    {
      Log (1, "pthread_create: %s", strerror (rc));
      rc = -1;
    }
    else
    {
      LockSem(&args.mutex); /* wait until thread releases this mutex */
      #ifdef HAVE_GETTID
      rc = args.tid;
      #else
      rc = (int)(0xffff & (int)tid);
      #endif
    }
    ReleaseSem(&args.mutex);
    CleanSem(&args.mutex);
  }
  #else
  if ((rc = BEGINTHREAD (F, STACKSIZE, arg)) < 0)
    Log (1, "_beginthread: %s", strerror (errno));
  #endif
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
