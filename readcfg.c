/*
 *  readcfg.c -- reads config
 *
 *  readcfg.c is a part of binkd project
 *
 *  Copyright (C) 1996-2003  Dima Maloff, 5047/13 and others
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
 * Revision 2.40  2003/08/18 17:19:13  stream
 * Partially implemented new configuration parser logic (required for config reload)
 *
 * Revision 2.39  2003/08/18 07:35:08  val
 * multiple changes:
 * - hide-aka/present-aka logic
 * - address mask matching via pmatch
 * - delay_ADR in STATE (define DELAY_ADR removed)
 * - ftnaddress_to_str changed to xftnaddress_to_str (old version #define'd)
 * - parse_ftnaddress now sets zone to domain default if it's omitted
 *
 * Revision 2.38  2003/08/16 09:47:25  gul
 * Autodetect tzoff if not specified
 *
 * Revision 2.37  2003/08/14 07:39:36  val
 * migrate from vfprintf() to vsnprintf() in Log(), new keyword `nolog'
 *
 * Revision 2.36  2003/08/12 09:31:46  val
 * don't strlower() mask in flag/exec since we now use pmatch_ncase()
 *
 * Revision 2.35  2003/08/12 09:23:00  val
 * migrate from pmatch() to pmatch_ncase()
 *
 * Revision 2.34  2003/08/11 08:33:16  val
 * better error handling in perl hooks
 *
 * Revision 2.33  2003/07/30 11:01:37  val
 * perl-dll keyword can be used even when PERLDL is not defined (does nothing)
 *
 * Revision 2.32  2003/07/28 10:23:33  val
 * Perl DLL dynamic load for Win32, config keyword perl-dll, nmake PERLDL=1
 *
 * Revision 2.31  2003/07/07 08:33:25  val
 * `perl-hooks' config keyword to specify perl script
 *
 * Revision 2.30  2003/06/30 22:48:36  hbrew
 * Allow to override -ip, -sip, -md, -nomd in add_node()
 *
 * Revision 2.29  2003/06/12 08:30:57  val
 * check pkt header feature, see keyword 'check-pkthdr'
 *
 * Revision 2.28  2003/06/12 08:21:43  val
 * 'skipmask' is replaced with 'skip', which allows more skipping features
 *
 * Revision 2.27  2003/06/07 08:46:25  gul
 * New feature added: shared aka
 *
 * Revision 2.26  2003/05/28 09:03:17  gul
 * Typo in prev patch
 *
 * Revision 2.25  2003/05/28 08:56:33  gul
 * Reread config if passwords file changed when -C switch specified
 *
 * Revision 2.24  2003/05/01 09:55:01  gul
 * Remove -crypt option, add global -r option (disable crypt).
 *
 * Revision 2.23  2003/03/25 13:17:53  gul
 * Check if inbound and temp-inbound are in the same partition
 *
 * Revision 2.22  2003/03/11 00:04:26  gul
 * Use patches for compile under MSDOS by MSC 6.0 with IBMTCPIP
 *
 * Revision 2.21  2003/03/10 10:57:45  gul
 * Extern declarations moved to header files
 *
 * Revision 2.20  2003/03/10 10:39:23  gul
 * New include file common.h
 *
 * Revision 2.19  2003/03/01 15:00:17  gul
 * Join skipmask and overwrite into common maskchain
 *
 * Revision 2.18  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.17  2003/02/23 16:31:21  gul
 * Add "-sip" option in node string.
 * Change "-ip" check logic.
 *
 * Revision 2.16  2003/02/22 21:32:46  gul
 * Amiga Style Outbound support
 *
 * Revision 2.15  2003/02/22 20:19:54  gul
 * Update copyrightes, 2002->2003
 *
 * Revision 2.14  2003/02/13 19:18:11  gul
 * minor fix
 *
 * Revision 2.13  2003/01/29 19:32:03  gul
 * Code cleanup, prevent segfault on bad config
 *
 * Revision 2.12  2003/01/16 14:34:11  gul
 * Fix segfault under unix
 *
 * Revision 2.11  2002/12/17 14:02:22  gul
 * change strcasecmp -> STRICMP
 *
 * Revision 2.10  2002/12/17 13:00:44  gul
 * Fix previous patch
 *
 * Revision 2.9  2002/12/10 21:31:30  gul
 * Bugfix for check filebox and outbound
 *
 * Revision 2.8  2002/11/14 13:01:43  gul
 * Bugfix for previous patch
 *
 * Revision 2.7  2002/11/12 17:41:02  gul
 * Check for (personal) outbox pointed to (common) outbound
 *
 * Revision 2.6  2002/07/21 10:35:44  gul
 * overwrite option
 *
 * Revision 2.5  2002/05/11 08:37:32  gul
 * Added token deletedirs
 *
 * Revision 2.4  2002/05/06 19:25:39  gul
 * new keyword inboundCase in config
 *
 * Revision 2.3  2002/02/22 00:18:34  gul
 * Run by-file events with the same command-line once after session
 *
 * Revision 2.2  2001/08/24 13:23:28  da
 * binkd/binkd.c
 * binkd/readcfg.c
 * binkd/readcfg.h
 * binkd/server.c
 * binkd/nt/service.c
 *
 * Revision 2.1  2001/02/15 11:03:18  gul
 * Added crypt traffic possibility
 *
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 * Revision 1.14  1997/10/23  03:45:34  mff
 * +fdinhist, +fdouthist, +root_domain, many fixes to hide pNod into
 * ftnnode.c
 *
 * Revision 1.13  1997/09/04  02:53:01  mff
 * Added fdinhist/fdouthist keywords to support FrontDoor-style history.
 * Added support for multiple hosts per node. Find_port() moved to
 * iptools.c
 *
 * Revision 1.12  1997/08/19  21:42:29  mff
 * Changes to support multiple hosts per node: in FTN_NODE
 * host/port pair replaced with asciiz string in ``hosts''
 *
 *
 * 1997/07/11  11:47:55  maxb
 * Added fdinhist and fdouthist keyword
 *
 * Revision 1.11  1997/06/16  05:42:30  mff
 * Added binlog and tzoff keywords.
 *
 * Revision 1.10  1997/05/17  08:43:23  mff
 * Flavours for fileboxes were ignored
 *
 * Revision 1.9  1997/03/28  06:36:28  mff
 * Added "exec" keyword
 *
 * Revision 1.8  1997/03/15  05:06:08  mff
 * Added -nr key to node statement
 *
 * Revision 1.6  1997/03/09  07:13:30  mff
 * getservbyname with iport/oport, added reading of syslog facility
 *
 * Revision 1.5  1997/02/07  06:55:11  mff
 * `include', extened `node', more?
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)
#define SYSLOG_NAMES
#include <syslog.h>
#endif

#include "Config.h"
#include "common.h"
#include "readcfg.h"
#include "tools.h"
#include "ftnaddr.h"
#include "ftnq.h"
#include "srif.h"
#include "iphdr.h"
#include "iptools.h"
#include "assert.h"
#include "readflo.h"

static char *current_path = "<command line>";
static int   current_line;
static char  linebuf[MAXCFGLINE + 1];
static char  spaces[] = " \n\t";

#define UNUSED_ARG(s)  (void)(s)

char siport[MAXSERVNAME + 1] = "";
char soport[MAXSERVNAME + 1] = "";
int havedefnode=0;
int iport = 0;
int oport = 0;
int call_delay = 60;
int rescan_delay = 60;
int nettimeout = DEF_TIMEOUT;
int oblksize = DEF_BLKSIZE;
int max_servers = 100;
int max_clients = 100;
int kill_dup_partial_files = 0;
int kill_old_partial_files = 0;
int kill_old_bsy = 0;
int percents = 0;
int minfree = -1;
int minfree_nonsecure = -1;
int debugcfg = 0;
int printq = 0;
int backresolv = 0;
char sysname[MAXSYSTEMNAME + 1] = "";
char sysop[MAXSYSOPNAME + 1] = "";
char location[MAXLOCATIONNAME + 1] = "";
char nodeinfo[MAXNODEINFO + 1] = "";
char inbound[MAXPATHLEN + 1] = ".";
char inbound_nonsecure[MAXPATHLEN + 1] = "";
char temp_inbound[MAXPATHLEN + 1] = "";

/* This is header of shared aka list */
SHARED_CHAIN * shares = 0;

