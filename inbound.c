/*
 *  inbound.c -- inbound handling
 *
 *  inbound.c is a part of binkd project
 *
 *  Copyright (C) 1996-1997  Dima Maloff, 5047/13
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
 * Revision 2.13  2003/06/10 07:43:35  gul
 * sdelete() - reliable delete file (wait for lock)
 *
 * Revision 2.12  2003/03/10 10:39:23  gul
 * New include file common.h
 *
 * Revision 2.11  2003/03/05 13:21:50  gul
 * Fix warnings
 *
 * Revision 2.10  2003/03/01 18:29:52  gul
 * Change size_t to off_t for file sizes and offsets
 *
 * Revision 2.9  2003/03/01 15:00:16  gul
 * Join skipmask and overwrite into common maskchain
 *
 * Revision 2.8  2003/02/27 15:37:19  gul
 * Bugfix in disk free space check
 *
 * Revision 2.7  2002/11/22 14:40:42  gul
 * Check free space on inbox if defined
 *
 * Revision 2.6  2002/10/03 10:23:26  gul
 * Check fprintf() & fclose() retcodes
 *
 * Revision 2.5  2002/07/22 19:38:23  gul
 * overwrite minor fix
 *
 * Revision 2.4  2002/07/21 10:35:44  gul
 * overwrite option
 *
 * Revision 2.3  2002/05/06 19:25:39  gul
 * new keyword inboundCase in config
 *
 * Revision 2.2  2001/10/23 08:33:44  gul
 * Change filename (not ext) in incoming *.req if already exists
 *
 * Revision 2.1  2001/04/23 07:58:57  gul
 * getfree() on large drives fixed
 *
 * Revision 2.0  2001/01/10 12:12:38  gul
 * Binkd is under CVS again
 *
 * Revision 1.10  1997/10/23  04:03:07  mff
 * minor fixes
 *
 * Revision 1.9  1997/05/17  08:42:35  mff
 * Added realname param to inb_test()
 *
 * Revision 1.8  1997/03/15  05:08:17  mff
 * utime() replaced with touch()
 *
 * Revision 1.7  1997/03/09  07:19:37  mff
 * Fixed bugs with `#' in filenames and incorrect killing of
 * partial packets
 *
 * Revision 1.6  1997/02/13  07:08:39  mff
 * Support for fileboxes
 *
 * Revision 1.5  1997/02/07  07:03:16  mff
 * Added more paranoia
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "Config.h"
#include "sys.h"
#include "common.h"
#include "readcfg.h"
#include "inbound.h"
#include "tools.h"
#include "protocol.h"
#include "readdir.h"

/* Removes both xxxxx.hr and it's xxxxx.dt */
static void remove_hr (char *path)
{
  int rc;

  strcpy (strrchr (path, '.'), ".dt");
  rc = delete (path);
  strcpy (strrchr (path, '.'), ".hr");
  if (rc == 0)
  {
    sdelete (path);
  }
}

static int creat_tmp_name (char *s, char *file, off_t size,
			    time_t time, FTN_ADDR *from, char *inbound)
{
  FILE *f;
  char tmp[20];
  char *t;
  char node[FTN_ADDR_SZ + 1];

  strnzcpy (s, inbound, MAXPATHLEN);
  strnzcat (s, PATH_SEPARATOR, MAXPATHLEN);
  t = s + strlen (s);
  while (1)
  {
    sprintf (tmp, "%08lx.hr", rnd ());
    strnzcat (s, tmp, MAXPATHLEN);
    if (create_empty_sem_file (s))
    {
      if ((f = fopen (s, "w")) == 0)
      {
	Log (1, "%s: %s", s, strerror (errno));
	delete (s);
	return 0;
      }
      ftnaddress_to_str (node, from);
      if (fprintf (f, "%s %li %li %s\n",
	           file, (long int) size, (long int) time, node) <= 0)
      {
	Log (1, "%s: %s", s, strerror (errno));
	fclose (f);
	delete (s);
	return 0;
      }
      if (fclose (f))
      {
	Log (1, "%s: %s", s, strerror (errno));
	delete (s);
	return 0;
      }
      break;
    }
    *t = 0;
  }
  return 1;
}

static int to_be_deleted (char *tmp_name, char *netname)
{
  struct stat sb;

  strcpy (strrchr (tmp_name, '.'), ".dt");
  if (stat (tmp_name, &sb) == 0 && kill_old_partial_files != 0 &&
      time (0) - sb.st_mtime > kill_old_partial_files)
  {
    Log (4, "found old .dt/.hr files for %s", netname);
    return 1;
  }
  strcpy (strrchr (tmp_name, '.'), ".hr");
  return 0;
}

