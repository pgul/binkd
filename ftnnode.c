/*
 *  ftnnode.c -- Handle our links
 *
 *  ftnnode.c is a part of binkd project
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
 * Revision 2.8  2003/03/31 19:51:29  gul
 * Fix prev patch
 *
 * Revision 2.7  2003/03/31 19:35:16  gul
 * Clean semaphores usage
 *
 * Revision 2.6  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.5  2003/02/22 15:53:45  gul
 * Bugfix with locking array of nodes in multithread version
 *
 * Revision 2.4  2003/02/22 14:30:18  gul
 * Make nNod var static
 *
 * Revision 2.3  2003/02/22 12:12:33  gul
 * Cleanup sources
 *
 * Revision 2.2  2002/05/10 17:46:06  gul
 * passwords file usage bugfix
 *
 * Revision 2.1  2001/02/15 11:03:18  gul
 * Added crypt traffic possibility
 *
 * Revision 2.0  2001/01/10 12:12:38  gul
 * Binkd is under CVS again
 *
 * Revision 1.3  1997/11/03  06:10:39  mff
 * +nodes_init()
 *
 * Revision 1.2  1997/10/23  04:07:55  mff
 * many changes to hide pNod int ftnnode.c
 *
 * Revision 1.1  1996/12/29  09:41:37  mff
 * Initial revision
 */

#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "ftnnode.h"
#include "ftnq.h"
#include "tools.h"
#include "sem.h"
#include "readcfg.h"

#if defined(HAVE_THREADS) || defined(AMIGA)
static MUTEXSEM NSem;
#endif

static int nNod = 0;
static FTN_NODE *pNod = 0;
static int nNodSorted = 0;

/*
 * Call this before all others functions from this file.
 */
void nodes_init (void)
{
  InitSem (&NSem);
}

void locknodesem (void)
{
  LockSem (&NSem);
}

void releasenodesem (void)
{
  ReleaseSem (&NSem);
}

void nodes_deinit(void)
{
  CleanSem (&NSem);
}

/*
 * Compares too nodes. 0 == don't match
 */
static int node_cmp (FTN_NODE *a, FTN_NODE *b)
{
  return ftnaddress_cmp (&a->fa, &b->fa);
}

/*
 * Sorts pNod array. Must NOT be called if NSem is locked!
 */
static void sort_nodes (void)
{
  qsort (pNod, nNod, sizeof (FTN_NODE), (int (*) (const void *, const void *)) node_cmp);
  nNodSorted = 1;
}

/*
 * Add a new node, or edit old settings for a node
 *
 * 1 -- ok, 0 -- error;
 */
static int add_node_nolock(FTN_ADDR *fa, char *hosts, char *pwd, char obox_flvr,
	      char *obox, char *ibox, int NR_flag, int ND_flag,
	      int crypt_flag, int MD_flag, int restrictIP)
{
  int cn;

  locknodesem();
  for (cn = 0; cn < nNod; ++cn)
  {
    if (!ftnaddress_cmp (&pNod[cn].fa, fa))
      break;
  }
  /* Node not found, create new entry */
  if (cn >= nNod)
  {
    cn = nNod;

    pNod = xrealloc (pNod, sizeof (FTN_NODE) * ++nNod);
    memset (pNod + cn, 0, sizeof (FTN_NODE));
    memcpy (&pNod[cn].fa, fa, sizeof (FTN_ADDR));
    strcpy (pNod[cn].pwd, "-");
    pNod[cn].hosts = NULL;
    pNod[cn].obox_flvr = 'f';
    pNod[cn].NR_flag = NR_OFF;
    pNod[cn].ND_flag = ND_OFF;
    pNod[cn].crypt_flag = CRYPT_OFF;
    pNod[cn].MD_flag = 0;
    pNod[cn].restrictIP = 0;

    /* We've broken the order... */
    nNodSorted = 0;
  }

  if(!pNod[cn].MD_flag) 
    pNod[cn].MD_flag = MD_flag;
  if(!pNod[cn].restrictIP) 
    pNod[cn].restrictIP = restrictIP;

  if (NR_flag != NR_USE_OLD)
    pNod[cn].NR_flag = NR_flag;
  if (ND_flag != ND_USE_OLD)
    pNod[cn].ND_flag = ND_flag;
  if (crypt_flag != CRYPT_USE_OLD)
    pNod[cn].crypt_flag = crypt_flag;

  if (hosts && *hosts)
  {
    if (pNod[cn].hosts) free (pNod[cn].hosts);	       
    pNod[cn].hosts = xstrdup (hosts);
  }

  if (pwd && *pwd && strcmp(pwd, "-"))
  {
    strnzcpy (pNod[cn].pwd, pwd, sizeof (pNod[cn].pwd));
  }

  if (obox_flvr != '-')
  {
    pNod[cn].obox_flvr = obox_flvr;
  }

  if (obox)
  {
    if (pNod[cn].obox)
      free (pNod[cn].obox);
    pNod[cn].obox = xstrdup (obox);
  }

  if (ibox)
  {
    if (pNod[cn].ibox)
      free (pNod[cn].ibox);
    pNod[cn].ibox = xstrdup (ibox);
  }

  releasenodesem();
  return 1;
}

