/*
 *  common.h -- common binkd declarations
 *
 *  common.h is a part of binkd project
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

/*
 * $Id$
 *
 * $Log$
 * Revision 2.3  2003/05/01 09:55:01  gul
 * Remove -crypt option, add global -r option (disable crypt).
 *
 * Revision 2.2  2003/03/10 10:57:45  gul
 * Extern declarations moved to header files
 *
 * Revision 2.1  2003/03/10 10:34:52  gul
 * *** empty log message ***
 *
 *
 */
#ifndef _common_h
#define _common_h

#include "iphdr.h"

#ifdef HAVE_THREADS
int add_socket(SOCKET sockfd);
int del_socket(SOCKET sockfd);
#else
#define add_socket(sockfd)
#define del_socket(sockfd)
#endif

#if defined(OS2) && defined(HAVE_THREADS)
void rel_grow_handles(int nh);
#else
#define rel_grow_handles(nh)
#endif

/*
 * Get free space in a directory
 */
unsigned long getfree (char *path);

/*
 * Set up break handler, set up exit list if needed
 */
int set_break_handlers (void);

/*
 * Runs a new thread or forks
 */
int branch (void (*) (void *), void *, size_t);

/*
 * From breaksig.c -- binkd runs this from exitlist or
 * from signal handler (Under NT)
 */
void exitfunc (void);

int checkcfg (void);

extern int n_servers, n_clients;
extern int binkd_exit;
extern int checkcfg_flag;	/* exit(3) on config change */
extern int pidcmgr;		/* pid for clientmgr */
extern int pidsmgr;		/* pid for server */
extern int no_MD5;
extern int no_crypt;
extern int server_flag, client_flag;
extern int poll_flag;
extern int inetd_flag;
extern int isService;

#endif
