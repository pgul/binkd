/*
 *  perlhooks.c -- perl-hooks interface
 *
 *  perlhooks.c is a part of binkd project
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
 * Revision 2.3  2003/06/27 07:45:36  val
 * fix to make with perl 5.6.0
 *
 * Revision 2.2  2003/06/26 10:34:02  val
 * some tips from mod_perl, maybe prevent perl from crashing in client mgr (win32)
 *
 * Revision 2.1  2003/06/20 10:37:02  val
 * Perl hooks for binkd - initial revision
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#ifndef _MSC_VER
#include <sys/wait.h>
#endif
#ifdef __OS2__
#define INCL_DOSPROCESS
#include <os2.h>
#endif

#ifdef _MSC_VER
#undef __STDC__
#include <sys/types.h>
#endif

#if defined(__NT__) && !defined(WIN32) /* WIN32 needed for perl-core include files */
#  define WIN32
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef _MSC_VER
# define NO_XSLOCKS
#endif
#ifndef _MSC_VER
# include <unistd.h>
#endif
//#ifdef _MSC_VER
//# include "win32iop.h"
//#endif
#if defined(__cplusplus)
}     /* extern "C" closed */
# ifndef EXTERN_C
#    define EXTERN_C extern "C"
#  endif
#else
#  ifndef EXTERN_C
#    define EXTERN_C extern
#  endif
#endif

/* ---------------- binkd stuff --------------- */
#include "sys.h"
#include "readcfg.h"
#include "tools.h"
#include "ftnaddr.h"
#include "ftndom.h"
#include "ftnnode.h"
#include "readflo.h"
#include "prothlp.h"
#include "protocol.h"
#include "protoco2.h"
#include "perlhooks.h"
/* ---------------- perl stuff --------------- */

#include <EXTERN.h>
#include <perl.h>
#if defined(__cplusplus)
extern "C" {
#endif
#include <XSUB.h>
#if defined(__cplusplus)
}     /* extern "C" closed */
#endif

/* perl prior to 5.6 support */
#ifndef get_sv
#  define get_sv perl_get_sv
#endif
  
#ifndef newSVuv
#  define newSVuv newSViv
#endif

#ifndef sv_undef
#  define sv_undef PL_sv_undef
#endif

#ifdef __GNUC__
#  define Perl___notused Perl___notused __attribute__ ((unused))
#endif

#if defined(HAVE_FORK) && !defined(Perl_get_context)
#  define Perl_get_context() perl
#endif
/* =========================== vars ================================== */

static PerlInterpreter *perl = NULL;        /* root object for all threads */

/* bits for subroutines, must correspond to perl_subnames */
typedef enum { 
  PERL_ON_START, PERL_ON_EXIT, PERL_ON_CALL, PERL_ON_ERROR,
  PERL_ON_HANDSHAKE, PERL_AFTER_HANDSHAKE, PERL_AFTER_SESSION, 
  PERL_BEFORE_RECV, PERL_AFTER_RECV, PERL_BEFORE_SEND, PERL_AFTER_SENT,
  PERL_ON_LOG
} perl_subs;
/* if 0, perl is disabled; if non-0, some subs are enabled */
int perl_ok = 0;
/* sub bit to corresponding name */
char *perl_subnames[] = {
  "on_start",
  "on_exit",
  "on_call",
  "on_error",
  "on_handshake",
  "after_handshake",
  "after_session",
  "before_recv",
  "after_recv",
  "before_send",
  "after_sent",
  "on_log"
};
/* if set, queue is managed from perl: sorting, etc (binkd logic is off) */
int perl_manages_queue = 0;
/* if set, export queue to perl subs and optionally refresh back */
int perl_wants_queue = 0;
/* constants */
struct perl_const { char *name; int value; } perl_consts[] = {
  { "SECURE", P_SECURE },
  { "NONSECURE", P_NONSECURE },
  { "WE_NONSECURE", P_WE_NONSECURE },
  { "REMOTE_NONSECURE", P_REMOTE_NONSECURE },
  { "OK", 1 },
  { "FAILED", 0 },
  { "BAD_CALL", BAD_CALL },
  { "BAD_MERR", BAD_MERR },
  { "BAD_MBSY", BAD_MBSY },
  { "BAD_IO", BAD_IO },
  { "BAD_TIMEOUT", BAD_TIMEOUT },
  { "BAD_AKA", BAD_AKA },
  { "BAD_AUTH", BAD_AUTH }
};

/* log levels */
#define LL_ERR  1
#define LL_WARN 2
#define LL_INFO 3
#define LL_LOG  3
#define LL_DBG  7
#define LL_DBG2 9

/* ---------------------------- defines ------------------------------- */

#define VK_ADD_intz(_sv, _name, _v)                 \
  if ( (_sv = perl_get_sv(_name, TRUE)) != NULL ) { \
    sv_setiv(_sv, _v); SvREADONLY(_sv);             \
  }
#define VK_ADD_strz(_sv, _name, _v)                 \
  if ( (_sv = perl_get_sv(_name, TRUE)) != NULL ) { \
    sv_setpv(_sv, _v ? _v : ""); SvREADONLY(_sv);   \
  }
#define VK_ADD_str(_sv, _name, _v)                            \
  if ( (_sv = perl_get_sv(_name, TRUE)) != NULL ) {           \
    if (_v) sv_setpv(_sv, _v); else sv_setsv(_sv, &sv_undef); \
    SvREADONLY(_sv);   \
  }
#define B4(l) (l>>24 & 0xff), (l>>16 & 0xff), (l>>8 & 0xff), (l & 0xff)
#define VK_ADD_ip(_sv, _name, _v)                             \
  if ( (_sv = perl_get_sv(_name, TRUE)) != NULL ) {           \
    if (_v) sv_setpvf(_sv, "%u.%u.%u.%u", B4(ntohl(_v)));     \
    else sv_setsv(_sv, &sv_undef);                            \
    SvREADONLY(_sv);                                          \
  }

#define VK_ADD_HASH_sv(_hv,_sv,_name)                  \
    if (_sv != NULL) {                                 \
      SvREADONLY_on(_sv);                              \
      hv_store(_hv, _name, strlen(_name), _sv, 0);     \
    }
#define VK_ADD_HASH_str(_hv,_sv,_name,_value)                            \
    if ( (_value != NULL) && (_sv = newSVpv(_value, 0)) != NULL ) {      \
      SvREADONLY_on(_sv);                                                \
      hv_store(_hv, _name, strlen(_name), _sv, 0);                       \
    }                                                                    \
    else hv_store(_hv, _name, strlen(_name), &sv_undef, 0);