#ifdef MAILBOX
/* FileBoxes dir */
char tfilebox[MAXPATHLEN + 1] = "";
/* BrakeBoxes dir */
char bfilebox[MAXPATHLEN + 1] = "";
int  deleteablebox = 0;
#endif
int  deletedirs = 0;
char logpath[MAXPATHLEN + 1] = "";
char binlogpath[MAXPATHLEN + 1] = "";
char fdinhist[MAXPATHLEN + 1] = "";
char fdouthist[MAXPATHLEN + 1] = "";
char pid_file[MAXPATHLEN + 1] = "";
#ifdef HTTPS
char proxy[MAXHOSTNAMELEN + 40] = "";
char socks[MAXHOSTNAMELEN + 40] = "";
#endif
char bindaddr[16] = "";
int loglevel = 4;
int conlog = 1;
int send_if_pwd = 0;
int tzoff = -1; /* autodetect */
char root_domain[MAXHOSTNAMELEN + 1] = "fidonet.net.";
int prescan = 0;
enum inbcasetype inboundcase = INB_SAVE;
int connect_timeout = 0;
struct conflist_type *config_list = NULL;
addrtype pkthdr_type = 0;
char *pkthdr_bad = NULL;
#ifdef AMIGADOS_4D_OUTBOUND
int aso = 0;
#endif
#ifdef WITH_PERL
char perl_script[MAXPATHLEN + 1] = "";
char perl_dll[MAXPATHLEN + 1] = "";
int perl_strict = 0;
#endif
struct maskchain *nolog = NULL;
struct akachain *akamask = NULL;

#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)

int syslog_facility = -1;

#endif

int tries = 0;
int hold = 0;
int hold_skipped = 60 * 60;
struct maskchain *overwrite = NULL;
struct skipchain *skipmask = NULL;

int nAddr = 0;
FTN_ADDR *pAddr = 0;

typedef struct _KEYWORD KEYWORD;
struct _KEYWORD
{
  const char *key;
  int  (*callback) (KEYWORD *key, int wordcount, char **words);
  void *var;
  long option1;
  long option2;
};

static int passwords (KEYWORD *key, int wordcount, char **words);
static int include (KEYWORD *key, int wordcount, char **words);
static int read_aka_list (KEYWORD *key, int wordcount, char **words);
static int read_domain_info (KEYWORD *key, int wordcount, char **words);
static int read_node_info (KEYWORD *key, int wordcount, char **words);
static int read_int (KEYWORD *key, int wordcount, char **words);
static int read_string (KEYWORD *key, int wordcount, char **words);
static int read_bool (KEYWORD *key, int wordcount, char **words);
static int read_flag_exec_info (KEYWORD *key, int wordcount, char **words);
static int read_rfrule (KEYWORD *key, int wordcount, char **words);
static int read_mask (KEYWORD *key, int wordcount, char **words);
static int read_akamask (KEYWORD *key, int wordcount, char **words);
static int read_inboundcase (KEYWORD *key, int wordcount, char **words);
static int read_port (KEYWORD *key, int wordcount, char **words);
static int read_skip (KEYWORD *key, int wordcount, char **words);
static int read_check_pkthdr (KEYWORD *key, int wordcount, char **words);

/* Helper functions for shared akas implementation */
static int read_shares (KEYWORD *key, int wordcount, char **words);
static void add_shares(SHARED_CHAIN * chain);
static void add_address(SHARED_CHAIN * chain, FTN_ADDR_CHAIN * aka);

#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)

static void read_syslog_facility (KEYWORD *, char *);

#endif

#define DONT_CHECK 0x7fffffffl

