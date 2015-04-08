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

#ifndef _prothlp_h
#define _prothlp_h

#include "btypes.h"

#define TF_ZERO(a) (memset(a, 0, sizeof(*a)))

int tfile_cmp (TFILE *a, char *netname, boff_t size, time_t time);

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
