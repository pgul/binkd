/*
 *  ftnq.h -- BSO interface
 *
 *  ftnq.h is a part of binkd project
 *
 *  Copyright (C) 1996-1998  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

/* $Id$
 *
 * $Log$
 * Revision 2.5  2003/06/20 10:37:02  val
 * Perl hooks for binkd - initial revision
 *
 * Revision 2.4  2003/05/28 14:32:57  gul
 * new function q_add_last_file() - add file to the end of queue
 *
 * Revision 2.3  2003/03/01 18:04:30  gul
 * Remove redundrant include <sys/time.h>
 *
 * Revision 2.2  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.1  2003/02/22 12:56:00  gul
 * Do not give unsecure mail to secuse link when send-if-pwd
 *
 * Revision 2.0  2001/01/10 12:12:38  gul
 * Binkd is under CVS again
 *
 * Revision 1.8  1998/05/05  23:59:33  mff
 * Added comment about FTNQ.time.
 *
 * Revision 1.7  1997/10/23  04:04:45  mff
 * q_not_empty() returns FTN_NODE now
 *
 * Revision 1.6  1997/06/16  05:47:13  mff
 * Remove old .bsy/.csy files, queue handling changed again.
 *
 * Revision 1.5  1997/02/13  07:08:39  mff
 * Support for fileboxes
 *
 * Revision 1.2  1996/12/14  07:04:39  mff
 * Addedd q_scan_addrs()
 */
#ifndef _ftnq_h
#define _ftnq_h

#include <stdio.h>
#include "Config.h"
#include "ftnaddr.h"
#include "ftnnode.h"

typedef struct _FTNQ FTNQ;
struct _FTNQ
{
  FTNQ *next;
  FTNQ *prev;

  FTN_ADDR fa;
  char flvr;			       /* 'I', 'i', 'C', 'c', 'D', 'd', 'O',
				        * 'o', 'F', 'f', 'H', 'h' */
  char action;			       /* 'd'elete, 't'runcate, '\0' -- none,
				        * remove on 's'uccessful session,
				        * after 'a'ny session */
  char type;			       /* 'm'ail (.out), .f'l'o, '*' -- a
				        * file from .flo (just for stats, it
				        * will never be selected for sending
				        * right from the queue, it will be
				        * send when parsing its .flo instead,
				        * now it's obsolete), other -- a file
				        * to send. */
  char path[MAXPATHLEN + 1];
  unsigned long size;
  time_t time;			       /* this field seems to be used only in
				        * cmp_filebox_files(), when sorting
				        * files from a filebox before sending */

  int sent;			       /* == 1, if the file have been sent */
};

#define SCAN_LISTED ((FTNQ*)-1)

/*
 * Scans outbound. Return value must be q_free()'d.
 */
FTNQ *q_scan (FTNQ *q);
void q_free (FTNQ *q);

/*
 * Add a file to the queue.
 */
FTNQ *q_add_file (FTNQ *q, char *filename, FTN_ADDR *fa1, char flvr, char action, char type);

/*
 * Add a file to the end of queue.
 */
FTNQ *q_add_last_file (FTNQ *q, char *filename, FTN_ADDR *fa1, char flvr, char action, char type);

/*
 * Adds to the q all files for n akas stored in fa
 */
FTNQ *q_scan_addrs (FTNQ *q, FTN_ADDR *fa, int n, int to);

/*
 * Scans fileboxes for n akas stored in fa
 */
FTNQ *q_scan_boxes (FTNQ *q, FTN_ADDR *fa, int n);

/*
 * 0 = the queue is empty.
 */
FTN_NODE *q_not_empty (void);

/*
 * Selects a node to make the next call. (It's alost like
 * q_not_empty(), but it will never select a node twice)
 */
FTN_NODE *q_next_node (void);

/*
 * Selects from q the next file for fa (Returns a pointer to a q element)
 */
FTNQ *select_next_file (FTNQ *q, FTN_ADDR *fa, int nAka);

/*
 * Just lists q, not more
 */
void q_list (FILE *out, FTNQ *q);

/*
 * Creates an empty .?lo
 */
int create_poll (FTN_ADDR *fa, int flvr);

/*
 * Set .hld for a node
 */
void hold_node (FTN_ADDR *fa, time_t hold_until);

/*
 * get size of files in the queue
 */
void q_get_sizes (FTNQ *q, unsigned long *netsize, unsigned long *filessize);

#define FQ_ZERO(x) (memset(x, 0, sizeof(*(x))), FA_ZERO(&((x)->fa)))
#define FQ_ISNULL(x) (FA_ISNULL(&((x)->fa)))


/*
 * Is F a good flavour letter?
 */
extern const char prio[];

#define isflvr(F) ((F) && strchr(prio,(F)))

/*
 * Compare flavours. `I' is the best, `\0' is the worst.
 */
#define MAXFLVR(a,b) ((strchr(prio, (a)) < strchr(prio, (b))) ? (a) : (b))

enum bad_try_type { BAD_NA, BAD_CALL, BAD_MERR, BAD_MBSY, BAD_IO, BAD_TIMEOUT, 
                    BAD_AKA, BAD_AUTH };

void bad_try (FTN_ADDR *fa, const char *error, const int where);
void good_try (FTN_ADDR *fa, char *comment);
void read_try (FTN_ADDR *fa, unsigned *nok, unsigned *nbad);
void write_try (FTN_ADDR *fa, unsigned *nok, unsigned *nbad, char *comment);

#endif