KEYWORD keywords[] =
{
  {"passwords", passwords, NULL, 0, 0},
  {"include", include, NULL, 0, 0},
  {"log", read_string, logpath, 'f', 0},
  {"loglevel", read_int, &loglevel, 0, DONT_CHECK},
  {"conlog", read_int, &conlog, 0, DONT_CHECK},
  {"binlog", read_string, binlogpath, 'f', 0},
  {"fdinhist", read_string, fdinhist, 'f', 0},
  {"fdouthist", read_string, fdouthist, 'f', 0},
  {"tzoff", read_int, &tzoff, DONT_CHECK, DONT_CHECK},
  {"domain", read_domain_info, NULL, 0, 0},
  {"address", read_aka_list, NULL, 0, 0},
  {"sysname", read_string, sysname, 0, MAXSYSTEMNAME},
  {"bindaddr", read_string, bindaddr, 0, 16},
  {"sysop", read_string, sysop, 0, MAXSYSOPNAME},
  {"location", read_string, location, 0, MAXLOCATIONNAME},
  {"nodeinfo", read_string, nodeinfo, 0, MAXNODEINFO},
  {"iport", read_port, &iport, 0, MAXSERVNAME},
  {"oport", read_port, &oport, 0, MAXSERVNAME},
  {"rescan-delay", read_int, &rescan_delay, 1, DONT_CHECK},
  {"call-delay", read_int, &call_delay, 1, DONT_CHECK},
  {"timeout", read_int, &nettimeout, 1, DONT_CHECK},
  {"oblksize", read_int, &oblksize, MIN_BLKSIZE, MAX_BLKSIZE},
  {"maxservers", read_int, &max_servers, 0, DONT_CHECK},
  {"maxclients", read_int, &max_clients, 0, DONT_CHECK},
  {"inbound", read_string, inbound, 'd', 0},
  {"inbound-nonsecure", read_string, inbound_nonsecure, 'd', 0},
  {"temp-inbound", read_string, temp_inbound, 'd', 0},
  {"node", read_node_info, NULL, 0, 0},
  {"defnode", read_node_info, NULL, 1, 0},
  {"kill-dup-partial-files", read_bool, &kill_dup_partial_files, 0, 0},
  {"kill-old-partial-files", read_int, &kill_old_partial_files, 1, DONT_CHECK},
  {"kill-old-bsy", read_int, &kill_old_bsy, 1, DONT_CHECK},
  {"percents", read_bool, &percents, 0, 0},
  {"minfree", read_int, &minfree, 0, DONT_CHECK},
  {"minfree-nonsecure", read_int, &minfree_nonsecure, 0, DONT_CHECK},
  {"flag", read_flag_exec_info, NULL, 'f', 0},
  {"exec", read_flag_exec_info, NULL, 'e', 0},
  {"debugcfg", read_bool, &debugcfg, 0, 0},
  {"printq", read_bool, &printq, 0, 0},
  {"try", read_int, &tries, 0, 0xffff},
  {"hold", read_int, &hold, 0, DONT_CHECK},
  {"hold-skipped", read_int, &hold_skipped, 0, DONT_CHECK},
  {"backresolv", read_bool, &backresolv, 0, 0},
  {"pid-file", read_string, pid_file, 'f', 0},
#ifdef HTTPS
  {"proxy", read_string, proxy, 0, MAXHOSTNAMELEN + 40},
  {"socks", read_string, socks, 0, MAXHOSTNAMELEN + 40},
#endif
#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)
  {"syslog", read_syslog_facility, &syslog_facility, 0, 0},
#endif
  {"ftrans", read_rfrule, NULL, 0, 0},
  {"send-if-pwd", read_bool, &send_if_pwd, 0, 0},
  {"root-domain", read_string, root_domain, 0, MAXHOSTNAMELEN},
  {"prescan", read_bool, &prescan, 0, 0},
  {"connect-timeout", read_int, &connect_timeout, 0, DONT_CHECK},
#ifdef MAILBOX
  {"filebox", read_string, tfilebox, 'd', 0},
  {"brakebox", read_string, bfilebox, 'd', 0},
  {"deletebox", read_bool, &deleteablebox, 0, 0},
#endif
  {"skip", read_skip, NULL, 0, 0},
  {"inboundcase", read_inboundcase, &inboundcase, 0, 0},
  {"deletedirs", read_bool, &deletedirs, 0, 0},
  {"overwrite", read_mask, &overwrite, 0, 0},

  /* shared akas definitions */
  {"share", read_shares, 0, 0, 0},
  /* check pkt header keyword */
  {"check-pkthdr", read_check_pkthdr, NULL, 0, 0},

#ifdef AMIGADOS_4D_OUTBOUND
  {"aso", read_bool, &aso, 0, 0},
#endif

#ifdef WITH_PERL
  {"perl-hooks", read_string, perl_script, 'f', 0},
  {"perl-dll", read_string, perl_dll, 'f', 0},
  {"perl-strict", read_bool, &perl_strict, 0, 0},
#endif

  {"nolog", read_mask, &nolog, 0, 0},
  {"hide-aka", read_akamask, &akamask, ACT_HIDE, 0},
  {"present-aka", read_akamask, &akamask, ACT_PRESENT, 0},
  {NULL, NULL, NULL, 0, 0}
};

static void debug_readcfg(void);

/*
 * Zero config data and set default values of other variables
 * Note: must be called on locked config
 */
void lock_config_structure(/*struct config_data *c*/)
{
//  if (++(c->usageCount) == 1)
  {
    /* First-time call: init default values */

    /*c->*/iport             = DEF_PORT;
    /*c->*/oport             = DEF_PORT;
    /*c->*/call_delay        = 60;
    /*c->*/rescan_delay      = 60;
    /*c->*/nettimeout        = DEF_TIMEOUT;
    /*c->*/oblksize          = DEF_BLKSIZE;
    /*c->*/max_servers       = 100;
    /*c->*/max_clients       = 100;
    /*c->*/minfree           = -1;
    /*c->*/minfree_nonsecure = -1;
    /*c->*/loglevel          = 4;
    /*c->*/inboundcase       = INB_SAVE;
    /*c->*/hold_skipped      = 60 * 60;

    strcpy(/*c->*/inbound, ".");
    strcpy(/*c->*/root_domain, "fidonet.net.");
  }
}

/* Check for (personal) outbox pointed to (common) outbound */
static int check_outbox(char *obox)
{
  FTN_DOMAIN *pd;
#ifndef UNIX
  char *OBOX, *PATH=NULL;
#endif
  if (obox == NULL)
    return 0;
#ifndef UNIX
  OBOX = strupper(xstrdup(obox));
#endif
  for (pd = pDomains; pd; pd=pd->next)
  {
    if (pd->alias4)
      continue;
    if (pd->path)
    {
      char *s;
#ifdef UNIX
      if (obox==strstr(obox, pd->path))
      {
        s = obox+strlen(pd->path);
        if ((*s == '\\' || *s == '/') && STRICMP(s+1, pd->dir) == 0)
          return 1;
      }
#else
      PATH = strupper(xstrdup(pd->path));
      if (OBOX==strstr(OBOX, PATH))
      {
        s = OBOX+strlen(PATH);
        if ((*s == '\\' || *s == '/') && STRICMP(s+1, pd->dir) == 0)
        {
          free(PATH);
          free(OBOX);
          return 1;
        }
      }
      free(PATH);
#endif
    }
  }
#ifndef UNIX
  free(OBOX);
#endif
  return 0;
}

