/*
 *  sys.h -- 1) very system dependent functions and macros
 *           2) include for <unistd.h> or <io.h>
 *           3) defines for u8, u16, u32
 *
 *  sys.h is a part of binkd project
 *
 *  Copyright (C) 1996  Dima Maloff, 5047/13
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
 * Revision 2.11.2.2  2004/08/24 11:32:44  gul
 * Fix typo in prev patch
 *
 * Revision 2.11.2.1  2004/08/03 19:52:56  gul
 * Change SIGCHLD handling, make signal handler more clean,
 * prevent occasional hanging (mutex deadlock) under linux kernel 2.6.
 *
 * Revision 2.11  2003/04/06 13:50:11  gul
 * dos sleep() realization
 *
 * Revision 2.10  2003/03/26 13:53:28  gul
 * Fix OS/2 compilation
 *
 * Revision 2.9  2003/03/11 09:21:30  gul
 * Fixed OS/2 Watcom compilation
 *
 * Revision 2.8  2003/03/11 00:04:26  gul
 * Use patches for compile under MSDOS by MSC 6.0 with IBMTCPIP
 *
 * Revision 2.7  2003/03/10 12:16:54  gul
 * Use HAVE_DOS_H macro
 *
 * Revision 2.6  2003/03/10 10:39:23  gul
 * New include file common.h
 *
 * Revision 2.5  2003/03/05 11:40:12  gul
 * Fix win32 compilation
 *
 * Revision 2.4  2003/03/03 23:41:20  gul
 * Try to resolve problem with active threads while exitproc running
 *
 * Revision 2.3  2003/03/01 20:16:27  gul
 * OS/2 IBM C support
 *
 * Revision 2.2  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.1  2003/02/22 12:12:34  gul
 * Cleanup sources
 *
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 * Revision 1.3  1997/10/23  03:34:15  mff
 * many, many changes (forget to ci a version or two)
 *
 * Revision 1.1  1996/12/14  07:13:04  mff
 * Initial revision
 *
 */
#ifndef _sys_h
#define _sys_h

#ifdef HAVE_UNISTD_H
  #include <unistd.h>
#endif
#ifdef HAVE_IO_H
  #include <io.h>
#endif
#ifdef HAVE_DOS_H
  #include <dos.h>
#endif

#if defined(__WATCOMC__) && !defined(__IBMC__)
  #include <utils.h>
  #ifdef __UTILS_32H /* toolkit 4.x or less */
    #pragma library("so32dll.lib")
    #pragma library("tcp32dll.lib")
  #else /* using toolkit 5.x */
    #define HAVE_ARPA_INET_H
    #define __IBMC__ 0
    #define __IBMCPP__ 0
  #endif
#endif

#if defined(WIN32)
  #include <windows.h>
  #include <winsock.h>
#endif

#ifdef OS2
  #define INCL_DOS
  #define INCL_ERRORS
  #include <os2.h>
  #include <process.h>
#endif

#ifdef HAVE_THREADS
  #include <process.h>
  #if defined(OS2)
    int gettid (void);
    #define PID() gettid()
  #elif defined(WIN32)
    #define PID() ((int)(0xffff & GetCurrentThreadId()))
  #else
    #define PID() ((int)gettid())
  #endif
#elif defined(__MSC__)
  #include <process.h>
  #define PID()    ((int)getpid())
  void dos_sleep(int);
  #define sleep(s) dos_sleep(s)
#else
  #include <sys/wait.h>
  #define PID() ((int)getpid())
#endif

#if defined(HAVE_FORK) && defined(HAVE_SIGPROCMASK) && defined(HAVE_WAITPID) && defined(SIG_BLOCK)
void switchsignal(int how);
#define blockchld()	switchsignal(SIG_BLOCK)
#define unblockchld()	switchsignal(SIG_UNBLOCK)
#define BLOCK_CHLD	1
#else
#define blockchld()
#define unblockchld()
#endif

#ifndef F_OK
  #define F_OK 0
#endif

#if defined(UNIX) || defined(AMIGA)
  /* To be sure rename will fail if the target exists */
  extern int o_rename (const char *from, const char *to);
  #define RENAME(f,t) o_rename(f,t)
#else
  #define RENAME(f,t) rename(f,t)
#endif

#ifdef VISUALCPP
  #define sleep(a) Sleep(a*1000)
  #define _beginthread(a,b,c,d) _beginthread(a,c,d)
#endif

#if defined(EBADTYPE) && !defined(ENOTDIR)
  #define ENOTDIR EBADTYPE /* __IBMC__ */
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#endif