#define VK_ADD_HASH_intz(_hv,_sv,_name,_value)                           \
    if ( (_sv = newSViv(_value)) != NULL ) {                             \
      SvREADONLY_on(_sv);                                                \
      hv_store(_hv, _name, strlen(_name), _sv, 0);                       \
    }
#define VK_ADD_HASH_int(_hv,_sv,_name,_value)                            \
    if (_value) { VK_ADD_HASH_intz(_hv,_sv,_name,_value) }

/* =========================== err handling ========================== */

/* since multi-thread handlers don't work anyway, use single copy ;) */
static int perl_olderr, perl_errpipe[2];

/* set up perl errors handler, redirect stderr to pipe */
/* !!! DON'T CALL FROM THREADS !!! */
static void handle_perlerr(int *olderr, int *errpipe) {
  *olderr = dup(fileno(stderr));
#if defined(UNIX) || defined(__OS2__)
  pipe(errpipe);
#elif defined(_MSC_VER)
  _pipe(errpipe, 0, 64);
#endif
  dup2(errpipe[1], fileno(stderr));
}

/* restore perl errors handler, read pipe to var and restore stderr */
/* !!! DON'T CALL FROM THREADS !!! */
static void restore_perlerr(int *olderr, int *errpipe) {
char buf[256];
FILE *R;
  close(errpipe[1]);
  dup2(*olderr, fileno(stderr));
  R = fdopen(errpipe[0], "r");
  buf[0] = 0;
  while ( fgets(buf, 256, R) ) {
    int n = strlen(buf);
    if (n > 0 && buf[n-1] == '\n') buf[--n] = 0;
    if (n > 0) Log(LL_ERR, "Perl error: %s", buf);
  }
  fclose(R);
}

/* =========================== xs ========================== */
/* interface to Log() */
#ifdef _MSC_VER
  EXTERN_C void xs_init (pTHXo);
  EXTERN_C void boot_DynaLoader (pTHXo_ CV* cv);
  EXTERN_C void perl_Log(pTHXo_ CV* cv);
  EXTERN_C void perl_aeq(pTHXo_ CV* cv);
  EXTERN_C void perl_arm(pTHXo_ CV* cv);
#else
  static XS(boot_DynaLoader);
#endif

#ifdef _MSC_VER
  EXTERN_C void perl_Log(pTHXo_ CV* cv)
#else
  static XS(perl_Log)
#endif
{
  dXSARGS;
  char *str;
  int lvl;
  STRLEN n_a;

  if (items != 1 && items != 2)
  { Log(LL_ERR, "wrong params number to Log (need 1 or 2, exist %d)", items);
    XSRETURN_EMPTY;
  }
  if (items == 2) {
    lvl = SvUV(ST(0));
    str = (char *)SvPV(ST(1), n_a); if (n_a == 0) str = "";
  } else {
    lvl = LL_LOG;
    str = (char *)SvPV(ST(0), n_a); if (n_a == 0) str = "";
  }
  Log(lvl, "%s", str);
  XSRETURN_EMPTY;
}
/* returns 1 if the first addr matches to any of the rest */
#ifdef _MSC_VER
  EXTERN_C void perl_aeq(pTHXo_ CV* cv)
#else
  static XS(perl_aeq)
#endif
{
  dXSARGS;
  STRLEN len;
  int i;
  char *a, *b;
  FTN_ADDR A, B;

  if (items == 1) { 
    Log(LL_ERR, "aeq() requires 2 or more parameters, %d exist", items);
    XSRETURN_UNDEF;
  }
  a = (char *)SvPV(ST(0), len); 
  if (len == 0 || !parse_ftnaddress(a, &A)) XSRETURN_UNDEF;
  exp_ftnaddress(&A);
  for (i = 1; i < items; i++) {
    b = (char *)SvPV(ST(i), len);
    if (len == 0 || !parse_ftnaddress(b, &B)) continue;
    exp_ftnaddress(&B);
    if (ftnaddress_cmp(&A, &B) == 0) XSRETURN_IV(1);
  }
  XSRETURN_IV(0);
}
/* deletes all the addresses from the array (the first arg) */
#ifdef _MSC_VER
  EXTERN_C void perl_arm(pTHXo_ CV* cv)
#else
  static XS(perl_arm)
#endif
{
  dXSARGS;
  STRLEN len;
  int i, j, k, N, m;
  char *a, *b;
  FTN_ADDR A, *B;
  AV *arr;
  SV **svp;

  if (items == 1) { 
    Log(LL_ERR, "arm() requires 2 or more parameters, %d exist", items);
    XSRETURN_UNDEF;
  }
  if (!SvROK(ST(0)) || SvTYPE(SvRV(ST(0))) != SVt_PVAV) {
    Log(LL_ERR, "first parameter to arm() should be array reference");
    XSRETURN_UNDEF;
  }
  arr = (AV*)SvRV(ST(0));
  B = xalloc( (items-1)*sizeof(FTN_ADDR) );
  /* i - args, k - valid addresses */
  for (i = k = 0; i < items; i++) {
    b = (char *)SvPV(ST(i), len);
    if (len == 0 || !parse_ftnaddress(b, B+k)) continue;
    exp_ftnaddress(B+k);
    k++;
  }
  /* j - array elements, i - valid addresses */
  N = av_len(arr) + 1;
  for (j = m = 0; j < N; j++) {
    int found = 0;
    svp = av_fetch(arr, j, NULL);
    if (j != m) av_store(arr, m, (*svp));
    if (!svp) { m++; continue; }
    a = (char *)SvPV((*svp), len);
    if (len == 0 || !parse_ftnaddress(a, &A)) { m++; continue; }
    exp_ftnaddress(&A);
    /* compare */
    for (i = 0; i < k; i++)
      if (ftnaddress_cmp(&A, B+i) == 0) { found = 1; break; }
    if (!found) m++;
  }
  /* reduce array size */
  if (m != N) AvFILLp(arr) = m-1;
  free(B);
  XSRETURN_IV(N - m);
}
/* xs_init */
#ifdef _MSC_VER
EXTERN_C void xs_init (pTHXo)
#else
static void xs_init(void)
#endif
{
  static char *file = __FILE__;
#if defined(__OS2__)
  newXS("DB_File::bootstrap", boot_DB_File, file);
  newXS("Fcntl::bootstrap", boot_Fcntl, file);
  newXS("POSIX::bootstrap", boot_POSIX, file);
  newXS("SDBM_File::bootstrap", boot_SDBM_File, file);
  newXS("IO::bootstrap", boot_IO, file);
  newXS("OS2::Process::bootstrap", boot_OS2__Process, file);
  newXS("OS2::ExtAttr::bootstrap", boot_OS2__ExtAttr, file);
  newXS("OS2::REXX::bootstrap", boot_OS2__REXX, file);
#else
  dXSUB_SYS;
#endif
  newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
  newXS("Log", perl_Log, file);
  newXS("aeq", perl_aeq, file);
  newXS("arm", perl_arm, file);
}

