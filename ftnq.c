/*
 *  ftnq.c -- BSO methods implementation
 *
 *  ftnq.c is a part of binkd project
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

#include "sys.h"
#include "readcfg.h"
#include "ftnq.h"
#include "ftnnode.h"
#include "ftnaddr.h"
#include "tools.h"
#include "readdir.h"
#include "iphdr.h"
#ifdef WITH_PERL
#include "perlhooks.h"
#endif

const char prio[] = "IiCcDdOoFfHh";
static const char flo_flvrs[] = "icdfhICDFH";
static const char out_flvrs[] = "icdohICDOH";

static FTNQ *q_add_dir (FTNQ *q, char *dir, FTN_ADDR *fa1, BINKD_CONFIG *config);
FTNQ *q_add_file (FTNQ *q, char *filename, FTN_ADDR *fa1, char flvr, char action, char type, BINKD_CONFIG *config);

/*
 * q_free(): frees memory allocated by q_scan()
 */
static int qn_free (FTN_NODE *fn, void *arg)
{
  UNUSED_ARG(arg);

  fn->hold_until = 0;
  fn->mail_flvr = fn->files_flvr = 0;
  fn->busy = 0;
  return 0;
}

void q_free (FTNQ *q, BINKD_CONFIG *config)
{
  if (q != SCAN_LISTED)
  {
    FTNQ *last = 0;

    while (q)
    {
      last = q;
      q = q->next;
    }
    while (last)
    {
      if (!last->prev)
      {
	free (last);
	break;
      }
      else
      {
	last = last->prev;
	xfree (last->next);
      }
    }
  }
  else
    foreach_node (qn_free, 0, config);
}

/*
 * q_scan: scans outbound. Return value must be q_free()'d.
 */

/* Need to pass two parameters: queue and config */
struct qn_scan_params
{
  FTNQ **pq;
  BINKD_CONFIG *config;
};
static int qn_scan (FTN_NODE *fn, void *arg)
{
  struct qn_scan_params *params = arg;

  *(params->pq) = q_scan_boxes (*(params->pq), &fn->fa, 1, 1, params->config);
  return 0;
}

FTNQ *q_scan (FTNQ *q, BINKD_CONFIG *config)
{
  char *s;
  char buf[MAXPATHLEN + 1], outb_path[MAXPATHLEN + 1];
  FTN_DOMAIN *curr_domain;
  struct qn_scan_params qn_params;

  for (curr_domain = config->pDomains.first; curr_domain; curr_domain = curr_domain->next)
  {
    DIR *dp;
    struct dirent *de;
    int len;

    if (curr_domain->alias4 == 0)
    {
      strnzcpy (outb_path, curr_domain->path, sizeof (buf) - 1);
/* `FOO:/bar means FOO:\..\bar on Amiga */
#ifndef AMIGA
      if (outb_path[strlen (outb_path) - 1] == ':')
	strcat (outb_path, PATH_SEPARATOR);
#endif

      if ((dp = opendir (outb_path)) == 0)
      {
	Log (1, "cannot opendir %s: %s", outb_path, strerror (errno));
	continue;
      }

      len = strlen (curr_domain->dir);
      strnzcpy (buf, curr_domain->path, sizeof (buf));
      strnzcpy (buf + strlen (buf), PATH_SEPARATOR, sizeof (buf) - strlen (buf));
      s = buf + strlen (buf);

      while ((de = readdir (dp)) != 0)
      {
	if (!STRNICMP (de->d_name, curr_domain->dir, len) &&
	    (de->d_name[len] == 0 ||
	     (de->d_name[len] == '.' && isxdigit (de->d_name[len + 1]))))
	{
	  FTN_ADDR fa;

	  FA_ZERO (&fa);
#ifdef AMIGADOS_4D_OUTBOUND
	  if (!config->aso)
#endif
	    fa.z = ((de->d_name[len] == '.') ?
		    strtol (de->d_name + len + 1, (char **) NULL, 16) :
		    curr_domain->z[0]);
	  if (de->d_name[len] == 0 || fa.z != curr_domain->z[0])
	  {
	    strcpy (fa.domain, curr_domain->name);
	    strnzcpy (buf + strlen (buf), de->d_name, sizeof (buf) - strlen (buf));
	    q = q_add_dir (q, buf, &fa, config);
	  }
	  *s = 0;
	}
      }
      closedir (dp);
    }
  }
  qn_params.pq     = &q;
  qn_params.config = config;
  foreach_node (qn_scan, &qn_params, config);
  return q;
}

