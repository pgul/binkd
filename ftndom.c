/*
 *  ftndom.c -- Source to handle FTN Domains
 *
 *  ftndom.c is a part of binkd project
 *
 *  Copyright (C) 1996,1997  Dima Maloff, 5047/13
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
 * Revision 2.4  2003/10/29 21:08:38  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.3  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.2  2003/08/24 19:42:08  gul
 * Get FTN-domain from matched zone in exp_ftnaddress()
 *
 * Revision 2.1  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.0  2001/01/10 12:12:37  gul
 * Binkd is under CVS again
 *
 * Revision 1.2  1997/10/23  04:08:50  mff
 * stricmp() -> STRICMP()
 *
 * Revision 1.1  1996/12/29  09:41:15  mff
 * Initial revision
 *
 */

#include <stdio.h>
#include "ftndom.h"
#include "tools.h"

/*
 * 0 == domain not found
 */
FTN_DOMAIN *get_domain_info (char *domain_name, FTN_DOMAIN *pDomains)
{
  FTN_DOMAIN *curr;

  for (curr = pDomains; curr; curr = curr->next)
    if (!STRICMP (curr->name, domain_name))
      return curr;
  return 0;
}

char *get_matched_domain (int zone, FTN_ADDR *pAddr, int nAddr, FTN_DOMAIN *pDomains)
{
  FTN_DOMAIN *curr;
  char *p = NULL;
  int n;

  /* Is it default zone for a domain? */
  for (curr = pDomains; curr; curr = curr->next)
    if (!curr->alias4 && curr->z[0] == zone)
    {
      p = curr->name;
      break;
    }
  if (p) return p;
  /* Do we have an AKA with this zone? */
  for (n = 0; n < nAddr; n++)
    if (zone == pAddr[n].z)
      return pAddr[n].domain;
  return pAddr[0].domain;
}
