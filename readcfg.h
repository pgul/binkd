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

#include "Config.h"
#include "iphdr.h"
#include "ftnaddr.h"
#include "ftndom.h"
#include "ftnnode.h"

#define MAXINCLUDELEVEL 8
#define MAXCFGLINE 1024

#define MAXSYSTEMNAME 120
#define MAXSYSOPNAME 120
#define MAXLOCATIONNAME 120
#define MAXNODEINFO 120

extern int nAddr;
extern FTN_ADDR *pAddr;
extern int iport;
extern int oport;
extern int oblksize;
extern int nettimeout;
extern int rescan_delay;
extern int call_delay;
extern int max_servers;
extern int max_clients;
extern char sysname[MAXSYSTEMNAME + 1];
extern char bindaddr[16];
extern char sysop[MAXSYSOPNAME + 1];
extern char location[MAXLOCATIONNAME + 1];
extern char nodeinfo[MAXNODEINFO + 1];
extern char inbound[MAXPATHLEN + 1];
extern char inbound_nonsecure[MAXPATHLEN + 1];
extern char temp_inbound[MAXPATHLEN + 1];
extern int kill_dup_partial_files;
extern int kill_old_partial_files;
extern int kill_old_bsy;
extern int minfree;
extern int minfree_nonsecure;
extern int tries;
extern int hold;
extern int hold_skipped;
extern int backresolv;
extern int send_if_pwd;
extern int debugcfg;
extern char logpath[MAXPATHLEN + 1];
extern char binlogpath[MAXPATHLEN + 1];
extern char fdinhist[MAXPATHLEN + 1];
extern char fdouthist[MAXPATHLEN + 1];
extern char pid_file[MAXPATHLEN + 1];
extern int loglevel;
extern int conlog;
extern int printq;
extern int percents;
extern int tzoff;
extern char root_domain[MAXHOSTNAMELEN + 1];
extern int prescan;
extern enum inbcasetype { INB_SAVE,INB_UPPER,INB_LOWER,INB_MIXED } inboundcase;
extern int deletedirs;
extern int havedefnode;
#ifdef MAILBOX
/* FileBoxes dir */
extern char tfilebox[MAXPATHLEN + 1];
/* BrakeBoxes dir */
extern char bfilebox[MAXPATHLEN + 1];
extern int deleteablebox;
#endif
#ifdef HTTPS
extern char proxy[MAXHOSTNAMELEN + 40];
extern char socks[MAXHOSTNAMELEN + 40];
#endif
#ifdef AMIGADOS_4D_OUTBOUND
extern int aso;
#endif
extern struct conflist_type 
  { char *path;
    struct conflist_type *next;
    unsigned long mtime;
  } *config_list;
extern struct maskchain
  {
    struct maskchain *next;
    char *mask;
  } *skipmask, *overwrite;

/*
 * Parses and reads the path as a config
 */
void readcfg (char *path);

int  get_host_and_port (int n, char *host, unsigned short *port, char *src, FTN_ADDR *fa);

char *mask_test(char *filename, struct maskchain *chain);

#endif
