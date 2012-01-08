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
 * Revision 2.41  2012/01/08 17:34:57  green
 * Avoid using MAXHOSTNAMELEN
 *
 * Revision 2.40  2012/01/08 14:09:04  green
 * Corrected initialization of getaddrinfo hints
 *
 * Revision 2.39  2012/01/07 11:54:04  green
 * Fix MSVC6 compilation errors
 *
 * Revision 2.38  2012/01/03 17:52:32  green
 * Implement FSP-1035 (SRV record usage)
 * - add SRV enabled getaddrinfo() wrapper (srv_gai.[ch])
 * - Unix (libresolv, autodetected) and Win32 support implemented
 * - Port information is stored as string now, i.e. may be service name
 *
 * Revision 2.37  2012/01/03 17:25:31  green
 * Implemented IPv6 support
 * - replace (almost) all getXbyY function calls with getaddrinfo/getnameinfo (RFC2553) calls
 * - Add compatibility layer for target systems not supporting RFC2553 calls in rfc2553.[ch]
 * - Add support for multiple listen sockets -- one for IPv4 and one for IPv6 (use V6ONLY)
 * - For WIN32 platform add configuration parameter IPV6 (mutually exclusive with BINKD9X)
 * - On WIN32 platform use Winsock2 API if IPV6 support is requested
 * - config: node IP address literal + port supported: [<ipv6 address>]:<port>
 *
 * Revision 2.36  2010/12/02 11:04:14  stas
 * if any of passwords for the node is presents, then don't change all passwords. The FIRST token "Node" with any not-empty password in the config file is set all passwords, the passwords from password-file is used for node if password don't set in main config file
 *
 * Revision 2.35  2004/10/25 17:04:54  gul
 * Process passwords file after all, independent of its place in config.
 * Use first password for node if several specified.
 *
 * Revision 2.34  2004/09/21 08:27:49  val
 * distinguish nodes, listed in binkd config and passwords file - overwrite defnode parameters (e.g. host) for the later
 * (hope, it'll fix reported bug with not calling defnode)
 *
 * Revision 2.33  2004/09/11 13:57:12  gul
 * Remove duplicate line
 *
 * Revision 2.32  2004/09/06 10:50:06  val
 * node data could be over-written by defnode data if hosts were set to `-', fixed
 *
 * Revision 2.31  2004/09/06 10:47:04  val
 * bandwidth limiting code advancements, `listed' session state fix
 *
 * Revision 2.30  2004/01/18 20:39:31  gul
 * Undo previous patch
 *
 * Revision 2.29  2004/01/15 14:49:12  gul
 * If only two passwords for a node specified in passwd-file, use its as
 * in,out+pkt but not in+out,pkt.
 *
 * Revision 2.28  2004/01/08 12:57:18  val
 * * parse up to 3 comma-separated passwords (in,pkt,out)
 * * use out password for outgoing sessions if it's set
 *
 * Revision 2.27  2004/01/07 12:07:47  gul
 * New function free_nodes()
 *
 * Revision 2.26  2003/11/21 19:39:59  stream
 * Initial support for "-noproxy" node option
 *
 * Revision 2.25  2003/10/29 21:08:38  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.24  2003/09/19 13:37:14  val
 * old get_defnode_info() logic returned for a while
 *
 * Revision 2.23  2003/09/16 06:38:44  val
 * correct IP checking algorithms (gul's one is buggy), correct get_defnode_info()
 *
 * Revision 2.22  2003/09/14 12:29:32  gul
 * Optimize a bit
 *
 * Revision 2.21  2003/09/08 16:39:39  stream
 * Fixed race conditions when accessing array of nodes in threaded environment
 * ("jumpimg node structures")
 *
 * Revision 2.20  2003/09/08 08:21:20  stream
 * Cleanup config semaphore, free memory of base config on exit.
 *
 * Revision 2.19  2003/08/26 21:01:10  gul
 * Fix compilation under unix
 *
 * Revision 2.18  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.17  2003/08/18 17:19:13  stream
 * Partially implemented new configuration parser logic (required for config reload)
 *
 * Revision 2.16  2003/07/12 18:06:40  gul
 * Fixed node output on debugcfg
 *
 * Revision 2.15  2003/06/30 22:48:36  hbrew
 * Allow to override -ip, -sip, -md, -nomd in add_node()
 *
 * Revision 2.14  2003/06/20 10:37:02  val
 * Perl hooks for binkd - initial revision
 *
 * Revision 2.13  2003/06/12 08:30:57  val
 * check pkt header feature, see keyword 'check-pkthdr'
 *
 * Revision 2.12  2003/05/04 08:49:05  gul
 * Fix previous patch
 *
 * Revision 2.11  2003/05/04 08:45:30  gul
 * Lock semaphores more safely for resolve and IP-addr print
 *
 * Revision 2.10  2003/05/01 09:55:01  gul
 * Remove -crypt option, add global -r option (disable crypt).
 *
 * Revision 2.9  2003/04/23 04:35:34  gul
 * Fix semaphores usage
 *
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
	      int MD_flag, int restrictIP, int HC_flag, int NP_flag, 
#ifdef BW_LIM
              long bw_send, long bw_recv,
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
    pn->restrictIP = RIP_OFF;
#ifdef BW_LIM
    pn->bw_send = bw_send; pn->bw_recv = bw_recv;
#endif

    /* We've broken the order... */
    config->nNodSorted = 0;
  }

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

  if (hosts && *hosts)
  {
    xfree (pn->hosts);
    pn->hosts = xstrdup (hosts);
  }

  /* pwd, "-" for no password (requirement of binkp protocol) */
  if ( (pwd && strcmp(pwd, "-"))
        && (!pn->pwd || strcmp(pn->pwd, "-") == 0)
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
	      int MD_flag, int restrictIP, int HC_flag, int NP_flag, 
#ifdef BW_LIM
              long bw_send, long bw_recv,
#endif
              BINKD_CONFIG *config)
{
  FTN_NODE *pn;

  locknodesem();
  pn = add_node_nolock(fa, hosts, pwd, pkt_pwd, out_pwd, obox_flvr, obox, ibox, 
                  NR_flag, ND_flag, MD_flag, restrictIP, HC_flag, NP_flag, 
#ifdef BW_LIM
                  bw_send, bw_recv,
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
  char *port;
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

  for (i=1; np->hosts && get_host_and_port(i, host, &port, np->hosts, fa, config)==1; i++)
  {
    if (!strcmp(host, "-"))
      continue;

    aiErr = srv_getaddrinfo(host, port ? port : "0", &hints, &ai);
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
#ifdef BW_LIM
    on->bw_send = np->bw_send; on->bw_recv = np->bw_recv;
#endif
    return on;
  }

  add_node_nolock(fa, np->hosts, NULL, NULL, NULL, np->obox_flvr, np->obox, np->ibox,
       np->NR_flag, np->ND_flag, np->MD_flag, np->restrictIP, np->HC_flag, np->NP_flag, 
#ifdef BW_LIM
       np->bw_send, np->bw_recv,
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
  np = search_for_node(&n, config);
  if ((!np || np->listed != NL_NODE) && config->havedefnode)
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
  int i, rc = 0;

  locknodesem();

  if (!config->nNodSorted)
    sort_nodes (config);

  for (i = 0; i < config->nNod; ++i)
  {
    FTN_NODE *n = config->pNodArray[i];

    if (!n->hosts)
      rc = func (get_node_info_nolock(&(n->fa), config), arg);
    else
      rc = func (n, arg);
    if (rc != 0)
      break;
  }
  releasenodesem();
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
    Log (1, "`%s' cannot be parsed as a Fido-style address\n", s);
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
      add_node_nolock (&target, "*", NULL, NULL, NULL, '-', NULL, NULL, NR_USE_OLD, ND_USE_OLD,
                       MD_USE_OLD, RIP_USE_OLD, HC_USE_OLD, NP_USE_OLD, 
#ifdef BW_LIM
                       BW_DEF, BW_DEF,
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
    if (node->pkt_pwd && node->pkt_pwd != (char*)&(node->pwd)) free(node->pkt_pwd);
    if (node->out_pwd && node->out_pwd != (char*)&(node->pwd)) free(node->out_pwd);
    free(node);
  }
  xfree(config->pNodArray);
}