/*
 * Adds to the q all files for n akas stored in fa
 */
FTNQ *q_scan_addrs (FTNQ *q, FTN_ADDR *fa, int n, int to, BINKD_CONFIG *config)
{
  char buf[MAXPATHLEN + 1];
  int  i;
  char *s;

  for (i = 0; i < n; ++i)
  {
    if (!to && config->send_if_pwd)
    {
      /* do not give unsecure mail even to secure link when send-if-pwd */
      FTN_NODE *fn;
      if ((fn = get_node_info(fa+i, config)) == NULL ||
           fn->pwd == NULL || strcmp(fn->pwd, "-") == 0)
	continue;
    }
    ftnaddress_to_filename (buf, fa + i, config);
    if (*buf)
    {
      if ((s = last_slash(buf)) != 0)
      {
	*s = 0;
	q = q_add_dir (q, buf, fa + i, config);
      }
    }
  }
  q = q_scan_boxes (q, fa, n, to, config);
  return q;
}

static int weight_by_name (char *s)
{
  if (ispkt (s))
    return 100;
  if (isarcmail (s))
    return 50;
  if (istic (s))
    return -100;
  return 0;
}

static int cmp_filebox_files (FTNQ *a, FTNQ *b)
{
  int w_a = weight_by_name (a->path);
  int w_b = weight_by_name (b->path);

  if (w_a - w_b == 0)
    return a->time - b->time;
  else
    return w_b - w_a;
}

#ifdef MAILBOX
static char to32(int N)
{
    if ((N >= 0) && (N <=9)) return '0'+N;
    if ((N > 9) && (N < 32)) return 'A'+ N - 10;
    return 'Z'; /* fake return */
}

static struct {
    char * ext;
    char   flv;
} brakeExt[] =
{
#ifdef UNIX
    {"immediate", 'i'},
    {"crash",     'c'},
    {"direct",    'd'},
    {"normal",    'f'},
    {"hold",      'h'},
#endif
    {"Immediate", 'i'},
    {"Crash",     'c'},
    {"Direct",    'd'},
    {"Normal",    'f'},
    {"Hold",      'h'}
};
#endif

static FTNQ *q_scan_box (FTNQ *q, FTN_ADDR *fa, char *boxpath, char flvr, int deleteempty, BINKD_CONFIG *config)
{
  int n_files = 0;
  DIR *dp;
  char buf[MAXPATHLEN + 1], *s;
  struct dirent *de;
  struct stat sb;

  strnzcpy (buf, boxpath, sizeof (buf));
  strnzcat (buf, PATH_SEPARATOR, sizeof (buf));
#ifdef UNIX
  if (access(boxpath, R_OK | W_OK) != 0) {
    if (access(boxpath, F_OK) == 0)
      Log (1, "No access to filebox `%s'", boxpath);
    return q;
  }
#endif
  s = buf + strlen (buf);
  if ((dp = opendir (boxpath)) != NULL)
  {
    while ((de = readdir (dp)) != 0)
    {
      sb.st_mtime = 0; /* ??? val: don't know how to get it if stat() isn't used */
      strnzcat (buf, de->d_name, sizeof (buf));
      if (de->d_name[0] != '.'
#if defined(_MSC_VER) || defined(DOS) && !defined(DJGPP)
          && (de->d_attrib & 0x1a) == 0 /* not hidden, directory or volume label */
#elif defined(OS2) && !defined(IBMC) && !defined(__WATCOMC__)
          && (de->d_attr & 0x1a) == 0   /* not hidden, directory or volume label */
#elif defined(__FreeBSD__)
          && (DTTOIF(de->d_type) & S_IFDIR) == 0  /* not directory */
#else
          && stat(buf, &sb) == 0 && (sb.st_mode & S_IFDIR) == 0 /* not directory */
#endif
         )
      {
        q = q_add_file (q, buf, fa, flvr, 'd', 0, config);
        n_files++;
      }
      *s = 0;
    }
    closedir (dp);
    if (n_files == 0 && deleteempty) {
      if (rmdir (boxpath) == 0)
        Log (3, "Empty filebox %s deleted", boxpath);
      else
        Log (1, "Cannot delete empty filebox %s: %s", boxpath, strerror (errno));
    }
  }
  return q;
}

/*
 * Scans fileboxes for n akas stored in fa
 */
