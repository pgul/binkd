/*
 * $Id$
 *
 * $Log$
 * Revision 2.1  2003/03/10 10:39:23  gul
 * New include file common.h
 *
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 *
 */
#ifndef _servmgr_h
#define _servmgr_h

#include "iphdr.h"

/*
 * Listens... Than calls protocol()
 */
void servmgr(void *arg);

extern SOCKET sockfd;

#endif
