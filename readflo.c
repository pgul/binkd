/*
 *  readflo.c -- Filename translation in ?lo-files
 *
 *  readflo.c is a part of binkd project
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
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 * Revision 1.1  1997/03/28  06:20:30  mff
 * Initial revision
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "Config.h"
#include "readflo.h"
#include "assert.h"

RF_RULE *rf_rules = 0;

/*
 * Reads a line from a flo to dst[MAXPATHLEN], sets action
 * 1 -- ok
 * 0 -- EOF
 */
int read_flo_line (char *dst, int *action, FILE *flo)
{
  char buf[MAXPATHLEN + 1];
  int i;

  while (1)
  {
    if (!fgets (buf, MAXPATHLEN, flo))
      return 0;

    for (i = strlen (buf) - 1; i > 0 && isspace (buf[i]); --i)
      buf[i] = 0;

    switch (*buf)
      {
	case 0:
	case '~':
	  continue;
	case '^':
	  *action = 'd';
	  strcpy (dst, buf + 1);
	  break;
	case '#':
	  *action = 't';
	  strcpy (dst, buf + 1);
	  break;
	default:
	  *action = 0;
	  strcpy (dst, buf);
	  break;
      }
    break;
  }
  return 1;
}

/*
 * Translates a flo line using rf_rules.
 * Returns 0 if no rf_rules defined, otherwise returned value
 * should be free()'d
 */
char *trans_flo_line (char *s)
{
  RF_RULE *curr;
  char buf[MAXPATHLEN + 1];

  if (rf_rules)
  {
    char *w;

    strnzcpy (buf, s, MAXPATHLEN);
    for (curr = rf_rules; curr; curr = curr->next)
    {
      w = ed (buf, curr->from, curr->to, NULL);
      strnzcpy (buf, w, MAXPATHLEN);
      free (w);
    }
    return xstrdup (buf);
  }
  else
    return 0;
}

/*
 * Add a translation rule for trans_flo_line ()
 * (From and to are saved as pointers!)
 */
void rf_rule_add (char *from, char *to)
{
  static RF_RULE *last_rule = 0;
  RF_RULE *new_rule = xalloc (sizeof (RF_RULE));

  memset (new_rule, 0, sizeof (RF_RULE));
  new_rule->from = from;
  new_rule->to = to;
  if (last_rule)
    last_rule->next = new_rule;
  else
    rf_rules = new_rule;
  last_rule = new_rule;
}
