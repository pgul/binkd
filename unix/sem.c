/*
 *  sem.c -- binkd's posix semaphore wrapper
 *
 *  sem.c is a part of binkd project
 *
 *  Copyright (C) 2014-2014  Pavel Gulchuk and others
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#include <time.h>
#include "../sys.h"
#include "../readcfg.h"
#include "../tools.h"
#include "../sem.h"

int _WaitSem(void *vpSem, int timeout) {
  EVENTSEM *sem = vpSem;
  struct timespec ts;
  int rc;

  pthread_mutex_lock(&(sem->mutex));
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += 5;
  rc = pthread_cond_timedwait(&(sem->cond), &(sem->mutex), &ts);
  pthread_mutex_unlock(&(sem->mutex));
  return rc;
}

