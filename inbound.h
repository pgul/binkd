#ifndef _inbound_h
#define _inbound_h

#include "protoco2.h"

/*
 * Checks if the file already exists in our inbound. !0=ok, 0=failed.
 * Sets realname[MAXPATHLEN]
 */
int inb_test (char *filename, boff_t size, time_t t,
              char *inbound, char *realname,
              enum renamestyletype ren_style);

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