FTNQ *q_scan_boxes (FTNQ *q, FTN_ADDR *fa, int n, int to, BINKD_CONFIG *config)
{
  FTN_NODE *node;
  int i;
#ifdef MAILBOX
  char buf[MAXPATHLEN + 1];
  char *s;
  int j;
#endif

  for (i = 0; i < n; ++i)
  {
    node = get_node_info (fa + i, config);
    if (!to && config->send_if_pwd)
    {
      /* do not give unsecure mail even to secure link when send-if-pwd */
      if (node == NULL || node->pwd == NULL || strcmp(node->pwd, "-") == 0)
	continue;
    }
#ifndef MAILBOX
    if (node && node->obox)
    {
      q = q_scan_box (q, fa+i, node->obox, node->obox_flvr, 0, config);
    }
#else
    if ((node && node->obox) || config->tfilebox[0] || config->bfilebox[0])
    {
      if (node && node->obox)
        q = q_scan_box (q, fa+i, node->obox, node->obox_flvr, 0, config);
      if (config->bfilebox[0]) {
        strnzcpy (buf, config->bfilebox, sizeof (buf));
        strnzcat (buf, PATH_SEPARATOR, sizeof (buf));
        snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
                "%s.%u.%u.%u.%u.",
                fa[i].domain,
                fa[i].z,
                fa[i].net,
                fa[i].node,
                fa[i].p);
        s = buf + strlen(buf);
        for (j = 0; j < sizeof(brakeExt)/sizeof(brakeExt[0]); j++) {
          strnzcat (buf, brakeExt[j].ext, sizeof (buf));
          q = q_scan_box (q, fa+i, buf, brakeExt[j].flv, config->deleteablebox, config);
          *s = '\0';
        }
      }
        
      if (config->tfilebox[0]) {
        strnzcpy ( buf, config->tfilebox, sizeof (buf));
        strnzcat ( buf, PATH_SEPARATOR, sizeof (buf));
        snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
                "%u.%u.%u.%u",
                fa[i].z,
                fa[i].net,
                fa[i].node,
                fa[i].p);
        q = q_scan_box (q, fa+i, buf, 'f', config->deleteablebox, config);
        strnzcat ( buf, ".H", sizeof (buf));
#ifdef UNIX
	{ struct stat st;
	  if (stat(buf, &st) != 0 || (st.st_mode & S_IFDIR) == 0)
	    buf[strlen(buf) - 1] = 'h';
	}
#endif
        q = q_scan_box (q, fa+i, buf, 'h', config->deleteablebox, config);

        strnzcpy ( buf, config->tfilebox, sizeof (buf));
        strnzcat ( buf, PATH_SEPARATOR, sizeof (buf));
        snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
                "%c%c%c%c%c%c%c%c.%c%c",
                to32(fa[i].z/32),     to32(fa[i].z%32),
                to32(fa[i].net/1024), to32((fa[i].net/32)%32), to32(fa[i].net%32),
                to32(fa[i].node/1024),to32((fa[i].node/32)%32),to32(fa[i].node%32),
                to32(fa[i].p/32),     to32(fa[i].p%32));
        q = q_scan_box (q, fa+i, buf, 'f', config->deleteablebox, config);
        strnzcat (buf, "H", sizeof (buf));
#ifdef UNIX
	{ struct stat st;
	  if (stat(buf, &st) != 0 || (st.st_mode & S_IFDIR) == 0)
	    buf[strlen(buf) - 1] = 'h';
	}
#endif
        q = q_scan_box (q, fa+i, buf, 'h', config->deleteablebox, config);
      }
    }
#endif
  }

  return q;
}


void process_hld (FTN_ADDR *fa, char *path, BINKD_CONFIG *config)
{
  FTN_NODE *node;
  long hold_until_tmp;

  if ((node = get_node_info(fa, config)) != NULL)
  {
    FILE *f;

    if ((f = fopen (path, "r")) == NULL ||
	fscanf (f, "%ld", &hold_until_tmp) != 1)
    {
      node->hold_until = 0;
    }
    else
    {
      node->hold_until = (time_t)hold_until_tmp;
    }
    if (f)
      fclose (f);

    if (node->hold_until <= safe_time())
    {
      node->hold_until = 0;
      delete (path);
    }
  }
}

