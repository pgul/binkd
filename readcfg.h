/*
 *  readcfg.h -- read config
 *
 *  readcfg.h is a part of binkd project
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
 * Revision 2.32  2004/10/25 17:04:54  gul
 * Process passwords file after all, independent of its place in config.
 * Use first password for node if several specified.
 *
 * Revision 2.31  2004/09/06 10:47:06  val
 * bandwidth limiting code advancements, `listed' session state fix
 *
 * Revision 2.30  2004/09/02 08:56:20  val
 * bandwidth limiting config parameter 'limit-rate'
 *
 * Revision 2.29  2004/01/07 12:23:40  gul
 * Remove zaccept keyword, receiving compressed files possibility
 * is always on now if binkd was compiled with zlib/bzip2 support.
 *
 * Revision 2.28  2003/12/26 20:11:32  gul
 * Add -d commandline switch - dump parsed config and exit;
 * remove 'debugcfg' config token.
 *
 * Revision 2.27  2003/10/29 21:08:39  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.26  2003/10/19 12:21:47  gul
 * Stream compression
 *
 * Revision 2.25  2003/10/14 07:20:40  gul
 * Fixed typo
 *
 * Revision 2.24  2003/09/24 07:32:17  val
 * bzlib2 compression support, new compression keyword: zlevel
 *
 * Revision 2.23  2003/09/15 06:57:09  val
 * compression support via zlib: config keywords, improvements, OS/2 code
 *
 * Revision 2.22  2003/09/12 07:37:58  val
 * compression support via zlib (preliminary)
 *
 * Revision 2.21  2003/09/08 16:39:39  stream
 * Fixed race conditions when accessing array of nodes in threaded environment
 * ("jumpimg node structures")
 *
 * Revision 2.20  2003/08/26 16:06:27  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.19  2003/08/23 15:51:51  stream
 * Implemented common list routines for all linked records in configuration
 *
 * Revision 2.18  2003/08/18 07:35:09  val
 * multiple changes:
 * - hide-aka/present-aka logic
 * - address mask matching via pmatch
 * - delay_ADR in STATE (define DELAY_ADR removed)
 * - ftnaddress_to_str changed to xftnaddress_to_str (old version #define'd)
 * - parse_ftnaddress now sets zone to domain default if it's omitted
 *
 * Revision 2.17  2003/08/14 07:39:36  val
 * migrate from vfprintf() to vsnprintf() in Log(), new keyword `nolog'
 *
 * Revision 2.16  2003/08/11 08:33:16  val
 * better error handling in perl hooks
 *
 * Revision 2.15  2003/07/30 11:01:37  val
 * perl-dll keyword can be used even when PERLDL is not defined (does nothing)
 *
 * Revision 2.14  2003/07/28 10:23:33  val
 * Perl DLL dynamic load for Win32, config keyword perl-dll, nmake PERLDL=1
 *
 * Revision 2.13  2003/07/07 08:33:25  val
 * `perl-hooks' config keyword to specify perl script
 *
 * Revision 2.12  2003/06/12 08:30:57  val
 * check pkt header feature, see keyword 'check-pkthdr'
 *
 * Revision 2.11  2003/06/12 08:21:43  val
 * 'skipmask' is replaced with 'skip', which allows more skipping features
 *
 * Revision 2.10  2003/06/07 08:46:25  gul
 * New feature added: shared aka
 *
 * Revision 2.9  2003/03/10 10:57:45  gul
 * Extern declarations moved to header files
 *
 * Revision 2.8  2003/03/10 10:39:23  gul
 * New include file common.h
 *
 * Revision 2.7  2003/03/01 18:52:49  gul
 * use time_t for mtime
 *
 * Revision 2.6  2003/03/01 15:00:17  gul
 * Join skipmask and overwrite into common maskchain
 *
 * Revision 2.5  2003/02/22 21:32:46  gul
 * Amiga Style Outbound support
 *
 * Revision 2.4  2003/02/22 14:30:18  gul
 * Make nNod var static
 *
 * Revision 2.3  2002/05/11 08:37:32  gul
 * Added token deletedirs
 *
 * Revision 2.2  2002/05/06 19:25:40  gul
 * new keyword inboundCase in config
 *
 * Revision 2.1  2001/08/24 13:23:28  da
 * binkd/binkd.c
 * binkd/readcfg.c
 * binkd/readcfg.h
 * binkd/server.c
 * binkd/nt/service.c
 *
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 * Revision 1.7  1997/10/23  03:44:03  mff
 * +fdinhist, +fdouthist, +root_domain
 *
 * 1997/07/11  11:51:17  maxb
 * Added fdinhist and fdouthist keywords.
 *
 * Revision 1.6  1997/06/16  05:42:30  mff
 * Added binlog and tzoff keywords.
 *
 * Revision 1.5  1997/03/28  06:36:28  mff
 * Added "exec" keyword
 */
#ifndef _readcfg_h
#define _readcfg_h

#include <stdio.h>

#include "Config.h"
#include "btypes.h"
#include "iphdr.h"

typedef struct _BINKD_CONFIG BINKD_CONFIG;

#define MAXINCLUDELEVEL 8
#define MAXCFGLINE 1024

/* val: enum for checks */
typedef enum { A_ALL=-1, A_LST=1, A_UNLST=2, A_PROT=4, A_UNPROT=8 } addrtype;