/* =========================== sys ========================== */

/* init root perl, parse hooks file, return success */
int perl_init(char *perlfile) {
  int rc, i;
  SV *sv;
  char *perlargs[] = {"", NULL, NULL};
  char *cfgfile, *cfgpath=NULL, *patharg=NULL;

  Log(LL_DBG, "perl_init(): %s", perlfile);
  /* try to find out the actual path to perl script and set dir to -I */
  i = 1;
  perlargs[i++] = perlfile;
  /* check perm */
#ifdef _MSC_VER
  if (_access(perlfile, R_OK))
#else
  if (access(perlfile, R_OK))
#endif
  { return 0; }
  /* init perl */
  perl = perl_alloc();
  perl_construct(perl);
  handle_perlerr(&perl_olderr, perl_errpipe);
  rc = perl_parse(perl, xs_init, i, perlargs, NULL);
  restore_perlerr(&perl_olderr, perl_errpipe);
  Log(LL_DBG, "perl_init(): parse rc=%d", rc);
  /* can't parse */
  if (rc) {
    perl_destruct(perl);
    perl_free(perl);
    perl = NULL;
    return 0;
  }
  /* setup consts */
  for (i = 0; i < sizeof(perl_consts)/sizeof(perl_consts[0]); i++) {
    VK_ADD_intz(sv, perl_consts[i].name, perl_consts[i].value);
  }
  /* setup vars */
  perl_setup();
  /* run main program body */
  Log(LL_DBG, "perl_init(): running body");
  perl_run(perl);
  /* scan for present hooks */
  for (i = 0; i < sizeof(perl_subnames)/sizeof(perl_subnames[0]); i++) {
    if (perl_get_cv(perl_subnames[i], NULL)) perl_ok |= (1 << i);
  }
  /* run on_start() */
  perl_on_start();
  /* init perl queue management */
  if (sv = perl_get_sv("want_queue", FALSE)) { perl_wants_queue = SvTRUE(sv); }
  if (sv = perl_get_sv("manage_queue", FALSE)) { perl_manages_queue = SvTRUE(sv); }
  if (perl_manages_queue && !perl_wants_queue) {
    Log(LL_ERR, "Perl queue management requires $want_queue to be set");
    perl_manages_queue = 0;
  }
  Log(LL_DBG, "perl_init(): end");
  return 1;
}

