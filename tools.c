/*
 *  tools.c -- misc utils
 *
 *  tools.c is a part of binkd project
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
 * Revision 2.8  2002/03/20 14:32:14  gul
 * Bugfix in ftrans
 *
 * Revision 2.7  2002/03/07 14:29:52  gul
 * print PID as unsigned to bsy/csy
 *
 * Revision 2.6  2002/02/25 21:33:56  gul
 * Dequote \hh in filenames as \xhh; both \20 and \x20 are space now (FSP-1011)
 *
 * Revision 2.5  2001/11/08 14:04:12  gul
 * bugfix
 *
 * Revision 2.4  2001/11/07 17:01:12  gul
 * Check size of buffer in strnzcpy()
 *
 * Revision 2.3  2001/10/23 08:33:44  gul
 * Change filename (not ext) in incoming *.req if already exists
 *
 * Revision 2.2  2001/09/30 13:49:59  gul
 * Do not put log to socket if run via inetd
 *
 * Revision 2.1  2001/09/24 10:31:39  gul
 * Build under mingw32
 *
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 * Revision 1.18  1998/05/05  04:37:54  mff
 * Now we add trailing '\n' to pids in pidfiles, added 2nd arg to strquote(),
 * added strdequote(), strwipe() does not touch ' ' and 0xff any more,
 * added istic(), ispkt(), isarcmail().
 *
 * Revision 1.17  1997/10/23  03:31:49  mff
 * minor bug fixes in mkpath*() (again!), added mutex into Log(),
 * stricmp() -> STRICMP(), getwordx() moved to getw.c, minor fixes.
 *
 * Revision 1.16  1997/06/16  05:38:21  mff
 * Changed Log() -> syslog() loglevel mapping, fixed endless recursion
 * in Log() when calling assert() from Log()
 *
 * Revision 1.15  1997/05/17  11:18:14  mff
 * + get_os_string()
 *
 * Revision 1.14  1997/05/17  08:40:35  mff
 * assert() in Log() changed a bit
 *
 * Revision 1.13  1997/05/08  05:23:20  mff
 * Fixed syslog feature of Log()
 *
 * Revision 1.12  1997/03/28  06:10:12  mff
 * + ed()
 *
 * Revision 1.11  1997/03/15  05:03:32  mff
 * added touch()
 *
 * Revision 1.10  1997/03/09  07:15:30  mff
 * GWX_*, syslog
 *
 * Revision 1.9  1997/02/07  06:45:52  mff
 * getwordx now handles not only first `#' as comment. Though it can be
 * quoted with "" or \. TODO: bitmapped arg to turn this and var substing off
 *
 * Revision 1.8  1997/01/09  05:33:35  mff
 * Now we don't mkdir("") (Thanks to Mike Malakhov)
 *
 * Revision 1.7  1996/12/29  09:47:42  mff
 * Added: create_empty_sem_file()
 *
 * Revision 1.6  1996/12/14  07:13:51  mff
 * log() is now Log()
 *
 * Revision 1.5  1996/12/05  08:01:11  mff
 * Changed parse_args(), removed free_args()
 *
 * Revision 1.4  1996/12/05  07:24:35  mff
 * Format strings for Log() should have no "\n" at the end
 *
 * Revision 1.3  1996/12/05  04:30:14  mff
 * Now we don't mkdir drives
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)
#include <syslog.h>
#endif

#include "assert.h"
#include "Config.h"
#include "sys.h"
#include "tools.h"
#include "readdir.h"		       /* for [sys/]utime.h */
#include "readcfg.h"
#include "sem.h"

#ifdef WIN32
extern int isService;
#endif

/*
 * Lowercase the string
 */
char *strlower (char *s)
{
  int i;

  for (i = 0; s[i]; ++i)
    s[i] = tolower (s[i]);
  return s;
}

