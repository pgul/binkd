#ifndef _protocol_h
#define _protocol_h

enum { P_NULL = 0, P_NONSECURE, P_SECURE };

void protocol(SOCKET s, FTN_NODE *fa);

#endif
