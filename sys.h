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
  #include <dos.h>
  #include <process.h>
  #if defined(OS2)
    int gettid ();
    #define PID() gettid()
  #elif defined(WIN32)
    #define PID() ((int)(0xffff & GetCurrentThreadId()))
  #else
    #define PID() ((int)gettid())
  #endif
#else
  #include <sys/wait.h>
  #define PID() ((int)getpid())
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

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

/*
 * Get free space in a directory
 */
unsigned long getfree (char *path);

/*
 * Set up break handler, set up exit list if needed
 */
int set_break_handlers ();

/*
 * Runs a new thread or forks
 */
int branch (void (*) (void *), void *, size_t);

/*
 * From breaksig.c -- binkd runs this from exitlist or
 * from signal handler (Under NT)
 */
void exitfunc (void);

#if defined(OS2) && defined(HAVE_THREADS)
void rel_grow_handles(int nh);
#else
#define rel_grow_handles(nh)
#endif

#endif