int mkpath0 (const char *path0)
{
  struct stat sb;

  if (stat (path0, &sb) == -1)
  {
    char *path, *s;
    int retval;

    if ((path = strdup (path0)) == NULL)
    {
      errno = ENOMEM;
      return -1;
    }
    if ((s = max (strrchr (path, '\\'), strrchr (path, '/'))) != 0)
    {
      *(s++) = 0;
      if (*path && mkpath0 (path) == -1)
      {
	free (path);
	return -1;
      }
    }
    else
      s = path;

    if (path0[strlen (path0) - 1] == ':')
      retval = 0;		       /* we found the device part of the
				        * path */
    else
      retval = MKDIR (path0);

    free (path);
    return retval;
  }
  else if (sb.st_mode & S_IFDIR)
  {
    return 0;
  }
  else
  {
    errno = ENOTDIR;
    return -1;
  }
}

int mkpath (char *s)
{
  char path[MAXPATHLEN];

  strnzcpy (path, s, MAXPATHLEN);
  if ((s = max (strrchr (path, '\\'), strrchr (path, '/'))) == 0)
    return 0;
  *s = 0;
  return mkpath0 (path);
}

unsigned long rnd ()
{
  static int i;

  if (!i)
  {
    i = 1;
    srand (time (0));
  }
  return (time (0) + rand ()) & 0xFFFFFFFFul;
}

/*
 * 1 -- created, 0 -- already busy
 */
int create_empty_sem_file (char *name)
{
  int h;

  if ((h = open (name, O_RDWR | O_CREAT | O_EXCL, 0666)) == -1)
    return 0;
  close (h);
  return 1;
}

int create_sem_file (char *name)
{
  int h, i;
  char buf[10];

  if ((h = open (name, O_RDWR | O_CREAT | O_EXCL, 0666)) == -1)
  { Log (5, "Can't create %s: %s", name, strerror(errno));
    return 0;
  }
  sprintf (buf, "%u\n", (int) getpid ());
  if ((i = write(h, buf, strlen(buf))) != (int)strlen(buf))
  { if (i == -1)
      Log (2, "Can't write to %s (handle %d): %s", name, h, strerror(errno));
    else
      Log (2, "Can't write %d bytes to %s, wrote only %d", strlen(buf), name, i);
  }
  close (h);
  Log (5, "created %s", name);
  return 1;
}

#if defined(EMX) || defined(__WATCOMC__)
#include <malloc.h>		       /* for _heapchk() */
#endif

void Log (int lev, char *s,...)
{
#if defined(HAVE_THREADS) || defined(AMIGA)

  static MUTEXSEM LSem;

#endif
#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)

  extern int syslog_facility;

#endif
#if defined(UNIX) || defined(OS2) || defined(AMIGA)

  extern int inetd_flag;

#endif

  static int first_time = 1;
  char timebuf[60];
  time_t t;
  struct tm *tm;
  va_list ap;
  static const char *marks = "!?+-";
  char ch = (0 <= lev && lev < (int) strlen (marks)) ? marks[lev] : ' ';

  if (first_time == 1)
  {
    InitSem (&LSem);
    first_time = 2;
  }

  time (&t);
  tm = localtime (&t);

  if (lev <= conlog
#if defined(UNIX) || defined(OS2) || defined(AMIGA)
      && !inetd_flag
#endif
     )
  {
    strftime (timebuf, sizeof (timebuf), "%H:%M", tm);
    LockSem (&LSem);
    fprintf (stderr, "%30.30s\r%c %s [%i] ", " ", ch, timebuf, (int) PID ());
    va_start (ap, s);
    vfprintf (stderr, s, ap);
    va_end (ap);
    if (lev >= 0)
      fputc ('\n', stderr);
    ReleaseSem (&LSem);
    if (lev < 0)
      return;
  }

  if (lev <= loglevel && *logpath)
  {
    FILE *logfile = 0;
    int i;

    LockSem (&LSem);
    for (i = 0; logfile == 0 && i < 10; ++i)
    {
      logfile = fopen (logpath, "a");
    }
    if (logfile)
    {
      if (first_time)
      {
	fputc ('\n', logfile);
	first_time = 0;
      }
      strftime (timebuf, sizeof (timebuf), "%d %b %H:%M:%S", tm);
      fprintf (logfile, "%c %s [%i] ", ch, timebuf, (int) PID ());
      va_start (ap, s);
      vfprintf (logfile, s, ap);
      va_end (ap);
      fputc ('\n', logfile);
      fclose (logfile);
    }
    else
      fprintf (stderr, "Cannot open %s: %s!\n", logpath, strerror (errno));
    ReleaseSem (&LSem);
  }
#ifdef WIN32
  if((!lev)&&(isService))
  {
    char tmp[256];
    va_start (ap, s);
    vsprintf (tmp, s, ap);
    va_end (ap);
    MessageBox(NULL, tmp, MYNAME, MB_OK|MB_ICONSTOP|0x00200000L|MB_SYSTEMMODAL|MB_SETFOREGROUND);
  }
#endif

#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)
  if (lev <= loglevel && syslog_facility >= 0)
  {
    static int opened = 0;
    static int log_levels[] =
    {
      /* Correspondence between binkd's loglevel and syslog's priority */
      LOG_ERR,			       /* 0 */
      LOG_WARNING,		       /* 1 */
      LOG_NOTICE,		       /* 2 */
      LOG_INFO,			       /* 3 */
      LOG_INFO,			       /* 4 */
      LOG_INFO,			       /* 5 */
      LOG_INFO,			       /* 6 */
      LOG_DEBUG			       /* other */
    };

    if (!opened)
    {
      opened = 1;
      openlog ("binkd", LOG_PID, syslog_facility);
    }

    if (lev < 0 || lev >= sizeof log_levels / sizeof (int))
      lev = sizeof log_levels / sizeof (int) - 1;

    va_start (ap, s);
    vsyslog (log_levels[lev], s, ap);
    va_end (ap);

  }
#endif

  if (lev == 0)
    exit (1);

#if defined(EMX) || defined(__WATCOMC__)
/*
  assert (_heapchk () == _HEAPOK || _heapchk () == _HEAPEMPTY);
*/
#endif
}