static int ConfigError(char *format, ...)
{
#define MAX_CONFIGERROR_PARAMS 6

  va_list args;
  int     data[MAX_CONFIGERROR_PARAMS];
  int     i;

  va_start(args, format);
  for (i = 0; i < MAX_CONFIGERROR_PARAMS; i++)
    data[i] = va_arg(args, int);
  Log(1, "%s: line %d: error in configuration files", current_path, current_line);
  Log(1, format, data[0], data[1], data[2], data[3], data[4], data[5]);
  va_end(args);
  return 0;
}

static int ConfigNeedNumber(char *s)
{
  return ConfigError("%s: expecting a number", s);
}

static int isDefined(char *value, char *name)
{
  if (*value == 0)
    return ConfigError("'%s' must be defined", name);
  return 1;
}

static int check_boxes(FTN_NODE *node, void *arg)
{
  struct stat st;
  char addr[FTN_ADDR_SZ + 1];

  ftnaddress_to_str(addr, &(node->fa));
  if (node->obox && node->obox[0])
  {
    if (stat(node->obox, &st) || (st.st_mode & S_IFDIR) == 0)
    {
      ConfigError("Outbox for %s does not exist (link %s)", node->obox, addr);
      return 1;
    }
    if (check_outbox(node->obox))
    {
      ConfigError("Outbox cannot point to outbound! (link %s)", addr);
      return 1;
    }
  }
  if (node->ibox && node->ibox[0])
  {
    if (stat(node->ibox, &st) || (st.st_mode & S_IFDIR) == 0)
    {
      ConfigError("Inbox for %s does not exist (link %s)", node->ibox, addr);
      return 1;
    }
    if (arg && st.st_dev != *(dev_t *)arg)
    {
      ConfigError("Inbox and temp-inbound must be in the same partition (link %s)", addr);
      return 1;
    }
  }
  return 0;
}

static int check_config(void)
{
  struct stat st, si;
  if (temp_inbound[0] && stat(temp_inbound, &st) == 0)
  {
    if (stat(inbound, &si) == 0 && st.st_dev != si.st_dev)
      return ConfigError("Inbound and temp-inbound must be in the same partition");
    if (stat(inbound_nonsecure, &si) == 0 && st.st_dev != si.st_dev)
      return ConfigError("Unsecure-inbound and temp-inbound must be in the same partition");
  }
  if (foreach_node(check_boxes, temp_inbound[0] ? &st.st_dev : NULL))
    return 0;
  return 1;
}

static void add_to_config_list(const char *path)
{
  struct conflist_type *pc;

  if (config_list)
  {
    for (pc = config_list; pc->next; pc = pc->next);
    pc->next = xalloc(sizeof(*pc));
    pc = pc->next;
  }
  else
  {
    config_list = xalloc(sizeof(*pc));
    pc = config_list;
  }
  pc->next = NULL;
  pc->path = xstrdup(path);
  pc->mtime = 0;
}

static int readcfg0 (char *path)
{
#define MAX_WORDS_ON_LINE 64

  FILE   *in;
  char   *words[MAX_WORDS_ON_LINE];
  int     success;

  if ((in = fopen (path, "r")) == 0)
    return ConfigError("cannot open %s: %s", path, strerror(errno));

  current_line = 0;
  current_path = path;

  add_to_config_list (path);

  for (success = 1; success && fgets (linebuf, sizeof (linebuf), in); )
  {
    int      wordcount;
    KEYWORD *k;

    ++current_line;

    /* Get array of all tokens on line */
    for (wordcount = 0; wordcount < MAX_WORDS_ON_LINE; wordcount++)
    {
      words[wordcount] = getword(linebuf, wordcount+1);
      if (words[wordcount] == NULL)
        break;
    }
    if (wordcount == MAX_WORDS_ON_LINE)
      ConfigError("warning: more then %d words on line will be ignored", MAX_WORDS_ON_LINE);

    if (wordcount != 0)
    {
      for (k = keywords; k->key; k++)
        if (!STRICMP (k->key, words[0]))
          break;
      if (k->key)
        success = k->callback(k, wordcount-1, words+1);
      else
        success = ConfigError("%s: unknown keyword", words[0]);
    }
    while (--wordcount > 0)
      free (words[wordcount]);
  }

  fclose (in);
  return success;
}

/*
 * Parses and reads _path as config.file
 */
void readcfg (char *_path)
{
  int  success = 0;

  // memset(&work_config, 0, sizeof(work_config));
  lock_config_structure(/* &work_config */);

  if (readcfg0 (_path)                 &&
      isDefined(sysname,  "sysname")   &&
      isDefined(sysop,    "sysop")     &&
      isDefined(location, "location")  &&
      isDefined(nodeinfo, "nodeinfo")
     )
  {
    do
    {
      if (!nAddr)
      {
        ConfigError("your address should be defined");
        break;
      }
      if (pDomains == 0)
      {
        ConfigError("at least one domain should be defined");
        break;
      }

      if (!*inbound_nonsecure)
        strcpy (inbound_nonsecure, inbound);

      if (!check_config())
        break;

      if (debugcfg)
        debug_readcfg ();

      /* All checks passed! */
      success = 1;

    } while (0);
  }

  if (!success)
  {
    /* Config error. Abort or continue? */
//    if (current_config == NULL)
      Log(0, "error in configuration, aborting");
//    else
//    {
//      Log(1, "error in configuration, using old config");
//      unlock_config_structure(&work_config);
//    }
  }
}

/*
 * Check number of arguments for command. Return 0 on error
 */
static int isArgCount(int expectedCount, int realCount)
{
  if (realCount != expectedCount)
    return ConfigError("%d argument(s) required", expectedCount);
  return 1;
}

static int SyntaxError(KEYWORD *k)
{
  return ConfigError("the syntax is incorrect for '%s'", k->key);
}

static void check_dir_path (char *s)
{
  if (s)
  {
    char *w = s + strlen (s) - 1;

    while (w >= s && (*w == '/' || *w == '\\'))
      *(w--) = 0;
  }
}

static int Config_ParseAddress(char *s, FTN_ADDR *a)
{
  if (!parse_ftnaddress (s, a /*, &work_config*/))
    return ConfigError("%s: the address cannot be parsed", s);
  return 1;
}


/*
 *  METHODS TO PROCESS KEYWORDS' ARGUMETS
 */

static int include (KEYWORD *key, int wordcount, char **words)
{
  static int level;

  char *old_path;
  int   old_line;
  int   success;

  UNUSED_ARG(key);

  if (!isArgCount(1, wordcount))
    return 0;

  if (level == MAXINCLUDELEVEL)
    return ConfigError("too many nested include commands");

  ++level;
  old_path = current_path;
  old_line = current_line;
  success  = readcfg0(words[0]);
  current_path = old_path;
  current_line = old_line;
  --level;

  return success;
}

