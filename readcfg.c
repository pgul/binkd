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

static char *path;
static int line;

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
int conlog = 0;
int send_if_pwd = 0;
int tzoff = 0;
char root_domain[MAXHOSTNAMELEN + 1] = "fidonet.net.";
int prescan = 0;
enum inbcasetype inboundcase = INB_SAVE;
int connect_timeout = 0;
struct conflist_type *config_list = NULL;
#ifdef AMIGADOS_4D_OUTBOUND
int aso = 0;
#endif

#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)

int syslog_facility = -1;

#endif

int tries = 0;
int hold = 0;
int hold_skipped = 60 * 60;
struct maskchain *skipmask = NULL, *overwrite = NULL;

int nAddr = 0;
FTN_ADDR *pAddr = 0;

typedef struct _KEYWORD KEYWORD;
struct _KEYWORD
{
  const char *key;
  void (*callback) (KEYWORD *key, char *s);
  void *var;
  long option1;
  long option2;
};

static void passwords (KEYWORD *, char *);
static void include (KEYWORD *, char *);
static void read_aka_list (KEYWORD *, char *);
static void read_domain_info (KEYWORD *, char *);
static void read_node_info (KEYWORD *, char *);
static void read_int (KEYWORD *, char *);
static void read_string (KEYWORD *, char *);
static void read_bool (KEYWORD *, char *);
static void read_flag_exec_info (KEYWORD *, char *);
static void read_rfrule (KEYWORD *, char *);
static void read_mask (KEYWORD *key, char *s);
static void read_inboundcase (KEYWORD *, char *);

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
  {"iport", read_string, siport, 0, MAXSERVNAME},
  {"oport", read_string, soport, 0, MAXSERVNAME},
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
  {"skipmask", read_mask, &skipmask, 0, 0},
  {"inboundcase", read_inboundcase, &inboundcase, 0, 0},
  {"deletedirs", read_bool, &deletedirs, 0, 0},
  {"overwrite", read_mask, &overwrite, 0, 0},
#ifdef AMIGADOS_4D_OUTBOUND
  {"aso", read_bool, &aso, 0, 0},
#endif
  {NULL, NULL, NULL, 0, 0}
};

#define TEST(var) if (!*var) Log (0, "%s: "#var" should be defined", path)

void readcfg0 (char *_path);
void debug_readcfg (void);