int o_memicmp (const void *s1, const void *s2, size_t n)
{
  int i;

  for (i = 0; i < (int) n; ++i)
    if (tolower (((char *) s1)[i]) != tolower (((char *) s2)[i]))
      return (tolower (((char *) s1)[i]) - tolower (((char *) s2)[i]));

  return 0;
}

int o_stricmp (const char *s1, const char *s2)
{
  int i;

  for (i = 0;; ++i)
  {
    if (tolower (s1[i]) != tolower (s2[i]))
      return (tolower (s1[i]) - tolower (s2[i]));
    if (!s1[i])
      return 0;
  }
}

int o_strnicmp (const char *s1, const char *s2, size_t n)
{
  int i;

  for (i = 0; i < (int) n; ++i)
  {
    if (tolower (s1[i]) != tolower (s2[i]))
      return (tolower (s1[i]) - tolower (s2[i]));
    if (!s1[i])
      return 0;
  }
  return 0;
}

/*
 * Quotes all special chars. free() it's retval!
 */
char *strquote (char *s, int flags)
{
  char *r = xalloc (strlen (s) * 4 + 1);
  int i;

  for (i = 0; *s; ++s)
  {
    if (((flags & SQ_CNTRL) && iscntrl (*s)) ||
	((flags & SQ_SPACE) && isspace (*s)) ||
	(*s == '\\'))
    {
      sprintf (r + i, "\\x%02x", *(unsigned char *) s);
      i += 4;
    }
    else
      r[i++] = *s;
  }
  r[i] = 0;
  return r;
}

/*
 * Reverse for strquote(), free it's return value!
 */
char *strdequote (char *s)
{
  char *r = xstrdup (s);
  int i = 0;

  while (*s)
  {
#define XD(x) (isdigit(x) ? ((x)-'0') : (tolower(x)-'a'+10))
    if (s[0] == '\\' && s[1] == 'x' && isxdigit (s[2]) && isxdigit (s[3]))
    {
      r[i++] = XD (s[2]) * 16 + XD (s[3]);
      s += 4;
    }
    else if (s[0] == '\\' && isxdigit (s[1]) && isxdigit (s[2]))
    {
      r[i++] = XD (s[1]) * 16 + XD (s[2]);
      s += 3;
    }
#undef XD
    else
      r[i++] = *(s++);
  }
  r[i] = 0;
  return r;
}

