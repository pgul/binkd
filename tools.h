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

/*
 * $Id$
 *
 * $Log$
 * Revision 2.9  2003/04/04 07:33:11  gul
 * Fix syntax error
 *
 * Revision 2.8  2003/04/02 13:12:58  gul
 * Try to use workaround for buggy windows time functions (timezone)
 *
 * Revision 2.7  2003/03/31 22:47:22  gul
 * remove workaround for msvc localtime() bug
 *
 * Revision 2.6  2003/03/31 20:28:24  gul
 * safe_localtime() and safe_gmtime() functions
 *
 * Revision 2.5  2003/03/05 13:21:51  gul
 * Fix warnings
 *
 * Revision 2.4  2003/03/01 20:16:27  gul
 * OS/2 IBM C support
 *
 * Revision 2.3  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.2  2002/05/06 19:25:40  gul
 * new keyword inboundCase in config
 *
 * Revision 2.1  2001/10/23 08:33:44  gul
 * Change filename (not ext) in incoming *.req if already exists
 *
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 * Revision 1.10  1997/10/23  03:26:47  mff
 * getword*() is in getw.h now
 *
 * Revision 1.9  1997/08/12  20:53:08  mff
 * - DUP(), stricmp() is now STRICMP(), etc.
 *
 * Revision 1.7  1997/05/17  11:18:14  mff
 * + get_os_string()
 *
 * Revision 1.6  1997/03/28  06:10:12  mff
 * + ed()
 */
#ifndef _tools_h
#define _tools_h

#include <stdarg.h>
#include <stddef.h>
#include <sys/types.h>
#include <time.h>
#ifdef VISUALCPP
#include <malloc.h>  /* for size_t */
#endif

#include "getw.h"

#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif

#ifndef max
#define max(x,y) ((x) > (y) ? (x) : (y))
#endif

void Log (int lev, char *s,...);

#define LOGINT(v) Log(6, "%s=%i\n", #v, (int)(v))

/*
 * (xalloc.c) [Re]allocate memory or log error
 */
void *xalloc (size_t size);
void *xrealloc (void *ptr, size_t size);
void *xstrdup (const char *str);

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
int create_sem_file (char *s);
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
int parse_args (int argc, char *argv[], char *src, char *ID);

/*
 * (pmatch.c) Returns true if the pattern matches the string.
 */
int pmatch (char *pattern, char *string);

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
int trunc (char *);

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
void print_args (char *buf, size_t sizeof_buf, int argc, char *argv[]);

/*
 * Dup argv
 */
char **mkargv (int argc, char **argv);

/*
 * Apply filename case style defined in inboundcase
 */
char *makeinboundcase (char *s);

/*
 * Thread-safe localtime & gmtime functions
 */
struct tm *safe_localtime(time_t *t, struct tm *tm);
struct tm *safe_gmtime(time_t *t, struct tm *tm);
time_t safe_time(void);

#endif
