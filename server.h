/*
 * $Id$
 *
 * $Log$
 * Revision 2.3  2003/08/26 16:06:27  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.2  2003/03/10 10:57:45  gul
 * Extern declarations moved to header files
 *
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

/*
 * Listens... Than calls protocol()
 */
void servmgr(void);

extern SOCKET sockfd;
extern int ext_rand;

#endif
