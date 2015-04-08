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

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Config.h"
#include "sys.h"
#include "tools.h"
#include "readflo.h"

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

    for (i = strlen (buf) - 1; i >= 0 && isspace (buf[i]); --i)
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
char *trans_flo_line (char *s, RF_RULE *rf_rules)
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
