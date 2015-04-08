#ifndef _protocol_h
#define _protocol_h

enum { P_NULL = 0, P_NONSECURE, P_SECURE, 
       P_NA = 0x100, P_WE_NONSECURE, P_REMOTE_NONSECURE };

void protocol(SOCKET s_in, SOCKET s_out, FTN_NODE *fn, FTN_ADDR *fa,
              char *current_addr, char *current_port, char *remote_ip, BINKD_CONFIG *config);

#endif
