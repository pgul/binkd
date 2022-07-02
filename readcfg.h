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
  boff_t size;
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
struct listenchain
{
  struct listenchain *next;
  char addr[42];
  char port[MAXSERVNAME + 1];
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

struct conflist_type
{
  struct conflist_type *next;
  char                 *path;
  time_t                mtime;
};

struct perl_var
{
  struct perl_var *next;
  char            *name, *val;
};

struct _BINKD_CONFIG
{
  int        usageCount;               /* when it reaches zero, config can be freed */

  int        nAddr;          /* total addresses defined */
  FTN_ADDR  *pAddr;          /* array of adresses */

  int        nNod;           /* number of nodes */
  FTN_NODE   **pNodArray;    /* array of pointers to nodes  */
  int        nNodSorted;     /* internal flag   */
  int        q_present;      /* BSO scan: queue not empty */

  char       iport[MAXSERVNAME + 1];
  char       oport[MAXSERVNAME + 1];
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
  int        remove_try_files;
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
  enum dontsendemptytype dontsendempty;
  enum renamestyletype   renamestyle;
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
  DEFINE_LIST(listenchain)   listen;
  DEFINE_LIST(_SHARED_CHAIN) shares; /* Linked list for shared akas header */
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
  DEFINE_LIST(zrule)         zrules;
#endif
#ifdef BW_LIM
  DEFINE_LIST(ratechain)     rates;
#endif
#ifdef WITH_PERL
  DEFINE_LIST(perl_var)      perl_vars;
#endif

  /*
   #ifdef HTTPS
   struct simplelistheader  proxylist;
   #endif
   */

#ifdef HTTPS
  char       proxy[BINKD_FQDNLEN + 40];
  char       socks[BINKD_FQDNLEN + 40];
#endif

  char       root_domain[BINKD_FQDNLEN + 1];
  char       sysname[MAXSYSTEMNAME + 1];
  char       bindaddr[42];
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
#if defined (WITH_ZLIB) && defined (ZLIBDL)
  char       zlib_dll[MAXPATHLEN + 1];
#endif
#if defined (WITH_BZLIB2) && defined (ZLIBDL)
  char       bzlib2_dll[MAXPATHLEN + 1];
#endif

#ifdef WITH_PERL
  char       perl_script[MAXPATHLEN + 1];
  char       perl_dll[MAXPATHLEN + 1];
  int        perl_strict;
  void       *perl;
  int        perl_ok;
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
BINKD_CONFIG *readcfg (char *path);

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
void unlock_config_structure(BINKD_CONFIG *c, int on_exit);

/*
 * Lists
 */

void simplelist_add(struct list_linkpoint *lp, void *data, int size);
void simplelist_free(struct list_linkpoint *lp, void (*destructor)(void *));

/*
 * Popular destructors
 */
void destroy_maskchain(void *p);

int  get_host_and_port (int n, char *host, char *port, char *src, FTN_ADDR *fa, BINKD_CONFIG *config);

char *mask_test(char *s, struct maskchain *chain);

#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
struct zrule *zrule_test(int type, char *s, struct zrule *root);
#endif

#ifdef BW_LIM
long parse_rate (char *w, char **err);
char *describe_rate(long rate);
#endif

#endif