/*
 * Makes file system-safe names by wiping suspicious chars with '_'
 */
char *strwipe (char *s)
{
  int i;

  for (i = 0; s[i]; ++i)
    if ((iscntrl (s[i]) || s[i] == '\\' || s[i] == '/' || s[i] == ':')
#ifdef WIN32 /* ungly hack */
        && ((s[i] & 0x80) == 0)
#endif
       )
      s[i] = '_';
  return s;
}

/*
 * Copyes not more than len chars from src into dst, but, unlike strncpy(),
 * it appends '\0' even if src is longer than len.
 */
char *strnzcpy (char *dst, const char *src, size_t len)
{
  dst[len - 1] = 0;
  return strncpy (dst, src, len - 1);
}

char *strnzcat (char *dst, const char *src, size_t len)
{
  int x = strlen (dst);

  if (len <= x) return dst;
  return strnzcpy (dst + x, src, len - x);
}

/*
 * Splits args ASCIIZ string onto argc separate words,
 * saves them as argv[0]...argv[argc-1]. Logs error
 * "ID: cannot parse args", if args containes less than argc words.
 */
int parse_args (int argc, char *argv[], char *src, char *ID)
{
  int i = 0;

  while (i < argc)
  {
    while (*src && isspace (*src))
      ++src;
    if (!*src)
      break;
    argv[i] = src;
    while (*src && !isspace (*src))
      ++src;
    ++i;
    if (!*src)
      break;
    src++[0] = 0;
  }
  if (i < argc)
  {
    Log (1, "%s: cannot parse args", ID, src);
    return 0;
  }
  else
    return 1;
}

/*
 * Set times for a file, 0 == success, -1 == error
 */
int touch (char *file, time_t t)
{
#ifndef OS2
  struct utimbuf utb;

  utb.actime = utb.modtime = t;
  return utime (file, &utb);
#else /* FastEcho deletes *.bsy by ctime :-( */
  APIRET r;
  FILESTATUS3 buf;
  struct stat st;

  if ((r = stat(file, &st)) == 0)
  {
    struct tm *tm;
#ifdef __WATCOMC__
    struct tm stm;
    tm = &stm;
    _localtime(&t, tm);
#else
    tm=localtime(&t);
#endif
    buf.fdateCreation.day=buf.fdateLastAccess.day=buf.fdateLastWrite.day=
        tm->tm_mday;
    buf.fdateCreation.month=buf.fdateLastAccess.month=buf.fdateLastWrite.month=
        tm->tm_mon+1;
    buf.fdateCreation.year=buf.fdateLastAccess.year=buf.fdateLastWrite.year=
        tm->tm_year-80;
    buf.ftimeCreation.twosecs=buf.ftimeLastAccess.twosecs=buf.ftimeLastWrite.twosecs=
        tm->tm_sec/2;
    buf.ftimeCreation.minutes=buf.ftimeLastAccess.minutes=buf.ftimeLastWrite.minutes=
        tm->tm_min;
    buf.ftimeCreation.hours=buf.ftimeLastAccess.hours=buf.ftimeLastWrite.hours=
        tm->tm_hour;
    buf.cbFile = buf.cbFileAlloc = st.st_size;
    buf.attrFile = FILE_ARCHIVED | FILE_NORMAL;
    r=DosSetPathInfo(file, FIL_STANDARD, &buf, sizeof(buf), 0);
    if (r == 32)
      r = 0; /* Can't touch opened *.bsy */
    if (r)
      Log (1, "touch: DosSetPathInfo(%s) retcode %d", file, r);
  }
  return (r!=0);
#endif
}

/*
 * Replaces all entries of a in s for b, returns edited line.
 * Returned value must be free()'d. Ignores case.
 * size parameter can be used only if "s" was created by malloc()
 */
