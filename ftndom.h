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

/*
 * $Id$
 *
 * $Log$
 * Revision 1.1.1.1  2001/01/10 11:34:58  gul
 * BinkD sources are under CVS again
 *
 * Revision 1.1  1996/12/29  09:41:28  mff
 * Initial revision
 *
 */
#ifndef _ftndomain_h
#define _ftndomain_h

#include "ftnaddr.h"

typedef struct _FTN_DOMAIN FTN_DOMAIN;
struct _FTN_DOMAIN
{
  char name[MAX_DOMAIN + 1];
  char *path;				    /* Outbound dir's path, ie
					     * "/var/spool/fido" */
  char *dir;				    /* Outbound dir's name, ie "outb" */
  int *z;
  FTN_DOMAIN *alias4;
  FTN_DOMAIN *next;
};

/*
 * 0 == domain not found
 */
FTN_DOMAIN *get_domain_info (char *domain_name);

/*
 * Returns the default domain
 */
FTN_DOMAIN *get_def_domain ();

#endif
