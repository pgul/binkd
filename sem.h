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
 * Revision 2.12  2004/01/03 12:17:43  stas
 * Implement full icon support (winNT/2k/XP)
 *
 * Revision 2.11  2003/08/26 16:06:27  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.10  2003/08/17 10:38:55  gul
 * Return semaphoring for log and binlog
 *
 * Revision 2.9  2003/08/16 06:21:12  gul
 * Log() semaphoring removed
 *
 * Revision 2.8  2003/05/04 08:45:30  gul
 * Lock semaphores more safely for resolve and IP-addr print
 *
 * Revision 2.7  2003/03/31 19:35:16  gul
 * Clean semaphores usage
 *
 * Revision 2.6  2003/03/26 13:53:28  gul
 * Fix OS/2 compilation
 *
 * Revision 2.5  2003/03/11 11:42:23  gul
 * Use event semaphores for exit threads
 *
 * Revision 2.4  2003/03/10 12:16:54  gul
 * Use HAVE_DOS_H macro
 *
 * Revision 2.3  2003/03/10 08:38:07  gul
 * Make n_servers/n_clients changes thread-safe
 *
 * Revision 2.2  2003/02/22 19:20:25  gul
 * Fix type in previous patch
 *
 * Revision 2.1  2003/02/22 12:12:34  gul
 * Cleanup sources
 *
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
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

typedef HANDLE EVENTSEM;

#elif defined(OS2)

#define INCL_DOS
#include <os2.h>
typedef HMTX MUTEXSEM;

typedef HEV  EVENTSEM;

#elif defined(AMIGA)

#include <exec/exec.h>
typedef struct SignalSemaphore MUTEXSEM;

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

#if defined(HAVE_THREADS) || defined(AMIGA)
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
extern EVENTSEM exitcmgr;
#define lockhostsem()		LockSem(&hostsem)
#define releasehostsem()	ReleaseSem(&hostsem)
#define lockresolvsem()		LockSem(&resolvsem)
#define releaseresolvsem()	ReleaseSem(&resolvsem)
#define threadsafe(exp)		LockSem(&varsem); exp; ReleaseSem(&varsem)
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
#define threadsafe(exp)		exp
#endif

#endif
