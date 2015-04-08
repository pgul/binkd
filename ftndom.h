/*
 *  ftndom.h -- Source to handle FTN Domains
 *
 *  ftndom.h is a part of binkd project
 *
 *  Copyright (C) 1996  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#ifndef _ftndomain_h
#define _ftndomain_h

#include "btypes.h"

/*
 * 0 == domain not found
 */
FTN_DOMAIN *get_domain_info (char *domain_name, FTN_DOMAIN *pDomains);

/*
 * Returns the matched domain by zone
 */
char *get_matched_domain (int zone, FTN_ADDR *pAddr, int nAddr, FTN_DOMAIN *pDomains);

#endif
