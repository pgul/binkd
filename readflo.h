/*
 *  readflo.h -- Filename translation in ?lo-files
 *
 *  readflo.h is a part of binkd project
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
 * Revision 1.1.1.1  2001/01/10 11:34:59  gul
 * BinkD sources are under CVS again
 *
 * Revision 1.1  1997/03/28  06:20:30  mff
 * Initial revision
 *
 */

#ifndef _readflo_h
#define _readflo_h

#include <stdio.h>

typedef struct _RF_RULE RF_RULE;
struct _RF_RULE
{
  char *from, *to;
  RF_RULE *next;
};

/*
 * Add a translation rule for trans_flo_line ()
 * (From and to are saved as pointers!)
 */
void rf_rule_add (char *from, char *to);

/*
 * Reads a line from a flo to dst[MAXPATHLEN], sets action
 * 1 -- ok
 * 0 -- EOF
 */
int read_flo_line (char *dst, int *action, FILE *flo);

/*
 * Translates a flo line using rf_rules.
 * Returns 0 if no rf_rules defined, otherwise returned value
 * should be free()'d
 */
char *trans_flo_line (char *s);

#endif
