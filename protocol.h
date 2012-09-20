/*
 * $Id$
 *
 * $Log$
 * Revision 2.4  2012/09/20 12:16:53  gul
 * Added "call via external pipe" (for example ssh) functionality.
 * Added "-a", "-f" options, removed obsoleted "-u" and "-i" (for win32).
 *
 * Revision 2.3  2003/08/26 16:06:27  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.2  2003/06/20 10:37:02  val
 * Perl hooks for binkd - initial revision
 *
 * Revision 2.1  2003/03/01 15:55:02  gul
 * Current outgoing address is now attibute of session, but not node
 *
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 *
 */
#ifndef _protocol_h
#define _protocol_h

enum { P_NULL = 0, P_NONSECURE, P_SECURE, 
       P_NA = 0x100, P_WE_NONSECURE, P_REMOTE_NONSECURE };

void protocol(SOCKET s_in, SOCKET s_out, FTN_NODE *fn, FTN_ADDR *fa, char *current_addr, BINKD_CONFIG *config);

#endif