/* Check for (personal) outbox pointed to (common) outbound */
static int check_outbox(char *obox)
{
  FTN_DOMAIN *pd=pDomains;
#ifndef UNIX
  char *OBOX, *PATH=NULL;
  if (obox == NULL) return 0;
  OBOX = strupper(xstrdup(obox));
#else
  if (obox == NULL) return 0;
#endif
  for (; pd; pd=pd->next)
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
        if ((*s == '\\' || *s == '/') && stricmp(s+1, pd->dir) == 0)
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

/*
 * Parses and reads _path as config.file
 */
void readcfg (char *_path)
{
  readcfg0 (_path);

  if ((iport = find_port (siport)) == 0
      || (oport = find_port (soport)) == 0)
    Log (0, "cannot find the port number");

  TEST (sysname);
  TEST (sysop);
  TEST (location);
  TEST (nodeinfo);

  if (!*inbound_nonsecure)
    strcpy (inbound_nonsecure, inbound);

  if (!nAddr)
    Log (0, "%s: your address should be defined", path);

  if (pDomains == 0)
    Log (0, "%s: at least one domain should be defined", path);

  if (debugcfg)
    debug_readcfg ();
}

void readcfg0 (char *_path)
{
  FILE *in;
  char buf[MAXCFGLINE + 1];
  char *w;

  line = 0;
  path = _path;

  if ((in = fopen (path, "r")) == 0)
    Log (0, "%s: %s", path, strerror (errno));

  if (checkcfg_flag)
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

  while (!feof (in))
  {
    if (!fgets (buf, sizeof (buf), in))
      break;
    ++line;

    if ((w = getword (buf, 1)) != 0)
    {
      int j;

      for (j = 0; keywords[j].key; ++j)
	if (!STRICMP (keywords[j].key, w))
	  break;

      if (keywords[j].key)
      {
	keywords[j].callback (keywords + j, buf);
      }
      else
      {
	Log (0, "%s: %i: %s: unknown keyword", path, line, w);
      }
      free (w);
    }
  }
  fclose (in);
}

/*
 *  METHODS TO PROCESS KEYWORDS' ARGUMETS
 */

static void include (KEYWORD *key, char *s)
{
  static int level = 0;

  if (++level > MAXINCLUDELEVEL)
  {
    Log (0, "%s: %i: too many nested include commands", path, line);
  }
  else
  {
    char *old_path = path;
    int old_line = line;
    char *w = getword (s, 2);

    if (w)
    {
      readcfg0 (w);
      free (w);
    }
    else
      Log (0, "%s: %i: filename expected", path, line);
    path = old_path;
    line = old_line;
    --level;
  }
}

static void passwords (KEYWORD *key, char *s)
{
  FILE *in;
  char buf[MAXCFGLINE + 1];  
  char *w = getword(s, 2);
  FTN_ADDR fa;

  if(!w) 
    Log (0, "%s: %i: password filename expected", path, line);
  if((in=fopen(w, "rt"))==NULL)
    Log (0, "%s: %i: unable to open password file (%s)", path, line, w);
  free(w);

  while (!feof (in))
  {
    if (!fgets (buf, sizeof (buf), in))
      break;
    for(w=buf;isspace(w[0]);w++);  /* skip spaces */
    if(w!=buf) strcpy(buf, w); 
    for(w=buf;(w[0])&&(!isspace(w[0]));w++);
    while(isspace(w[0]))           /* go to the password */
    {
      w[0]=0;
      w++;
    }
    if((!w[0])||(!parse_ftnaddress (buf, &fa))) 
      continue;     /* Do not process if any garbage found */
    exp_ftnaddress (&fa);
    strcpy(buf, w);
    for(w=buf;(w[0])&&(!isspace(w[0]));w++);
    w[0]=0;
    if (!add_node (&fa, NULL, buf, '-', NULL, NULL,
                   NR_USE_OLD, ND_USE_OLD, CRYPT_USE_OLD, 0, 0))
      Log (0, "%s: add_node() failed", w[0]);
  }
  fclose(in);
}

static void read_aka_list (KEYWORD *key, char *s)
{
  int i;
  char *w;

  for (i = 1; (w = getword (s, i + 1)) != 0; ++i)
  {
    pAddr = xrealloc (pAddr, sizeof (FTN_ADDR) * (nAddr + 1));
    if (!parse_ftnaddress (w, pAddr + nAddr))
    {
      Log (0, "%s: %i: %s: the address cannot be parsed", path, line, w);
    }
    if (!is4D (pAddr + nAddr))
    {
      Log (0, "%s: %i: %s: must be at least a 4D address", path, line, w);
    }
    if (!pAddr[nAddr].domain[0])
    {
      if (!pDomains)
	Log (0, "%s: %i: at least one domain must be defined first", path, line);
      strcpy (pAddr[nAddr].domain, get_def_domain ()->name);
    }
    ++nAddr;
    free (w);
  }
}

static void read_domain_info (KEYWORD *key, char *s)
{
  char *w1 = getword (s, 2);
  char *w2 = getword (s, 3);
  char *w3 = getword (s, 4);
  FTN_DOMAIN *new_domain;

  if (!w1 || !w2 || !w3)
    Log (0, "%s: %i: domain: not enough args", path, line);

  if (get_domain_info (w1) == 0)
  {
    new_domain = xalloc (sizeof (FTN_DOMAIN));
    strnzcpy (new_domain->name, w1, sizeof (new_domain->name));
    if (!STRICMP (w2, "alias-for"))
    {
      FTN_DOMAIN *tmp_domain;

      if ((tmp_domain = get_domain_info (w3)) == 0)
	Log (0, "%s: %i: %s: undefined domain", path, line, w3);
      else
	new_domain->alias4 = tmp_domain;
      free (w2);
    }
    else
    {
      char *s;
      int z;

      if ((z = atoi (w3)) <= 0)
	Log (0, "%s: %i: invalid zone", path, line);

      new_domain->z = xalloc (sizeof (int) * 2);
      new_domain->z[0] = z;
      new_domain->z[1] = 0;
      new_domain->alias4 = 0;

      for (s = w2 + strlen (w2) - 1; (*s == '/' || *s == '\\') && s >= w2; --s)
	*s = 0;
      if ((s = max (strrchr (w2, '\\'), strrchr (w2, '/'))) == 0)
      {
	new_domain->dir = w2;
	new_domain->path = xstrdup (".");
      }
      else
      {
	new_domain->dir = xstrdup (s + 1);
	for (; *s == '/' || *s == '\\'; --s)
	  *s = 0;
	new_domain->path = w2;
      }
      if (strchr (new_domain->dir, '.'))
	Log (0, "%s: %i: there should be no extension for "
	     "the base outbound name", path, line);
    }
    new_domain->next = pDomains;
    pDomains = new_domain;
  }
  else
  {
    Log (0, "%s: %i: %s: duplicate domain", path, line, w1);
  }
  free (w1);
  free (w3);
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

static void read_node_info (KEYWORD *key, char *s)
{
#define ARGNUM 6
  char *w[ARGNUM], *tmp;
  int i, j, NR_flag = NR_USE_OLD, ND_flag = ND_USE_OLD;
  int MD_flag = 0, crypt_flag = CRYPT_USE_OLD, restrictIP = 0;
  FTN_ADDR fa;

  memset (w, 0, sizeof (w));
  i = 0;			       /* index in w[] */
  j = 2;			       /* number of word in the source string */
  
  if(key->option1) /* defnode */
  {
	  w[i++]=xstrdup("0:0/0.0@defnode");
	  havedefnode=1;
  }

  while (1)
  {
    if ((tmp = getword (s, j++)) == NULL)
      break;

    if (tmp[0] == '-')
    {
      if (tmp[1] != '\0')
      {
        if (STRICMP (tmp, "-md") == 0)
          MD_flag = 1;
        else if (STRICMP (tmp, "-nomd") == 0)
          MD_flag = (-1);
        else if (STRICMP (tmp, "-nr") == 0)
	  NR_flag = NR_ON;
	else if (STRICMP (tmp, "-nd") == 0)
	{
	  NR_flag = NR_ON;
	  ND_flag = ND_ON;
	}
	else if (STRICMP (tmp, "-ip") == 0)
	  restrictIP = 1; /* allow matched or unresolvable */
	else if (STRICMP (tmp, "-sip") == 0)
	  restrictIP = 2; /* allow only resolved and matched */
	else if (STRICMP (tmp, "-crypt") == 0)
	  crypt_flag = CRYPT_ON;
	else
	  Log (0, "%s: %i: %s: unknown option for `node' keyword", path, line, tmp);
      }
      else
      {
	/* Process "-": skip w[i]. Let it be filled with default NULL */
	++i;
      }
    }
    else if (i >= ARGNUM)
      Log (0, "%s: %i: too many argumets for `node' keyword", path, line);
    else
      w[i++] = tmp;
  }

  if (i == 0)
    Log (0, "%s: %i: the address is not specified in the node string", path, line);
  if (!parse_ftnaddress (w[0], &fa))
    Log (0, "%s: %i: %s: the address cannot be parsed", path, line, w[0]);
  else
    exp_ftnaddress (&fa);

  if (w[2] && w[2][0] == 0)
    Log (0, "%s: %i: empty password", path, line);
  if (w[3] && w[3][0] != '-' && !isflvr (w[3][0]))
    Log (0, "%s: %i: %s: incorrect flavour", path, line, w[3]);
  check_dir_path (w[4]);
  check_dir_path (w[5]);

  if (check_outbox(w[4]))
    Log (0, "Outbox cannot point to outbound! (link %s)", w[0]);

  if (!add_node (&fa, w[1], w[2], (char)(w[3] ? w[3][0] : '-'), w[4], w[5],
		 NR_flag, ND_flag, crypt_flag, MD_flag, restrictIP))
    Log (0, "%s: add_node() failed", w[0]);

  for (i = 0; i < ARGNUM; ++i)
    if (w[i])
      free (w[i]);

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
static void read_string (KEYWORD *key, char *s)
{
  struct stat sb;
  char *target = (char *) (key->var);
  char *w;

  if ((w = getword (s, 2)) == NULL)
    Log (0, "%s: %i: missing an argument for `%s'", path, line, key->key);

  if (getword (s, 3) != NULL)
    Log (0, "%s: %i: extra arguments for `%s'", path, line, key->key);

  strnzcpy (target, w, key->option1 == 0 ? key->option2 : MAXPATHLEN);
  free (w);

  if (key->option1 != 0)
  {
    w = target + strlen (target) - 1;
    while (w >= target && (*w == '/' || *w == '\\'))
    {
      if (key->option1 == 'f')
      {
	Log (0, "%s: %i: unexpected `%c' at the end of filename",
	     path, line, *w);
      }
      *(w--) = 0;
    }
    if (key->option1 == 'd' && (stat (target, &sb) == -1 ||
				!(sb.st_mode & S_IFDIR)))
    {
      Log (0, "%s: %i: %s: incorrect directory", path, line, target);
    }
  }
}

static void read_int (KEYWORD *key, char *s)
{
  int *target = (int *) (key->var);
  char *w;

  if ((w = getword (s, 2)) == NULL)
    Log (0, "%s: %i: missing an argument for `%s'", path, line, key->key);

  if (getword (s, 3) != NULL)
    Log (0, "%s: %i: extra arguments for `%s'", path, line, key->key);

  *target = atoi (w);
  free (w);

  if ((key->option1 != DONT_CHECK && *target < key->option1) ||
      (key->option2 != DONT_CHECK && *target > key->option2))
    Log (0, "%s: %i: %i: incorrect value", path, line, *target);
}

static void read_inboundcase (KEYWORD *key, char *s)
{
  enum inbcasetype *target = (enum inbcasetype *) (key->var);
  char *w;

  if ((w = getword (s, 2)) == NULL)
    Log (0, "%s: %i: missing an argument for `%s'", path, line, key->key);

  if (getword (s, 3) != NULL)
    Log (0, "%s: %i: extra arguments for `%s'", path, line, key->key);

  *target = 0;

  if (!STRICMP (w, "save"))
    *target = INB_SAVE;
  else if (!STRICMP (w, "upper"))
    *target = INB_UPPER;
  else if (!STRICMP (w, "lower"))
    *target = INB_LOWER;
  else if (!STRICMP (w, "mixed"))
    *target = INB_MIXED;
  else
    Log (0, "%s: %i: the syntax is incorrect for '%s'", path, line, key->key);

  free (w);
}


#if defined (HAVE_VSYSLOG) && defined (HAVE_FACILITYNAMES)
static void read_syslog_facility (KEYWORD *key, char *s)
{
  int *target = (int *) (key->var);
  char *w;

  if ((w = getword (s, 2)) != 0 && getword (s, 3) == 0)
  {
    int i;

    for (i = 0; facilitynames[i].c_name; ++i)
      if (!strcmp (facilitynames[i].c_name, w))
	break;

    if (facilitynames[i].c_name == 0)
      Log (0, "%s: %i: %s: incorrect facility name", path, line, w);
    *target = facilitynames[i].c_val;
    free (w);
  }
  else
    Log (0, "%s: %i: the syntax is incorrect", path, line);
}
#endif

static void read_rfrule (KEYWORD *key, char *s)
{
  char *w1, *w2;

  if ((w1 = getword (s, 2)) != 0 &&
      (w2 = getword (s, 3)) != 0 &&
      getword (s, 4) == 0)
  {
    rf_rule_add (w1, w2);
  }
  else
    Log (0, "%s: %i: the syntax is incorrect", path, line);
}

static void mask_add(char *mask, struct maskchain **chain)
{
  struct maskchain *ps;

  if (*chain == NULL)
  {
    *chain = xalloc(sizeof(**chain));
    ps = *chain;
  }
  else
  {
    for (ps = *chain; ps->next; ps = ps->next);
    ps->next = xalloc(sizeof(*ps));
    ps = ps->next;
  }
  ps->next = NULL;
  ps->mask = xstrdup(mask);
}

char *mask_test(char *netname, struct maskchain *chain)
{
  struct maskchain *ps;

  for (ps = chain; ps; ps = ps->next)
    if (pmatch(ps->mask, netname))
      return ps->mask;
  return NULL;
}

static void read_mask (KEYWORD *key, char *s)
{
  char *w;
  int i;

  for (i=2; (w = getword (s, i)) != NULL; i++)
    mask_add (w, (struct maskchain **) (key->var));
  if (i == 2)
    Log (0, "%s: %i: the syntax is incorrect", path, line);
}

static void read_bool (KEYWORD *key, char *s)
{
  if (getword (s, 2) == 0)
  {
    *(int *) (key->var) = 1;
  }
  else
    Log (0, "%s: %i: the syntax is incorrect", path, line);
}

static void read_flag_exec_info (KEYWORD *key, char *s)
{
  EVT_FLAG *tmp;
  char *path, *w, **body;
  int i;
  static EVT_FLAG *last = 0;

  if ((path = getword (s, 2)) == 0)
    Log (0, "%s: %i: the syntax is incorrect", path, line);
  for (i = 2; (w = getword (s, i + 1)) != 0; ++i)
  {
    tmp = xalloc (sizeof (EVT_FLAG));
    memset (tmp, 0, sizeof (EVT_FLAG));
    if (key->option1 == 'f')
      body = &(tmp->path);
    else if (key->option1 == 'e')
      body = &(tmp->command);
    else
      continue; /* should never happens */
    *body = path;
    if (**body == '!')
    {
      tmp->imm = 1;
      body[0]++;
    }
    tmp->pattern = w;
    strlower (tmp->pattern);

    tmp->next = 0;
    if (last == 0)
      evt_flags = tmp;
    else
      last->next = tmp;
    last = tmp;
  }
}

void debug_readcfg (void)
{
  int i;
  char buf[80];
  FTN_DOMAIN *curr_domain;

  printf ("addr:");
  for (i = 0; i < nAddr; ++i)
  {
    ftnaddress_to_str (buf, pAddr + i);
    printf (" %s", buf);
  }
  printf ("\n");
  for (curr_domain = pDomains; curr_domain; curr_domain = curr_domain->next)
  {
    if (curr_domain->alias4 == 0)
      printf ("`%s', `%s', `%s'.\n",
	      curr_domain->name,
	      curr_domain->path,
	      curr_domain->dir);
    else
      printf ("`%s' alias for `%s'.\n",
	      curr_domain->name,
	      curr_domain->alias4->name);
  }
  printf ("\n");
  print_node_info (stdout);
  printf ("\n");
}
