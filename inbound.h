/*
 * $Id$
 *
 * $Log$
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

/*
 * Checks if the file already exists in our inbound. !0=ok, 0=failed.
 * Sets realname[MAXPATHLEN]
 */
int inb_test (char *filename, off_t size, time_t t,
	       char *inbound, char *realname);

/*
 * Open a partial file in the inbound directory for appending.
 * Creates it if the file does not exist
 */
FILE *inb_fopen (char *netname, off_t size, time_t t,
		  FTN_ADDR *from, int nfa, char *inbound, int state);

/*
 * File is complete, rename it to it's realname. 1=ok, 0=failed.
 * Sets realname[MAXPATHLEN]
 */
int inb_done (char *netname, off_t size, time_t t,
	       FTN_ADDR *from, int nfa, char *inbound, char *realname);

/*
 * Remove partial file
 */
int inb_reject (char *netname, off_t size, time_t time,
		 FTN_ADDR *from, int nfa, char *inbound);

/*
 * Searches for the ``file'' in the inbound and returns it's tmp name in s.
 * S must have MAXPATHLEN chars. Returns 0 on error, 1=found, 2=created.
 */
int find_tmp_name (char *s, char *file, off_t size,
		    time_t time, FTN_ADDR *from, int nfa, char *inbound);

#endif
