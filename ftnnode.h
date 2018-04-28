/*
 *  ftnnode.h -- Handle our links
 *
 *  ftnnode.h is a part of binkd project
 *
 *  Copyright (C) 1996  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#ifndef _ftnnode_h
#define _ftnnode_h

#include "btypes.h"

/*
 * Call this before all others functions from this file.
 */
void nodes_init (void);

/*
 * Call this before exit to free resources.
 */
void nodes_deinit (void);

/*
 * Return up/downlink info by fidoaddress. 0 == node not found
 */
FTN_NODE *get_node_info (FTN_ADDR *fa, BINKD_CONFIG *config);

/*
 * Add a new node, or edit old settings for a node
 */
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
              BINKD_CONFIG *config);

#define NL_UNLISTED  0                 /* node is unlisted (dynamically added) */
#define NL_NODE      1                 /* node is listed in binkd config */
#define NL_PASSWORDS 2                 /* node is listed in passwords file */

#define NR_ON        1
#define NR_OFF       0
#define NR_USE_OLD  -1		       /* Use old value */

#define ND_ON        1
#define ND_OFF       0
#define ND_USE_OLD  -1		       /* Use old value */

#define MD_ON        1
#define MD_OFF      -1
#define MD_USE_OLD   0		       /* Use old value, default value */

#define RIP_ON       1
#define RIP_OFF      0
#define RIP_SIP      2		       /* Strict IP check (-sip) */
#define RIP_USE_OLD -1		       /* Use old value */

#define HC_ON        1
#define HC_OFF      -1
#define HC_USE_OLD   0		       /* Use old value, default value */

#define NP_ON       1
#define NP_OFF      0
#define NP_USE_OLD -1		       /* Use old value */

#define AF_USE_OLD -1		       /* Use old value */

#ifdef BW_LIM
#define BW_DEF     -100                /* default value: 100% */
#endif

#define RESOLVE_TTL 3600               /* DNS resolution again after 1 hour */

/*
 * Iterates through nodes while func() == 0.
 */
int foreach_node (int (*func) (FTN_NODE *fn, void *a2), void *a3, BINKD_CONFIG *config);

/*
 * Free pNodArray
 */
void free_nodes(BINKD_CONFIG *config);

/*
 * Create a poll for an address (in "z:n/n.p" format) (0 -- bad)
 */
#ifndef POLL_NODE_FLAVOUR
#define POLL_NODE_FLAVOUR 'd'
#endif
int poll_node (char *s, BINKD_CONFIG *config);

#endif
