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
 * Revision 2.5  2003/10/29 21:08:39  gul
 * Change include-files structure, relax dependences
 *
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

#include "btypes.h"

#define TF_ZERO(a) (memset(a, 0, sizeof(*a)))

int tfile_cmp (TFILE *a, char *netname, off_t size, time_t time);

/* Adds a file to killlist */
void add_to_killlist (KILLLIST **killlist, int *n_killlist, char *name, int cond);
void q_to_killlist (KILLLIST **killlist, int *n_killlist, FTNQ *q);
void free_killlist (KILLLIST **killlist, int *n_killlist);
void process_killlist (KILLLIST *killlist, int n_killlist, int flag);

/* Adds a file to rcvdlist */
void add_to_rcvdlist (RCVDLIST **rcvdlist, int *n_rcvdlist, char *name);
void free_rcvdlist (RCVDLIST **rcvdlist, int *n_rcvdlist);

/* Creates a netname from a local name */
#ifdef AMIGADOS_4D_OUTBOUND
void netname_ (char *s, TFILE *q, int aso);
#define netname(s, q, config) netname_(s, q, (config)->aso)
#else
void netname_ (char *s, TFILE *q);
#define netname(s, q, config) netname_(s, q)
#endif

#endif
