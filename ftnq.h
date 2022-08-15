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

#ifndef _ftnq_h
#define _ftnq_h

#include "btypes.h"

#define SCAN_LISTED ((FTNQ*)-1)

/*
 * Scans outbound. Return value must be q_free()'d.
 */
FTNQ *q_scan (FTNQ *q, BINKD_CONFIG *config);
void q_free (FTNQ *q, BINKD_CONFIG *config);

/*
 * Add a file to the queue.
 */
FTNQ *q_add_file (FTNQ *q, char *filename, FTN_ADDR *fa1, char flvr, char action, char type, BINKD_CONFIG *config);

/*
 * Add a file to the end of queue.
 */
FTNQ *q_add_last_file (FTNQ *q, char *filename, FTN_ADDR *fa1, char flvr, char action, char type, BINKD_CONFIG *config);

/*
 * Adds to the q all files for n akas stored in fa
 */
FTNQ *q_scan_addrs (FTNQ *q, FTN_ADDR *fa, int n, int to, BINKD_CONFIG *config);

/*
 * Scans fileboxes for n akas stored in fa
 */
FTNQ *q_scan_boxes (FTNQ *q, FTN_ADDR *fa, int n, int to, BINKD_CONFIG *config);

/*
 * Sort files in the queue
 */
FTNQ *q_sort (FTNQ *q, FTN_ADDR *fa, int nAka, BINKD_CONFIG *cfg);

/*
 * 0 = the queue is empty.
 */
FTN_NODE *q_not_empty (BINKD_CONFIG *config);

/*
 * Selects a node to make the next call. (It's alost like
 * q_not_empty(), but it will never select a node twice)
 */
FTN_NODE *q_next_node (BINKD_CONFIG *config);

/*
 * Selects from q the next file for fa (Returns a pointer to a q element)
 */
FTNQ *select_next_file (FTNQ *q, FTN_ADDR *fa, int nAka);

/*
 * Just lists q, not more
 */
void q_list (FILE *out, FTNQ *q, BINKD_CONFIG *config);

/*
 * Creates an empty .?lo
 */
int create_poll (FTN_ADDR *fa, int flvr, BINKD_CONFIG *config);

/*
 * Set .hld for a node
 */
void hold_node (FTN_ADDR *fa, time_t hold_until, BINKD_CONFIG *config);

/*
 * get size of files in the queue
 */
void q_get_sizes (FTNQ *q, uintmax_t *netsize, uintmax_t *filessize);

/* 
 * Calculate quantity of freqs in the queue
 */
int q_freq_num (FTNQ *q);

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

void bad_try (FTN_ADDR *fa, const char *error, const int where, BINKD_CONFIG *config);
void good_try (FTN_ADDR *fa, char *comment, BINKD_CONFIG *config);
void read_try (FTN_ADDR *fa, unsigned *nok, unsigned *nbad, BINKD_CONFIG *config);
void write_try (FTN_ADDR *fa, unsigned *nok, unsigned *nbad, char *comment, BINKD_CONFIG *config);
void remove_try (FTN_ADDR *fa, BINKD_CONFIG *config);

#endif
