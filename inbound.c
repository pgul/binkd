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
 * Revision 2.26  2003/12/05 23:39:36  gul
 * Bugfix on inb_done() with ND-mode
 *
 * Revision 2.25  2003/10/30 10:57:46  gul
 * Change inb_done arguments, optimize a bit
 *
 * Revision 2.24  2003/10/30 10:36:59  gul
 * Do not append file partially received from busy remote aka,
 * non-destructive skip it.
 *
 * Revision 2.23  2003/10/29 21:08:38  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.22  2003/08/28 07:35:54  gul
 * Cosmetics in log
 *
 * Revision 2.21  2003/08/26 22:18:48  gul
 * Fix compilation under w32-mingw and os2-emx
 *
 * Revision 2.20  2003/08/26 21:01:10  gul
 * Fix compilation under unix
 *
 * Revision 2.19  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.18  2003/08/25 18:25:34  gul
 * Remove partial if received part more then total size
 *
 * Revision 2.17  2003/08/23 15:51:51  stream
 * Implemented common list routines for all linked records in configuration
 *
 * Revision 2.16  2003/07/07 08:38:18  val
 * safe pkthdr-reading function (to byte order and struct alignment)
 *
 * Revision 2.15  2003/06/20 10:37:02  val
 * Perl hooks for binkd - initial revision
 *
 * Revision 2.14  2003/06/12 08:30:57  val
 * check pkt header feature, see keyword 'check-pkthdr'
 *
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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "readcfg.h"
#include "inbound.h"
#include "common.h"
#include "tools.h"
#include "protocol.h"
#include "readdir.h"
#include "ftnaddr.h"
#include "ftnnode.h"
#ifdef WITH_PERL
#include "perlhooks.h"
#endif

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

static int creat_tmp_name (char *s, TFILE *file, FTN_ADDR *from, char *inbound)
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
      if (fprintf (f, "%s %li %li %s\n", file->netname, (long int)file->size,
                   (long int) file->time, node) <= 0)
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

static int to_be_deleted (char *tmp_name, char *netname, BINKD_CONFIG *config)
{
  struct stat sb;

  strcpy (strrchr (tmp_name, '.'), ".dt");
  if (stat (tmp_name, &sb) == 0 && config->kill_old_partial_files != 0 &&
      time (0) - sb.st_mtime > config->kill_old_partial_files)
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
static int find_tmp_name (char *s, TFILE *file, STATE *state, BINKD_CONFIG *config)
{
  char buf[MAXPATHLEN + 80];
  DIR *dp;
  struct dirent *de;
  FILE *f;
  int i, found = 0;
  char *t, *inbound;

  inbound = state->inbound;
  if (config->temp_inbound[0])
    inbound = config->temp_inbound;

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

      if (!strcmp (w[0], file->netname) && parse_ftnaddress (w[3], &fa, config->pDomains.first))
      {
	for (i = 0; i < state->nallfa; i++)
	  if (!ftnaddress_cmp (&fa, state->fa + i))
	    break;
	if (file->size == (off_t) atol (w[1]) &&
	    (file->time & ~1) == (atol (w[2]) & ~1) &&
	    i < state->nallfa)
	{ /* non-destructive skip file from busy aka */
	  if (i >= state->nfa)
	  {
	    Log (2, "Skip partial file %s: aka %s busy", w[0], w[3]);
	    for (i = 0; i < 4; ++i)
	      xfree (w[i]);
	    return 0;
	  }
	  else
	    found = 1;
	}
	else if (config->kill_dup_partial_files && i < state->nallfa)
	{
	  Log (5, "dup partial file %s removed", w[0]);
	  remove_hr (s);
	}
      }
      else if (to_be_deleted (s, w[0], config))
      {
	remove_hr (s);
      }

      for (i = 0; i < 4; ++i)
	xfree (w[i]);
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
    if (creat_tmp_name (s, file, state->fa, inbound))
      found = 2;
    else
      return 0;
  }

  /* Replacing .hr with .dt */
  strcpy (strrchr (s, '.'), ".dt");
  return found;
}

FILE *inb_fopen (STATE *state, BINKD_CONFIG *config)
{
  char buf[MAXPATHLEN + 1];
  struct stat sb;
  FILE *f;

  if (!find_tmp_name (buf, &(state->in), state, config))
    return 0;

fopen_again:
  if ((f = fopen (buf, "ab")) == 0)
  {
    Log (1, "%s: %s", buf, strerror (errno));
    return 0;
  }
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
    int req_free = ((state->state == P_SECURE) ? config->minfree : config->minfree_nonsecure);

    freespace = getfree(buf);
    freespace2 = getfree(state->inbound);
    if (freespace > freespace2) freespace = freespace2;
    if (sb.st_size > state->in.size)
    {
      Log (1, "Partial size %lu > %lu (file size), delete partial", 
           (unsigned long) sb.st_size, (unsigned long) state->in.size);
      fclose (f);
      if (trunc_file (buf) && sdelete (buf)) return 0;
      goto fopen_again;
    }
    if (req_free >= 0 &&
	freespace < (unsigned long)(state->in.size - sb.st_size + 1023) / 1024 + req_free)
    {
      Log (1, "no enough free space in %s (%luK, req-d %luK)",
	   (freespace == freespace2) ? state->inbound : config->temp_inbound,
	   freespace,
	   (unsigned long) (state->in.size - sb.st_size + 1023) / 1024 + req_free);
      fclose (f);
      return 0;
    }
  }
  else
    Log (1, "%s: fstat: %s", state->in.netname, strerror (errno));

  return f;
}