/* set config vars to root perl */
void perl_setup(void) {
  SV 	*sv;
  HV 	*hv, *hv2;
  AV 	*av;
  char  buf[FTN_ADDR_SZ];
  int   i;

  if (!perl) return;
  Log(LL_DBG, "perl_setup(): perl context %p", Perl_get_context());

  hv = perl_get_hv("config", TRUE);
  hv_clear(hv);
  VK_ADD_HASH_str(hv, sv, "log", logpath);
  VK_ADD_HASH_intz(hv, sv, "loglevel", loglevel);
  VK_ADD_HASH_intz(hv, sv, "conlog", conlog);
  VK_ADD_HASH_intz(hv, sv, "tzoff", tzoff);
  VK_ADD_HASH_str(hv, sv, "sysname", sysname);
  VK_ADD_HASH_str(hv, sv, "sysop", sysop);
  VK_ADD_HASH_str(hv, sv, "location", location);
  VK_ADD_HASH_str(hv, sv, "nodeinfo", nodeinfo);
  VK_ADD_HASH_str(hv, sv, "bindaddr", bindaddr);
  VK_ADD_HASH_intz(hv, sv, "iport", iport);
  VK_ADD_HASH_intz(hv, sv, "oport", oport);
  VK_ADD_HASH_intz(hv, sv, "maxservers", max_servers);
  VK_ADD_HASH_intz(hv, sv, "maxclients", max_clients);
  VK_ADD_HASH_intz(hv, sv, "oblksize", oblksize);
  VK_ADD_HASH_str(hv, sv, "inbound", inbound);
  VK_ADD_HASH_str(hv, sv, "inbound_nonsecure", inbound_nonsecure);
  VK_ADD_HASH_intz(hv, sv, "inboundcase", inboundcase);
  VK_ADD_HASH_str(hv, sv, "temp_inbound", temp_inbound);
  VK_ADD_HASH_intz(hv, sv, "minfree", minfree);
  VK_ADD_HASH_intz(hv, sv, "minfree_nonsecure", minfree_nonsecure);
  VK_ADD_HASH_intz(hv, sv, "hold", hold);
  VK_ADD_HASH_intz(hv, sv, "hold_skipped", hold_skipped);
  VK_ADD_HASH_intz(hv, sv, "backresolv", backresolv);
  VK_ADD_HASH_intz(hv, sv, "send_if_pwd", send_if_pwd);
  VK_ADD_HASH_str(hv, sv, "filebox", tfilebox);
  VK_ADD_HASH_str(hv, sv, "brakebox", bfilebox);
  VK_ADD_HASH_str(hv, sv, "root_domain", root_domain);
  VK_ADD_HASH_int(hv, sv, "check_pkthdr", pkthdr_type);
  VK_ADD_HASH_str(hv, sv, "pkthdr_badext", pkthdr_bad);
  Log(LL_DBG2, "perl_setup(): %%config done");
  /* domain */
  hv = perl_get_hv("domain", TRUE);
  hv_clear(hv);
  {
    FTN_DOMAIN *cur = pDomains;
    while (cur) {
      hv2 = newHV();
      if (!cur->alias4) {
        VK_ADD_HASH_str(hv2, sv, "path", cur->path);
        VK_ADD_HASH_str(hv2, sv, "dir", cur->dir);
        VK_ADD_HASH_int(hv2, sv, "defzone", cur->z[0]);
      } else {
        VK_ADD_HASH_str(hv2, sv, "path", cur->alias4->path);
        VK_ADD_HASH_str(hv2, sv, "dir", cur->alias4->dir);
        VK_ADD_HASH_int(hv2, sv, "defzone", cur->alias4->z[0]);
      }
      sv = newRV_noinc( (SV*)hv2 );
      VK_ADD_HASH_sv(hv, sv, cur->name);
      cur = cur->next;
    }
  }
  Log(LL_DBG2, "perl_setup(): %%domain done");
  /* address -> me */
  av = perl_get_av("addr", TRUE);
  av_clear(av);
  for (i = 0; i < nAddr; i++) {
    ftnaddress_to_str(buf, &(pAddr[i]));
    sv = newSVpv(buf, 0);
    SvREADONLY_on(sv);
    av_push(av, sv);
  }
  Log(LL_DBG2, "perl_setup(): @addr done");
  /* ftrans */
  av = perl_get_av("ftrans", TRUE);
  av_clear(av);
  {
    RF_RULE *cur = rf_rules;
    while (cur) {
      hv2 = newHV();
      VK_ADD_HASH_str(hv2, sv, "from", cur->from);
      VK_ADD_HASH_str(hv2, sv, "to", cur->to);
      sv = newRV_noinc( (SV*)hv2 );
      av_push(av, sv);
      cur = cur->next;
    }
  }
  Log(LL_DBG2, "perl_setup(): @ftrans done");
  /* overwrite */
  av = perl_get_av("overwrite", TRUE);
  av_clear(av);
  {
    struct maskchain *cur = overwrite;
    while (cur) {
      sv = newSVpv(cur->mask, 0);
      SvREADONLY_on(sv);
      av_push(av, sv);
      cur = cur->next;
    }
  }
  Log(LL_DBG2, "perl_setup(): @overwrite done");
  /* skip */
  av = perl_get_av("skip", TRUE);
  av_clear(av);
  {
    struct skipchain *cur = skipmask;
    while (cur) {
      hv2 = newHV();
      VK_ADD_HASH_str(hv2, sv, "mask", cur->mask);
      VK_ADD_HASH_intz(hv2, sv, "type", cur->atype);
      VK_ADD_HASH_intz(hv2, sv, "size", cur->size);
      VK_ADD_HASH_intz(hv2, sv, "destr", cur->destr);
      sv = newRV_noinc( (SV*)hv2 );
      av_push(av, sv);
      cur = cur->next;
    }
  }
  Log(LL_DBG2, "perl_setup(): @skip done");
  /* share */
  hv = perl_get_hv("share", TRUE);
  hv_clear(hv);
  {
    SHARED_CHAIN *cur = shares;
    FTN_ADDR_CHAIN *fa;
    while (cur) {
      av = newAV();
      fa = cur->sfa;
      while (fa) {
        ftnaddress_to_str(buf, &(fa->fa));
        sv = newSVpv(buf, 0);
        SvREADONLY_on(sv);
        av_push(av, sv);
        fa = fa->next;
      }
      ftnaddress_to_str(buf, &(cur->sha));
      sv = newRV_noinc( (SV*)av );
      VK_ADD_HASH_sv(hv, sv, buf);
      cur = cur->next;
    }
  }
  Log(LL_DBG2, "perl_setup(): %%share done");
  /* node */
  hv = perl_get_hv("node", TRUE);
  hv_clear(hv);
  for (i = 0; i < nNod; i++) {
    hv2 = newHV();
    VK_ADD_HASH_str(hv2, sv, "hosts", pNod[i].hosts);
    VK_ADD_HASH_str(hv2, sv, "pwd", pNod[i].pwd);
    VK_ADD_HASH_str(hv2, sv, "ibox", pNod[i].ibox);
    VK_ADD_HASH_str(hv2, sv, "obox", pNod[i].obox);
    buf[0] = pNod[i].obox_flvr; buf[1] = 0;
    VK_ADD_HASH_str(hv2, sv, "obox_flvr", buf);
    VK_ADD_HASH_int(hv2, sv, "NR", pNod[i].NR_flag);
    VK_ADD_HASH_int(hv2, sv, "ND", pNod[i].ND_flag);
    VK_ADD_HASH_int(hv2, sv, "MD", pNod[i].MD_flag);
    VK_ADD_HASH_int(hv2, sv, "HC", pNod[i].HC_flag);
    VK_ADD_HASH_int(hv2, sv, "IP", pNod[i].restrictIP);
    sv = newRV_noinc( (SV*)hv2 );
    ftnaddress_to_str(buf, &(pNod[i].fa));
    VK_ADD_HASH_sv(hv, sv, buf);
  }
  Log(LL_DBG2, "perl_setup(): %%node done");
}

/* deallocate root perl, call on_exit() if master==1 */
void perl_done(int master) {
  Log(LL_DBG, "perl_done(): perl=%p", perl);
  if (perl) {
    /* run on_exit() */
    if (master) perl_on_exit();
    /* de-allocate */
    Log(LL_DBG, "perl_done(): destructing perl %p", perl);
#ifndef _MSC_VER
    perl_destruct(perl);
    perl_free(perl);
#endif
    perl = NULL;
    Log(LL_DBG, "perl_done(): end");
  }
}

#ifdef HAVE_THREADS
/* clone root perl */
void *perl_init_clone() {
  UV cflags = 0;
  PerlInterpreter *p;

  Log(LL_DBG2, "perl_init_clone(), parent perl=%p", perl);
  if (perl) {
    PERL_SET_CONTEXT(perl);
#if defined(WIN32) && defined(CLONEf_CLONE_HOST)
    cflags |= CLONEf_CLONE_HOST | CLONEf_KEEP_PTR_TABLE;
#endif
    p = perl_clone(perl, cflags);
    /* perl<5.6.1 hack, see http://www.apache.jp/viewcvs.cgi/modperl-2.0/src/modules/perl/modperl_interp.c.diff?r1=1.9&r2=1.10 */
    if (p) { 
      dTHXa(p); 
      PERL_SET_CONTEXT(aTHX);
      if (PL_scopestack_ix == 0) { ENTER; } 
    }
  }
  else p = NULL;
  Log(LL_DBG, "perl_init_clone(): new clone %p", p);
  return p;
}
/* destruct a clone */
void perl_done_clone(void *p) {
  Log(LL_DBG, "perl_done_clone(): destructing clone %p", p);
  if (p == NULL) return;
  PL_perl_destruct_level = 2;
  PERL_SET_CONTEXT((PerlInterpreter *)p); /* as in mod_perl */
  perl_destruct((PerlInterpreter *)p);
/* #ifndef WIN32 - mod_perl has it unless CLONEf_CLONE_HOST */
  perl_free((PerlInterpreter *)p);
/* #endif */
}
#endif

