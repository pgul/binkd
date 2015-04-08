/*
 *  sem.h -- semaphores for multithreaded version
 *
 *  sem.h is a part of binkd project
 *
 *  Copyright (C) 1996  Fydodor Ustinov, FIDONet 2:5020/79
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#ifndef _SEM_H_
#define _SEM_H_

#if defined(WIN32)

#include <windows.h>
typedef HANDLE MUTEXSEM;

typedef HANDLE EVENTSEM;

#elif defined(OS2)

#define INCL_DOS
#include <os2.h>
typedef HMTX MUTEXSEM;

typedef HEV  EVENTSEM;

#elif defined(AMIGA)

#include <exec/exec.h>
typedef struct SignalSemaphore MUTEXSEM;

#elif defined(WITH_PTHREADS)

#include <pthread.h>
typedef pthread_mutex_t MUTEXSEM;
typedef struct { pthread_cond_t cond;
                 pthread_mutex_t mutex;
               } EVENTSEM;

#endif


/*
 *    Initialise Semaphores.
 */

int _InitSem (void *);

/*
 *    Clean Semaphores.
 */

int _CleanSem (void *);

/*
 *    Wait & lock semaphore
 */

int _LockSem (void *);

/*
 *    Release Semaphore.
 */

int _ReleaseSem (void *);

/*
 *    Initialise Event Semaphores.
 */

int _InitEventSem (void *);

/*
 *    Post Semaphore.
 */

int _PostSem (void *);

/*
 *    Wait Semaphore.
 */

int _WaitSem (void *, int);

/*
 *    Clean Event Semaphores.
 */

int _CleanEventSem (void *);

#if defined(WITH_PTHREADS)
  #define InitSem(sem)       pthread_mutex_init(sem, NULL)
  #define CleanSem(sem)      pthread_mutex_destroy(sem)
  #define LockSem(sem)       pthread_mutex_lock(sem)
  #define ReleaseSem(sem)    pthread_mutex_unlock(sem)
  #define InitEventSem(sem)  (pthread_cond_init(&((sem)->cond), NULL), pthread_mutex_init(&((sem)->mutex), NULL))
  #define PostSem(sem)       (LockSem(&((sem)->mutex)), pthread_cond_signal(&((sem)->cond)), ReleaseSem(&((sem)->mutex)))
  #define WaitSem(sem, sec)  _WaitSem(sem, sec)
  #define CleanEventSem(sem) (pthread_cond_destroy(&((sem)->cond)), pthread_mutex_destroy(&((sem)->mutex)))
#elif defined(HAVE_THREADS) || defined(AMIGA)
  #define InitSem(vpSem) _InitSem(vpSem)
  #define CleanSem(vpSem) _CleanSem(vpSem)
  #define LockSem(vpSem) _LockSem(vpSem)
  #define ReleaseSem(vpSem) _ReleaseSem(vpSem)
  #define InitEventSem(vpSem) _InitEventSem(vpSem)
  #define PostSem(vpSem) _PostSem(vpSem)
  #define WaitSem(vpSem, sec) _WaitSem(vpSem, sec)
  #define CleanEventSem(vpSem) _CleanEventSem(vpSem)
#else		/* Do nothing */
  #define InitSem(vpSem)
  #define CleanSem(vpSem)
  #define LockSem(vpSem)
  #define ReleaseSem(vpSem)
  #define InitEventSem(vpSem)
  #define PostSem(vpSem)
  #define WaitSem(vpSem, sec)
  #define CleanEventSem(vpSem)
#endif

#ifdef HAVE_THREADS
extern MUTEXSEM hostsem;
extern MUTEXSEM resolvsem;
extern MUTEXSEM lsem;
extern MUTEXSEM blsem;
extern MUTEXSEM varsem;
extern MUTEXSEM config_sem;
extern EVENTSEM eothread;
extern EVENTSEM wakecmgr;
#define lockhostsem()		LockSem(&hostsem)
#define releasehostsem()	ReleaseSem(&hostsem)
#define lockresolvsem()		LockSem(&resolvsem)
#define releaseresolvsem()	ReleaseSem(&resolvsem)
#define threadsafe(exp)		LockSem(&varsem); exp; ReleaseSem(&varsem)
#if defined(PERL_MULTITHREAD)
#define lockperlsem()		LockSem(&perlsem);
#define releaseperlsem()	ReleaseSem(&perlsem);
#else
#define lockperlsem()
#define releaseperlsem()
#endif
#ifdef OS2
extern MUTEXSEM fhsem;
#endif
#ifdef WIN32
extern MUTEXSEM iconsem;
#endif
#else
#define lockhostsem()
#define releasehostsem()
#define lockresolvsem()
#define releaseresolvsem()
#define lockperlsem()
#define releaseperlsem()
#define threadsafe(exp)		exp
#endif

#endif
