/*
 *  prothlp.h -- Some tools for protocol.c
 *
 *  prothlp.h is a part of binkd project
 *
 *  Copyright (C) 1997  Dima Maloff, 5047/13
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
 * Revision 2.4  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.3  2003/06/26 13:21:32  gul
 * More clean status process in no-NR mode
 *
 * Revision 2.2  2003/03/01 18:29:52  gul
 * Change size_t to off_t for file sizes and offsets
 *
 * Revision 2.1  2002/02/22 00:18:34  gul
 * Run by-file events with the same command-line once after session
 *
 * Revision 2.0  2001/01/10 12:12:38  gul
 * Binkd is under CVS again
 *
 * Revision 1.1  1997/03/28  06:49:13  mff
 * Initial revision
 *
 */
#ifndef _prothlp_h
#define _prothlp_h

#include "ftnq.h"

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

#define TF_ZERO(a) (memset(a, 0, sizeof(*a)))

int tfile_cmp (TFILE *a, char *netname, off_t size, time_t time);

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

/* Adds a file to killlist */
void add_to_killlist (KILLLIST **killlist, int *n_killlist, char *name, int cond);
void q_to_killlist (KILLLIST **killlist, int *n_killlist, FTNQ *q);
void free_killlist (KILLLIST **killlist, int *n_killlist);
void process_killlist (KILLLIST *killlist, int n_killlist, int flag);

/* List of files received in the current batch */
typedef struct _RCVDLIST RCVDLIST;
struct _RCVDLIST
{
  char name[MAXPATHLEN + 1];		    /* file's name */
};

/* Adds a file to rcvdlist */
void add_to_rcvdlist (RCVDLIST **rcvdlist, int *n_rcvdlist, char *name);
void free_rcvdlist (RCVDLIST **rcvdlist, int *n_rcvdlist);

/* Creates a netname from a local name */
void netname (char *s, TFILE *q, BINKD_CONFIG *config);

#endif
