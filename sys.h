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
 * Revision 2.21  2004/08/06 07:05:22  gul
 * Fixed typo in prev patch
 *
 * Revision 2.20  2004/08/04 19:51:40  gul
 * Change SIGCHLD handling, make signal handler more clean,
 * prevent occasional hanging (mutex deadlock) under linux kernel 2.6.
 *
 * Revision 2.19  2004/08/04 13:15:36  gul
 * Define u16 and u32 types more clean
 *
 * Revision 2.18  2004/08/04 11:32:29  gul
 * Attemp to support large files (>4G)
 *
 * Revision 2.17  2004/08/04 06:40:27  gul
 * Use uintmax_t and PRIuMAX for printing file size (off_t)
 *
 * Revision 2.16  2003/12/24 00:36:40  gul
 * Move system-dependent macros from Config.h to sys.h,
 * add pipe() wrapper for mingw32.
 *
 * Revision 2.15  2003/09/21 17:51:09  gul
 * Fixed PID in logfile for perl stderr handled messages in fork version.
 *
 * Revision 2.14  2003/08/18 09:15:39  gul
 * Cosmetics
 *
 * Revision 2.13  2003/08/14 08:29:22  gul
 * Use snprintf() from sprintf.c if no such libc function
 *
 * Revision 2.12  2003/07/19 06:59:34  hbrew
 * Complex patch:
 * * nt/w32tools.c: Fix warnings
 * * nt/w32tools.c: Fix typo in #ifdef
 * * nt/win9x.c: Fix type in #include
 * * Config.h, sys.h, branch.c, nt/service.c,
 *     nt/win9x.c, : _beginthread()-->BEGINTHREAD()
 * * binkd.c, common.h, mkfls/nt95-msvc/Makefile.dep,
 *     nt/service.c, nt/w32tools.c,nt/win9x.c: cosmitic code cleanup
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

#ifdef HAVE_INTTYPES_H
  #include <inttypes.h>
#endif
#ifdef HAVE_UNISTD_H
  #include <unistd.h>
#endif
#ifdef HAVE_IO_H
  #include <io.h>
#endif
#ifdef HAVE_DOS_H
  #include <dos.h>
#endif
#ifdef HAVE_STDARG_H
  #include <stdarg.h>
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
  extern int mypid;
  #define PID() mypid
#endif

#if defined(HAVE_FORK) && defined(HAVE_SIGPROCMASK) && defined(HAVE_WAITPID) && defined(SIG_BLOCK)
  void switchsignal(int how);
  #define blockchld()    switchsignal(SIG_BLOCK)
  #define unblockchld()  switchsignal(SIG_BLOCK)
  #define BLOCK_CHLD     1
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
  #define sleep(a) Sleep((a)*1000)
  #define pipe(h)  _pipe(h, 0, 64)
#endif

#ifdef __MINGW32__
  #define sleep(a) _sleep((a)*1000ul)
  #define pipe(h)  _pipe(h, 0, 64)
#endif

#if defined(EBADTYPE) && !defined(ENOTDIR)
  #define ENOTDIR EBADTYPE /* __IBMC__ */
#endif

#ifndef HAVE_SNPRINTF
#ifdef HAVE_STDARG_H
int snprintf (char *str, size_t count, const char *fmt,...);
#else
int snprintf (va_alist) va_dcl;
#endif
#endif
#ifndef HAVE_VSNPRINTF
int vsnprintf (char *str, size_t count, const char *fmt, va_list args);
#endif

#ifndef O_BINARY
  #define O_BINARY 0
#endif
#if defined(__WATCOMC__) || defined(VISUALCPP) || defined(__MINGW32__) || defined(IBMC) || defined(__MSC__)
  #define MKDIR(s) mkdir(s)
#else
  #define MKDIR(s) mkdir(s, 0755)
#endif

#if defined(__WATCOMC__) || defined(__EMX__) /* expand list if necessary */
#define BEGINTHREAD(a, b, c)   _beginthread(a, NULL, b, c)
#elif defined(HAVE_THREADS)
#define BEGINTHREAD(a, b, c)   _beginthread(a, b, c)
#endif

typedef unsigned char u8;
#if defined(SIZEOF_INT) && SIZEOF_INT!=0
#if SIZEOF_SHORT==2
typedef unsigned short int u16;
#else
#error Cannot find type for 16-bit integer!
#endif
#if SIZEOF_INT==4
typedef unsigned int u32;
#elif SIZEOF_LONG==4
typedef unsigned long int u32;
#else
#error Cannot find type for 32-bit integer!
#endif
#else /* SIZEOF undefined, use defaults */
typedef unsigned short int u16;
typedef unsigned long int u32;
#endif

#ifndef PRIdMAX
#define PRIdMAX "ld"
#define PRIuMAX "lu"
#endif
#ifndef HAVE_INTMAX_T
typedef long int intmax_t;
typedef unsigned long int uintmax_t;
#endif
#ifndef HAVE_STRTOUMAX
#define strtoumax(ptr, endptr, base)	strtoul(ptr, endptr, base)
#endif

#ifndef HAVE_FSEEKO
#define fseeko(f, offset, whence)	fseek(f, (long)(offset), whence)
#define ftello(f)			(off_t)ftell(f)
#endif

#define UNUSED_ARG(s)  (void)(s)

#endif
