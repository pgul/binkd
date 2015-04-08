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

#include <stdio.h>
#include "sys.h"
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
  char *p;
  int n;

  /* Is it default zone for a domain? */
  for (curr = pDomains; curr; curr = curr->next)
    if (!curr->alias4 && curr->z[0] == zone)
      return curr->name;
 
  /* Do we have an AKA with this zone? */
  if(pAddr)
    for (n = 0; n < nAddr; n++)
      if (zone == pAddr[n].z)
        return pAddr[n].domain;

  /* No defined domain, try to guess defaults */
  if (nAddr && pAddr)
    p = pAddr[0].domain;   /* If we have nodes defined, use main AKA */
  else
    p = pDomains->name;    /* Use first domain (at least one always defined at this point) */
  Log(1, "Cannot find domain for zone %d, assuming '%s'", zone, p);
  return p;
}