static void process_bsy (FTN_ADDR *fa, char *path, BINKD_CONFIG *config)
{
  char *s = path + strlen (path) - 4;
  FTN_NODE *node;
  struct stat sb;

  if (stat (path, &sb) == 0 && config->kill_old_bsy != 0
      && time (0) - sb.st_mtime > config->kill_old_bsy)
  {
    char buf[FTN_ADDR_SZ + 1];

    ftnaddress_to_str (buf, fa);
    Log (2, "found old %s file for %s", s, buf);
    delete (path);
  }
  else
  {
    if ((node = get_node_info (fa, config)) != 0 && node->busy != 'b' &&
	   (!STRICMP (s, ".bsy") || !STRICMP (s, ".csy")))
    {
      node->busy = tolower (s[1]);
    }
  }
}

/*
 * Adds files from outbound directory _dir_ to _q_. _fa1_ is
 * the address wildcard for this outbound. E.g.
 *     c:\bbs\outbound\                     fa1 = 2:-1/-1.-1@fidonet
 * or
 *     c:\bbs\outbound\00030004.pnt\        fa1 = 2:3/4.-1@fidonet
 * or even
 *     c:\bbs\outbound\00030004.pnt\        fa1 = 2:3/4.5@fidonet
 */
static FTNQ *q_add_dir (FTNQ *q, char *dir, FTN_ADDR *fa1, BINKD_CONFIG *config)
{
  DIR *dp;
  FTN_ADDR fa2;
  char buf[MAXPATHLEN + 1];
  int j;
  char *s;

  if ((dp = opendir (dir)) != 0)
  {
    struct dirent *de;

    while ((de = readdir (dp)) != 0)
    {
#ifdef AMIGADOS_4D_OUTBOUND
      if (config->aso)
      {
        char ext[4];
        int matched = 0;
        size_t nlen = strlen(s = de->d_name);

	for (; *s && isgraph(*s) != 0; s++);
	if ((size_t)(s - de->d_name) != nlen)
	  continue;

        memcpy (&fa2, fa1, sizeof(FTN_ADDR));

        if (sscanf(s = de->d_name, "%u.%u.%u.%u.%3s%n",
	         (unsigned*)(&fa2.z), (unsigned*)(&fa2.net), (unsigned*)(&fa2.node),
	         (unsigned*)(&fa2.p), ext, &matched) != 5 ||
	    (size_t)matched != nlen || strlen(ext) != 3)
	  continue;

        if ((fa1->z != -1 && fa1->z != fa2.z) ||
	    (fa1->net != -1 && fa1->net != fa2.net) ||
	    (fa1->node != -1 && fa1->node != fa2.node) ||
	    (fa1->p != -1 && fa1->p != fa2.p))
	  continue;

        strnzcpy(buf, dir, sizeof(buf));
        strnzcpy(buf + strlen(buf), PATH_SEPARATOR, sizeof(buf) - strlen(buf));
        strnzcpy(buf + strlen(buf), s, sizeof(buf) - strlen(buf));

        if (!STRICMP(ext, "bsy") || !STRICMP(ext, "csy"))
	  process_bsy(&fa2, buf, config);

        if (!get_node_info(&fa2, config) && !is5D(fa1))
          continue;

        if (strchr(out_flvrs, ext[0]) &&
		  tolower(ext[1]) == 'u' && tolower(ext[2]) == 't')
	  /* Adding *.?ut */
	  q = q_add_file(q, buf, &fa2, ext[0], 'd', 'm', config);
        else if (!STRICMP(ext, "req"))
	  /* Adding *.req */
	  q = q_add_file(q, buf, &fa2, 'h', 's', 'r', config);
        else if (!STRICMP(ext, "hld"))
	  process_hld(&fa2, buf, config);
        else if (strchr(flo_flvrs, ext[0]) &&
		        tolower(ext[1]) == 'l' && tolower(ext[2]) == 'o')
	  /* Adding *.?lo */
	  q = q_add_file(q, buf, &fa2, ext[0], 'd', 'l', config);
	else if (!STRICMP (s + 9, "stc"))
	{
	  /* Adding *.stc */
	  q = q_add_file (q, buf, &fa2, 'h', 0, 's', config);
	}
      }
      else
#endif /* AMIGADOS_4D_OUTBOUND */
      {
        s = de->d_name;

        for (j = 0; j < 8; ++j)
	  if (!isxdigit (s[j]))
	    break;

        if (j != 8 || strlen(s) != 12 || s[8] != '.' || strchr(s+9, '.'))
	  continue;

	/* fa2 will store dest.address for the current (de->d_name) file */
	memcpy (&fa2, fa1, sizeof (FTN_ADDR));

	if (fa1->node != -1 && fa1->p != 0)
	  sscanf (s, "%8x", (unsigned *)&fa2.p);   /* We now in /xxxxyyyy.pnt */
	else
	  sscanf (s, "%4x%4x", (unsigned *)&fa2.net, (unsigned *)&fa2.node);

	/* add the file if wildcard (f1) match the address (fa2) */
	if (fa1->node != -1 && fa1->p != -1 && ftnaddress_cmp (fa1, &fa2))
	  continue;

	strnzcpy (buf, dir, sizeof (buf));
	strnzcpy (buf + strlen (buf), PATH_SEPARATOR,
		  sizeof (buf) - strlen (buf));
	strnzcpy (buf + strlen (buf), s, sizeof (buf) - strlen (buf));

	if (!STRICMP (s + 9, "pnt") && fa2.p == -1)
	{
	  struct stat sb;

	  if (stat (buf, &sb) == 0 && sb.st_mode & S_IFDIR)
	    q = q_add_dir (q, buf, &fa2, config);
	  continue;
	}
	if (fa2.p == -1)
	  fa2.p = 0;

	if (!STRICMP (s + 9, "bsy") || !STRICMP (s + 9, "csy"))
	  process_bsy (&fa2, buf, config);

	if (!config->havedefnode && !get_node_info (&fa2, config) && !is5D (fa1))
	  continue;
	if (strchr (out_flvrs, s[9]) &&
	    tolower (s[10]) == 'u' && tolower (s[11]) == 't')
	{
	  /* Adding *.?ut */
	  q = q_add_file (q, buf, &fa2, s[9], 'd', 'm', config);
	}
	else if (!STRICMP (s + 9, "req"))
	{
	  /* Adding *.req */
	  q = q_add_file (q, buf, &fa2, 'h', 's', 'r', config);
	}
	else if (!STRICMP (s + 9, "hld"))
	  process_hld (&fa2, buf, config);
	else if (strchr (flo_flvrs, s[9]) &&
	         tolower (s[10]) == 'l' && tolower (s[11]) == 'o')
	{
	  /* Adding *.?lo */
	  q = q_add_file (q, buf, &fa2, s[9], 'd', 'l', config);
	}
	else if (!STRICMP (s + 9, "stc"))
	{
	  /* Adding *.stc */
	  q = q_add_file (q, buf, &fa2, 'h', 0, 's', config);
	}
      }
    }
    closedir (dp);
  }
  else
    Log (1, "cannot opendir %s: %s", dir, strerror (errno));
  return q;
}