static int passwords (KEYWORD *key, int wordcount, char **words)
{
  FILE *in;
  FTN_ADDR fa;

  UNUSED_ARG(key);

  if (!isArgCount(1, wordcount))
    return 0;

  if ((in = fopen(words[0], "rt")) == NULL)
    return ConfigError("unable to open password file (%s)", words[0]);

  add_to_config_list(words[0]);

  while (fgets (linebuf, sizeof (linebuf), in))
  {
    char *node, *password;

    node = strtok(linebuf, spaces);
    if (node)
    {
      password = strtok(NULL, spaces);
      if (password && parse_ftnaddress (node, &fa /*, &work_config */)) /* Do not process if any garbage found */
      {
        exp_ftnaddress (&fa /*, &work_config*/);
        add_node (&fa, NULL, password, '-', NULL, NULL,
                  NR_USE_OLD, ND_USE_OLD, MD_USE_OLD, RIP_USE_OLD, HC_USE_OLD /*, NP_USE_OLD, &work_config */);
      }
    }
  }
  fclose(in);

  return 1;
}

static int read_aka_list (KEYWORD *key, int wordcount, char **words)
{
  int       i;
  FTN_ADDR *a;

  if (wordcount == 0)
    return SyntaxError(key);

  for (i = 0; i < wordcount; i++)
  {
    pAddr = xrealloc (pAddr, sizeof (FTN_ADDR) * (nAddr+1));
    a = pAddr + nAddr;

    if (!Config_ParseAddress(words[i], a))
      return 0;
    if (!is4D (a))
      return ConfigError("%s: must be at least a 4D address", words[i]);
    if (a->domain[0] == 0)
    {
      //if (work_config.pDomains.first == NULL)
      if (!pDomains)
        return ConfigError("at least one domain must be defined first");
      strcpy (a->domain, get_def_domain (/*&work_config*/)->name);
    }
    ++nAddr;
  }

  return 1;
}

static int read_domain_info (KEYWORD *key, int wordcount, char **words)
{
  FTN_DOMAIN new_domain, *tmp_domain;

  UNUSED_ARG(key);

  if (!isArgCount(3, wordcount))
    return 0;

  if (get_domain_info (words[0] /*, &work_config*/))
    return ConfigError("%s: duplicate domain", words[0]);

  memset(&new_domain, 0, sizeof(new_domain));
  strnzcpy (new_domain.name, words[0], sizeof (new_domain.name));

  if (!STRICMP (words[1], "alias-for"))
  {
    if ((tmp_domain = get_domain_info (words[2] /*, &work_config*/)) == 0)
      return ConfigError("%s: undefined domain", words[2]);
    new_domain.alias4 = tmp_domain;
  }
  else
  {
    char *s, *new_dir, *new_path;
    int   z;

    if ((z = atoi (words[2])) <= 0)
      return ConfigError("%s: invalid zone", words[2]);

    new_domain.z[0] = z;
    new_domain.z[1] = 0;
    new_domain.alias4   = 0;

    check_dir_path(words[1]);
    if ((s = last_slash(words[1])) == 0)
    {
      new_dir  = words[1];
      new_path = ".";
    }
    else
    {
      *s = 0;
      new_dir  = s + 1;
      new_path = words[1];
      check_dir_path(new_path);
    }

    if (strchr (new_dir, '.'))
      return ConfigError("there should be no extension for the base outbound name");

    new_domain.dir  = xstrdup(new_dir);
    new_domain.path = xstrdup(new_path);
  }

  /*  simplelist_add(&work_config.pDomains, &new_domain, sizeof(new_domain)); */

  tmp_domain = xalloc(sizeof(FTN_DOMAIN));
  *tmp_domain = new_domain;
  tmp_domain->next = pDomains;
  pDomains = tmp_domain;

  return 1;
}

static int read_node_info (KEYWORD *key, int wordcount, char **words)
{
#define ARGNUM 6
  char *w[ARGNUM], *tmp;
  int   i, j;
  int   NR_flag = NR_USE_OLD, ND_flag = ND_USE_OLD, HC_flag = HC_USE_OLD,
        MD_flag = MD_USE_OLD, restrictIP = RIP_USE_OLD;
  // int   NP_flag = NP_USE_OLD;
  FTN_ADDR fa;

  memset(w, 0, sizeof(w)); /* init by NULL's */
  i = 0;                   /* index in w[] */

  if (key->option1) /* defnode */
  {
    w[i++] = "0:0/0.0@defnode";
    havedefnode = 1;
  }

  for (j = 0; j < wordcount; j++)
  {
    tmp = words[j];

    if (tmp[0] == '-' && tmp[1] != 0)
    {
      if (STRICMP (tmp, "-md") == 0)
        MD_flag = MD_ON;
      else if (STRICMP (tmp, "-nomd") == 0)
        MD_flag = MD_OFF;
      else if (STRICMP (tmp, "-nr") == 0)
        NR_flag = NR_ON;
      else if (STRICMP (tmp, "-nd") == 0)
      {
        NR_flag = NR_ON;
        ND_flag = ND_ON;
      }
      else if (STRICMP (tmp, "-ip") == 0)
        restrictIP = RIP_ON;  /* allow matched or unresolvable */
      else if (STRICMP (tmp, "-sip") == 0)
        restrictIP = RIP_SIP; /* allow only resolved and matched */
      else if (STRICMP (tmp, "-crypt") == 0)
        ConfigError("obsolete %s option ignored", tmp);
      else if (STRICMP (tmp, "-hc") == 0)
        HC_flag = HC_ON;
      else if (STRICMP (tmp, "-nohc") == 0)
        HC_flag = HC_OFF;
//      else if (STRICMP (tmp, "-noproxy") == 0)
//        NP_flag = NP_ON;
      else
        return ConfigError("%s: unknown option for `node' keyword", tmp);
    }
    else if (i >= ARGNUM)
      return ConfigError("too many argumets for `node' keyword");
    else
    {   /* '-' will place default NULL */
      if (tmp[0] == '-')
        tmp = NULL;
      w[i++] = tmp;
    }
  }

  if (w[0] == NULL)
    return ConfigError("missing node address");
  if (!Config_ParseAddress (w[0], &fa))
    return 0;
  exp_ftnaddress (&fa /*, &work_config */);

  if (w[2] && w[2][0] == 0)
    return ConfigError("empty password");
  if (w[3] && !isflvr (w[3][0]))
    return ConfigError("%s: incorrect flavour", w[3]);
  check_dir_path (w[4]);
  check_dir_path (w[5]);

  add_node (&fa, w[1], w[2], (char)(w[3] ? w[3][0] : '-'), w[4], w[5],
            NR_flag, ND_flag, MD_flag, restrictIP, HC_flag /*, NP_flag, &work_config */);

  return 1;
#undef ARGNUM
}

