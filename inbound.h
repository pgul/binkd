#ifndef _inbound_h
#define _inbound_h

/*
 * Checks if the file already exists in our inbound. !0=ok, 0=failed.
 * Sets realname[MAXPATHLEN]
 */
int inb_test (char *filename, size_t size, time_t t,
	       char *inbound, char *realname);

/*
 * Open a partial file in the inbound directory for appending.
 * Creates it if the file does not exist
 */
FILE *inb_fopen (char *netname, size_t size, time_t t,
		  FTN_ADDR *from, int nfa, char *inbound);

/*
 * File is complete, rename it to it's realname. 1=ok, 0=failed.
 * Sets realname[MAXPATHLEN]
 */
int inb_done (char *netname, size_t size, time_t t,
	       FTN_ADDR *from, int nfa, char *inbound, char *realname);

/*
 * Remove partial file
 */
int inb_reject (char *netname, size_t size, time_t time,
		 FTN_ADDR *from, int nfa, char *inbound);

/*
 * Searches for the ``file'' in the inbound and returns it's tmp name in s.
 * S must have MAXPATHLEN chars. Returns 0 on error, 1=found, 2=created.
 */
int find_tmp_name (char *s, char *file, size_t size,
		    time_t time, FTN_ADDR *from, int nfa, char *inbound);

#endif