/*
 * Add a file to the queue.
 */
FTNQ *q_add_file (FTNQ *q, char *filename, FTN_ADDR *fa1, char flvr, char action, char type, BINKD_CONFIG *config)
{
  const int argc=3;
  char *argv[3];
  char str[MAXPATHLEN+1];

  SHARED_CHAIN   * chn;
  FTN_ADDR_CHAIN * fcn;
  /* If filename for shared address was scanned,
   * call this functions for all nodes for this
   * sharea address (assumed, that recursively
   * shared aka impossible due to user brains :))
   * Recursion in shared aka definitions imply
   * infinite recursion in this function -
   * please, be careful!
   */
  for(chn = config->shares.first;chn;chn = chn->next)
  {
    if (ftnaddress_cmp(fa1,&chn->sha) == 0)
    {
      for(fcn = chn->sfa.first; fcn; fcn = fcn->next)
      {
        q_add_file(q,filename,&fcn->fa,flvr,action,type, config);
      }
    }
  }

  if (q != SCAN_LISTED)
  {
    FTNQ *new_file;

    if (type == 's')
    { char *p;
      FILE *f;

      f = fopen(filename, "r");
      if (f == NULL)
      { Log(1, "Can't open %s: %s", filename, strerror(errno));
        return q;
      }
      if (!fgets(str, sizeof(str), f))
      { Log(1, "Incorrect status (can't fgets), ignored");
        fclose(f);
        return q;
      }
      fclose(f);
      if (*str && isspace(*str))
      { Log(1, "Incorrect status (space first), ignored");
        return q;
      }
      for (p=str+strlen(str)-1; isspace(*p); *p--='\0');
      Log(2, "Status is '%s'", str);
      if (!parse_args (argc, argv, str, "Status"))
      { Log(1, "Incorrect status, ignored");
        return q;
      }
    }

    new_file = xalloc (sizeof (FTNQ));
    FQ_ZERO (new_file);

    new_file->next = q;
    if (q)
      q->prev = new_file;
    q = new_file;

    if (fa1)
      memcpy (&q->fa, fa1, sizeof (FTN_ADDR));

    q->flvr = flvr;
    q->action = action;
    q->type = type;
    q->sent = 0;

    if (type == 's')
    { q->size = (boff_t) strtoumax(argv[1], NULL, 10);
      q->time = safe_atol(argv[2], NULL);
      strnzcpy (q->path, argv[0], MAXPATHLEN);
    }
    else
      strnzcpy (q->path, filename, MAXPATHLEN);
  }
  else
  {
    FTN_NODE *node;

    if ((node = get_node_info (fa1, config)) != NULL)
    {
      if (type == 'm')
	node->mail_flvr = MAXFLVR (flvr, node->mail_flvr);
      else
	node->files_flvr = MAXFLVR (flvr, node->files_flvr);
    }
  }
  return q;
}

