/*
 *  btypes.h -- defines types and structures used in binkd
 *
 *  btypes.h is a part of binkd project
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
 * Revision 2.1  2003/10/29 21:08:38  gul
 * Change include-files structure, relax dependences
 *
 */
#ifndef _btypes_h
#define _btypes_h

#include <sys/types.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif

#include "Config.h"

struct list_itemlink  { void *next; };  /* in the beginning of each item! */
struct list_linkpoint { struct list_itemlink *last; }; /* in the beginning of each list! */

#define TYPE_LIST(itemtype)  struct simplelist_##itemtype
#define DEFINE_LIST(itemtype)       \
  TYPE_LIST(itemtype)               \
  {                                 \
  struct list_linkpoint linkpoint;  \
  struct itemtype *first;           \
  }

typedef struct _FTN_DOMAIN FTN_DOMAIN;
struct _FTN_DOMAIN
{
  FTN_DOMAIN *next;
  char name[MAX_DOMAIN + 1];
  char *path;				    /* Outbound dir's path, ie
					     * "/var/spool/fido" */
  char *dir;				    /* Outbound dir's name, ie "outb" */
  int z[2];
  FTN_DOMAIN *alias4;
};

typedef struct _FTN_ADDR FTN_ADDR;
struct _FTN_ADDR
{
  char domain[MAX_DOMAIN + 1];
  int z, net, node, p; /* -1==unknown or wildcard */
};

typedef struct _FTN_NODE FTN_NODE;
struct _FTN_NODE
{
  char *hosts;			       /* "host1:port1,host2:port2,*" */

  FTN_ADDR fa;
  char pwd[MAXPWDLEN + 1];
  char obox_flvr;
  char *obox;
  char *ibox;
  int NR_flag;
  int ND_flag;
  int MD_flag;
  int HC_flag;
  int restrictIP;
  /* int NP_flag; */                      /* no proxy */

  time_t hold_until;
  int busy;			       /* 0=free, 'c'=.csy, other=.bsy */
  int mail_flvr;		       /* -1=no mail, other=it's flavour */
  int files_flvr;		       /* -1=no files, other=it's flavour */
};

typedef struct _FTNQ FTNQ;
struct _FTNQ
{
  FTNQ *next;
  FTNQ *prev;

  FTN_ADDR fa;
  char flvr;			       /* 'I', 'i', 'C', 'c', 'D', 'd', 'O',
				        * 'o', 'F', 'f', 'H', 'h' */
  char action;			       /* 'd'elete, 't'runcate, '\0' -- none,
				        * remove on 's'uccessful session,
				        * after 'a'ny session */
  char type;			       /* 'm'ail (.out), .f'l'o, '*' -- a
				        * file from .flo (just for stats, it
				        * will never be selected for sending
				        * right from the queue, it will be
				        * send when parsing its .flo instead,
				        * now it's obsolete), other -- a file
				        * to send. */
  char path[MAXPATHLEN + 1];
  unsigned long size;
  time_t time;			       /* this field seems to be used only in
				        * cmp_filebox_files(), when sorting
				        * files from a filebox before sending */

  int sent;			       /* == 1, if the file have been sent */
};

/* A file in transfer */
typedef struct _TFILE TFILE;
struct _TFILE
{
  char path[MAXPATHLEN + 1];		    /* It's name */
  char flo[MAXPATHLEN + 1];		    /* It's .?lo */
  char netname[MAX_NETNAME + 1];	    /* It's "netname" */
  char action;				    /* 'd'elete, 't'runcate, '\0' -- * *
					     * none */
  char type;
  off_t size;
  time_t start;				    /* The transfer started at... */
  time_t time;				    /* Filetime */
  FILE *f;
  FTN_ADDR fa;
};

/* Files to kill _after_ session */
typedef struct _KILLLIST KILLLIST;
struct _KILLLIST
{
  char name[MAXPATHLEN + 1];		    /* file's name */
  char cond;				    /* after 's'uccessful session,
					     * '\0' -- in any case */
};

typedef struct _EVTQ
{
  char *path;
  struct _EVTQ *next;
  char evt_type;
} EVTQ;

/* List of files received in the current batch */
typedef struct _RCVDLIST RCVDLIST;
struct _RCVDLIST
{
  char name[MAXPATHLEN + 1];		    /* file's name */
};

enum inbcasetype { INB_SAVE,INB_UPPER,INB_LOWER,INB_MIXED };

#endif
