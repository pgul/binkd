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
 * Revision 1.1  2001/01/10 11:34:58  gul
 * Initial revision
 *
 * Revision 1.2  1997/10/23  04:08:50  mff
 * stricmp() -> STRICMP()
 *
 * Revision 1.1  1996/12/29  09:41:15  mff
 * Initial revision
 *
 */

#include "ftndom.h"
#include "tools.h"

FTN_DOMAIN *pDomains = 0;

/*
 * 0 == domain not found
 */
FTN_DOMAIN *get_domain_info (char *domain_name)
{
  FTN_DOMAIN *curr;

  for (curr = pDomains; curr; curr = curr->next)
    if (!STRICMP (curr->name, domain_name))
      return curr;
  return 0;
}

/*
 * Returns the default domain
 */
FTN_DOMAIN *get_def_domain ()
{
  FTN_DOMAIN *curr;

  for (curr = pDomains; curr->next; curr = curr->next);
  return curr;
}
