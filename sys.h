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
 * Revision 2.34.2.1  2014/08/09 15:17:44  gul
 * Large files support on Win32 (backport from develop branch)
 *
 * Revision 2.34  2012/05/14 06:14:59  gul
 * More safe signal handling
 *
 * Revision 2.33  2012/01/03 17:25:32  green
 * Implemented IPv6 support
 * - replace (almost) all getXbyY function calls with getaddrinfo/getnameinfo (RFC2553) calls
 * - Add compatibility layer for target systems not supporting RFC2553 calls in rfc2553.[ch]
 * - Add support for multiple listen sockets -- one for IPv4 and one for IPv6 (use V6ONLY)
 * - For WIN32 platform add configuration parameter IPV6 (mutually exclusive with BINKD9X)
 * - On WIN32 platform use Winsock2 API if IPV6 support is requested
 * - config: node IP address literal + port supported: [<ipv6 address>]:<port>
 *
 * Revision 2.32  2010/12/12 12:32:11  gul
 * Fix previous patch
 *
 * Revision 2.31  2010/12/12 09:44:11  gul
 * Use Sleep() instead of select(0,NULL,NUL,NULL,...) under WIN32
 *
 * Revision 2.30  2009/05/31 07:16:17  gul
 * Warning: many changes, may be unstable.
 * Perl interpreter is now part of config and rerun on config reload.
 * Perl 5.10 compatibility.
 * Changes in outbound queue managing and sorting.
 *
 * Revision 2.29  2006/10/19 11:12:54  stas
 * Fix segmentation fault in mingw build (illegal fprintf format string)
 *
 * Revision 2.28  2006/07/24 21:00:32  gul
 * use MSG_NOSIGNAL in send()
 *
 * Revision 2.27  2005/10/02 16:35:21  gul
 * *** empty log message ***
 *
 * Revision 2.26  2005/09/28 07:18:49  gul
 * gettvtime() (time of day with more then second exactitude) for win32.
 * Thanks to Alexander Reznikov.
 *
 * Revision 2.25  2005/09/27 20:15:43  gul
 * Hopefully fixed compilation under windows
 *
 * Revision 2.24  2005/09/26 19:01:03  gul
 * bw limits code partially rewrited (not tested)
 *
 * Revision 2.23  2005/04/05 07:31:12  gul
 * Fixed bug in have_intmax_t detection
 *
 * Revision 2.22  2004/08/24 11:31:22  gul
 * Fix typo in prev patch
 *
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
#ifdef HAVE_STDINT_H
  #include <stdint.h>
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
  #include <fcntl.h>
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
  #define blocksig()     switchsignal(SIG_BLOCK)
  #define unblocksig()   switchsignal(SIG_UNBLOCK)
  #define BLOCK_SIG      1
#else
  #define blocksig()
  #define unblocksig()
#endif

#ifndef F_OK
  #define F_OK 0
#endif

#ifndef HAVE_MSG_NOSIGNAL
  #define MSG_NOSIGNAL 0
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

  /* MSVC (Visual Studio) Versions (_MSC_VER):
   * 2010(10): 1600
   * 2005( 8): 1400
   * 2000( 6): 1200
   */

  /* [u]intmax_t introduced at least in MSVC 2010 */
  #if _MSC_VER >= 1600
    #include <stdint.h>
  #else
    typedef          __int64  intmax_t;
    typedef unsigned __int64 uintmax_t;
  #endif
  #define HAVE_INTMAX_T

  #if _MSC_VER <= 1200
    /* Only one type of 64-bit stat() in old versions */
    #define stat  _stati64
    #define fstat _fstati64
  #else
    /* There are FOUR types of stat() in these MSVC! (for all 32/64 bits time_t and filesize combinations) */
    /* Use "filesize: 64 bits, time_t: default" version */
    #define stat  _stat64
    #define fstat _fstat64
  #endif

  #if _MSC_VER <= 1200
    /* These functions exists, but in static libraries only and not declared in headers... */
    #ifdef _DLL  /* /MD option - with runtime DLL */
      #error Use static build to get 64-bit file I/O (nmake ... STATIC=1)
    #endif
    _CRTIMP int     __cdecl _fseeki64(FILE *, __int64, int);
    _CRTIMP __int64 __cdecl _ftelli64(FILE *);
  #endif
  #define fseeko _fseeki64
  #define ftello _ftelli64
  #define HAVE_FSEEKO

  #define PRIdMAX "I64i"
  #define PRIuMAX "I64u"

  #if _MSC_VER <= 1200
    /* TODO: although nobody uses 2nd and 3rd parameters now, write normal wrapper - safe_umax(s) or whatever */
    #define strtoumax(s, p, n) _atoi64(s)
  #else
    #define strtoumax _strtoui64
  #endif
  #define HAVE_STRTOUMAX

#endif

#ifdef __MINGW32__
  #define sleep(a) Sleep((a)*1000)
  #define pipe(h)  _pipe(h, 0, 64)
#endif

#if defined(WIN32)
  /* if no waiting sockets, call Sleep() instead of select() */
  #define SELECT(n, r, w, e, t)   \
	(((r)==0 || !FD_ISSET(n-1,r)) && ((w)==0 || !FD_ISSET(n-1,w)) ? \
	 Sleep((t)->tv_sec*1000+((t)->tv_usec+999)/1000),0 : select(n,r,w,e,t))
#else
  #define SELECT(n, r, w, e, t)  select(n, r, w, e, t)
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

/*
 * We cannot use off_t because Microsoft, as usual, sucks - in MSVC off_t
 * is forced to 32 bits. So, DON'T USE OFF_T ANYWHERE IN THE CODE.
 * Use our own boff_t instead.
 */
#ifdef VISUALCPP
  typedef __int64 boff_t;
#else
  typedef off_t   boff_t;
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

#ifndef HAVE_INTMAX_T
 typedef long int intmax_t;
 #undef PRIdMAX
 #define PRIdMAX "ld"
 typedef unsigned long int uintmax_t;
 #undef PRIuMAX
 #define PRIuMAX "lu"
#endif

#ifndef PRIdMAX
#define PRIdMAX "ld"
#define PRIuMAX "lu"
#endif

#ifndef HAVE_STRTOUMAX
#define strtoumax(ptr, endptr, base)	strtoul(ptr, endptr, base)
#endif

#ifndef HAVE_FSEEKO
#define fseeko(f, offset, whence)	fseek(f, (long)(offset), whence)
#define ftello(f)			(boff_t)ftell(f)
#endif

#define UNUSED_ARG(s)  (void)(s)

#if defined(HAVE_GETTIMEOFDAY)
#define gettvtime(tv)	gettimeofday(tv, NULL)
#elif defined(WIN32)
void gettvtime(struct timeval *tv);
#else
#define gettvtime(tv)	((tv)->tv_sec=time(NULL),(tv)->tv_usec=0)
#endif

#endif
