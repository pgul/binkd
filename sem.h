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

/*
 * $Id$
 *
 * $Log$
 * Revision 1.1  2001/01/10 11:34:59  gul
 * Initial revision
 *
 * Revision 1.5  1997/10/23  03:40:27  mff
 * Amiga port
 *
 * Revision 1.3  1996/12/05  06:56:18  mff
 * Changed to support multiple semaphores
 */

#ifndef _SEM_H_
#define _SEM_H_

#if defined(WIN32)

#include <windows.h>
typedef HANDLE MUTEXSEM;

#elif defined(OS2)

#ifdef __WATCOMC__
#define __IBMC__ 0
#define __IBMCPP__ 0
#endif

#define INCL_DOS
#include <os2.h>
typedef HMTX MUTEXSEM;

#elif defined(AMIGA)

#include <exec/exec.h>
typedef struct SignalSemaphore MUTEXSEM;

#endif


/*
 *    Initialise Semaphores.
 */

int _InitSem (void *);

#if defined(HAVE_THREADS) || defined(AMIGA)
#define InitSem(vpSem) _InitSem(vpSem)
#else
#define InitSem(vpSem)			    /* Do Nothing */
#endif

/*
 *    Clean Semaphores.
 */

int _CleanSem (void *);

#if defined(HAVE_THREADS) || defined(AMIGA)
#define CleanSem(vpSem) _CleanSem(vpSem)
#else
#define CleanSem(vpSem)			    /* Do Nothing */
#endif

/*
 *    Wait & lock semaphore
 */

int _LockSem (void *);

#if defined(HAVE_THREADS) || defined(AMIGA)
#define LockSem(vpSem) _LockSem(vpSem)
#else
#define LockSem(vpSem)			    /* Do Nothing */
#endif

/*
 *    Release Semaphore.
 */

int _ReleaseSem (void *);

#if defined(HAVE_THREADS) || defined(AMIGA)
#define ReleaseSem(vpSem) _ReleaseSem(vpSem)
#else
#define ReleaseSem(vpSem)		    /* Do Nothing */
#endif

#endif