char *ed (char *s, char *a, char *b, unsigned int *size)
{
   int i, j;
   unsigned int sz;
   int len_a=a?strlen(a):0;
   int len_b=b?strlen(b):0;
   char *r=s;
   
   if((!len_a)||(!s)) return r;
   if(!size) 
   {
     sz=strlen(s)+1;
     r=xstrdup(s);
   }
   else sz=*size;
   for(i=j=0;i<(int)strlen(r);i++)
   {
     if(tolower(r[i])!=tolower(a[j++]))
     {
       j=0;
       continue;
     }
     if(a[j]) continue;
     if(strlen(r)-len_a+len_b>=sz)
     {
       if(len_b<64) sz+=64;
       else sz+=len_b;
       r=xrealloc(r, sz);
     }
     i-=len_a-1;
     memmove(r+i+len_b, r+i+len_a, strlen(r+i+len_a)+1);
     if(len_b)
       memcpy(r+i, b, len_b);
     j=0;
     i+=len_b-1;
   }
   if(size) *size=sz;
   return r;
}

/*
 * Remove/trucate a file, log this
 */
int delete (char *path)
{
  int rc;

  if ((rc = unlink (path)) != 0)
    Log (1, "error unlinking `%s': %s", path, strerror (errno));
  else
    Log (5, "unlinked `%s'", path);

  return rc;
}

int trunc (char *path)
{
  int h;

  if ((h = open (path, O_WRONLY | O_TRUNC)) == -1)
  {
    Log (1, "cannot truncate `%s': %s", path, strerror (errno));
    return -1;
  }
  else
  {
    Log (4, "truncated %s", path);
    close (h);
    return 0;
  }
}

/*
 * Get the string with OS name/version
 */
#ifdef HAVE_UNAME
#include <sys/utsname.h>
#endif

char *get_os_string ()
{
  static char os[80];

  strcpy (os, "/");

#ifdef HAVE_UNAME
  {
    struct utsname name;

    if (uname (&name) == 0)
    {
      strnzcat (os, name.sysname, sizeof (os));
      return os;
    }
  }
#endif

#ifdef OS
  strnzcat (os, OS, sizeof (os));
#else
  *os = 0;
#endif

  return os;
}

/*
 * Test netnames against some wildcards
 */
int ispkt (char *s)
{
  return pmatch ("*.[Pp][Kk][Tt]", s);
}

int isreq (char *s)
{
  return pmatch ("*.[Rr][Ee][Qq]", s);
}

int istic (char *s)
{
  return pmatch ("*.?[Ii][Cc]", s);
}

int isarcmail (char *s)
{
  /* *.su? *.mo? *.tu? *.we? *.th? *.fr? *.sa? */
  return (pmatch ("*.[Ss][Uu]?", s) ||
	  pmatch ("*.[Mm][Oo]?", s) ||
	  pmatch ("*.[Tt][Uu]?", s) ||
	  pmatch ("*.[Ww][Ee]?", s) ||
	  pmatch ("*.[Tt][Hh]?", s) ||
	  pmatch ("*.[Ff][Rr]?", s) ||
	  pmatch ("*.[Ss][Aa]?", s));
}

/*
 * Formats and prints argv into buf (for logging purposes)
 */
void print_args (char *buf, size_t sizeof_buf, int argc, char *argv[])
{
  int i, j, quote;

  assert (sizeof_buf > 5);
  *buf = 0;
  for (i = 0; i < argc; ++i)
  {
    quote = 0;
    if (argv[i][0] == 0)
      quote = 1;
    else
      for (j = 0; argv[i][j]; ++j)
	if (argv[i][j] <= ' ')
	{
	  quote = 1;
	  break;
	}

    strnzcat (buf, " ", sizeof_buf);
    if (quote)
      strnzcat (buf, "\"", sizeof_buf);
    strnzcat (buf, argv[i], sizeof_buf);
    if (quote)
      strnzcat (buf, "\"", sizeof_buf);
  }
}

/*
 * Dup argv
 */
char **mkargv (int argc, char **argv)
{
  int i;
  char **p;

  p = (char **) xalloc ((argc + 1) * sizeof (p));

  for (i = 0; i < argc; i++)
    p[i] = xstrdup (argv[i]);

  p[i] = NULL;

  return p;
}
