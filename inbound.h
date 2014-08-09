/*
 * $Id$
 *
 * $Log$
 * Revision 2.8.2.1  2014/08/09 15:17:43  gul
 * Large files support on Win32 (backport from develop branch)
 *
 * Revision 2.8  2005/07/04 18:24:43  gul
 * Move events checking and running to inb_test() for reducing repeated code;
 * do not run immediate events twice;
 * fixed argus-style freqs (not tested).
 *
 * Revision 2.7  2004/10/19 16:28:19  gul
 * Do not remove complete received but not renamed partial files
 * for prevent data loss in ND-mode.
 * Remove all partial files for node after successfull session.
 *
 * Revision 2.6  2003/10/30 10:57:46  gul
 * Change inb_done arguments, optimize a bit
 *
 * Revision 2.5  2003/10/30 10:36:59  gul
 * Do not append file partially received from busy remote aka,
 * non-destructive skip it.
 *
 * Revision 2.4  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.3  2003/06/20 10:37:02  val
 * Perl hooks for binkd - initial revision
 *
 * Revision 2.2  2003/03/01 18:29:52  gul
 * Change size_t to off_t for file sizes and offsets
 *
 * Revision 2.1  2002/11/22 14:40:42  gul
 * Check free space on inbox if defined
 *
 * Revision 2.0  2001/01/10 12:12:38  gul
 * Binkd is under CVS again
 *
 *
 */
#ifndef _inbound_h
#define _inbound_h

#include "protoco2.h"

/*
 * Checks if the file already exists in our inbound. !0=ok, 0=failed.
 * Sets realname[MAXPATHLEN]
 */
int inb_test (char *filename, boff_t size, time_t t,
	       char *inbound, char *realname);

/*
 * Open a partial file in the inbound directory for appending.
 * Creates it if the file does not exist
 */
FILE *inb_fopen (STATE *state, BINKD_CONFIG *config);

/*
 * File is complete, rename it to it's realname. 1=ok, 0=failed.
 * Sets realname[MAXPATHLEN]
 */
int inb_done (TFILE *file, STATE *state, BINKD_CONFIG *config);

/*
 * Remove partial file
 */
int inb_reject (STATE *state, BINKD_CONFIG *config);

/*
 * Remove all partial files for remote
 */
void inb_remove_partial (STATE *state, BINKD_CONFIG *config);

#endif
