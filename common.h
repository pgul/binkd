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

#ifdef WIN32
enum serviceflags{
   w32_noservice=0,
   w32_installservice=1,
   w32_uninstallservice=-1,
   w32_startservice=2,
   w32_stopservice=-2,
   w32_restartservice=3,
   w32_queryservice=4,
   w32_run_as_service=-4
 };
#define CTRL_SERVICESTOP_EVENT    254
#define CTRL_SERVICERESTART_EVENT 255
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

extern int n_servers, n_clients;
extern int binkd_exit;
extern int checkcfg_flag;	/* exit(3) on config change */
extern int pidcmgr;		/* pid for clientmgr */
extern int pidCmgr;             /* real pid for clientmgr (not 0) */
extern int pidsmgr;		/* pid for server */
extern char **saved_envp;
extern int no_MD5;
extern int no_crypt;
extern int server_flag, client_flag;
extern int poll_flag;
extern int inetd_flag;
extern int quiet_flag;
extern int verbose_flag;
#ifdef BINKD_DAEMONIZE
extern int daemon_flag;
#endif
#ifdef WIN32
#ifndef BINKD9X
extern int tray_flag;
#endif
#endif
#ifdef HAVE_FORK
extern int got_sigchld, got_sighup;
void sighandler(int signo);
void chld(int *childcount);
#define check_child(counter) if (got_sigchld) { blocksig(); chld(counter); unblocksig(); }
#else
#define check_child(counter)
#endif

#endif