/* set array of fido addresses */
static void setup_addrs(char *name, int n, FTN_ADDR *p) {
  char  buf[FTN_ADDR_SZ];
  AV    *av;
  int   i;

  av = perl_get_av(name, TRUE);
  av_clear(av);
  for (i = 0; i < n; i++) {
    ftnaddress_to_str(buf, p+i);
    av_push(av, newSVpv(buf, 0));
  }
  SvREADONLY( (SV*)av );
}

/* set session vars */
static void setup_session(STATE *state, int lvl) {
  SV 	*sv;
  HV 	*hv, *hv2;
  AV 	*av;
  int   i;
  struct sockaddr_in sin;
  socklen_t sin_len = sizeof(sin);

  if (!perl) return;
  Log(LL_DBG2, "perl_setup_session(), perl context %p", Perl_get_context());
  if (!Perl_get_context()) return;

  /* lvl 1 */
  if (lvl >= 1 && state->perl_set_lvl < 1) {
    VK_ADD_intz(sv, "call", state->to != NULL);
    VK_ADD_intz(sv, "start", (int)state->start_time);
    VK_ADD_str (sv, "host", state->peer_name);
    if (getpeername(state->s, (struct sockaddr *)&sin, &sin_len) != -1)
      { VK_ADD_ip(sv, "ip", sin.sin_addr.s_addr); }
      else { VK_ADD_ip(sv, "ip", 0ul); }
    VK_ADD_ip(sv, "our_ip", state->our_ip);
    state->perl_set_lvl = 1;
  }
  /* lvl 2 */
  if (lvl >= 2 && state->perl_set_lvl < 2) {
    int secure;
    unsigned long netsize, filessize;
    if (state->state_ext != P_NA) secure = state->state_ext;
      else secure = state->state;
    VK_ADD_intz(sv, "secure", secure);
    VK_ADD_str (sv, "sysname", state->sysname);
    VK_ADD_str (sv, "sysop", state->sysop);
    VK_ADD_str (sv, "location", state->location);
    q_get_sizes (state->q, &netsize, &filessize);
    VK_ADD_intz(sv, "traf_mail", netsize);
    VK_ADD_intz(sv, "traf_file", filessize);
    hv = perl_get_hv("opt", TRUE);
    hv_clear(hv);
    VK_ADD_HASH_intz(hv, sv, "ND", state->ND_flag);
    VK_ADD_HASH_intz(hv, sv, "NR", state->NR_flag);
    VK_ADD_HASH_intz(hv, sv, "MD", state->MD_flag);
    VK_ADD_HASH_intz(hv, sv, "crypt", state->crypt_flag);
    setup_addrs("he", state->nfa, state->fa);
    if (state->nAddr && state->pAddr)
      setup_addrs("me", state->nAddr, state->pAddr);
      else setup_addrs("me", nAddr, pAddr);
    state->perl_set_lvl = 2;
  }
  /* lvl 3 */
  if (lvl >= 3 && state->perl_set_lvl < 3) {
    VK_ADD_intz(sv, "bytes_rcvd", state->bytes_rcvd);
    VK_ADD_intz(sv, "bytes_sent", state->bytes_sent);
    VK_ADD_intz(sv, "files_rcvd", state->files_rcvd);
    VK_ADD_intz(sv, "files_sent", state->files_sent);
  }
  Log(LL_DBG2, "perl_setup_session() end");
}

/* setup a queue */
static void setup_queue(STATE *state, FTNQ *queue) {
  char  buf[FTN_ADDR_SZ];
  AV   *av;
  HV   *hv;
  SV   *sv;
  FTNQ *q;

  Log(LL_DBG2, "perl_setup_queue()");
  av = perl_get_av("queue", TRUE);
  av_clear(av);
  for (q = queue; q; q = q->next) {
    hv = newHV();
    VK_ADD_HASH_str (hv, sv, "file", q->path);
    VK_ADD_HASH_intz(hv, sv, "size", q->size);
    VK_ADD_HASH_intz(hv, sv, "time", q->time);
    VK_ADD_HASH_intz(hv, sv, "sent", q->sent);
    buf[0] = q->flvr; buf[1] = 0;
    VK_ADD_HASH_str (hv, sv, "flvr", buf);
    buf[0] = q->action; buf[1] = 0;
    VK_ADD_HASH_str (hv, sv, "action", buf);
    buf[0] = q->type; buf[1] = 0;
    VK_ADD_HASH_str (hv, sv, "type", buf);
    ftnaddress_to_str(buf, &(q->fa));
    VK_ADD_HASH_str (hv, sv, "addr", buf);
    sv = newRV_noinc( (SV*)hv );
    av_push(av, (SV*)sv);
  }
  Log(LL_DBG2, "perl_setup_queue() end");
}

/* refresh queue */
static FTNQ *refresh_queue(STATE *state, FTNQ *queue) {
  FTNQ *q = NULL, *qp = NULL, *q0 = NULL;
  AV *av;
  HV *hv;
  SV **svp;
  STRLEN len;
  int i, n;
  char *s;

  Log(LL_DBG2, "perl_refresh_queue()");
  av = perl_get_av("queue", FALSE);
  if (!av) { Log(LL_DBG2, "perl_refresh_queue(): @queue undefined"); return queue; }
  n = av_len(av) + 1;
  for (i = 0; i < n; i++) {
    svp = av_fetch(av, i, 0);
    if (!svp || !SvROK(*svp)) continue;
    hv = (HV*)SvRV(*svp);
    if (SvTYPE(hv) != SVt_PVHV) continue;
    svp = hv_fetch(hv, "file", 4, 0);
    if (!svp || !SvOK(*svp)) continue;
    qp = q;
    q = xalloc( sizeof(FTNQ) ); FQ_ZERO(q);
    if (!q0) q0 = q;
      else { qp->next = q; q->prev = qp; }
    s = SvPV(*svp, len);
    strnzcpy(q->path, s, MAXPATHLEN);
    svp = hv_fetch(hv, "size", 4, 0);
    if (!svp || !SvOK(*svp)) q->size = 0;
      else q->size = SvIV(*svp);
    svp = hv_fetch(hv, "time", 4, 0);
    if (!svp || !SvOK(*svp)) q->time = 0;
      else q->time = SvIV(*svp);
    svp = hv_fetch(hv, "sent", 4, 0);
    if (svp && SvOK(*svp)) q->sent = SvIV(*svp); else q->sent = 0;
    svp = hv_fetch(hv, "flvr", 4, 0);
    if (svp && SvOK(*svp)) { s = SvPV(*svp, len); q->flvr = s[0]; }
      else q->flvr = 'f';
    svp = hv_fetch(hv, "action", 6, 0);
    if (svp && SvOK(*svp)) { s = SvPV(*svp, len); q->action = s[0]; }
      else q->action = 0;
    svp = hv_fetch(hv, "type", 4, 0);
    if (svp && SvOK(*svp)) { s = SvPV(*svp, len); q->type = s[0]; }
      else q->type = 0;
    svp = hv_fetch(hv, "addr", 4, 0);
    if (svp && SvOK(*svp)) {
      s = SvPV(*svp, len);
      if (!parse_ftnaddress(s, &(q->fa))) q->fa = state->fa[0];
    } else q->fa = state->fa[0];
  }
  if (queue != SCAN_LISTED) q_free(queue);
  Log(LL_DBG2, "perl_refresh_queue() end");
  return q0;
}

