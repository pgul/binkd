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

#include <stdlib.h>
#include <string.h>

#include "sys.h"
#include "readcfg.h"
#include "ftnnode.h"
#include "ftnaddr.h"
#include "sem.h"
#include "tools.h"
#include "ftnq.h"
#include "iphdr.h"
#include "rfc2553.h"
#include "srv_gai.h"

#if defined(HAVE_THREADS) || defined(AMIGA)
static MUTEXSEM NSem;
#endif

/*
 * Call this before all others functions from this file.
 */
void nodes_init (void)
{
  InitSem (&NSem);
}

static void locknodesem (void)
{
  LockSem (&NSem);
}

static void releasenodesem (void)
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
static int node_cmp (FTN_NODE **pa, FTN_NODE **pb)
{
  return ftnaddress_cmp (&(*pa)->fa, &(*pb)->fa);
}

/*
 * Sorts pNod array. Must NOT be called if NSem is locked!
 */
static void sort_nodes (BINKD_CONFIG *config)
{
  qsort (config->pNodArray, config->nNod, sizeof (FTN_NODE *), (int (*) (const void *, const void *)) node_cmp);
  config->nNodSorted = 1;
}

/*
 * Add a new node, or edit old settings for a node
 */
static FTN_NODE *add_node_nolock (FTN_ADDR *fa, char *hosts, char *pwd, char *pkt_pwd, char *out_pwd,
              char obox_flvr, char *obox, char *ibox, int NR_flag, int ND_flag,
	      int MD_flag, int restrictIP, int HC_flag, int NP_flag, char *pipe,
	      int IP_afamily,
#ifdef BW_LIM
              long bw_send, long bw_recv,
#endif
#ifdef AF_FORCE
              int AFF_flag,
#endif
              BINKD_CONFIG *config)
{
  int cn;
  FTN_NODE *pn = NULL; /* avoid compiler warning */

  for (cn = 0; cn < config->nNod; ++cn)
  {
    pn = config->pNodArray[cn];
    if (!ftnaddress_cmp (&(pn->fa), fa))
      break;
  }
  /* Node not found, create new entry */
  if (cn >= config->nNod)
  {
    cn = config->nNod;
    config->pNodArray = xrealloc (config->pNodArray, sizeof (FTN_NODE *) * ++(config->nNod));
    config->pNodArray[cn] = pn = xalloc(sizeof(FTN_NODE));
    memset (pn, 0, sizeof (FTN_NODE));
    memcpy (&(pn->fa), fa, sizeof (FTN_ADDR));
    strcpy (pn->pwd, "-");
    pn->hosts = NULL;
    pn->obox_flvr = 'f';
    pn->NR_flag = NR_OFF;
    pn->ND_flag = ND_OFF;
    pn->NP_flag = NP_OFF;
    pn->MD_flag = MD_USE_OLD;
    pn->HC_flag = HC_USE_OLD;
    pn->pipe = NULL;
    pn->restrictIP = RIP_OFF;
    pn->IP_afamily = AF_UNSPEC;
#ifdef BW_LIM
    pn->bw_send = bw_send; pn->bw_recv = bw_recv;
#endif
#ifdef AF_FORCE
    pn->AFF_flag = AFF_flag;
#endif

    /* We've broken the order... */
    config->nNodSorted = 0;
  }

  pn->recheck = safe_time() + RESOLVE_TTL;
  if (MD_flag != MD_USE_OLD)
    pn->MD_flag = MD_flag;
  if (restrictIP != RIP_USE_OLD)
    pn->restrictIP = restrictIP;
  if (NR_flag != NR_USE_OLD)
    pn->NR_flag = NR_flag;
  if (ND_flag != ND_USE_OLD)
    pn->ND_flag = ND_flag;
  if (NP_flag != NP_USE_OLD)
    pn->NP_flag = NP_flag;
  if (HC_flag != HC_USE_OLD)
    pn->HC_flag = HC_flag;
  if (IP_afamily != AF_USE_OLD)
    pn->IP_afamily = IP_afamily;

  if (pipe != NULL)
  {
    xfree (pn->pipe);
    pn->pipe = xstrdup (pipe);
  }

  if (hosts && *hosts)
  {
    xfree (pn->hosts);
    pn->hosts = xstrdup (hosts);
  }

  /* pwd, "-" for no password (requirement of binkp protocol) */
  if ( (pwd && strcmp(pwd, "-"))
        && strcmp(pn->pwd, "-") == 0
        && (!pn->pkt_pwd || strcmp(pn->pkt_pwd, "-") == 0)
        && (!pn->out_pwd || strcmp(pn->out_pwd, "-") == 0)
     ) /* if any of passwords for the node is presents, then don't change all passwords */
  {
    strnzcpy(pn->pwd, pwd, sizeof(pn->pwd));
    /* if NULL keep pointer to pn->pwd, if "-" use no password */
    if (pkt_pwd) {
      if (strcmp(pkt_pwd, "-")) pn->pkt_pwd = xstrdup(pkt_pwd);
      else pn->pkt_pwd = NULL;
    }
    else pn->pkt_pwd = (char*)&(pn->pwd);
    /* if NULL keep pointer to pn->pwd, if "-" use no password */
    if (out_pwd) {
      if (strcmp(out_pwd, "-")) pn->out_pwd = xstrdup(out_pwd);
      else pn->out_pwd = NULL;
    }
    else pn->out_pwd = (char*)&(pn->pwd);
  }

  if (obox_flvr != '-')
  {
    pn->obox_flvr = obox_flvr;
  }

  if (obox)
  {
    xfree (pn->obox);
    pn->obox = xstrdup (obox);
  }

  if (ibox)
  {
    xfree (pn->ibox);
    pn->ibox = xstrdup (ibox);
  }

  return pn;
}