/*
 *  Gets hostname/portnumber for ``n''-th host in ``src'' string (1 ... N)
 *    <src> = <host> [ "," <src> ]
 *    <host> = "*"
 *    <host> = <hostname> [ ":" <service> ]
 *
 *  "*" will expand in corresponding domain name for ``fn''
 *                        (2:5047/13 --> "f13.n5047.z2.fidonet.net.")
 *
 *  ``Host'' should contain at least MAXHOSTNAMELEN bytes.
 *
 *  Returns 0 on error, -1 on EOF, 1 otherwise
 */
int get_host_and_port (int n, char *host, unsigned short *port, char *src, FTN_ADDR *fa)
{
  int rc = 0;
  char *s = getwordx2 (src, n, 0, ",;", "");

  if (s)
  {
    char *t = strchr (s, ':');

    if (t)
      *t = 0;

    if (!strcmp (s, "*"))
      ftnaddress_to_domain (host, fa);
    else
      strnzcpy (host, s, MAXHOSTNAMELEN);

    if (!t)
    {
      *port = oport;
      rc = 1;
    }
    else if ((*port = find_port (t + 1)) != 0)
      rc = 1;

    free (s);
  }
  else
    rc = -1;
  return rc;
}

/*
 * Read a string (key->option1 == 0)
 * or a directory name (key->option1 == 'd')
 * or a file name (key->option1 == 'f')
 */
static int read_string (KEYWORD *key, int wordcount, char **words)
{
  struct stat sb;
  char *target = (char *) (key->var);
  char *w;

  if (!isArgCount(1, wordcount))
    return 0;

  strnzcpy (target, words[0], key->option1 == 0 ? key->option2 : MAXPATHLEN);

  if (key->option1 != 0)
  {
    w = target + strlen (target) - 1;
    while (w >= target && (*w == '/' || *w == '\\'))
    {
      if (key->option1 == 'f')
        return ConfigError("unexpected `%c' at the end of filename", *w);
      *(w--) = 0;
    }
    if (key->option1 == 'd' && (stat (target, &sb) == -1 ||
                                !(sb.st_mode & S_IFDIR)))
    {
      return ConfigError("%s: incorrect directory", target);
    }
  }

  return 1;
}

static int read_int (KEYWORD *key, int wordcount, char **words)
{
  int  *target = (int *) (key->var);
  char *s;

  if (!isArgCount(1, wordcount))
    return 0;

  for (s = words[0]; *s; s++)
  {
    if (isdigit(*s) || (s == words[0] && *s == '-'))
      continue;
    return ConfigNeedNumber(words[0]);
  }

  *target = atoi (words[0]);

  if ((key->option1 != DONT_CHECK && *target < key->option1) ||
      (key->option2 != DONT_CHECK && *target > key->option2))
    return ConfigError("%i: incorrect value", *target);

  return 1;
}

static int read_bool (KEYWORD *key, int wordcount, char **words)
{
  if (wordcount != 0)
    return SyntaxError(key);

  (void) words;
  *(int *) (key->var) = 1;
  return 1;
}

static int read_inboundcase (KEYWORD *key, int wordcount, char **words)
{
  enum inbcasetype *target = (enum inbcasetype *) (key->var);

  if (!isArgCount(1, wordcount))
    return 0;

  if (!STRICMP (words[0], "save"))
    *target = INB_SAVE;
  else if (!STRICMP (words[0], "upper"))
    *target = INB_UPPER;
  else if (!STRICMP (words[0], "lower"))
    *target = INB_LOWER;
  else if (!STRICMP (words[0], "mixed"))
    *target = INB_MIXED;
  else
    return SyntaxError(key);

  return 1;
}

#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)
static int read_syslog_facility (KEYWORD *key, int wordcount, char **words)
{
  int *target = (int *) (key->var);
  int i;

  if (isArgCount(1, wordcount))
    return 0;

  for (i = 0; facilitynames[i].c_name; ++i))
    if (!strcmp (facilitynames[i].c_name, words[0]))
    {
      *target = facilitynames[i].c_val;
      return 1;
    }
  return ConfigError("%s: incorrect facility name", words[0]);
}
#endif

static int read_rfrule (KEYWORD *key, int wordcount, char **words)
{
  UNUSED_ARG(key);

  if (!isArgCount(2, wordcount))
    return 0;

  rf_rule_add(xstrdup(words[0]), xstrdup(words[1]));

  return 1;
}

static void mask_add(char *mask, struct maskchain **chain)
{
  struct maskchain *ps, *newmask;

  newmask = xalloc(sizeof(*newmask));
  newmask->next = NULL;
  newmask->mask = xstrdup(mask);

  if (*chain == NULL)
    *chain = newmask;
  else
  {
    for (ps = *chain; ps->next; ps = ps->next);
    ps->next = newmask;
  }
}

char *mask_test(char *s, struct maskchain *chain)
{
  struct maskchain *ps;

  for (ps = chain; ps; ps = ps->next)
    if (pmatch_ncase(ps->mask, s))
      return ps->mask;
  return NULL;
}

static int read_mask (KEYWORD *key, int wordcount, char **words)
{
  int i;

  if (wordcount == 0)
    return SyntaxError(key);

  for (i = 0; i < wordcount; i++)
    mask_add(words[i], (struct maskchain **) (key->var));

  return 1;
}

static int akamask_add(char *aka, char *mask, struct akachain **chain, int type)
{
  struct akachain *ps, *newmask;

  newmask = xalloc(sizeof(*newmask));

  newmask->next = NULL;
  if (!Config_ParseAddress(aka, &(newmask->fa)))
  {
    free(newmask);
    return 0;
  }
  exp_ftnaddress(&(newmask->fa));
  if (*mask == '!') { mask++; type |= 0x80; }
  newmask->type = type;
  newmask->mask = xstrdup(mask);

  if (*chain == NULL)
    *chain = newmask;
  else
  {
    for (ps = *chain; ps->next; ps = ps->next);
    ps->next = newmask;
  }

  return 1;
}