/* =========================== hooks ========================== */

/* start, after init */
void perl_on_start(void) {
  STRLEN n_a;

  if (perl_ok & (1 << PERL_ON_START)) {
     Log(LL_DBG, "perl_on_start(), perl=%p", Perl_get_context());
#ifndef HAVE_THREADS
     handle_perlerr(&perl_olderr, perl_errpipe);
#endif
     { dSP;
       ENTER;
       SAVETMPS;
       PUSHMARK(SP);
       PUTBACK;
       perl_call_pv(perl_subnames[PERL_ON_START], G_EVAL|G_VOID);
       SPAGAIN;
       PUTBACK;
       FREETMPS;
       LEAVE;
     }
#ifndef HAVE_THREADS
     restore_perlerr(&perl_olderr, perl_errpipe);
#endif
     if (SvTRUE(ERRSV))
     {
       Log(LL_ERR, "Perl on_start() error: %s", SvPV(ERRSV, n_a));
     }
     Log(LL_DBG, "perl_on_start() end");
  }
}

/* exit, just before destruction */
void perl_on_exit(void) {
  STRLEN n_a;

  if (perl_ok & (1 << PERL_ON_EXIT)) {
#ifdef HAVE_THREADS
     PERL_SET_CONTEXT(perl);
#endif
     Log(LL_DBG, "perl_on_exit(), perl=%p", Perl_get_context());
#ifndef HAVE_THREADS
     handle_perlerr(&perl_olderr, perl_errpipe);
#endif
     { dSP;
       ENTER;
       SAVETMPS;
       PUSHMARK(SP);
       PUTBACK;
       perl_call_pv(perl_subnames[PERL_ON_EXIT], G_EVAL|G_VOID);
       SPAGAIN;
       PUTBACK;
       FREETMPS;
       LEAVE;
     }
#ifndef HAVE_THREADS
     restore_perlerr(&perl_olderr, perl_errpipe);
#endif
     if (SvTRUE(ERRSV))
     {
       Log(LL_ERR, "Perl on_exit() error: %s", SvPV(ERRSV, n_a));
     }
     Log(LL_DBG, "perl_on_exit() end");
  }
}

/* before outgoing call */
int perl_on_call(FTN_NODE *node) {
  char   buf[FTN_ADDR_SZ];
  int    rc;
  SV     *svret, *sv;
  STRLEN len;

  if (perl_ok & (1 << PERL_ON_CALL)) {
#if 0
PERL_SET_CONTEXT(perl);
#endif
    Log(LL_DBG, "perl_on_call(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return 1;
    { dSP;
      ftnaddress_to_str(buf, &(node->fa));
      VK_ADD_str(sv, "addr", buf);
#ifndef HAVE_THREADS
      handle_perlerr(&perl_olderr, perl_errpipe);
#endif
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_ON_CALL], G_EVAL|G_SCALAR);
      SPAGAIN;
      svret=POPs;
      if (!SvOK(svret)) rc = 1; else rc = SvIV(svret);
      PUTBACK;
      FREETMPS;
      LEAVE;
#ifndef HAVE_THREADS
      restore_perlerr(&perl_olderr, perl_errpipe);
#endif
      if (SvTRUE(ERRSV)) {
        Log(LL_ERR, "Perl %s eval error: %s", 
            perl_subnames[PERL_ON_CALL], SvPV(ERRSV, len));
      }
    }
    Log(LL_DBG, "perl_on_call() end");
    return rc;
  }
  return 1;
}

/* after unsuccessfull call */
int perl_on_error(FTN_ADDR *addr, const char *error, const int where) {
  char   buf[FTN_ADDR_SZ];
  int    rc;
  SV     *svret, *sv;
  STRLEN len;

  if (perl_ok & (1 << PERL_ON_ERROR)) {
#if 0
PERL_SET_CONTEXT(perl);
#endif
    Log(LL_DBG, "perl_on_error(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return 1;
    { dSP;
      ftnaddress_to_str(buf, addr);
      VK_ADD_str (sv, "addr", buf);
      VK_ADD_str (sv, "error", error);
      VK_ADD_intz(sv, "where", where);
#ifndef HAVE_THREADS
      handle_perlerr(&perl_olderr, perl_errpipe);
#endif
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_ON_ERROR], G_EVAL|G_SCALAR);
      SPAGAIN;
      svret=POPs;
      if (!SvOK(svret)) rc = 1; else rc = SvIV(svret);
      PUTBACK;
      FREETMPS;
      LEAVE;
#ifndef HAVE_THREADS
      restore_perlerr(&perl_olderr, perl_errpipe);
#endif
      if (SvTRUE(ERRSV)) {
        Log(LL_ERR, "Perl %s eval error: %s", 
            perl_subnames[PERL_ON_ERROR], SvPV(ERRSV, len));
      }
    }
    Log(LL_DBG, "perl_on_error() end");
    return rc;
  }
  return 1;
}

