/*
 * srv_getaddrinfo.h -- SRV support for getaddrinfo
 *
 * Copyright 2011 Andre Grueneberg for BinkD
 *
 * inspired by getsrvinfo.c, Copyright 2009 Red Hat, Inc.
 * and by      ruli_getaddrinfo.c, Copyright (C) 2004 Goeran Weinholt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/*
 * $Id$
 *
 * $Log$
 * Revision 2.2  2012/01/22 01:27:19  green
 * Implement FSP1035 support for OS/2/Watcom
 *
 * Revision 2.1  2012/01/03 17:52:32  green
 * Implement FSP-1035 (SRV record usage)
 * - add SRV enabled getaddrinfo() wrapper (srv_gai.[ch])
 * - Unix (libresolv, autodetected) and Win32 support implemented
 * - Port information is stored as string now, i.e. may be service name
 *
 */

#ifndef __SRV_GETADDRINFO_H__
#define __SRV_GETADDRINFO_H__

#if defined(HAVE_RESOLV_H) || defined(WITH_FSP1035)
#include "iphdr.h"
#include "rfc2553.h"
#ifdef OS2
#include "os2/ns_parse.h"
#endif


#define SRVGAI_DNSRESPLEN 1024

extern int srv_getaddrinfo(const char *node, const char *service,
			   const struct addrinfo *hints,
			   struct addrinfo **res);
#else	/*  defined(HAVE_RESOLV_H) || defined(WITH_FSP1035) */
/* fallback if we don't have a resolver */
#define srv_getaddrinfo(a,b,c,d) getaddrinfo(a,b,c,d)
#endif

#endif /* __SRV_GETADDRINFO_H__ */