/*
 * Add a file to the end of queue.
 */
FTNQ *q_add_last_file (FTNQ *q, char *filename, FTN_ADDR *fa1, char flvr, char action, char type, BINKD_CONFIG *config)
{
  FTNQ *new_file, *pq;

  new_file = q_add_file (NULL, filename, fa1, flvr, action, type, config);
  if (new_file == NULL) return q;
  if (q == NULL) return new_file;
  for (pq = q; pq->next; pq = pq->next);
  new_file->prev = pq;
  pq->next = new_file;
  return q;
}

/*
 * q_list: just lists q, not more
 */

typedef struct
{
  int first_pass;
  FILE *out;
} qn_list_arg;

static int qn_list (FTN_NODE *fn, void *arg)
{
  char tmp[60], buf[FTN_ADDR_SZ + 1];
  qn_list_arg *a = (qn_list_arg *) arg;

  if (fn->mail_flvr || fn->files_flvr || fn->busy)
  {
    if (fn->hold_until > 0)
    {
      struct tm tm;
      safe_localtime (&fn->hold_until, &tm);
      strftime (tmp, sizeof (tmp), " (hold until %Y/%m/%d %H:%M:%S)", &tm);
    }
    else
      *tmp = 0;

    ftnaddress_to_str (buf, &fn->fa);
    fprintf (a->out, "%c %c%c %s%s%s\n",
	     a->first_pass ? '$' : ' ', fn->mail_flvr ? fn->mail_flvr : '-',
	     fn->files_flvr ? fn->files_flvr : '-', buf, tmp,
	     fn->busy ? (fn->busy == 'c' ? "\tcall" : "\tbusy") : "");
    if (a->first_pass)
      a->first_pass = 0;
  }
  return 0;
}

void q_list (FILE *out, FTNQ *q, BINKD_CONFIG *config)
{
  char buf[FTN_ADDR_SZ + 1];

  if (q == SCAN_LISTED)
  {
    qn_list_arg qnla;

    qnla.first_pass = 1;
    qnla.out = out;

    foreach_node (qn_list, &qnla, config);
  }
  else
  {
    for (; q; q = q->next)
    {
      if (!q->sent)
      {
	ftnaddress_to_str (buf, &q->fa);
	fprintf (out, "%-20s %c%c%c %8" PRIuMAX " %s\n",
		 buf, q->flvr, q->action ? q->action : '-',
		 q->type ? q->type : '-',
		 (uintmax_t)q->size, q->path);
      }
    }
  }
}

static int q_cmp(FTNQ *a, FTNQ *b, FTN_ADDR *fa, int nAka)
{
  int i;

  /* 1. Do not sort sent files, move its to the end of queue */
  if (a->sent || b->sent)
    return b->sent - a->sent;
  /* 2. Compare AKA */
  if (!ftnaddress_cmp (&a->fa, &b->fa)) {
    if (FA_ISNULL(&a->fa)) return -1;
    if (FA_ISNULL(&b->fa)) return 1;
    for (i = 0; i < nAka; i++) {
      if (!ftnaddress_cmp (&a->fa, fa + i)) return -1;
      if (!ftnaddress_cmp (&b->fa, fa + i)) return 1;
    }
    /* Files for different unknown akas? Hmm... */
  }
  /* 3. Compare status */
  if (a->type != b->type) {
    char typeorder[] = { 's', 'r', 'm', 'l', 'd' };
    for (i = 0; i < sizeof(typeorder)/sizeof(typeorder[0]); i++) {
      if (a->type == typeorder[i]) return -1;
      if (b->type == typeorder[i]) return 1;
    }
    /* Different unknown types? Hmm... */
  }
  /* 4. Compare files in filebox */
  if (a->type == 'd' && b->type == 'd') {
    return cmp_filebox_files(a, b);
  }
  /* No differences */
  return 0;
}

