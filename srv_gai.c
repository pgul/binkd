/*
 * srv_getaddrinfo.c -- SRV support for getaddrinfo
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "sys.h"
#include "iphdr.h"
#include "srv_gai.h"

int srv_getaddrinfo(const char *node, const char *service,
		    const struct addrinfo *hints,
		    struct addrinfo **res)
{
    char *srv_name;
    size_t srv_name_size;
    char tgt_port[6];
#ifdef WIN32
    PDNS_RECORD resp, entry;
    char *tgt_name;
#else
    char tgt_name[BINKD_FQDNLEN + 1];
    unsigned char resp[SRVGAI_DNSRESPLEN];
    ns_msg nsb;
    ns_rr rrb;
    int rlen, i, rrlen;
    const unsigned char *p;
    struct in_addr dummy_addr;
#endif
    int rc;
    struct addrinfo *ai, **ai_last = res;

    /* we need sensible information for all parameters */
    if (!node || (node && !*node) || !service || (service && !*service) ||
	    !hints || !res)
	return getaddrinfo(node, service, hints, res);

    /* only domain names are supported */
    if (hints->ai_flags & AI_NUMERICHOST)
	return getaddrinfo(node, service, hints, res);

    /* detect IP addresses */
    if ((hints->ai_family == AF_INET || hints->ai_family == AF_UNSPEC) && 
#ifdef WIN32
	    inet_addr(node) != INADDR_NONE
#else
	    inet_aton(node, &dummy_addr) != 0
#endif
	    )
	return getaddrinfo(node, service, hints, res);
#ifdef AF_INET6
    if ((hints->ai_family == AF_INET6 || hints->ai_family == AF_UNSPEC) && 
	    strchr(node, ':'))
	return getaddrinfo(node, service, hints, res);
#endif

    /* only named services are supported */
    if ((hints->ai_flags & AI_NUMERICSERV) || *service == '0' ||
	    atoi(service) > 0)
	return getaddrinfo(node, service, hints, res);

    /* only TCP and UDP are supported */
    if (hints->ai_socktype != SOCK_STREAM && hints->ai_socktype != SOCK_DGRAM)
	return getaddrinfo(node, service, hints, res);

    /*              _ <service>           ._  tcp . <node>           \0 */
    srv_name_size = 1 + strlen(service) + 2 + 3 + 1 + strlen(node) + 1;
    srv_name = malloc(srv_name_size);
    if (!srv_name)
	return EAI_MEMORY;

    /* construct domain name for SRV query */
    snprintf(srv_name, srv_name_size, "_%s._%s.%s", service,
	    hints->ai_socktype == SOCK_STREAM ? "tcp" : "udp", node);

#ifdef WIN32
    rc = DnsQuery(srv_name, DNS_TYPE_SRV, DNS_QUERY_STANDARD, NULL, &resp, NULL);
#else
    rlen = res_search(srv_name, ns_c_in, ns_t_srv, resp, sizeof(resp));
#endif
    free(srv_name);

    /* fallback if DNS query does not return a single entry */
#ifdef WIN32
    if (rc != ERROR_SUCCESS)
#else
    if (rlen < 1)
#endif
	return getaddrinfo(node, service, hints, res);

#ifndef WIN32
    /* fallback in case we cannot parse the response */
    if (ns_initparse(resp, rlen, &nsb) < 0)
	return getaddrinfo(node, service, hints, res);
#endif

    /* iterate through result set -- might be incomplete */
#ifdef WIN32
    for (entry = resp; entry != NULL; entry = entry->pNext) {
	switch (entry->wType) {
	case DNS_TYPE_SRV:
	    snprintf(tgt_port, sizeof(tgt_port), "%d", entry->Data.SRV.wPort);
	    tgt_name = entry->Data.SRV.pNameTarget;
#else
    for (i = 0; i < ns_msg_count(nsb, ns_s_an); i++) {
	rc = ns_parserr(&nsb, ns_s_an, i, &rrb);
	if (rc < 0)
	    continue;

	if (ns_rr_class(rrb) != ns_c_in)
	    continue;

	switch (ns_rr_type(rrb)) {
	case ns_t_srv:
	    rrlen = ns_rr_rdlen(rrb);
	    if (rrlen < 8)	/* 2+2+2 and at least 2 for host */
		break;

	    /* extract host and port */
	    p = ns_rr_rdata(rrb);
	    rc = dn_expand(resp, resp+rlen, p+6, tgt_name, sizeof(tgt_name));
	    if (rc < 2)
		break;
	    snprintf(tgt_port, sizeof(tgt_port), "%u", 
		    (unsigned int)p[4] << 8 | (unsigned int)p[5]);
#endif

	    /* resolve and add to end of list */
	    if (getaddrinfo(tgt_name, tgt_port, hints, ai_last) != 0)
		break;

	    /* get end of list */
	    for (ai=*ai_last; ai != NULL; ai = ai->ai_next)
		ai_last = &(ai->ai_next);

	    break;
	default:    /* someone stupid might use CNAME */
	    break;
	}
    }

#ifdef WIN32
    /* free result set */
    DnsRecordListFree(resp, DnsFreeRecordList);
#endif

    /* fallback in case no resolution via SRV is possible */
    if (ai_last == res)
	return getaddrinfo(node, service, hints, res);

    return 0;
}