FTN_NODE *add_node (FTN_ADDR *fa, char *hosts, char *pwd, char *pkt_pwd, char *out_pwd,
              char obox_flvr, char *obox, char *ibox, int NR_flag, int ND_flag,
	      int MD_flag, int restrictIP, int HC_flag, int NP_flag, char *pipe,
	      int IP_afamily,
#ifdef BW_LIM
              long bw_send, long bw_recv,
#endif
#ifdef AF_FORCE
              int AFF_flag,
#endif
              BINKD_CONFIG *config)
{
  FTN_NODE *pn;

  locknodesem();
  pn = add_node_nolock(fa, hosts, pwd, pkt_pwd, out_pwd, obox_flvr, obox, ibox, 
                  NR_flag, ND_flag, MD_flag, restrictIP, HC_flag, NP_flag, pipe,
		  IP_afamily,
#ifdef BW_LIM
                  bw_send, bw_recv,
#endif
#ifdef AF_FORCE
                  AFF_flag,
#endif
                  config);
  releasenodesem();
  return pn;
}

static FTN_NODE *search_for_node(FTN_NODE *np, BINKD_CONFIG *config)
{
  FTN_NODE **npp;

  npp = (FTN_NODE **) bsearch (&np, config->pNodArray, config->nNod, sizeof (FTN_NODE *),
                               (int (*) (const void *, const void *)) node_cmp);
  return npp ? *npp : NULL;
}

