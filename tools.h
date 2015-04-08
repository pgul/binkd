/*
 *  tools.h -- misc utils
 *
 *  tools.h is a part of binkd project
 *
 *  Copyright (C) 1996-1998  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#ifndef _tools_h
#define _tools_h

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <sys/types.h>
#include <time.h>
#ifdef VISUALCPP
#include <malloc.h>  /* for size_t */
#endif

#include "getw.h"
#include "btypes.h"

#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif

#ifndef max
#define max(x,y) ((x) > (y) ? (x) : (y))
#endif

void vLog (int lev, char *s, va_list ap);
void Log (int lev, char *s, ...);
void InitLog(int loglevel, int conlog, char *logpath, void *first);

#define LOGINT(v) Log(6, "%s=%i\n", #v, (int)(v))

/*
 * (xalloc.c) [Re]allocate memory or log error
 */
void *xalloc (size_t size);
void *xrealloc (void *ptr, size_t size);
void *xstrdup (const char *str);
void *xstrcat (char **str, const char *s2);
void xfree(void *ptr);

/*
 * Compare strings ignoring case
 */
int o_stricmp (const char *s1, const char *s2);
int o_memicmp (const void *s1, const void *s2, size_t n);
int o_strnicmp (const char *s1, const char *s2, size_t n);

#define STRICMP(a,b)    o_stricmp(a,b)
#define MEMICMP(a,b,c)  o_memicmp(a,b,c)
#define STRNICMP(a,b,c) o_strnicmp(a,b,c)

/*
 * Lowercase the string
 */
char *strlower (char *);

/*
 * Uppercase the string
 */
char *strupper (char *);

/*
 * Copyes not more than len chars from src into dst, but, unlike strncpy(),
 * it appends '\0' even if src is longer than len.
 */
char *strnzcpy (char *dst, const char *src, size_t len);
char *strnzcat (char *dst, const char *src, size_t len);

/*
 * Quotes all special chars. free() it's retval!
 */
#define SQ_CNTRL 1
#define SQ_SPACE 2
char *strquote (char *s, int flags);

/*
 * Reverse for strquote(), free it's return value!
 */
char *strdequote (char *s);

/*
 * Makes file system-safe names by wiping suspicious chars with '_'
 */
char *strwipe (char *s);

/*
 * 1 -- created, 0 -- already busy
 */
int create_sem_file (char *s, int loglevel);
int create_empty_sem_file (char *s);

/*
 */
unsigned long rnd (void);

/*
 * Makes all dirs in the path
 */
int mkpath (char *s);

/*
 * Splits args ASCIIZ string onto argc separate words,
 * saves them as argv[0]...argv[argc-1]. Logs error
 * "ID: cannot parse args", if args containes less than argc words.
 */
char *parse_args (int argc, char *argv[], char *src, char *ID);

/*
 * (pmatch.c) Returns true if the pattern matches the string.
 */
int xpmatch (char *pattern, char *string, int ncase);
#define pmatch(pattern, string) xpmatch(pattern, string, 0)
#define pmatch_ncase(pattern, string) xpmatch(pattern, string, 1)

/*
 * Set times for a file, 0 == success, -1 == error
 */
int touch (char *file, time_t t);

/*
 * Replaces all entries of a in s for b, returns edited line.
 * Returned value must be free()'d. Ignores case.
 */
char *ed (char *src, char *a, char *b, size_t *size);

/*
 * Remove/trucate a file, log this
 */
int delete (char *);
int trunc_file (char *);

#ifdef UNIX
#define sdelete(file) delete(file)
#else
/*
 * reliable remove a file (wait for lock), log this
 */
int sdelete (char *);
#endif

/*
 * Get the string with OS name/version
 */
char *get_os_string (void);

/*
 * Test netnames against some wildcards
 */
int ispkt (char *s);
int isreq (char *s);
int isarcmail (char *s);
int istic (char *s);

/*
 * Formats and prints argv into buf (for logging purposes)
 */
void print_args (char *buf, size_t sizeof_buf, char *argv[]);

/*
 * Dup argv
 * Assume argv is NULL-terminated if argc<0 (used for envp)
 */
char **mkargv (int argc, char **argv);

/*
 * Apply filename case style defined in inboundcase
 */
char *makeinboundcase (char *s, enum inbcasetype inbcase);

/*
 * Thread-safe localtime & gmtime functions
 */
#ifdef WIN32
time_t safe_time(void);
#else
#define safe_time()		time(NULL)
#endif
#ifdef HAVE_LOCALTIME_R
#define safe_localtime(t, tm)	localtime_r(t, tm)
#define safe_gmtime(t, tm)	gmtime_r(t, tm)
#else
struct tm *safe_gmtime(time_t *t, struct tm *tm);
struct tm *safe_localtime(time_t *t, struct tm *tm);
#endif
int tz_off(time_t t, int tzoff);


/* Safe string to long conversion: negative converts using atol,
 * positive: strtoul();
 * leading spaces - impossibled!
 * Return error message in msg[0] (static string) and set errno.
 * errno set to zero if no error
 */
long safe_atol(char* str, char** msg);

/* Return last directory separator in file name, or NULL if no path present */
char * last_slash(char *s);

/* Extract filename from path */
char *extract_filename(char *s);

/* parse FTN address of the pkt header byte array */
int pkt_getaddr(unsigned char *raw, 
                short *oz, short *onet, short *onode, short *op,
                short *dz, short *dnet, short *dnode, short *dp);
/* set FTN address into the pkt header byte array */
int pkt_setaddr(unsigned char *raw, 
                short oz, short onet, short onode, short op,
                short dz, short dnet, short dnode, short dp);

#endif
