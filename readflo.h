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

#ifndef _readflo_h
#define _readflo_h

typedef struct _RF_RULE RF_RULE;
struct _RF_RULE
{
  RF_RULE *next;
  char *from, *to;
};

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
char *trans_flo_line (char *s, RF_RULE *rf_rules);

#endif
