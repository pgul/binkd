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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "sys.h"
#include "readcfg.h"
#include "inbound.h"
#include "common.h"
#include "tools.h"
#include "protocol.h"
#include "readdir.h"
#include "ftnaddr.h"
#include "ftnnode.h"
#include "srif.h"
#ifdef WITH_PERL
#include "perlhooks.h"
#endif

/* Removes both xxxxx.hr and it's xxxxx.dt */
static void remove_hr (char *path)
{
  strcpy (strrchr (path, '.'), ".dt");
  delete (path);
  strcpy (strrchr (path, '.'), ".hr");
  delete (path);
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
      if (from) ftnaddress_to_str (node, from); else strcpy(node, "0:0/0.0@unknown");
      if (fprintf (f, "%s %" PRIuMAX " %" PRIuMAX " %s\n", file->netname,
                   (uintmax_t) file->size,
                   (uintmax_t) file->time, node) <= 0)
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

static int to_be_deleted (char *tmp_name, char *netname, boff_t filesize, BINKD_CONFIG *config)
{
  struct stat sb, sd, *sp;

  if (config->kill_old_partial_files == 0) return 0;
  if (stat (tmp_name, &sd) != 0) return 0;
  strcpy (strrchr (tmp_name, '.'), ".dt");
  sp = &sb;
  if ((stat (tmp_name, &sb) == 0 ? sb.st_size != filesize : (sp = &sd, 1)) &&
      time (0) - sp->st_mtime > config->kill_old_partial_files)
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

    if ((f = fopen (s, "r")) == NULL)
    {
      Log (1, "find_tmp_name: %s: %s", de->d_name, strerror (errno));
    }
    else if (fgets (buf, sizeof (buf), f)==NULL)
    {  /* This .hr is empty, now checks to old */
      fclose (f);
      if (to_be_deleted (s, "unknown", (boff_t)-1, config))
      {
        Log (5, "old empty partial file %s is removed", de->d_name);
        remove_hr (s);
      }
    }
    else
    {
      char *w[4];
      FTN_ADDR fa;

      fclose (f);
      FA_ZERO (&fa);
      for (i = 0; i < 4; ++i)
        w[i] = getwordx (buf, i + 1, GWX_NOESC);
      if (!w[3])
      {
        if (to_be_deleted (s, "unknown", (boff_t)-1, config))
        {
          Log (5, "old partial file %s with garbage is removed", de->d_name);
          remove_hr (s);
        }
      }
      else
      {
        i = -1;
        if (parse_ftnaddress (w[3], &fa, config->pDomains.first))
          for (i = 0; i < state->nallfa; i++)
            if (!ftnaddress_cmp (&fa, state->fa + i))
              break;

        if (file == NULL)
        {
          if (i >= 0 && i < state->nallfa && !state->skip_all_flag)
          {
            Log (5, "partial file %s removed", w[0]);
            remove_hr (s);
          }
        }
        else if (i >= 0 && !strcmp (w[0], file->netname))
        {
          if (file->size == (boff_t) strtoumax (w[1], NULL, 10) &&
              (file->time & ~1) == (time_t) (safe_atol (w[2], NULL) & ~1) &&
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
        else if (to_be_deleted (s, w[0], strtoumax (w[1], NULL, 10), config))
        {
          Log (5, "old partial file %s removed", w[0]);
          remove_hr (s);
        }

        for (i = 0; i < 4; ++i)
          xfree (w[i]);
      }
    }
    if (found)
      break;
    else
      *t = 0;
  }
  closedir (dp);

  if (file == NULL)
    return 0;

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

void inb_remove_partial (STATE *state, BINKD_CONFIG *config)
{
  char buf[MAXPATHLEN + 1];

  find_tmp_name (buf, NULL, state, config);
}

FILE *inb_fopen (STATE *state, BINKD_CONFIG *config)
{
  char buf[MAXPATHLEN + 1];
  struct stat sb;
  FILE *f;
  int fd;

  if (!find_tmp_name (buf, &(state->in), state, config))
    return 0;

fopen_again:
  if ((fd = open (buf, O_CREAT|O_APPEND|O_RDWR|O_BINARY|O_NOINHERIT, 0666)) == -1)
  {
    Log (1, "%s: %s", buf, strerror (errno));
    return 0;
  }
  if ((f = fdopen (fd, "ab")) == 0)
  {
    Log (1, "%s: %s", buf, strerror (errno));
    return 0;
  }
  fseeko(f, 0, SEEK_END);               /* Work-around MSVC bug */

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

    freespace2 = getfree(state->inbound);
    if ( config->temp_inbound[0] &&
         !strncmp(config->temp_inbound,buf,strlen(config->temp_inbound)) )
    {
      freespace = getfree(config->temp_inbound);
      if (freespace > freespace2) freespace = freespace2;
    }
    else
      freespace = freespace2;
    if (sb.st_size > state->in.size)
    {
      Log (1, "Partial size %" PRIuMAX " > %" PRIuMAX " (file size), delete partial",
           (uintmax_t) sb.st_size, (uintmax_t) state->in.size);
      fclose (f);
      if (trunc_file (buf) && sdelete (buf)) return 0;
      goto fopen_again;
    }
    if (req_free >= 0 &&
        freespace < (state->in.size - sb.st_size + 1023) / 1024 + (unsigned long)req_free)
    {
      Log (1, "no enough free space in %s (%luK, req-d %" PRIuMAX "K)",
           (freespace == freespace2) ? state->inbound : config->temp_inbound,
           freespace,
           (uintmax_t) (state->in.size - sb.st_size + 1023) / 1024 + req_free);
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
    /* Replacing .dt with .hr and removing temp. file */
    strcpy (strrchr (tmp_name, '.'), ".hr");
    remove_hr (tmp_name);
    return 1;
  }
}

/* val: check if pkt header is one of the session aka */
int check_pkthdr(STATE *state, char *netname, char *tmp_name,
                 char *real_name, BINKD_CONFIG *config) {
  FTN_NODE *node;
  FILE *PKT;
  unsigned char buf[58];
  int i, check = 0, listed = 0, secure = (state->state == P_SECURE);
  short cz, cn, cf, cp;
  /* ext not defined - no check */
  if (config->pkthdr_bad == NULL) return 1;
  /* lookup in node records */
  for (i = 0; i < state->nallfa; i++)
    if ( (node = get_node_info(state->fa+i, config)) != NULL ) {
      if (node->fa.z > 0) listed = 1;
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
  else if ( !fread(buf, sizeof(buf), 1, PKT) )
      Log (1, "file %s read error: %s, header check failed for %s", tmp_name, strerror (errno), netname);
  else if ( !pkt_getaddr(buf, &cz, &cn, &cf, &cp, NULL, NULL, NULL, NULL) )
      Log (1, "pkt %s version is %d, expected 2; header check failed", netname, buf[18]+buf[19]*0x100);
  else {
    check = 1;
    Log (5, "pkt addr is %d:%d/%d.%d for %s", cz, cn, cf, cp, netname);
    /* do check */
    for (i = 0; i < state->nallfa; i++)
      if ( (cz < 0 || (state->fa+i)->z == cz) &&
           (cn < 0 || (state->fa+i)->net == cn ) &&
           ( (state->fa+i)->node == cf ) &&
           (cp < 0 || (state->fa+i)->p == cp) ) {
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

/* make next possible realname */
static int next_inb_filename (char *fname, char **s, enum renamestyletype ren_style)
{
  int num;
  char *p;

  if (ren_style == RENAME_POSTFIX)
  {
    if (**s == '\0')
    {
      strcpy(*s, ".1");
      return 1;
    }
    num = atoi(s[0]+1);
    if (num <= 0 || num > MAX_INB_RENAME)
      return 0;
    sprintf(*s + 1, "%i", num + 1);
    return 1;
  }

  if ((ren_style == RENAME_EXTENSION && **s == '\0') ||
      (ren_style == RENAME_BODY && **s == '.'))
  { /* first change */
    *--*s = '0'; /* funny syntax :) */
    return 1;
  }
  for (p = *s; isalnum(p[1]); p++);
  while (1)
  {
    if (*p == '9')
      *p = 'a';
    else if ((ren_style == RENAME_EXTENSION && *p == 'z') ||
             (ren_style == RENAME_BODY && *p == 'f'))
    {
      *p-- = '0';
      if (strchr(PATH_SEPARATOR, *p) || *p == '.')
        return 0;
      if (p + 1 != *s)
        continue;
      *s = p;
      *p = '0';
      return 1;
    }
    else
      ++*p;
    break;
  }
  return 1;
}

/*
 * File is complete, rename it to it's realname. 1=ok, 0=failed.
 */
int inb_done (TFILE *file, STATE *state, BINKD_CONFIG *config)
{
  char tmp_name[MAXPATHLEN + 1];
  char real_name[MAXPATHLEN + 10];
  char szAddr[FTN_ADDR_SZ + 1];
  char *s, *u, *netname;
  int  unlinked = 0, i;
  enum renamestyletype ren_style;

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
    for (i=0; ; i++)
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
    /* check pkt file header */
    if (ispkt (netname))
      check_pkthdr(state, netname, tmp_name, real_name, config);

    s = real_name + strlen (real_name);

    ren_style = config->renamestyle;
    /* for *.pkt, arcmail, *.?ic (tic, zic etc.) and *.req change name but not extension */
    if (ispkt (netname) || istic (netname) || isarcmail (netname) || isreq (netname))
    {
      s -= 4;
      ren_style = RENAME_BODY;
    }

    if (touch (tmp_name, file->time) != 0)
      Log (1, "touch %s: %s", tmp_name, strerror (errno));

    while (RENAME (tmp_name, real_name))
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

      if (!next_inb_filename(real_name, &s, ren_style))
      {
        if (ren_style == RENAME_EXTENSION)
        {
          ren_style = RENAME_POSTFIX;
          strnzcpy (real_name, state->inbound, MAXPATHLEN);
          strnzcat (real_name, PATH_SEPARATOR, MAXPATHLEN);
          s = real_name + strlen (real_name);
          strnzcat (real_name, u = makeinboundcase (strdequote (netname), (int)config->inboundcase), MAXPATHLEN);
          free (u);
          strwipe (s);
          continue;
        }
        Log (1, "cannot rename %s to it's realname! (data stored in %s)",
             netname, tmp_name);
        *real_name = 0;
        return 0;
      }
    }
    Log (2, "%s -> %s", netname, real_name);
  }

  /* Replacing .dt with .hr and removing temp. file */
  strcpy (strrchr (tmp_name, '.'), ".hr");
  sdelete (tmp_name);

  if (*real_name)
  {
    /* Set flags */
    if (evt_test(&(state->evt_queue), real_name, config->evt_flags.first))
      state->q = evt_run(state->q, real_name, state->delay_EOB > 0,
                         state, config);
  }
  if (state->delay_EOB && isreq(file->netname))
    state->delay_EOB--;
  ftnaddress_to_str (szAddr, state->fa);
  state->bytes_rcvd += file->size;
  state->files_rcvd++;
  Log (2, "rcvd: %s (%" PRIuMAX ", %.2f CPS, %s)", file->netname,
       (uintmax_t) file->size,
       (double) (file->size) /
       (safe_time() == file->start ? 1 : (safe_time() - file->start)), szAddr);
  return 1;
}

/*
 * Checks if the file already exists in our inbound. !0=ok, 0=failed.
 */
int inb_test (char *filename, boff_t size, time_t t,
               char *inbound, char fp[],
               enum renamestyletype ren_style)
{
  char *s, *u;
  struct stat sb;

  strnzcpy (fp, inbound, MAXPATHLEN);
  strnzcat (fp, PATH_SEPARATOR, MAXPATHLEN);
  s = fp + strlen (fp);
  strnzcat (fp, u = strdequote (filename), MAXPATHLEN);
  free (u);
  strwipe (s);

  s = fp + strlen (fp);
  if (ispkt (filename) || istic (filename) || isarcmail (filename) || isreq (filename))
  {
    s -= 4;
    ren_style = RENAME_BODY;
  }

  while (stat (fp, &sb) == 0)
  {
    if (sb.st_size == size && (sb.st_mtime & ~1) == (t & ~1))
      return 1;
    if (!next_inb_filename(fp, &s, ren_style))
      return 0;
  }
  return 0;
}
