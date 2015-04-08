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
void perl_after_session(STATE *, int);  /* after session done */

int perl_before_recv(STATE *, boff_t offs); /* before receiving file */
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