static FTN_NODE *get_defnode_info(FTN_ADDR *fa, FTN_NODE *on, BINKD_CONFIG *config)
{
  struct addrinfo *ai, hints;
  int aiErr;
  FTN_NODE n, *np;
  char host[BINKD_FQDNLEN + 1];       /* current host/port */
  char port[MAXPORTSTRLEN + 1] = { 0 };
  int i;

  /* setup hints for getaddrinfo */
  memset((void *)&hints, 0, sizeof(hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  strcpy(n.fa.domain, "defnode");
  n.fa.z=n.fa.net=n.fa.node=n.fa.p=0;
  np = search_for_node(&n, config);

  if (!np) /* we don't have defnode info */
    return on;

  for (i=1; np->hosts && get_host_and_port(i, host, port, np->hosts, fa, config)==1; i++)
  {
    if (!strcmp(host, "-"))
      continue;

    aiErr = srv_getaddrinfo(host, port[0] ? port : NULL, &hints, &ai);
    if (aiErr != 0) continue;
    freeaddrinfo(ai);
    sprintf (host+strlen(host), ":%s", port);
    i=0;
    break;
  }
  if (i)
    strcpy(host, "-");
  /* following section will copy defnode parameters to the node record */
  if (on)
  { /* on contains only passwd */
    on->hosts=xstrdup(/*host*/np->hosts);
    on->NR_flag=np->NR_flag;
    on->ND_flag=np->ND_flag;
    on->MD_flag=np->MD_flag;
    on->NP_flag=np->NP_flag;
    on->HC_flag=np->HC_flag;
    on->restrictIP=np->restrictIP;
    on->pipe=np->pipe;
    on->IP_afamily=np->IP_afamily;
#ifdef BW_LIM
    on->bw_send = np->bw_send; on->bw_recv = np->bw_recv;
#endif
#ifdef AF_FORCE
    on->AFF_flag = np->AFF_flag;
#endif
    return on;
  }

  add_node_nolock(fa, np->hosts, NULL, NULL, NULL, np->obox_flvr, np->obox, 
       np->ibox, np->NR_flag, np->ND_flag, np->MD_flag, np->restrictIP, 
       np->HC_flag, np->NP_flag, np->pipe, np->IP_afamily,
#ifdef BW_LIM
       np->bw_send, np->bw_recv,
#endif
#ifdef AF_FORCE
       np->AFF_flag,
#endif
       config);
  sort_nodes (config);
  memcpy (&n.fa, fa, sizeof (FTN_ADDR));
  return search_for_node(&n, config);
}
/*
 * Return up/downlink info by fidoaddress. 0 == node not found
 */
static FTN_NODE *get_node_info_nolock (FTN_ADDR *fa, BINKD_CONFIG *config)
{
  FTN_NODE n, *np;

  if (!config->nNodSorted)
    sort_nodes (config);
  memcpy (&n.fa, fa, sizeof (FTN_ADDR));

  /* search from previously stored nodes */
  np = search_for_node(&n, config);

  /* not found or not in config file and recheck required ... */
  if (( !np || 
        (np->listed != NL_NODE && np->recheck < safe_time())) 
      && config->havedefnode) 
    /* ... try resolve from defnode */
    np=get_defnode_info(fa, np, config);
  if (np && !np->hosts) /* still no hosts? */
    np->hosts = xstrdup("-");
  return np;
}

/*
 * Find up/downlink info by fidoaddress and write info into node var.
 * Return pointer to node structure or NULL if node not found.
 */
FTN_NODE *get_node_info (FTN_ADDR *fa, BINKD_CONFIG *config)
{
  FTN_NODE *n;

  locknodesem();
  n = get_node_info_nolock(fa, config);
  releasenodesem();
  return n;
}

/*
 * Iterates through nodes while func() == 0.
 */
int foreach_node (int (*func) (FTN_NODE *, void *), void *arg, BINKD_CONFIG *config)
{
  int nNod, i, rc = 0;
  FTN_NODE **pNodArray;

  locknodesem();

  if (!config->nNodSorted)
    sort_nodes (config);

  /* copy node array and release semaphore */
  /* avoid deadlock if get_node_info() used by func() */
  nNod = config->nNod;
  pNodArray = xalloc(nNod * sizeof(FTN_NODE *));
  memcpy(pNodArray, config->pNodArray, nNod * sizeof(FTN_NODE *));
  releasenodesem();

  for (i = 0; i < nNod; ++i)
  {
    FTN_NODE *n = pNodArray[i];

    if (!n->hosts)
      rc = func (get_node_info(&(n->fa), config), arg);
    else
      rc = func (n, arg);
    if (rc != 0)
      break;
  }
  xfree(pNodArray);
  return rc;
}

/*
 * Create a poll for an address (in "z:n/n.p" format) (0 -- bad)
 */
int poll_node (char *s, BINKD_CONFIG *config)
{
  FTN_ADDR target;

  if (!parse_ftnaddress (s, &target, config->pDomains.first))
  {
    Log (1, "`%s' cannot be parsed as a Fido-style address", s);
    return 0;
  }
  else
  {
    char buf[FTN_ADDR_SZ + 1];

    exp_ftnaddress (&target, config->pAddr, config->nAddr, config->pDomains.first);
    ftnaddress_to_str (buf, &target);
    Log (4, "creating a poll for %s (`%c' flavour)", buf, POLL_NODE_FLAVOUR);
    locknodesem();
    if (!get_node_info_nolock (&target, config))
      add_node_nolock (&target, "*", NULL, NULL, NULL, '-', NULL, NULL, 
		       NR_USE_OLD, ND_USE_OLD, MD_USE_OLD, RIP_USE_OLD, 
		       HC_USE_OLD, NP_USE_OLD, NULL, AF_USE_OLD,
#ifdef BW_LIM
                       BW_DEF, BW_DEF,
#endif
#ifdef AF_FORCE
                       0,
#endif
                       config);
    releasenodesem();
    return create_poll (&target, POLL_NODE_FLAVOUR, config);
  }
}

/*
 * Free pNodArray
 * Semaphoring is not needed
 */
void free_nodes(BINKD_CONFIG *config)
{
  int i;
  FTN_NODE *node;

  for (i = 0; i < config->nNod; i++)
  {
    node = config->pNodArray[i];
    xfree(node->hosts);
    xfree(node->obox);
    xfree(node->ibox);
    xfree(node->pipe);
    if (node->pkt_pwd && node->pkt_pwd != (char*)&(node->pwd)) free(node->pkt_pwd);
    if (node->out_pwd && node->out_pwd != (char*)&(node->pwd)) free(node->out_pwd);
    free(node);
  }
  xfree(config->pNodArray);
}