/* before xmitting ADR */
char *perl_on_handshake(STATE *state) {
  char buf[FTN_ADDR_SZ];
  char *prc;
  int i;
  STRLEN len;
  AV *he, *me;
  SV *svret, **svp;

  if (perl_ok & (1 << PERL_ON_HANDSHAKE)) {
    Log(LL_DBG, "perl_on_handshake(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return NULL;
    { dSP;
      if ( (me = perl_get_av("me", FALSE)) != NULL ) av_undef(me);
      he = perl_get_av("he", TRUE);
      av_clear(he);
      if (!state->to) {
        for (i = 0; i < state->nfa; i++) {
          ftnaddress_to_str(buf, &(state->fa[i]));
          av_push(he, newSVpv(buf, 0));
        }
      } else {
          ftnaddress_to_str(buf, &(state->to->fa));
          av_push(he, newSVpv(buf, 0));
      }
      setup_session(state, 1);
#ifndef HAVE_THREADS
      handle_perlerr(&perl_olderr, perl_errpipe);
#endif
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_ON_HANDSHAKE], G_EVAL|G_SCALAR);
      SPAGAIN;
      svret=POPs;
      if (SvTRUE(svret)) prc = xstrdup(SvPV(svret, len)); else prc = NULL;
      PUTBACK;
      FREETMPS;
      LEAVE;
#ifndef HAVE_THREADS
      restore_perlerr(&perl_olderr, perl_errpipe);
#endif
      if (SvTRUE(ERRSV)) {
        Log(LL_ERR, "Perl %s eval error: %s", 
            perl_subnames[PERL_ON_HANDSHAKE], SvPV(ERRSV, len));
        if (prc) free(prc);
        prc = NULL;
      }
      /* make array of our aka */
      else if (!prc && ((me = perl_get_av("me", FALSE)) != NULL)) {
        FTN_ADDR addr;
        int n = 0, N = av_len(me) + 1;
        if (N > 0) state->pAddr = xalloc(N*sizeof(FTN_ADDR));
        for (i = 0; i < N; i++) {
          svp = av_fetch(me, i, NULL);
          if (svp == NULL) continue;
          if (!parse_ftnaddress(SvPV(*svp, len), &addr)) continue;
          exp_ftnaddress(&addr);
          state->pAddr[n++] = addr;
        }
        state->nAddr = n;
        if (n == 0) Log(LL_WARN, "Perl on_handshake(): @me contains no valid addresses");
      }
    }
    Log(LL_DBG, "perl_on_handshake() end");
    return prc;
  }
  return NULL;
}

/* after handshake complete */
char *perl_after_handshake(STATE *state) {
  char *prc;
  STRLEN len;
  SV *svret;

  if (perl_ok & (1 << PERL_AFTER_HANDSHAKE)) {
    Log(LL_DBG, "perl_after_handshake(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return NULL;
    { dSP;
      if (state->perl_set_lvl < 2) setup_session(state, 2);
      if (perl_wants_queue) setup_queue(state, state->q);
#ifndef HAVE_THREADS
      handle_perlerr(&perl_olderr, perl_errpipe);
#endif
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_AFTER_HANDSHAKE], G_EVAL|G_SCALAR);
      SPAGAIN;
      svret=POPs;
      if (SvTRUE(svret)) prc = xstrdup(SvPV(svret, len)); else prc = NULL;
      PUTBACK;
      FREETMPS;
      LEAVE;
#ifndef HAVE_THREADS
      restore_perlerr(&perl_olderr, perl_errpipe);
#endif
      if (SvTRUE(ERRSV)) {
        Log(LL_ERR, "Perl %s eval error: %s", 
            perl_subnames[PERL_AFTER_HANDSHAKE], SvPV(ERRSV, len));
        if (prc) free(prc);
        prc = NULL;
      }
      else if (perl_manages_queue) state->q = refresh_queue(state, state->q);
    }
    Log(LL_DBG, "perl_after_handshake() end");
    return prc;
  }
  return NULL;
}

/* after session done */
void perl_after_session(STATE *state, char *status) {
  SV *sv;
  STRLEN len;

  if (perl_ok & (1 << PERL_AFTER_SESSION)) {
    Log(LL_DBG, "perl_after_session(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return;
    { dSP;
      if (state->perl_set_lvl < 3) setup_session(state, 3);
      VK_ADD_intz(sv, "rc", (STRICMP(status, "OK") == 0));
#ifndef HAVE_THREADS
      handle_perlerr(&perl_olderr, perl_errpipe);
#endif
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_AFTER_SESSION], G_EVAL|G_VOID);
      SPAGAIN;
      PUTBACK;
      FREETMPS;
      LEAVE;
#ifndef HAVE_THREADS
      restore_perlerr(&perl_olderr, perl_errpipe);
#endif
      if (SvTRUE(ERRSV)) {
        Log(LL_ERR, "Perl %s eval error: %s", 
            perl_subnames[PERL_AFTER_SESSION], SvPV(ERRSV, len));
      }
    }
    Log(LL_DBG, "perl_after_session() end");
  }
}

/* before receiving file */
int perl_before_recv(STATE *state, off_t offs) {
  int    rc;
  STRLEN len;
  SV     *svret, *sv;

  if (perl_ok & (1 << PERL_BEFORE_RECV)) {
    Log(LL_DBG, "perl_before_recv(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return 0;
    { dSP;
      if (state->perl_set_lvl < 2) setup_session(state, 2);
      if (perl_wants_queue) setup_queue(state, state->q);
      VK_ADD_str (sv, "name", state->in.netname);
      VK_ADD_intz(sv, "size", state->in.size);
      VK_ADD_intz(sv, "time", state->in.time);
      VK_ADD_intz(sv, "offs", offs);
#ifndef HAVE_THREADS
      handle_perlerr(&perl_olderr, perl_errpipe);
#endif
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_BEFORE_RECV], G_EVAL|G_SCALAR);
      SPAGAIN;
      svret=POPs;
      if (SvOK(svret)) rc = SvIV(svret); else rc = 0;
      PUTBACK;
      FREETMPS;
      LEAVE;
#ifndef HAVE_THREADS
      restore_perlerr(&perl_olderr, perl_errpipe);
#endif
      if (SvTRUE(ERRSV)) {
        Log(LL_ERR, "Perl %s eval error: %s", 
            perl_subnames[PERL_BEFORE_RECV], SvPV(ERRSV, len));
        rc = 0;
      }
      else if (perl_manages_queue) state->q = refresh_queue(state, state->q);
    }
    Log(LL_DBG, "perl_before_recv() end");
    return rc;
  }
  return 0;
}