FTNQ *q_sort (FTNQ *q, FTN_ADDR *fa, int nAka, BINKD_CONFIG *cfg)
{
  /*
   * InsertSort
   * You're free to improve this function, replace it with 
   * quick/merge/heap or any other effective sorting algorithm
   */
  FTNQ *head, *tail, *qnext, *cur;

  if (q == NULL) return q;
  qnext = q->next;
  head = tail = q;
  q->next = NULL;
  while ((q = qnext)) {
    qnext = q->next;
    /* insert q into new queue */
    for (cur = head; cur; cur = cur->next) {
      if (q_cmp(cur, q, fa, nAka) > 0)
        break;
    }
    q->next = cur;
    if (cur) {
      q->prev = cur->prev;
      if (cur->prev)
        cur->prev->next = q;
      else
        head = q;
      cur->prev = q;
    } else {
      q->prev = tail;
      tail->next = q;
      tail = q;
    }
  }
  return head;
}

/*
 * Selects from q the next file for fa (Returns a pointer to a q element)
 */
FTNQ *select_next_file (FTNQ *q, FTN_ADDR *fa, int nAka)
{
  FTNQ *curr;

  for (curr = q; curr; curr = curr->next) {
    if (!curr->sent) { curr->sent = 1; return curr; }
  }
  return NULL;
}

/*
 * get size of files in the queue
 */
void q_get_sizes (FTNQ *q, uintmax_t *netsize, uintmax_t *filessize)
{
  FTNQ *curr;
  struct stat st;
  char *p;

  *netsize = *filessize = 0;
  for (curr = q; curr; curr = curr->next)
  {
    if (curr->type == 'l')
    { FILE *f;
      char str[MAXPATHLEN+2];

      if (curr->size)
        *filessize += curr->size;
      else if ((f = fopen(curr->path, "r")) != NULL)
      {
        curr->size = 0;
        curr->time = 0;
        while (fgets (str, sizeof(str), f))
        {
          if (*str == '~' || *str == '$') continue;
          if ((p = strchr(str, '\n')) != NULL) *p='\0';
          p=str;
          if (*str == '#' || *str == '^') p++;
          if (stat(p, &st) == 0) {
            *filessize += st.st_size;
            curr->size += st.st_size;
            if (st.st_mtime > curr->time) curr->time = st.st_mtime;
          }
        }
        fclose(f);
      }
    }
    else if (curr->type == 's')
      *filessize += curr->size;
    else
    {
      if (curr->size == 0)
      {
        if (stat(curr->path, &st) == 0) {
          curr->size = st.st_size;
          curr->time = st.st_mtime;
        }
      }
      *(curr->type == 'm' ? netsize : filessize) += curr->size;
    }
  }
}

/* 
 * Calculate quantity of freqs in the queue
 */
int q_freq_num (FTNQ *q)
{
  FTNQ *curr;
  int freqs = 0;

  for (curr = q; curr; curr = curr->next)
    if (curr->type == 'r')
      freqs++;
  return freqs;
}

/*
 * q_not_empty () == 0: the queue is empty.
 */

typedef struct
{
  int maxflvr;
  FTN_NODE *fn;
} qn_not_empty_arg;

static int qn_not_empty (FTN_NODE *fn, void *arg)
{
  qn_not_empty_arg *a = (qn_not_empty_arg *) arg;

  if (!fn->busy && strcmp (fn->hosts, "-") && fn->hold_until < safe_time())
  {
    if (a->maxflvr != MAXFLVR (fn->mail_flvr, MAXFLVR (fn->files_flvr, a->maxflvr)))
    {
      a->maxflvr = MAXFLVR (fn->mail_flvr, fn->files_flvr);
      a->fn = fn;
    }
  }
  return 0;
}

FTN_NODE *q_not_empty (BINKD_CONFIG *config)
{
  qn_not_empty_arg arg;

  arg.maxflvr = 0;
  arg.fn = 0;

  foreach_node (qn_not_empty, &arg, config);

  if (arg.maxflvr && tolower (arg.maxflvr) != 'h')
    return arg.fn;
  else
    return 0;
}

FTN_NODE *q_next_node (BINKD_CONFIG *config)
{
  FTN_NODE *fn = q_not_empty (config);

  if (fn == 0)
    return 0;
  else
  {
    fn->mail_flvr = fn->files_flvr = 0;
    fn->busy = 'c';
    return fn;
  }
}