static int read_akamask (KEYWORD *key, int wordcount, char **words)
{
  if (!isArgCount(2, wordcount))
    return 0;

  return akamask_add(words[0], words[1], (struct akachain**)(key->var), key->option1);
}

static addrtype parse_addrtype(char *w)
{
  if (STRICMP (w, "all") == 0) return A_ALL;
  else if (STRICMP (w, "secure") == 0) return A_PROT;
  else if (STRICMP (w, "unsecure") == 0) return A_UNPROT;
  else if (STRICMP (w, "listed") == 0) return A_LST;
  else if (STRICMP (w, "unlisted") == 0) return A_UNLST;
  else return 0;
}

void skip_add(char *mask, off_t sz, addrtype at, int destr)
{
  struct skipchain *ps = skipmask;

  if (!skipmask) { ps = skipmask = xalloc(sizeof(*ps)); }
  else { while (ps->next) ps = ps->next; ps = ps->next = xalloc(sizeof(*ps)); }
  ps->next = NULL;
  ps->mask = xstrdup(mask);
  ps->size = sz > 0 ? sz << 10 : sz; /* to kilobytes if > 0 */
  ps->atype = at;
  ps->destr = destr;
}
/* skip [all|listed|unlisted|secure|unsecure] [!]<size>|- <mask>... */
static int read_skip (KEYWORD *key, int wordcount, char **words)
{
  int i, destr = 0, maskonly = 0;
  off_t sz = 0;
  addrtype at = A_ALL;

  for (i = 0; i < wordcount; i++)
  {
    char *w = words[i];

    if (i == 0 && isalpha(*w))
    {
      if ( !(at = parse_addrtype(w)) )
        return ConfigError("incorrect address type '%s'", w);
      continue;
    }
    if (i < 2 && maskonly == 0)
    {
      if (*w == '-' && w[1] == 0) { sz = -1; continue; }
      if (*w == '!') { destr = 1; w++; }
      if (!isdigit(*w))
        return ConfigNeedNumber(w);
      sz = atoi(w);
      maskonly = 1; /* size detected, only masks are allowed further */
      continue;
    }
    skip_add(w, sz, at, destr);
    maskonly = 2; /* at least one filemask present */
  }

  if (maskonly != 2)
    return ConfigError("expecting at least one filemask");

  return 1;
}
/* check-pkthdr [all|secure|unsecure|listed|unlisted] <ext> */
static int read_check_pkthdr (KEYWORD *key, int wordcount, char **words)
{
  char *w;

  if (wordcount == 0)
    return SyntaxError(key);

  w = words[0];
  if ((pkthdr_type = parse_addrtype(w)) != 0)
  {
    w = words[1];
    if (!isArgCount(2, wordcount))
      return 0;
  }
  else
  {
    pkthdr_type = A_ALL;
    if (!isArgCount(1, wordcount))
      return 0;
  }

  if (*w == '.')
    w++;
  pkthdr_bad = xstrdup(w);

  return 1;
}

static int read_port (KEYWORD *key, int wordcount, char **words)
{
  int *target = (int *) (key->var);

  if (!isArgCount(1, wordcount))
    return 0;

  if ((*target = find_port (words[0])) == 0)
    return ConfigError("%s: bad port number", words[0]);

  return 1;
}

static int read_flag_exec_info (KEYWORD *key, int wordcount, char **words)
{
  EVT_FLAG *new_event, *last;
  char     *path;
  int      i;
  char     **body;

  if (wordcount < 2)
    return SyntaxError(key);

  last = evt_flags;
  if (last)
    for (; last->next; last = last->next)
      ;

  path = words[0];
  for (i = 1; i < wordcount; i++)
  {
    new_event = xalloc(sizeof(*new_event));
    memset(new_event, 0, sizeof(*new_event));

    if (key->option1 == 'f')
      body = &(new_event->path);
    else if (key->option1 == 'e')
      body = &(new_event->command);
    else
      continue; /* should never happens */
    if (*path == '!')
    {
      new_event->imm = 1;
      *body = xstrdup(path + 1);
    }
    else
      *body = xstrdup(path);
    new_event->pattern = xstrdup(words[i]);
    /* strlower (new_event->pattern); */

    if (last == NULL)
      evt_flags = new_event;
    else
      last->next = new_event;
    last = new_event;

   /* simplelist_add(&work_config.evt_flags, &new_event, sizeof(new_event)); */
  }

  return 1;
}

static int print_node_info_1 (FTN_NODE *fn, void *arg)
{
  char szfa[FTN_ADDR_SZ + 1];

  ftnaddress_to_str (szfa, &fn->fa);
  printf ("\n    %-20.20s %s %s %c %s %s%s%s%s%s%s%s%s%s",
	   szfa, fn->hosts ? fn->hosts : "-", fn->pwd,
	   fn->obox_flvr, fn->obox ? fn->obox : "-",
	   fn->ibox ? fn->ibox : "-",
	   (fn->NR_flag == NR_ON) ? " -nr" : "",
	   (fn->ND_flag == ND_ON) ? " -nd" : "",
	   (fn->ND_flag == MD_ON) ? " -md" : "",
	   (fn->ND_flag == MD_OFF) ? " -nomd" : "",
	   (fn->HC_flag == HC_ON) ? " -hc" : "",
	   (fn->HC_flag == HC_OFF) ? " -nohc" : "",
	   (fn->restrictIP == RIP_ON) ? " -ip" : "",
	   (fn->restrictIP == RIP_SIP) ? " -sip" : "");
  return 0;
}

static char *describe_addrtype(addrtype a)
{
  switch (a)
  {
  case A_ALL:    return "all";
  case A_PROT:   return "secure";
  case A_UNPROT: return "unsecure";
  case A_LST:    return "listed";
  case A_UNLST:  return "unlisted";
  }
  return "???";
}

