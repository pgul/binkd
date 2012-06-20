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

/*
 * $Id$
 *
 * $Log$
 * Revision 2.23  2012/06/20 22:41:46  green
 * 1 hour timeout for defnode resolutions
 *
 * Revision 2.22  2012/01/22 13:54:12  green
 * Allow limiting IPv4/6 usage per node using new flags -4/-6
 *
 * Revision 2.21  2005/10/28 07:13:28  stas
 * Change poll flavour to 'direct' and allow specify poll flavour char at compile time using macro POLL_NODE_FLAVOUR
 *
 * Revision 2.20  2004/09/21 08:27:49  val
 * distinguish nodes, listed in binkd config and passwords file - overwrite defnode parameters (e.g. host) for the later
 * (hope, it'll fix reported bug with not calling defnode)
 *
 * Revision 2.19  2004/09/06 10:47:04  val
 * bandwidth limiting code advancements, `listed' session state fix
 *
 * Revision 2.18  2004/01/08 12:57:18  val
 * * parse up to 3 comma-separated passwords (in,pkt,out)
 * * use out password for outgoing sessions if it's set
 *
 * Revision 2.17  2004/01/07 12:09:45  gul
 * New function free_nodes()
 *
 * Revision 2.16  2003/11/21 19:39:59  stream
 * Initial support for "-noproxy" node option
 *
 * Revision 2.15  2003/10/29 21:08:38  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.14  2003/09/08 16:39:39  stream
 * Fixed race conditions when accessing array of nodes in threaded environment
 * ("jumpimg node structures")
 *
 * Revision 2.13  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.12  2003/08/18 17:19:13  stream
 * Partially implemented new configuration parser logic (required for config reload)
 *
 * Revision 2.11  2003/06/30 22:48:36  hbrew
 * Allow to override -ip, -sip, -md, -nomd in add_node()
 *
 * Revision 2.10  2003/06/20 10:37:02  val
 * Perl hooks for binkd - initial revision
 *
 * Revision 2.9  2003/06/12 08:30:57  val
 * check pkt header feature, see keyword 'check-pkthdr'
 *
 * Revision 2.8  2003/05/01 09:55:01  gul
 * Remove -crypt option, add global -r option (disable crypt).
 *
 * Revision 2.7  2003/03/31 19:35:16  gul
 * Clean semaphores usage
 *
 * Revision 2.6  2003/03/11 00:04:25  gul
 * Use patches for compile under MSDOS by MSC 6.0 with IBMTCPIP
 *
 * Revision 2.5  2003/03/01 15:55:02  gul
 * Current outgoing address is now attibute of session, but not node
 *
 * Revision 2.4  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.3  2003/02/22 15:53:46  gul
 * Bugfix with locking array of nodes in multithread version
 *
 * Revision 2.2  2003/02/22 11:45:41  gul
 * Do not resolve hosts if proxy or socks5 using
 *
 * Revision 2.1  2001/02/15 11:03:18  gul
 * Added crypt traffic possibility
 *
 * Revision 2.0  2001/01/10 12:12:38  gul
 * Binkd is under CVS again
 *
 * Revision 1.6  1997/11/03  06:10:39  mff
 * +nodes_init()
 *
 * Revision 1.5  1997/10/23  04:06:48  mff
 * many changes to hide pNod int ftnnode.c
 *
 * Revision 1.4  1997/08/19  21:32:09  mff
 * host/port replaced with hosts -- ASCIIz in "host1,host2,..,hostN" form
 *
 * Revision 1.3  1997/06/16  05:49:13  mff
 * Removed bitmapped flavours.
 *
 * Revision 1.2  1997/02/07  07:07:00  mff
 * busy, flvrs, hold_until, ibox, obox, obox_flvr
 *
 * Revision 1.1  1996/12/29  09:41:45  mff
 * Initial revision
 *
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
	      int MD_flag, int restrictIP, int HC_flag, int NP_flag, 
	      int IP_afamily,
#ifdef BW_LIM
              long bw_send, long bw_recv,
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
