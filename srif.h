/*
 *  srif.h -- Create flags or run external programs on mail events
 *
 *  srif.h is a part of binkd project
 *
 *  Copyright (C) 1996-1997  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#ifndef _srif_h
#define _srif_h

typedef struct _EVT_FLAG EVT_FLAG;
struct _EVT_FLAG
{
  EVT_FLAG *next;
  char *path;				    /* Set this flag if != NULL */
  char *command;			    /* Run this command if != NULL */
  char *pattern;
  int imm;				    /* Immediate flag */
};

/*
 * Tests if filename matches any of EVT_FLAG's patterns.
 */
int evt_test (EVTQ **eq, char *filename, EVT_FLAG *evt_flags);

/*
 * Runs external programs using S.R.I.F. interface
 * if the name matches one of our "exec"'s
 */
FTNQ *evt_run (FTNQ *q, char *filename0, int imm_freq,
               STATE *st, BINKD_CONFIG *config);

/*
 * Sets flags for all matched with evt_test events
 */
void evt_set (EVTQ *eq);

#endif
