/*
 * $Id$
 *
 * $Log$
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 *
 */
#ifndef _protocol_h
#define _protocol_h

enum { P_NULL = 0, P_NONSECURE, P_SECURE };

void protocol(SOCKET s, FTN_NODE *fa);

#endif