/*
 * Searches for the ``file'' in the inbound and returns it's tmp name in s.
 * S must have MAXPATHLEN chars. Returns 0 on error, 1=found, 2=created.
 */
int find_tmp_name (char *s, char *file, off_t size,
		    time_t time, FTN_ADDR *from, int nfa, char *inbound)
{
  char buf[MAXPATHLEN + 80];
  DIR *dp;
  struct dirent *de;
  FILE *f;
  int i, found = 0;
  char *t;

  if (temp_inbound[0])
    inbound = temp_inbound;

  if ((dp = opendir (inbound)) == 0)
  {
    Log (1, "cannot opendir %s: %s", inbound, strerror (errno));
    return 0;
  }

  strnzcpy (s, inbound, MAXPATHLEN);
  strnzcat (s, PATH_SEPARATOR, MAXPATHLEN);
  t = s + strlen (s);
  while ((de = readdir (dp)) != 0)
  {
    for (i = 0; i < 8; ++i)
      if (!isxdigit (de->d_name[i]))
	break;
    if (i < 8 || STRICMP (de->d_name + 8, ".hr"))
      continue;
    strnzcat (s, de->d_name, MAXPATHLEN);
    if ((f = fopen (s, "r")) == 0 || !fgets (buf, sizeof (buf), f))
    {
      Log (1, "find_tmp_name: %s: %s", de->d_name, strerror (errno));
      if (f)
	fclose (f);
    }
    else
    {
      char *w[4];
      FTN_ADDR fa;

      fclose (f);
      FA_ZERO (&fa);
      for (i = 0; i < 4; ++i)
	w[i] = getwordx (buf, i + 1, GWX_NOESC);

      if (!strcmp (w[0], file) && parse_ftnaddress (w[3], &fa))
      {
	for (i = 0; i < nfa; i++)
	  if (!ftnaddress_cmp (&fa, from + i))
	    break;
	if (size == (off_t) atol (w[1]) &&
	    (time & ~1) == (atol (w[2]) & ~1) &&
	    i < nfa)
	{
	  found = 1;
	}
	else if (kill_dup_partial_files && i < nfa)
	{
	  Log (4, "dup partial file (%s):", w[0]);
	  remove_hr (s);
	}
      }
      else if (to_be_deleted (s, w[0]))
      {
	remove_hr (s);
      }

      for (i = 0; i < 4; ++i)
	if (w[i])
	  free (w[i]);
    }
    if (found)
      break;
    else
      *t = 0;
  }
  closedir (dp);

  /* New file */
  if (!found)
  {
    Log (5, "file not found, trying to create a tmpname");
    if (creat_tmp_name (s, file, size, time, from, inbound))
      found = 2;
    else
      return 0;
  }

  /* Replacing .hr with .dt */
  strcpy (strrchr (s, '.'), ".dt");
  return found;
}

FILE *inb_fopen (char *netname, off_t size, time_t time, FTN_ADDR *from,
                 int nfa, char *inbound, int secure_flag)
{
  char buf[MAXPATHLEN + 1];
  struct stat sb;
  FILE *f;

  if (!find_tmp_name (buf, netname, size, time, from, nfa, inbound))
    return 0;

  if ((f = fopen (buf, "ab")) == 0)
    Log (1, "%s: %s", buf, strerror (errno));
  fseek (f, 0, SEEK_END);	       /* Work-around MSVC bug */

#if defined(OS2)
  DosSetFHState(fileno(f), OPEN_FLAGS_NOINHERIT);
#elif defined(EMX)
  fcntl(fileno(f), F_SETFD, FD_CLOEXEC);
#endif

  /* Checking for free space */
  if (fstat (fileno (f), &sb) == 0)
  {
    /* Free space req-d (Kbytes) */
    unsigned long freespace, freespace2;
    int req_free = ((secure_flag == P_SECURE) ? minfree : minfree_nonsecure);

    freespace = getfree(buf);
    freespace2 = getfree(inbound);
    if (freespace > freespace2) freespace = freespace2;
    if (req_free >= 0 &&
	freespace < (unsigned long)(size - sb.st_size + 1023) / 1024 + req_free)
    {
      Log (1, "no enough free space in %s (%luK, req-d %luK)",
	   (freespace == freespace2) ? inbound : temp_inbound,
	   freespace,
	   (unsigned long) (size - sb.st_size + 1023) / 1024 + req_free);
      fclose (f);
      return 0;
    }
  }
  else
    Log (1, "%s: fstat: %s", netname, strerror (errno));

  return f;
}