static void debug_readcfg (void)
{
  KEYWORD *k;
  char szfa[FTN_ADDR_SZ + 1];

  for (k = keywords; k->key; k++)
  {
    printf("%-24s ", k->key);
    if (k->callback == read_string)
      printf("\"%s\"", k->var);
    else if (k->callback == read_int || k->callback == read_port)
      printf("%d", *(int *)(k->var));
    else if (k->callback == read_bool)
      printf(*(int *)(k->var) ? "[yes]" : "<not defined>");
    else if (k->callback == include)
    {
      struct conflist_type *c;
      for (c = /*work_config.*/config_list/*.first*/; c; c = c->next)
        printf("\n    %s", c->path);
    }
    else if (k->callback == read_domain_info)
    {
      FTN_DOMAIN *c;
      for (c = /*work_config.*/pDomains/*.first*/; c; c = c->next)
        if (c->alias4)
          printf("\n    %s alias-for %s", c->name, c->alias4->name);
        else
          printf("\n    %s %s/%s %d", c->name, c->path, c->dir, c->z[0]);
    }
    else if (k->callback == read_aka_list)
    {
      int  i;

      for (i = 0; i < /*work_config.*/nAddr; i++)
      {
        ftnaddress_to_str (szfa, /*work_config.*/pAddr + i);
        printf("\n    %s", szfa);
      }
    }
    else if (k->callback == read_node_info)
    {
      if (k->option1 == 0) /* not defnode */
        foreach_node (print_node_info_1, 0 /*, &work_config*/);
      else
        printf(/*work_config.*/havedefnode ? "[see 0:0/0@defnode]" : "<not defined>");
    }
    else if (k->callback == read_flag_exec_info)
    {
      EVT_FLAG *c;
      char     *s;

      for (c = /*work_config.*/evt_flags/*.first*/; c; c = c->next)
      {
        s = (k->option1 == 'f' ? c->path : c->command);
        if (s)
          printf("\n    %s\"%s\" on \"%s\"", (c->imm ? "[Imm] " : ""),
                 s, c->pattern);
      }
    }
    /*
     else if (k->callback == read_string_list)
     {
     struct simplelistheader *l = k->var;
     struct simplelist *c;

     for (c = l->first; c; c = c->next)
     printf("\n    %s", c->data);
     }*/
    else if (k->callback == read_rfrule)
    {
      RF_RULE *c;

      for (c = /*work_config.*/rf_rules/*.first*/; c; c = c->next)
        printf("\n    \"%s\" => \"%s\"", c->from, c->to);
    }
    else if (k->callback == read_inboundcase)
    {
      switch (/*work_config.*/inboundcase)
      {
      case INB_SAVE:  printf("save");  break;
      case INB_UPPER: printf("upper"); break;
      case INB_LOWER: printf("lower"); break;
      case INB_MIXED: printf("mixed"); break;
      default:        printf("???");   break;
      }
    }
    else if (k->callback == read_skip)
    {
      struct skipchain *sk;
      for (sk = skipmask; sk; sk = sk->next)
        printf("\n    %s %c%d \"%s\"", describe_addrtype(sk->atype),
               (sk->destr ? '!' : ' '), (sk->size < 0 ? sk->size : sk->size >> 10),
               sk->mask);
    }
    else if (k->callback == read_mask)
    {
      struct maskchain *p;

      for (p = *(struct maskchain **)k->var; p; p = p->next)
        printf("\n    \"%s\"", p->mask);
    }
    else if (k->callback == read_akamask)
    {
      if (k->option1 == ACT_HIDE)
        printf("\n    [see present-aka]");
      else
      {
        struct akachain *p;

        for (p = *(struct akachain **)k->var; p; p = p->next)
        {
          ftnaddress_to_str(szfa, &p->fa);
          printf("\n    %s %s %c%s", ((p->type & 0x7F) == ACT_HIDE ? "hide-aka" : "present-aka"),
                 szfa, (p->type & 0x80 ? '!' : ' '), p->mask);
        }
      }
    }
    else if (k->callback == read_check_pkthdr)
    {
      if (pkthdr_type == 0)
        printf("<disabled>");
      else
        printf("%s %s", describe_addrtype(pkthdr_type), pkthdr_bad);
    }
    else if (k->callback == read_shares)
    {
      SHARED_CHAIN   *ch;
      FTN_ADDR_CHAIN *fch;

      for (ch = shares; ch; ch = ch->next)
      {
        ftnaddress_to_str(szfa, &ch->sha);
        printf("\n    %s", szfa);
        for (fch = ch->sfa; fch; fch = fch->next)
        {
          ftnaddress_to_str(szfa, &fch->fa);
          printf(" %s", szfa);
        }
      }
    }
#ifdef HTTPS
    /*
     else if (k->callback == read_proxy)
     {
     struct proxy_info *prox;

     if (k->option1 == 'p')
     for (prox = work_config.proxylist.first; prox; prox=prox->next)
     printf("\n    %s %s:%d user=`%s' password=`%s'",
     connect_method_name[prox->method],
     prox->host, prox->port, prox->username, prox->password);
     else
     printf("[see `proxy']");
     }*/
#endif
    else
      printf("[this item cannot be displayed]");
    printf("\n");
  }
}

/*
 * Read shared akas info:
 * share  share_address  node1 [node2 [...]]
 */
static int read_shares (KEYWORD *key, int wordcount, char **words)
{
  int i;
  SHARED_CHAIN   *chn;
  FTN_ADDR_CHAIN *fchn;
  KEYWORD        *k;

  if (wordcount < 2)
    return SyntaxError(key);

  chn = xalloc(sizeof(SHARED_CHAIN));
  chn->next = 0;
  chn->sfa  = 0;
  if (!Config_ParseAddress(words[0], &chn->sha))
  {
    free(chn);
    return 0;
  }
  exp_ftnaddress(&chn->sha);

  /* To scan outgoing mails to shared node
   * `node share_addr' string is simulated
   */
  for (k = keywords; k->key; k++)
    if (!STRICMP (k->key, "node"))
    {
      /* Our first argument passed to read_node_info as-is, with argc = 1 */
      if (read_node_info(k, 1, words) == 0)
      {
        free(chn);
        return 0;
      }
      break;
    }

  for (i = 1; i < wordcount; ++i)
  {
    fchn = xalloc(sizeof(FTN_ADDR_CHAIN));
    fchn->own  = chn;
    fchn->next = 0;
    if (!Config_ParseAddress (words[i], &fchn->fa))
      return 0;  /* !!! known memory leak - will be fixed !!! */
    exp_ftnaddress(&fchn->fa);
    add_address(chn, fchn);
  }

  add_shares(chn);
  return 1;
}

static void add_shares(SHARED_CHAIN *chain)
{
  if (shares == 0)
    shares = chain;
  else
  {
    SHARED_CHAIN *sh = shares;
    while (sh->next) sh = sh->next;
    sh->next = chain;
  }
}

static void add_address(SHARED_CHAIN *chain, FTN_ADDR_CHAIN *aka)
{
  if (chain->sfa == 0)
    chain->sfa = aka;
  else
  {
    FTN_ADDR_CHAIN *fac = chain->sfa;
    while (fac->next) fac = fac->next;
    fac->next = aka;
  }
}
