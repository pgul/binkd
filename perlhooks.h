/*
 *  perlhooks.h -- perl-hooks interface
 *
 *  perlhooks.h is a part of binkd project
 *
 *  Copyright (C) 2003  val khokhlov, FIDONet 2:550/180
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
 * Revision 2.15  2009/06/02 17:09:35  gul
 * Build binkd for OS/2 with perl support
 *
 * Revision 2.14  2009/05/31 07:16:17  gul
 * Warning: many changes, may be unstable.
 * Perl interpreter is now part of config and rerun on config reload.
 * Perl 5.10 compatibility.
 * Changes in outbound queue managing and sorting.
 *
 * Revision 2.13  2009/05/26 13:04:35  gul
 * New perl hooks:
 * need_reload() - is it needed to reload config
 * config_loaded() - after successful reading config
 *
 * Revision 2.12  2005/10/03 06:49:04  gul
 * Fixed typos in previous patch
 *
 * Revision 2.11  2005/10/02 21:47:35  gul
 * setup_rlimit() perl hook
 *
 * Revision 2.10  2005/09/23 12:24:33  gul
 * define $hosts variable for on_call() perl hook (can be changed).
 * Changes for $proxy and $socks are now local for the single outgoing call.
 *
 * Revision 2.9  2005/03/28 10:15:13  val
 * manage proxy/socks via perl-hook on_call()
 *
 * Revision 2.8  2003/10/30 10:57:46  gul
 * Change inb_done arguments, optimize a bit
 *
 * Revision 2.7  2003/10/30 10:37:00  gul
 * Do not append file partially received from busy remote aka,
 * non-destructive skip it.
 *
 * Revision 2.6  2003/10/29 21:08:39  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.5  2003/09/05 06:49:06  val
 * Perl support restored after config reloading patch
 *
 * Revision 2.4  2003/08/18 09:41:00  gul
 * Little cleanup in handle perl errors
 *
 * Revision 2.3  2003/08/18 07:29:09  val
 * multiple changes:
 * - perl error handling made via fork/thread
 * - on_log() perl hook
 * - perl: msg_send(), on_send(), on_recv()
 * - unless using threads define log buffer via xalloc()
 *
 * Revision 2.2  2003/08/13 08:20:45  val
 * try to avoid mixing Log() output and Perl errors in stderr
 *
 * Revision 2.1  2003/06/20 10:37:02  val
 * Perl hooks for binkd - initial revision
 *
*/
#ifndef _PERLHOOKS_H_
#define _PERLHOOKS_H_

#include "prothlp.h"
#include "protoco2.h"

enum perl_skip_type { SKIP_ND=1, SKIP_D=2 };

extern char *perl_subnames[];  /* names for perl subs */

#if defined(HAVE_THREADS) && defined(PERL_MULTITHREAD)
#include "sem.h"
extern MUTEXSEM perlsem;
#endif

int perl_init(char *, BINKD_CONFIG *); /* init root perl, parse hooks file, return success */
void perl_done(BINKD_CONFIG *, int);   /* deinit perl */
#ifdef HAVE_THREADS
void *perl_init_clone(BINKD_CONFIG *); /* clone root perl */
void perl_done_clone(void *);          /* destruct a clone */
#endif

void perl_on_start(BINKD_CONFIG *cfg); /* start, after init */
int perl_on_call(FTN_NODE *, BINKD_CONFIG *, char **hosts
#ifdef HTTPS
		 , char **proxy, char **socks
#endif
			); /* before outgoing call */
int perl_on_error(BINKD_CONFIG *, FTN_ADDR *, const char *, const int); /* on errors: bad_try() */
char *perl_on_handshake(STATE *);       /* before xmitting ADR */
char *perl_after_handshake(STATE *);    /* after handshake complete */
void perl_after_session(STATE *, char *); /* after session done */

int perl_before_recv(STATE *, off_t offs); /* before receiving file */
int perl_after_recv(STATE *, TFILE *, char *tmp_name,
		char *real_name);  /* after file has been received */
int perl_before_send(STATE *);         /* before sending file */
int perl_after_sent(STATE *, int);     /* after file has been sent */

int perl_on_log(char *, int, int *);   /* when writing string to log */

int perl_on_send(STATE *, t_msg *, char **, char **); /* on msg_send2 */
int perl_on_recv(STATE *, char *, int);               /* when recv a block */

void perl_config_loaded(BINKD_CONFIG *cfg); /* config loaded */
int perl_need_reload(BINKD_CONFIG *, struct conflist_type *, int);    /* need to reload config? */

#ifdef BW_LIM
int perl_setup_rlimit(STATE *, BW *, char *);
#endif

#endif