/* after file has been received */
/* return 0 to keep real_name, 1 to update real_name and try renaming,
          2 to kill tmp_name after having renamed it manually */
int perl_after_recv(STATE *state, char *netname, off_t size, time_t time, 
                    FTN_ADDR *fa, int nfa, char *tmp_name, 
                    char *real_name) {
  int    rc;
  STRLEN len;
  SV     *svret, *sv;

  if (perl_ok & (1 << PERL_AFTER_RECV)) {
    Log(LL_DBG, "perl_after_recv(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return 0;
    { dSP;
      if (state->perl_set_lvl < 2) setup_session(state, 2);
      if (perl_wants_queue) setup_queue(state, state->q);
      VK_ADD_str (sv, "name", netname);
      VK_ADD_intz(sv, "size", size);
      VK_ADD_intz(sv, "time", time);
      VK_ADD_str (sv, "tmpfile", tmp_name);
      VK_ADD_str (sv, "file", real_name); if (sv) { SvREADONLY_off(sv); }
#ifndef HAVE_THREADS
      handle_perlerr(&perl_olderr, perl_errpipe);
#endif
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_AFTER_RECV], G_EVAL|G_SCALAR);
      SPAGAIN;
      svret=POPs;
      if (SvOK(svret)) rc = SvIV(svret); else rc = 0;
      PUTBACK;
      FREETMPS;
      LEAVE;
#ifndef HAVE_THREADS
      restore_perlerr(&perl_olderr, perl_errpipe);
#endif
      if (SvTRUE(ERRSV)) {
        Log(LL_ERR, "Perl %s eval error: %s", 
            perl_subnames[PERL_AFTER_RECV], SvPV(ERRSV, len));
        rc = 0;
      }
      /* update real_name */
      else {
        if (perl_manages_queue) state->q = refresh_queue(state, state->q);
        if (rc == 1) {
          sv = perl_get_sv("file", FALSE);
          if (sv && SvOK(sv)) {
            char *s = SvPV(sv, len);
            strnzcpy(real_name, len ? s : "", MAXPATHLEN);
            if (!len || !*s) rc = 2; /* turn off binkd renaming, kill */
          }
          else rc = 2;           /* turn off binkd renaming, kill */
          if (rc == 1) rc = 0;   /* turn on binkd renaming of updated real_name */
        }
      }
    }
    Log(LL_DBG, "perl_after_recv() end");
    return rc;
  }
  return 0;
}

/* before sending file */
int perl_before_send(STATE *state) {
  int    rc;
  STRLEN len;
  SV     *svret, *sv;

  if (perl_ok & (1 << PERL_BEFORE_SEND)) {
    Log(LL_DBG, "perl_before_send(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return 0;
    { dSP;
      if (state->perl_set_lvl < 2) setup_session(state, 2);
      if (perl_wants_queue) setup_queue(state, state->q);
      VK_ADD_str (sv, "file", state->out.path);
      VK_ADD_str (sv, "name", state->out.netname); if (sv) { SvREADONLY_off(sv); }
      VK_ADD_intz(sv, "size", state->out.size);
      VK_ADD_intz(sv, "time", state->out.time);
#ifndef HAVE_THREADS
      handle_perlerr(&perl_olderr, perl_errpipe);
#endif
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_BEFORE_SEND], G_EVAL|G_SCALAR);
      SPAGAIN;
      svret=POPs;
      if (SvOK(svret)) rc = SvIV(svret); else rc = 0;
      PUTBACK;
      FREETMPS;
      LEAVE;
#ifndef HAVE_THREADS
      restore_perlerr(&perl_olderr, perl_errpipe);
#endif
      if (SvTRUE(ERRSV)) {
        Log(LL_ERR, "Perl %s eval error: %s", 
            perl_subnames[PERL_BEFORE_SEND], SvPV(ERRSV, len));
        rc = 0;
      }
      else {
        if (perl_manages_queue) state->q = refresh_queue(state, state->q);
        if (rc == 0) {
          sv = perl_get_sv("name", FALSE);
          if (sv && SvOK(sv)) {
            char *s = SvPV(sv, len);
            strnzcpy(state->out.netname, s, MAX_NETNAME);
          }
          else rc = 1;
        }
      }
    }
    Log(LL_DBG, "perl_before_send() end");
    return rc;
  }
  return 0;
}

/* after file has been sent */
int perl_after_sent(STATE *state, int n) {
  char   buf[2];
  int    rc;
  STRLEN len;
  SV     *svret, *sv;

  if (perl_ok & (1 << PERL_AFTER_SENT)) {
    Log(LL_DBG, "perl_after_sent(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return 0;
    { dSP;
      if (state->perl_set_lvl < 2) setup_session(state, 2);
      if (perl_wants_queue) setup_queue(state, state->q);
      VK_ADD_str (sv, "file", state->sent_fls[n].path);
      VK_ADD_str (sv, "name", state->sent_fls[n].netname);
      VK_ADD_intz(sv, "size", state->sent_fls[n].size);
      VK_ADD_intz(sv, "time", state->sent_fls[n].time);
      VK_ADD_intz(sv, "start", state->sent_fls[n].start);
      buf[0] = state->sent_fls[n].action; buf[1] = 0;
      VK_ADD_str (sv, "action", buf); if (sv) { SvREADONLY_off(sv); }
#ifndef HAVE_THREADS
      handle_perlerr(&perl_olderr, perl_errpipe);
#endif
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_AFTER_SENT], G_EVAL|G_SCALAR);
      SPAGAIN;
      svret=POPs;
      if (SvOK(svret)) rc = SvIV(svret); else rc = 0;
      PUTBACK;
      FREETMPS;
      LEAVE;
#ifndef HAVE_THREADS
      restore_perlerr(&perl_olderr, perl_errpipe);
#endif
      if (SvTRUE(ERRSV)) {
        Log(LL_ERR, "Perl %s eval error: %s", 
            perl_subnames[PERL_AFTER_SENT], SvPV(ERRSV, len));
        rc = 0;
      }
      else {
        if (perl_manages_queue) state->q = refresh_queue(state, state->q);
        if (rc) {
          sv = perl_get_sv("action", FALSE);
          if (sv && SvOK(sv)) {
            char *s = SvPV(sv, len);
            state->sent_fls[n].action = len ? *s : 0;
          }
        }
      }
    }
    Log(LL_DBG, "perl_after_sent() end");
    return rc;
  }
  return 0;
}

/* when writing string to log */
void perl_on_log(char *s) {}