int inb_reject (char *netname, off_t size,
		 time_t time, FTN_ADDR *from, int nfa, char *inbound)
{
  char tmp_name[MAXPATHLEN + 1];

  if (find_tmp_name (tmp_name, netname, size, time, from, nfa, inbound) != 1)
  {
    Log (1, "missing tmp file for %s!", netname);
    return 0;
  }
  else
  {
    Log (2, "rejecting %s", netname);
    /* Replacing .dt with .hr and removing temp. file */
    strcpy (strrchr (tmp_name, '.'), ".hr");
    remove_hr (tmp_name);
    return 1;
  }
}

/*
 * File is complete, rename it to it's realname. 1=ok, 0=failed.
 * Sets realname[MAXPATHLEN]
 */
int inb_done (char *netname, off_t size, time_t time,
	      FTN_ADDR *from, int nfa, char *inbound, char *real_name)
{
  char tmp_name[MAXPATHLEN + 1];
  char *s, *u;
  int  unlinked = 0, i;

  *real_name = 0;

  if (find_tmp_name (tmp_name, netname, size, time, from, nfa, inbound) != 1)
  {
    Log (1, "missing tmp file for %s!", netname);
    return 0;
  }

  strnzcpy (real_name, inbound, MAXPATHLEN);
  strnzcat (real_name, PATH_SEPARATOR, MAXPATHLEN);
  s = real_name + strlen (real_name);
  strnzcat (real_name, u = makeinboundcase (strdequote (netname)), MAXPATHLEN);
  free (u);
  strwipe (s);

  if (mask_test(netname, overwrite) && !ispkt(netname) && !isarcmail(netname))
  {
    for(i=0; ; i++)
    {
      unlinked |= (unlink(real_name) == 0);
      if (!RENAME (tmp_name, real_name))
      {
        Log (1, "%s -> %s%s", netname, real_name, unlinked?" (overwrited)":"");
	break;
      }
      if ((errno != EEXIST && errno != EACCES && errno != EAGAIN) || i==10)
      {
        Log (1, "cannot rename %s to it's realname: %s! (data stored in %s)",
             netname, strerror (errno), tmp_name);
        *real_name = 0;
        return 0;
      }
    }
  } else
  {

    s = real_name + strlen (real_name) - 1;

    /* gul: for *.pkt and *.?ic (tic, zic etc.) change name but not extension */
    /* ditto for arcmail -- mff */
    if (ispkt (netname) || istic (netname) || isarcmail (netname) || isreq (netname))
      s -= 4;

    if (touch (tmp_name, time) != 0)
      Log (1, "touch %s: %s", tmp_name, strerror (errno));

    while (1)
    {
      if (!RENAME (tmp_name, real_name))
      {
        Log (2, "%s -> %s", netname, real_name);
        break;
      }
      else
      {
        if (errno != EEXIST && errno != EACCES && errno != EAGAIN)
        {
          Log (1, "cannot rename %s to it's realname: %s! (data stored in %s)",
               netname, strerror (errno), tmp_name);
          *real_name = 0;
          return 0;
        }
        Log (2, "error renaming `%s' to `%s': %s",
	     netname, real_name, strerror (errno));
      }

      if (isalpha (*s) && toupper (*s) != 'Z')
        ++*s;
      else if (isdigit (*s) && toupper (*s) != '9')
        ++*s;
      else if (*s == '9')
        *s = 'a';
      else if (*--s == '.' || *s == '\\' || *s == '/')
      {
        Log (1, "cannot rename %s to it's realname! (data stored in %s)",
	     netname, tmp_name);
        *real_name = 0;
        return 0;
      }
    }
  }

  /* Replacing .dt with .hr and removing temp. file */
  strcpy (strrchr (tmp_name, '.'), ".hr");
  sdelete (tmp_name);
  return 1;
}

/*
 * Checks if the file already exists in our inbound. !0=ok, 0=failed.
 * Sets realname[MAXPATHLEN]
 */
int inb_test (char *filename, off_t size, time_t t,
	       char *inbound, char fp[])
{
  char *s, *u;
  struct stat sb;

  strnzcpy (fp, inbound, MAXPATHLEN);
  strnzcat (fp, PATH_SEPARATOR, MAXPATHLEN);
  s = fp + strlen (fp);
  strnzcat (fp, u = strdequote (filename), MAXPATHLEN);
  free (u);
  strwipe (s);

  return stat (fp, &sb) == 0 && sb.st_size == size &&
    (sb.st_mtime & ~1) == (t & ~1);
}