/*
 * Creates an empty .?lo
 */
int create_poll (FTN_ADDR *fa, int flvr, BINKD_CONFIG *config)
{
  char buf[MAXPATHLEN + 1];
  char ext[5];
  int rc = 0;
  struct stat st;

  strcpy (ext, ".flo");
  if (flvr && strchr (flo_flvrs, flvr))
    ext[1] = tolower (flvr);
  ftnaddress_to_filename (buf, fa, config);
  if (*buf)
  {
    mkpath (buf);
    strnzcat (buf, ext, sizeof (buf));
    if (stat (buf, &st) == 0) return 1; /* already exists */
    if ((rc = create_empty_sem_file (buf)) == 0)
      if (errno != EEXIST)
        Log (1, "cannot create %s: %s", buf, strerror (errno));
  }
  else
    Log (1, "%s: unknown domain", fa->domain);
  return rc;
}

/*
 * Set .hld for a node
 */
void hold_node (FTN_ADDR *fa, time_t hold_until, BINKD_CONFIG *config)
{
  char buf[MAXPATHLEN + 1];
  char addr[FTN_ADDR_SZ + 1];
  char time[80];
  struct tm tm;

  safe_localtime (&hold_until, &tm);
  strftime (time, sizeof (time), "%Y/%m/%d %H:%M:%S", &tm);
  ftnaddress_to_str (addr, fa);
  ftnaddress_to_filename (buf, fa, config);
  Log (2, "holding %s (%s)", addr, time);
  if (*buf)
  {
    FILE *f;
    FTN_NODE *fn;

    strnzcat (buf, ".hld", sizeof (buf));
    if ((f = fopen (buf, "w")) != NULL)
    {
      fprintf (f, "%lu", (unsigned long int) hold_until);
      fclose (f);
      if ((fn = get_node_info (fa, config)) != NULL)
	fn->hold_until = hold_until;
    }
    else
    {
      Log (1, "%s: %s", buf, strerror (errno));
    }
  }
}

void write_try (FTN_ADDR *fa, unsigned *nok, unsigned *nbad, char *comment, BINKD_CONFIG *config)
{
  char buf[MAXPATHLEN + 1];

  if (config->tries > 0)
  {
    ftnaddress_to_filename (buf, fa, config);
    if (*buf)
    {
      FILE *f;

      strnzcat (buf, ".try", sizeof (buf));
      if ((f = fopen (buf, "wb")) != NULL)
      {
	fprintf (f, "%c%c%c%c%c%s",
		 *nok & 0xff, *nok >> 8,
		 *nbad & 0xff, *nbad >> 8,
		 (int) strlen (comment),
		 comment);
	fclose (f);
      }
    }
  }
}

void read_try (FTN_ADDR *fa, unsigned *nok, unsigned *nbad, BINKD_CONFIG *config)
{
  char buf[MAXPATHLEN + 1];

  ftnaddress_to_filename (buf, fa, config);
  if (*buf)
  {
    FILE *f;
    unsigned char ch1, ch2, ch3, ch4;

    strnzcat (buf, ".try", sizeof (buf));
    if ((f = fopen (buf, "rb")) != NULL &&
	fscanf (f, "%c%c%c%c", &ch1, &ch2, &ch3, &ch4) == 4)
    {
      *nok = ch1 + (ch2 << 8);
      *nbad = ch3 + (ch4 << 8);
    }
    else
    {
      *nok = *nbad = 0;
    }
    if (f)
      fclose (f);
  }
}

void bad_try (FTN_ADDR *fa, const char *error, const int where, BINKD_CONFIG *config)
{
  unsigned nok, nbad;

#ifdef WITH_PERL
  perl_on_error(config, fa, error, where);
#else
  UNUSED_ARG(where);
#endif
  if (config->tries == 0) return;
  read_try (fa, &nok, &nbad, config);
  if (config->tries > 0 && ++nbad >= (unsigned) config->tries)
  {
    nok = nbad = 0;
    hold_node (fa, safe_time() + config->hold, config);
  }
  write_try (fa, &nok, &nbad, (char *) error, config);
}

void good_try (FTN_ADDR *fa, char *comment, BINKD_CONFIG *config)
{
  unsigned nok, nbad;

  if (config->tries == 0) return;
  read_try (fa, &nok, &nbad, config);
  nbad = 0;
  ++nok;
  write_try (fa, &nok, &nbad, comment, config);
}
