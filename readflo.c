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
 * Revision 2.6  2013/11/07 16:21:33  stream
 * Lot of fixes to support 2G+ files. Supports 2G+ on Windows/MSVC
 *
 * Revision 2.5  2003/10/29 21:08:39  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.4  2003/08/26 21:01:10  gul
 * Fix compilation under unix
 *
 * Revision 2.3  2003/08/26 16:06:27  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.2  2003/08/23 15:51:51  stream
 * Implemented common list routines for all linked records in configuration
 *
 * Revision 2.1  2002/11/12 17:27:46  gul
 * Ignore empty (\r\n) line in lo-files
 *
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 * Revision 1.1  1997/03/28  06:20:30  mff
 * Initial revision
 *
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
