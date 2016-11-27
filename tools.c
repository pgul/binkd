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

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)
#include <syslog.h>
#endif

#include "sys.h"
#include "readcfg.h"
#include "common.h"
#include "tools.h"
#include "readdir.h"		       /* for [sys/]utime.h */
#include "sem.h"
#include "assert.h"

#ifdef WITH_PERL
#include "perlhooks.h"
#endif

#if defined(WIN32) && !defined(BINKD9X)
#include "nt/w32tools.h"
#endif

/*
 * We can call Log() even when we have no config ready. So, we must keep
 * internal variables which will be updated when config is loaded
 */
static int  current_loglevel = 1;
static int  current_conlog   = 1;
static char *current_logpath; /* This is malloc'ed string and can be NULL */
static struct maskchain *current_nolog = NULL;

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

/*
 * Uppercase the string
 */
char *strupper (char *s)
{
  int i;

  for (i = 0; s[i]; ++i)
    s[i] = toupper (s[i]);
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
    if ((s = last_slash(path)) != 0)
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
  if ((s = last_slash(path)) == 0)
    return 0;
  *s = 0;
  return mkpath0 (path);
}

unsigned long rnd (void)
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

int create_sem_file (char *name, int errloglevel)
{
  int h, i;
  char buf[16];

  if ((h = open (name, O_RDWR | O_CREAT | O_EXCL, 0666)) == -1)
  { Log (errloglevel, "Can't create %s: %s", name, strerror(errno));
    return 0;
  }
  snprintf (buf, sizeof (buf), "%u\n", (int) getpid ());
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

#ifndef HAVE_LOCALTIME_R
struct tm *safe_gmtime(time_t *t, struct tm *tm)
{
  threadsafe(memcpy(tm, gmtime(t), sizeof(*tm)));
  return tm;
}
#endif

#ifdef WIN32

static void stime_to_tm(const SYSTEMTIME *stime, struct tm *tm)
{
  tm->tm_year = stime->wYear-1900;
  tm->tm_mon  = stime->wMonth-1;
  tm->tm_mday = stime->wDay;
  tm->tm_wday = stime->wDayOfWeek;
  tm->tm_hour = stime->wHour;
  tm->tm_min  = stime->wMinute;
  tm->tm_sec  = stime->wSecond;
}

static void tm_to_stime(const struct tm *tm, SYSTEMTIME *stime)
{
  stime->wYear         = tm->tm_year+1900;
  stime->wMonth        = tm->tm_mon+1;
  stime->wDay          = tm->tm_mday;
  stime->wDayOfWeek    = tm->tm_wday;
  stime->wHour         = tm->tm_hour;
  stime->wMinute       = tm->tm_min;
  stime->wSecond       = tm->tm_sec;
  stime->wMilliseconds = 0;
}

static int safe_cmptime(const struct tm *tm1, const struct tm *tm2)
{
  if (tm1->tm_year > tm2->tm_year) return  1;
  if (tm1->tm_year < tm2->tm_year) return -1;
  if (tm1->tm_mon  > tm2->tm_mon ) return  1;
  if (tm1->tm_mon  < tm2->tm_mon ) return -1;
  if (tm1->tm_mday > tm2->tm_mday) return  1;
  if (tm1->tm_mday < tm2->tm_mday) return -1;
  if (tm1->tm_hour > tm2->tm_hour) return  1;
  if (tm1->tm_hour < tm2->tm_hour) return -1;
  return 0;
}

time_t safe_time(void)
{
  /* gmtime(t) should be SystemTime */
  SYSTEMTIME stime;
  struct tm utctm, tm;
  time_t t;
  int i;

  GetSystemTime(&stime);
  stime_to_tm(&stime, &utctm);
  t = time(NULL);
  t -= (t-utctm.tm_sec)%60;
  for (i=0; i<24; i++) {
    safe_gmtime(&t, &tm);
    if (tm.tm_hour == utctm.tm_hour) return t;
    t += safe_cmptime(&tm, &utctm) > 0 ? -3600 : 3600;
  }
  return t;
}
#endif

#if !defined(HAVE_LOCALTIME_R)
#ifdef WIN32
struct tm *safe_localtime(time_t *t, struct tm *tm)
{
  FILETIME utcftime, localftime;
  SYSTEMTIME stime;

  safe_gmtime(t, tm);
  /* convert gmtime to localtime */
  tm_to_stime(tm, &stime);
  SystemTimeToFileTime(&stime, &utcftime);
  FileTimeToLocalFileTime(&utcftime, &localftime);
  FileTimeToSystemTime(&localftime, &stime);
  stime_to_tm(&stime, tm);
  return tm;
}
#else
struct tm *safe_localtime(time_t *t, struct tm *tm)
{
  threadsafe(memcpy(tm, localtime(t), sizeof(*tm)));
  return tm;
}
#endif
#endif

void InitLog(int loglevel, int conlog, char *logpath, void *first)
{
  LockSem(&lsem);
  xfree(current_logpath);
  current_logpath  = NULL;   /* just in case if xstrdup() fails */
  current_loglevel = loglevel;
  current_conlog   = conlog;
  current_logpath  = xstrdup(logpath);
  current_nolog    = (struct maskchain *)first;
  ReleaseSem(&lsem);
}

void vLog (int lev, char *s, va_list ap)
{
  static int first_time = 1;
  static const char *month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                 "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
  char buf[1024];
  int ok = 1;

  /* make string in buffer */
  vsnprintf(buf, sizeof(buf), s, ap);
  /* do perl hooks */
#ifdef WITH_PERL
  if (!perl_on_log(buf, sizeof(buf), &lev)) ok = 0;
#endif
  /* match against nolog */
  if (mask_test(buf, current_nolog) != NULL ) ok = 0;
  /* log output */
  if (ok)
  { /* if (ok) */

    char *using_logpath=NULL;
    struct tm tm;
    time_t t;
    static const char *marks = "!?+-";
    char ch = (0 <= lev && lev < (int) strlen (marks)) ? marks[lev] : ' ';
    t = safe_time();
    safe_localtime (&t, &tm);

    if (lev <= current_conlog && !inetd_flag)
    {
      LockSem(&lsem);
      fprintf (stderr, "%30.30s\r%c %02d:%02d [%u] %s%s", " ", ch,
           tm.tm_hour, tm.tm_min, (unsigned) PID (), buf, (lev >= 0) ? "\n" : "");
      fflush (stderr);
      ReleaseSem(&lsem);
      if (lev < 0)
        return;
    }

    using_logpath = (current_logpath && *current_logpath) ?
                                 current_logpath : getenv(BINKD_LOGPATH_ENVIRON);
    if (lev <= current_loglevel && using_logpath)
    {
      FILE *logfile = 0;
      int i;

      LockSem(&lsem);
      for (i = 0; logfile == 0 && i < 10; ++i)
        logfile = fopen (using_logpath, "a");
      if (logfile)
      {
        fprintf (logfile, "%s%c %02d %s %02d:%02d:%02d [%u] %s\n",
               first_time ? "\n" : "", ch,
               tm.tm_mday, month[tm.tm_mon], tm.tm_hour, tm.tm_min, tm.tm_sec,
               (unsigned) PID (), buf);
        fclose (logfile);
        first_time = 0;
      }
      else
        fprintf (stderr, "Cannot open %s: %s!\n", using_logpath, strerror (errno));
      ReleaseSem(&lsem);
    }
#ifdef WIN32
#ifdef BINKD9X
    if (!lev)
#else
    if (lev < 1 && isService() > 0)
#endif
      MessageBox(NULL, buf, MYNAME, MB_OK|MB_ICONSTOP|0x00200000L|MB_SYSTEMMODAL|MB_SETFOREGROUND);
#endif

#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)
    if (lev <= current_loglevel && syslog_facility >= 0)
    {
      static int opened = 0;
      static int log_levels[] =
      {
        /* Correspondence between binkd's loglevel and syslog's priority */
        LOG_ERR,		       /* 0 */
        LOG_WARNING,		       /* 1 */
        LOG_NOTICE,		       /* 2 */
        LOG_INFO,		       /* 3 */
        LOG_INFO,		       /* 4 */
        LOG_INFO,		       /* 5 */
        LOG_INFO,		       /* 6 */
        LOG_DEBUG		       /* other */
      };

      if (!opened)
      {
        opened = 1;
        openlog ("binkd", LOG_PID, syslog_facility);
      }

      if (lev < 0 || lev >= sizeof log_levels / sizeof (int))
        lev = sizeof log_levels / sizeof (int) - 1;

      vsyslog (log_levels[lev], s, ap);

    }
#endif
  } /* if (ok) */

  if (lev == 0)
    exit (1);

#if defined(EMX) || defined(__WATCOMC__)
/*
  assert (_heapchk () == _HEAPOK || _heapchk () == _HEAPEMPTY);
*/
#endif
}

void Log (int lev, char *s, ...)
{
  va_list ap;

  va_start(ap, s);
  vLog(lev, s, ap);
  va_end(ap);
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
#ifdef WIN32 /* ugly hack */
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
  size_t x = strlen (dst);

  if (len <= x) return dst;
  return strnzcpy (dst + x, src, len - x);
}

/*
 * Splits args ASCIIZ string onto argc separate words,
 * saves them as argv[0]...argv[argc-1]. Logs error
 * "ID: cannot parse args", if args containes less than argc words.
 */
char *parse_args (int argc, char *argv[], char *src, char *ID)
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
    return NULL;
  }
  else
    return src;
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
    struct tm tm;
    safe_localtime(&t, &tm);
    buf.fdateCreation.day=buf.fdateLastAccess.day=buf.fdateLastWrite.day=
        tm.tm_mday;
    buf.fdateCreation.month=buf.fdateLastAccess.month=buf.fdateLastWrite.month=
        tm.tm_mon+1;
    buf.fdateCreation.year=buf.fdateLastAccess.year=buf.fdateLastWrite.year=
        tm.tm_year-80;
    buf.ftimeCreation.twosecs=buf.ftimeLastAccess.twosecs=buf.ftimeLastWrite.twosecs=
        tm.tm_sec/2;
    buf.ftimeCreation.minutes=buf.ftimeLastAccess.minutes=buf.ftimeLastWrite.minutes=
        tm.tm_min;
    buf.ftimeCreation.hours=buf.ftimeLastAccess.hours=buf.ftimeLastWrite.hours=
        tm.tm_hour;
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
char *ed (char *s, char *a, char *b, size_t *size)
{
   size_t i, j, sr, sz;
   size_t len_a=a?strlen(a):0;
   size_t len_b=b?strlen(b):0;
   char *r=s;

   if ((!len_a) || (!s)) return r;
   if (!size)
   {
     sz=strlen(s)+1;
     r=xstrdup(s);
   }
   else sz=*size;
   sr=strlen(r);
   for (i=j=0; i<sr; i++)
   {
     if (tolower(r[i])!=tolower(a[j]))
     {
       i-=j;
       j=0;
       continue;
     }
     j++;
     if (a[j]) continue;
     if (sr-len_a+len_b>=sz)
     {
       if (len_b<64) sz+=64;
       else sz+=len_b;
       r=xrealloc(r, sz);
     }
     i-=len_a-1;
     if (len_a!=len_b)
       memmove(r+i+len_b, r+i+len_a, sr-i-len_a+1);
     if (len_b)
       memcpy(r+i, b, len_b);
     j=0;
     i+=len_b-1;
     sr+=len_b-len_a;
   }
   if (size) *size=sz;
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

int trunc_file (char *path)
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

#ifndef UNIX
/*
 * reliable remove a file (wait for lock), log this
 */
int sdelete (char *path)
{
  int i, rc;

  for (i=0; i<5; i++) {
    if ((rc = unlink (path)) == 0) {
      Log (6, "unlinked `%s'", path);
      return 0;
    }
    else if (errno == EPERM || errno == EACCES || errno == EAGAIN)
      sleep (1);
    else
      break;
  }
  Log (1, "error unlinking `%s': %s", path, strerror (errno));

  return rc;
}
#endif

/*
 * Get the string with OS name/version
 */
#ifdef HAVE_UNAME
#include <sys/utsname.h>
#endif

char *get_os_string (void)
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
  return pmatch_ncase("*.pkt", s);
}