int inb_reject (STATE *state, BINKD_CONFIG *config)
{
  char tmp_name[MAXPATHLEN + 1];

  if (find_tmp_name (tmp_name, &state->in, state, config) != 1)
  {
    Log (1, "missing tmp file for %s!", state->in.netname);
    return 0;
  }
  else
  {
    Log (2, "rejecting %s", state->in.netname);
    /* Replacing .dt with .hr and removing temp. file */
    strcpy (strrchr (tmp_name, '.'), ".hr");
    remove_hr (tmp_name);
    return 1;
  }
}

/* val: check if pkt header is one of the session aka */
int check_pkthdr(int nfa, FTN_ADDR *from, char *netname, char *tmp_name, 
                 char *real_name, BINKD_CONFIG *config) {
  FTN_NODE *node;
  FILE *PKT;
  PKTHDR buf;
  int i, check = 0, listed = 0, secure = 0;
  short cz = -1, cn = -1, cf = -1, cp = -1;
  /* ext not defined - no check */
  if (config->pkthdr_bad == NULL) return 1;
  /* lookup in node records */
  for (i = 0; i < nfa; i++)
    if ( (node = get_node_info(from+i, config)) != NULL ) {
      if (node->fa.z > 0) listed = 1;
      if (strcmp(node->pwd, "-") != 0) secure = 1;
      /* no check is forced */
      if (node->HC_flag == HC_OFF) return 1;
      /* check is on for one aka */
      else if (node->HC_flag == HC_ON) check = 1;
    }
  /* consider default values, if check is not forced */
  if (!check && 
       ( (config->pkthdr_type == 0) ||
         (config->pkthdr_type == A_PROT && !secure) ||
         (config->pkthdr_type == A_UNPROT && secure) ||
         (config->pkthdr_type == A_LST && !listed) ||
         (config->pkthdr_type == A_UNLST && listed) )
     ) return 1;
  /* parse pkt header */
  check = 0;
  if ( (PKT = fopen(tmp_name, "rb")) == NULL )
      Log (1, "can't open file %s: %s, header check failed for %s", tmp_name, strerror (errno), netname);
  else if ( !read_pkthdr(PKT, &buf) )
      Log (1, "file %s read error: %s, header check failed for %s", tmp_name, strerror (errno), netname);
  else if (buf.pkt_ver != 2)
      Log (1, "pkt %s version is %d, expected 2; header check failed", netname, buf.pkt_ver);
  else {
    cf = buf.onode;
    cn = buf.onet;
    /* qmail, zmail - not point-aware */
    if (buf.pcode_lo == 0x29 || buf.pcode_lo == 0x35) cz = buf.qmail_ozone;
    /* type 2 (fsc-39), 2+ */
    else if (buf.cw_lo & 2) {
      cz = buf.ozone;
      cp = buf.opoint;
      if (buf.cw_lo == buf.cwv_lo && buf.cw_hi == buf.cwv_hi) {
        if (buf.opoint && cn == -1) cn = buf.aux_net;
      }
    }
    check = 1;
    Log (5, "pkt addr is %d:%d/%d.%d for %s", cz, cn, cf, cp, netname);
    /* do check */
    for (i = 0; i < nfa; i++)
      if ( (cz < 0 || (from+i)->z == cz) &&
           (cn < 0 || (from+i)->net == cn ) && 
           ( (from+i)->node == cf ) &&
           (cp < 0 || (from+i)->p == cp) ) {
                                             if (PKT != NULL) fclose(PKT);
                                             return 1;                 
                                           }
  }
  if (PKT != NULL) fclose(PKT);
  /* change pkt ext to bad */
  if (check) Log (1, "bad pkt addr: %d:%d/%d.%d (file %s)", cz, cn, cf, cp, netname);
  i = strlen(real_name); check = 0;
  while (i > 0 && real_name[--i] != '.') check++;
  if (i > 0) {
    int len = strlen(config->pkthdr_bad)+1;
    if (len > check) len = check;
    memcpy(real_name+i+1, config->pkthdr_bad, len);
  }
  return 0;
}

/*
 * File is complete, rename it to it's realname. 1=ok, 0=failed.
 * Sets realname[MAXPATHLEN]
 */
int inb_done (TFILE *file, char *real_name, STATE *state, BINKD_CONFIG *config)
{
  char tmp_name[MAXPATHLEN + 1];
  char *s, *u, *netname;
  int  unlinked = 0, i;

  *real_name = 0;
  netname = file->netname;

  if (find_tmp_name (tmp_name, file, state, config) != 1)
  {
    Log (1, "missing tmp file for %s!", netname);
    return 0;
  }

  strnzcpy (real_name, state->inbound, MAXPATHLEN);
  strnzcat (real_name, PATH_SEPARATOR, MAXPATHLEN);
  s = real_name + strlen (real_name);
  strnzcat (real_name, u = makeinboundcase (strdequote (netname), (int)config->inboundcase), MAXPATHLEN);
  free (u);
  strwipe (s);

#ifdef WITH_PERL
  if (perl_after_recv(state, file, tmp_name, real_name)) {
    /* Replacing .dt with .hr and removing temp. file */
    if (access(tmp_name, 0) == 0) sdelete (tmp_name);
    strcpy (strrchr (tmp_name, '.'), ".hr");
    sdelete (tmp_name);
    return 1;
  }
#endif

  if (mask_test(netname, config->overwrite.first) && !ispkt(netname) && !isarcmail(netname))
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
    /* val: check pkt file header */
    if (ispkt (netname)) check_pkthdr(state->nallfa, state->fa, netname, tmp_name, real_name, config);

    if (touch (tmp_name, file->time) != 0)
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