/* val: use for overwrite and nolog */
struct maskchain
{
  struct maskchain *next;
  char *mask;
};
/* val: struct for skipmask */
struct skipchain
{
  struct skipchain *next;
  char *mask;
  addrtype atype;
  off_t size;
  int destr;
};
/* val: struct for hide-aka, present-aka */
struct akachain
{
  struct akachain *next;
  FTN_ADDR fa;
  char *mask;
  enum { ACT_UNKNOWN=0, ACT_HIDE, ACT_PRESENT } type;
};
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
/* val: struct for zallow, zdeny */
struct zrule
{
  struct zrule *next;
  char *mask;
  enum { ZRULE_ALLOW, ZRULE_DENY } type;
};
#endif
#ifdef BW_LIM
/* val: struct for limit-rate */
struct ratechain
{
  struct ratechain *next;
  char *mask;
  addrtype atype;
  long rate;
};
#endif

struct _BINKD_CONFIG
{
  int        usageCount;               /* when it reaches zero, config can be freed */

  int        nAddr;          /* total addresses defined */
  FTN_ADDR  *pAddr;          /* array of adresses */

  int        nNod;           /* number of nodes */
  FTN_NODE   **pNodArray;    /* array of pointers to nodes  */
  int        nNodSorted;     /* internal flag   */

  int        iport;
  int        oport;
  int        oblksize;
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
  int        zminsize;
  int        zlevel;
#endif
  int        nettimeout;
  int        connect_timeout;
  int        rescan_delay;
  int        call_delay;
  int        max_servers;
  int        max_clients;
  int        kill_dup_partial_files;
  int        kill_old_partial_files;
  int        kill_old_bsy;
  int        minfree;
  int        minfree_nonsecure;
  int        tries;
  int        hold;
  int        hold_skipped;
  int        backresolv;
  int        send_if_pwd;
  int        debugcfg;
  int        loglevel;
  int        conlog;
  int        printq;
  int        percents;
  int        tzoff;
  int        prescan;
  enum inbcasetype inboundcase;
  int        deletedirs;
  int        havedefnode;
#ifdef AMIGADOS_4D_OUTBOUND
  int        aso;
#endif
  addrtype   pkthdr_type;
  char      *pkthdr_bad;

  DEFINE_LIST(conflist_type) config_list;
  DEFINE_LIST(_FTN_DOMAIN)   pDomains;
  DEFINE_LIST(maskchain)     overwrite, nolog;
  DEFINE_LIST(skipchain)     skipmask;
  DEFINE_LIST(_RF_RULE)      rf_rules;
  DEFINE_LIST(_EVT_FLAG)     evt_flags;
  DEFINE_LIST(akachain)      akamask;
  DEFINE_LIST(_SHARED_CHAIN) shares; /* Linked list for shared akas header */
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
  DEFINE_LIST(zrule)         zrules;
#endif
#ifdef BW_LIM
  DEFINE_LIST(ratechain)     rates;
#endif

  /*
   #ifdef HTTPS
   struct simplelistheader  proxylist;
   #endif
   */

#ifdef HTTPS
  char       proxy[MAXHOSTNAMELEN + 40];
  char       socks[MAXHOSTNAMELEN + 40];
#endif

  char       root_domain[MAXHOSTNAMELEN + 1];
  char       sysname[MAXSYSTEMNAME + 1];
  char       bindaddr[16];
  char       sysop[MAXSYSOPNAME + 1];
  char       location[MAXLOCATIONNAME + 1];
  char       nodeinfo[MAXNODEINFO + 1];
  char       inbound[MAXPATHLEN + 1];
  char       inbound_nonsecure[MAXPATHLEN + 1];
  char       temp_inbound[MAXPATHLEN + 1];
  char       logpath[MAXPATHLEN + 1];
  char       binlogpath[MAXPATHLEN + 1];
  char       fdinhist[MAXPATHLEN + 1];
  char       fdouthist[MAXPATHLEN + 1];
  char       pid_file[MAXPATHLEN + 1];
  char       passwords[MAXPATHLEN + 1];
#ifdef MAILBOX
  char       tfilebox[MAXPATHLEN + 1];   /* FileBoxes dir */
  char       bfilebox[MAXPATHLEN + 1];   /* BrakeBoxes dir */
  int        deleteablebox;
#endif

#ifdef WITH_PERL
  char       perl_script[MAXPATHLEN + 1];
  char       perl_dll[MAXPATHLEN + 1];
  int        perl_strict;
#endif

};

extern BINKD_CONFIG  *current_config;

#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)
extern int syslog_facility;
#endif

/*
 * Parses and reads the path as a config
 * Return 1 if config has been loaded Ok
 */
int readcfg (char *path);

/*
 * Checking for changed config files and reloading if necessary.
 * Return 1 if config has been reloaded Ok
 */
int checkcfg (void);

/*
 * Dump parsed config
 */
void debug_readcfg (void);

/*
 * Locks current config structure and return pointer to it in one
 * thread-safe operation
 */
BINKD_CONFIG *lock_current_config(void);

/*
 * Increment lock counter on specific config
 */
void lock_config_structure(BINKD_CONFIG *c);

/*
 * Release config structure after usage
 */
void unlock_config_structure(BINKD_CONFIG *c);

/*
 * Lists
 */

void simplelist_add(struct list_linkpoint *lp, void *data, int size);
void simplelist_free(struct list_linkpoint *lp, void (*destructor)(void *));

/*
 * Popular destructors
 */
void destroy_maskchain(void *p);

int  get_host_and_port (int n, char *host, unsigned short *port, char *src, FTN_ADDR *fa, BINKD_CONFIG *config);

char *mask_test(char *s, struct maskchain *chain);

#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
struct zrule *zrule_test(int type, char *s, struct zrule *root);
#endif

#ifdef BW_LIM
long parse_rate (char *w, char **err);
char *describe_rate(long rate);
#endif

#endif