int isreq (char *s)
{
  return pmatch_ncase("*.req", s);
}

int istic (char *s)
{
  return pmatch_ncase("*.?ic", s);
}

int isarcmail (char *s)
{
  return (pmatch_ncase("*.su?", s) ||
          pmatch_ncase("*.mo?", s) ||
          pmatch_ncase("*.tu?", s) ||
          pmatch_ncase("*.we?", s) ||
          pmatch_ncase("*.th?", s) ||
          pmatch_ncase("*.fr?", s) ||
          pmatch_ncase("*.sa?", s));
}

/*
 * Formats and prints argv into buf (for logging purposes)
 */
void print_args (char *buf, size_t sizeof_buf, char *argv[])
{
  int i, j, quote;

  assert (sizeof_buf > 5);
  *buf = 0;
  for (i = 0; argv[i]; ++i)
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
 * Assume argv is NULL-terminated if argc<0 (used for envp)
 */
char **mkargv (int argc, char **argv)
{
  int i;
  char **p;

  if (argv == NULL)
    return NULL;

  if (argc < 0)
    for (argc = 0; argv[argc]; argc++);

  p = (char **) xalloc ((argc + 1) * sizeof (*p));

  for (i = 0; i < argc; i++)
    p[i] = xstrdup (argv[i]);

  p[i] = NULL;

  return p;
}

/*
 * Apply filename case style defined in inboundcase
 */
char *makeinboundcase (char *s, enum inbcasetype inbcase)
{
  int i;

  switch (inbcase)
  {
      case INB_UPPER:
	s = strupper(s);
	Log (8, "uppercase filename");
        break;
      case INB_LOWER:
	s = strlower(s);
	Log (8, "lowercase filename");
        break;
      case INB_MIXED:
        s[0] = toupper (s[0]);
        for (i = 1; s[i]; ++i)
          s[i] = isalnum(s[i-1]) ? tolower(s[i]) : toupper(s[i]);
        Log (8, "mixing filename case");
        break;
      default:
	Log (8, "nothing to do with filename case");
        break;
  }

  return s;
}

/* Safe string to long conversion: negative converts using atol,
 * positive: strtoul();
 * leading spaces - impossibled!
 * Return error message in msg[0] (static string) and set errno.
 * errno set to zero if no error
 */
long safe_atol(char* str, char** msg){
  unsigned long ul=0;
  long l;

  if(str){
    errno = 0;
    if( str[0]=='-' ){  /* negative value */
      l = atol(str);
      if(errno==ERANGE && msg){
         *msg = "Out of range: number too small";
      }
      return l;
    }
    ul = strtoul(str,NULL,10);
    if(errno==ERANGE && msg){
       *msg = "Out of range: number too big";
    }
  }else{
    errno = EINVAL;
    if (msg) {
      *msg = "Invalid argument (NULL instead string)";
   }
  }
  return (long)ul;
}

/* Return last directory separator in file name, or NULL if no path present */
char * last_slash(char *s)
{
  char *ret;

  for (ret = NULL; *s; s++)
    if (*s == '/' || *s == '\\')
      ret = s;
  return ret;
}

/* Extract filename from path */
char *extract_filename(char *s)
{
  char *tmp = max(last_slash(s), strrchr(s, ':'));
  return tmp? tmp+1: s;
}

#define GET_BYTE2(arr,x) (arr[(x)] + arr[(x)+1]*0x100)
#define SET_BYTE2(arr,x,v) { arr[(x)] = v & 0xff; arr[(x)+1] = (v/0x100) & 0xff; }
/* parse FTN address of the pkt header byte array */
int pkt_getaddr(unsigned char *raw,
                short *oz, short *onet, short *onode, short *op,
                short *dz, short *dnet, short *dnode, short *dp)
{
  if (oz)    *oz    = -1;
  if (onet)  *onet  = -1;
  if (onode) *onode = -1;
  if (op)    *op    = -1;
  if (dz)    *dz    = -1;
  if (dnet)  *dnet  = -1;
  if (dnode) *dnode = -1;
  if (dp)    *dp    = -1;
  /* wrong pkt version */
  if (GET_BYTE2(raw, 18) != 2) return 0;
  /* fts-1 net and node */
  if (onode) *onode = GET_BYTE2(raw, 0);
  if (onet)  *onet  = GET_BYTE2(raw, 20);
  if (dnode) *dnode = GET_BYTE2(raw, 2);
  if (dnet)  *dnet  = GET_BYTE2(raw, 22);
  /* qmail, zmail - not point-aware, get zone */
  if (raw[24] == 0x29 || raw[24] == 0x35) {
    if (oz) *oz = GET_BYTE2(raw, 34);
    if (dz) *dz = GET_BYTE2(raw, 36);
  }
  /* type-2 */
  else if (raw[44] & 1) {
    if (oz) *oz = GET_BYTE2(raw, 46);
    if (op) *op = GET_BYTE2(raw, 50);
    if (dz) *dz = GET_BYTE2(raw, 48);
    if (dp) *dp = GET_BYTE2(raw, 52);
    /* type 2+ */
    if (raw[44] == raw[41] && raw[45] == raw[40]) {
      if (onet && GET_BYTE2(raw, 50) != 0 && *onet == -1)
        *onet = GET_BYTE2(raw, 38);
    }
  }
  return 1;
}
/* set FTN address into the pkt header byte array */
int pkt_setaddr(unsigned char *raw,
                short oz, short onet, short onode, short op,
                short dz, short dnet, short dnode, short dp)
{
  /* wrong pkt version */
  if (GET_BYTE2(raw, 18) != 2) return 0;
  /* fts-1 net and node */
  if (onode >= 0) SET_BYTE2(raw, 0, onode); if (onet >= 0) SET_BYTE2(raw, 20, onet);
  if (dnode >= 0) SET_BYTE2(raw, 2, dnode); if (dnet >= 0) SET_BYTE2(raw, 22, dnet);
  /* qmail, zmail - not point-aware, get zone */
  if (raw[24] == 0x29 || raw[24] == 0x35) {
    if (oz >= 0) SET_BYTE2(raw, 34, oz);
    if (dz >= 0) SET_BYTE2(raw, 36, dz);
  }
  /* type-2 */
  else if (raw[44] & 1) {
    if (oz >= 0) SET_BYTE2(raw, 46, oz); if (op >= 0) SET_BYTE2(raw, 50, op);
    if (dz >= 0) SET_BYTE2(raw, 48, dz); if (dp >= 0) SET_BYTE2(raw, 52, dp);
    /* type 2+ */
    if (raw[44] == raw[41] && raw[45] == raw[40]) {
      if (GET_BYTE2(raw, 50) != 0) { raw[38] = raw[20]; raw[39] = raw[21]; raw[20] = raw[21] = -1; }
    }
  }
  return 1;
}

int tz_off(time_t t, int tzoff)
{
  struct tm tm;
  time_t gt;

  if (tzoff != -1) return tzoff/60;
  safe_gmtime (&t, &tm);
  tm.tm_isdst = 0;
  gt = mktime(&tm);
  safe_localtime (&t, &tm);
  tm.tm_isdst = 0;
  return (int)(((long)mktime(&tm)-(long)gt)/60);
}

#ifdef WIN32
#include <sys/timeb.h>

void gettvtime(struct timeval *tv)
{
  struct _timeb tb;

  _ftime(&tb);
  tv->tv_sec = tb.time;
  tv->tv_usec = tb.millitm * 1000;
}
#endif