int add_node (FTN_ADDR *fa, char *hosts, char *pwd, char obox_flvr,
	      char *obox, char *ibox, int NR_flag, int ND_flag,
	      int crypt_flag, int MD_flag, int restrictIP)
{
  int res;
  locknodesem();
  res = add_node_nolock(fa, hosts, pwd, obox_flvr, obox, ibox, NR_flag, ND_flag,
                        crypt_flag, MD_flag, restrictIP);
  releasenodesem();
  return res;
}

static FTN_NODE *get_defnode_info(FTN_ADDR *fa, FTN_NODE *on)
{
  struct hostent *he;
  FTN_NODE n, *np;
  char host[MAXHOSTNAMELEN + 1];       /* current host/port */
  unsigned short port;
  int i;

  strcpy(n.fa.domain, "defnode");
  n.fa.z=n.fa.net=n.fa.node=n.fa.p=0;
  np = (FTN_NODE *) bsearch (&n, pNod, nNod, sizeof (FTN_NODE),
                 (int (*) (const void *, const void *)) node_cmp);

  if (!np) /* we don't have defnode info */
    return on;

  for (i=1; get_host_and_port(i, host, &port, np->hosts, fa)==1; i++)
  {
    lockhostsem();
    he=gethostbyname(host);
    releasehostsem();
    if (!he) continue;
    sprintf (host+strlen(host), ":%d", port);
    i=0;
    break;
  }
  if (i)
    strcpy(host, "-");
  if (on)
  { /* on contains only passwd */
    on->hosts=xstrdup(host);
    on->NR_flag=np->NR_flag;
    on->ND_flag=np->ND_flag;
    on->MD_flag=np->MD_flag;
    on->ND_flag=np->ND_flag;
    on->restrictIP=np->restrictIP;
    return on;
  }

  if(!add_node_nolock(fa, host, NULL, np->obox_flvr, np->obox, np->ibox, 
       np->NR_flag, np->ND_flag, np->crypt_flag, np->MD_flag, np->restrictIP))
    return NULL;
  sort_nodes ();
  memcpy (&n.fa, fa, sizeof (FTN_ADDR));
  return (FTN_NODE *) bsearch (&n, pNod, nNod, sizeof (FTN_NODE),
                      (int (*) (const void *, const void *)) node_cmp);
}
/*
 * Return up/downlink info by fidoaddress. 0 == node not found
 */
FTN_NODE *get_node_info (FTN_ADDR *fa)
{
  FTN_NODE n, *np;

  if (!nNodSorted)
    sort_nodes ();
  memcpy (&n.fa, fa, sizeof (FTN_ADDR));
  np = (FTN_NODE *) bsearch (&n, pNod, nNod, sizeof (FTN_NODE),
			   (int (*) (const void *, const void *)) node_cmp);
  if ((!np || !np->hosts) && havedefnode)
    np=get_defnode_info(fa, np);
  else if (np && !np->hosts)
    /* node exists only in passwords and defnode is not defined */
    np->hosts = xstrdup("-");
  return np;
}

/*
 * Find up/downlink info by fidoaddress and write info into node var.
 * Return pointer to node structure or NULL if node not found.
 */
FTN_NODE *get_node (FTN_ADDR *fa, FTN_NODE *node)
{
  FTN_NODE *n;

  locknodesem();
  if ((n = get_node_info(fa)) == NULL)
  {
    releasenodesem();
    return NULL;
  }
  memcpy(node, n, sizeof(*node));
  releasenodesem();
  return node;
}

/*
 * Iterates through nodes while func() == 0.
 */
int foreach_node (int (*func) (FTN_NODE *, void *), void *arg)
{
  int i, rc = 0;

  locknodesem();
  for (i = 0; i < nNod; ++i)
  {
    if (!pNod[i].hosts)
      rc = func (get_node_info(&(pNod[i].fa)), arg);
    else
      rc = func (pNod + i, arg);
    if (rc != 0)
      break;
  }
  releasenodesem();
  return rc;
}

static int print_node_info_1 (FTN_NODE *fn, void *arg)
{
  char szfa[FTN_ADDR_SZ + 1];

  ftnaddress_to_str (szfa, &fn->fa);
  fprintf ((FILE *) arg, "%-20.20s %s %s %c %s %s%s%s\n",
	   szfa, fn->hosts ? fn->hosts : "-", fn->pwd,
	   fn->obox_flvr, fn->obox ? fn->obox : "-",
	   fn->ibox ? fn->ibox : "-",
	   (fn->NR_flag == NR_ON) ? " -NR" : "",
	   (fn->ND_flag == ND_ON) ? " -ND" : "");
  return 0;
}

void print_node_info (FILE *out)
{
  foreach_node (print_node_info_1, out);
}

/*
 * Create a poll for an address (in "z:n/n.p" format) (0 -- bad)
 */
int poll_node (char *s)
{
  FTN_ADDR target;

  if (!parse_ftnaddress (s, &target))
  {
    Log (1, "`%s' cannot be parsed as a Fido-style address\n", s);
    return 0;
  }
  else
  {
    char buf[FTN_ADDR_SZ + 1];

    exp_ftnaddress (&target);
    ftnaddress_to_str (buf, &target);
    Log (4, "creating a poll for %s (`%c' flavour)", buf, POLL_NODE_FLAVOUR);
    locknodesem();
    if (!get_node_info (&target))
      if (!add_node_nolock (&target, "*", 0, '-', 0, 0, 0, 0, 0, 0, 0))
	Log (1, "%s: add_node() failed", buf);
    releasenodesem();
    return create_poll (&target, POLL_NODE_FLAVOUR);
  }
}
