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
 * Revision 2.42  2003/12/24 00:37:26  gul
 * Fixed mingw with PERLDL compilation
 *
 * Revision 2.41  2003/11/19 18:07:20  gul
 * Use foreach_node() for fill %node hash
 *
 * Revision 2.40  2003/10/30 10:57:46  gul
 * Change inb_done arguments, optimize a bit
 *
 * Revision 2.39  2003/10/30 10:36:59  gul
 * Do not append file partially received from busy remote aka,
 * non-destructive skip it.
 *
 * Revision 2.38  2003/10/29 21:08:39  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.37  2003/10/27 23:22:54  gul
 * Fix OS/2 compilation
 *
 * Revision 2.36  2003/10/27 21:31:26  gul
 * Autodetect perl version, fix warning
 *
 * Revision 2.35  2003/10/27 16:16:22  gul
 * Fix warnings with perl58
 *
 * Revision 2.34  2003/10/24 14:19:28  val
 * missing OS/2 code for PERLDL improvements added
 *
 * Revision 2.33  2003/10/24 06:40:13  val
 * PERLDL support for both Perl 5.6 and 5.8 versions
 *
 * Revision 2.32  2003/10/22 14:24:51  stas
 * Remove obsolete defines
 *
 * Revision 2.31  2003/10/20 12:08:07  gul
 * Minor bugfix in perl error handling under win32
 *
 * Revision 2.30  2003/10/19 12:21:46  gul
 * Stream compression
 *
 * Revision 2.29  2003/10/07 15:30:09  gul
 * Fix warnings
 *
 * Revision 2.28  2003/10/04 00:46:53  hbrew
 * Avoid warning in perl.h
 *
 * Revision 2.27  2003/09/24 07:32:16  val
 * bzlib2 compression support, new compression keyword: zlevel
 *
 * Revision 2.26  2003/09/21 17:51:08  gul
 * Fixed PID in logfile for perl stderr handled messages in fork version.
 *
 * Revision 2.25  2003/09/21 17:34:27  gul
 * Change perl stderr handling for thread vertions,
 * some small changes.
 *
 * Revision 2.24  2003/09/18 07:18:21  val
 * fix to assure dll has been loaded before calling perl funcs
 *
 * Revision 2.23  2003/09/15 06:57:09  val
 * compression support via zlib: config keywords, improvements, OS/2 code
 *
 * Revision 2.22  2003/09/11 08:26:11  val
 * fix for Perl hooks code after steam's patch for pNodArray
 *
 * Revision 2.21  2003/09/08 06:36:51  val
 * (a) don't call exitfunc for perlhook fork'ed process
 * (b) many compilation warnings in perlhooks.c fixed
 *
 * Revision 2.20  2003/09/05 06:49:06  val
 * Perl support restored after config reloading patch
 *
 * Revision 2.19  2003/08/26 17:01:26  gul
 * Correct previous patch
 *
 * Revision 2.18  2003/08/26 16:45:52  gul
 * binkd/2 perl dynamic DLL load
 *
 * Revision 2.16  2003/08/26 07:43:54  stream
 * Use generic lists
 *
 * Revision 2.15  2003/08/25 14:09:03  gul
 * Added exp_ftnaddress() to refresh_queue()
 *
 * Revision 2.14  2003/08/22 09:41:36  val
 * add check perl!=NULL in perl_on_handshake
 *
 * Revision 2.13  2003/08/22 09:37:39  val
 * missing function import for PERLDL: sv_setpvn()
 *
 * Revision 2.12  2003/08/18 09:41:00  gul
 * Little cleanup in handle perl errors
 *
 * Revision 2.11  2003/08/18 09:15:39  gul
 * Cosmetics
 *
 * Revision 2.10  2003/08/18 07:35:08  val
 * multiple changes:
 * - hide-aka/present-aka logic
 * - address mask matching via pmatch
 * - delay_ADR in STATE (define DELAY_ADR removed)
 * - ftnaddress_to_str changed to xftnaddress_to_str (old version #define'd)
 * - parse_ftnaddress now sets zone to domain default if it's omitted
 *
 * Revision 2.9  2003/08/18 07:29:09  val
 * multiple changes:
 * - perl error handling made via fork/thread
 * - on_log() perl hook
 * - perl: msg_send(), on_send(), on_recv()
 * - unless using threads define log buffer via xalloc()
 *
 * Revision 2.8  2003/08/13 08:20:45  val
 * try to avoid mixing Log() output and Perl errors in stderr
 *
 * Revision 2.7  2003/08/11 08:33:16  val
 * better error handling in perl hooks
 *
 * Revision 2.6  2003/07/28 10:23:33  val
 * Perl DLL dynamic load for Win32, config keyword perl-dll, nmake PERLDL=1
 *
 * Revision 2.5  2003/07/07 08:42:02  val
 * check real length of SvPV() when importing queue element from perl
 *
 * Revision 2.4  2003/07/04 08:13:17  val
 * core dump in perl_clone() fixed, works with 5.6.0+
 *
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

#ifdef HAVE_FORK
#include <sys/wait.h>
#endif

#ifdef OS2
#ifdef PERLDL
#define INCL_DOSMODULEMGR
#endif
#define INCL_DOSPROCESS
#include <os2.h>
#endif

#ifdef _MSC_VER
#undef __STDC__
#endif
#include <sys/types.h>
#include <sys/stat.h>

#if defined(__NT__) && !defined(WIN32) /* WIN32 needed for perl-core include files */
#  define WIN32
#endif

#ifdef WIN32
# define NO_XSLOCKS
#endif
#ifndef EXTERN_C
#  define EXTERN_C extern
#endif

/* ---------------- binkd stuff --------------- */
#include "sys.h"
#include "readcfg.h"
#include "tools.h"
#include "ftnaddr.h"
#include "ftnq.h"
#include "ftnnode.h"
#include "readflo.h"
#include "iphdr.h"
#include "protoco2.h"
#include "sem.h"
#include "protocol.h"
#include "perlhooks.h"
/* ---------------- perl stuff --------------- */
/* dynamic load */
#ifdef PERLDL
# define Perl_sv_2pv_flags		(hl_Perl_sv_2pv_flags)
# define Perl_sv_2pv			(hl_Perl_sv_2pv)
# define Perl_sv_2uv			(*dl_Perl_sv_2uv)
# define Perl_sv_2mortal		(*dl_Perl_sv_2mortal)
# define Perl_newSViv			(*dl_Perl_newSViv)
# define Perl_av_store			(*dl_Perl_av_store)
# define Perl_av_fetch			(*dl_Perl_av_fetch)
# define Perl_av_len			(*dl_Perl_av_len)
# define Perl_newXS			(*dl_Perl_newXS)
# define Perl_sv_2bool			(*dl_Perl_sv_2bool)
# define perl_run			(*dl_perl_run)
# define Perl_sv_setiv			(*dl_Perl_sv_setiv)
# define perl_free			(*dl_perl_free)
# define perl_destruct			(*dl_perl_destruct)
# define perl_parse			(*dl_perl_parse)
# define perl_construct			(*dl_perl_construct)
# define perl_alloc			(*dl_perl_alloc)
# define Perl_newAV			(*dl_Perl_newAV)
# define Perl_av_push			(*dl_Perl_av_push)
# define Perl_av_clear			(*dl_Perl_av_clear)
# define Perl_newRV_noinc		(*dl_Perl_newRV_noinc)
# define Perl_newHV			(*dl_Perl_newHV)
# define Perl_hv_store			(*dl_Perl_hv_store)
# define Perl_newSVpv			(*dl_Perl_newSVpv)
# define Perl_hv_clear			(*dl_Perl_hv_clear)
# define Perl_push_scope		(*dl_Perl_push_scope)
# define Perl_pop_scope			(*dl_Perl_pop_scope)
# define Perl_free_tmps			(*dl_Perl_free_tmps)
# define Perl_markstack_grow		(*dl_Perl_markstack_grow)
# define Perl_save_int			(*dl_Perl_save_int)
# define Perl_sv_2iv			(*dl_Perl_sv_2iv)
# define Perl_sv_setsv_flags		(hl_Perl_sv_setsv_flags)
# define Perl_sv_setsv			(hl_Perl_sv_setsv)
# define Perl_sv_setpv			(*dl_Perl_sv_setpv)
# define Perl_sv_setpvn			(*dl_Perl_sv_setpvn)
# define Perl_av_undef			(*dl_Perl_av_undef)
# define Perl_hv_fetch			(*dl_Perl_hv_fetch)
#ifdef OS2
# define PL_errgv			(*dl_PL_errgv)
# define PL_stack_sp			(*dl_PL_stack_sp)
# define PL_markstack_ptr		(*dl_PL_markstack_ptr)
# define PL_sv_undef			(*dl_PL_sv_undef)
# define PL_stack_base			(*dl_PL_stack_base)
# define PL_markstack_max		(*dl_PL_markstack_max)
# define PL_tmps_ix			(*dl_PL_tmps_ix)
# define PL_tmps_floor			(*dl_PL_tmps_floor)
# define Perl_sv_setpvf			(*dl_Perl_sv_setpvf)
# define perl_get_sv			(*dl_perl_get_sv)
# define perl_get_av			(*dl_perl_get_av)
# define perl_get_hv			(*dl_perl_get_hv)
# define perl_get_cv			(*dl_perl_get_cv)
# define perl_call_pv			(*dl_perl_call_pv)
#else
# define Perl_Ierrgv_ptr		(*dl_Perl_Ierrgv_ptr)
# define Perl_Tstack_sp_ptr		(*dl_Perl_Tstack_sp_ptr)
# define Perl_Tmarkstack_ptr_ptr	(*dl_Perl_Tmarkstack_ptr_ptr)
# define Perl_Isv_undef_ptr		(*dl_Perl_Isv_undef_ptr)
# define Perl_Tstack_base_ptr		(*dl_Perl_Tstack_base_ptr)
# define Perl_Tmarkstack_max_ptr	(*dl_Perl_Tmarkstack_max_ptr)
# define Perl_Ttmps_ix_ptr		(*dl_Perl_Ttmps_ix_ptr)
# define Perl_Ttmps_floor_ptr		(*dl_Perl_Ttmps_floor_ptr)
# define Perl_sv_setpvf_nocontext	(*dl_Perl_sv_setpvf_nocontext)
# define Perl_get_sv			(*dl_Perl_get_sv)
# define Perl_get_av			(*dl_Perl_get_av)
# define Perl_get_hv			(*dl_Perl_get_hv)
# define Perl_get_cv			(*dl_Perl_get_cv)
# define Perl_call_pv			(*dl_Perl_call_pv)
# define Perl_get_context		(*dl_Perl_get_context)
# define Perl_set_context		(*dl_Perl_set_context)
# define perl_clone			(*dl_perl_clone)
# define Perl_Iperl_destruct_level_ptr	(*dl_Perl_Iperl_destruct_level_ptr)
# define Perl_Tscopestack_ix_ptr	(*dl_Perl_Tscopestack_ix_ptr)
# define Perl_Tmarkstack_ptr    	(*dl_Perl_Tmarkstack_ptr)
# define Perl_TXpv_ptr			(*dl_Perl_TXpv_ptr)
# define boot_DynaLoader		(*dl_boot_DynaLoader)
#endif
#endif

#ifdef DEBUG
#  undef DEBUG
#endif

#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>

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

#ifndef aTHX_
#  define aTHX_
#endif

#ifdef q
#  undef q
#endif

#ifdef __GNUC__
#  define Perl___notused Perl___notused __attribute__ ((unused))
#endif

#ifdef PERLDL

#if PERL_REVISION<5 || (PERL_REVISION==5 && PERL_VERSION<8)
# define SV_GMAGIC		2      /* sv.h */
#endif

#ifdef OS2
#define pTHX_
#define pTHX void
#define pTHXo_
#define pTHXo void
#define _Const
#define OS2_Perl_data	(*dl_OS2_Perl_data)
typedef void (*XSUBADDR_t)(CV* cv _CPERLproto);
typedef void(*XSINIT_t)(void);
#else
#define _Const const
#endif

/* below are the prototypes as they should appear:
PERL_CALLCONV char*	(*dl_Perl_sv_2pv)(pTHX_ SV* sv, STRLEN *lp);
PERL_CALLCONV UV	(*dl_Perl_sv_2uv)(pTHX_ SV* sv);
PERL_CALLCONV void*	(*dl_Perl_get_context)(void);
PERL_CALLCONV void	(*dl_Perl_set_context)(void *thx);
PERL_CALLCONV SV*	(*dl_Perl_sv_2mortal)(pTHX_ SV* sv);
PERL_CALLCONV SV*	(*dl_Perl_newSViv)(pTHX_ IV i);
PERL_CALLCONV SV**	(*dl_Perl_av_store)(pTHX_ AV* ar, I32 key, SV* val);
PERL_CALLCONV SV**	(*dl_Perl_av_fetch)(pTHX_ AV* ar, I32 key, I32 lval);
PERL_CALLCONV I32	(*dl_Perl_av_len)(pTHX_ AV* ar);
PERL_CALLCONV void	(*dl_Perl_av_push)(pTHX_ AV* ar, SV* val);
PERL_CALLCONV void	(*dl_Perl_av_clear)(pTHX_ AV* ar);
PERL_CALLCONV CV*	(*dl_Perl_newXS)(pTHX_ char* name, XSUBADDR_t f, char* filename);
PERL_CALLCONV bool	(*dl_Perl_sv_2bool)(pTHX_ SV* sv);
PERL_CALLCONV CV*	(*dl_Perl_get_cv)(pTHX_ const char* name, I32 create);
PERL_CALLCONV SV*	(*dl_Perl_get_sv)(pTHX_ const char* name, I32 create);
PERL_CALLCONV AV*	(*dl_Perl_get_av)(pTHX_ const char* name, I32 create);
PERL_CALLCONV HV*	(*dl_Perl_get_hv)(pTHX_ const char* name, I32 create);
PERL_CALLCONV PerlInterpreter*	(*dl_perl_alloc)(void);
PERL_CALLCONV void	(*dl_perl_construct)(PerlInterpreter* interp);
PERL_CALLCONV void	(*dl_perl_destruct)(PerlInterpreter* interp);
PERL_CALLCONV void	(*dl_perl_free)(PerlInterpreter* interp);
PERL_CALLCONV int	(*dl_perl_run)(PerlInterpreter* interp);
PERL_CALLCONV int	(*dl_perl_parse)(PerlInterpreter* interp, XSINIT_t xsinit, int argc, char** argv, char** env);
PERL_CALLCONV PerlInterpreter*	(*dl_perl_clone)(PerlInterpreter* interp, UV flags);
PERL_CALLCONV void	(*dl_Perl_sv_setiv)(pTHX_ SV* sv, IV num);
PERL_CALLCONV void	(*dl_Perl_sv_setsv)(pTHX_ SV* dsv, SV* ssv);
PERL_CALLCONV void	(*dl_Perl_sv_setpv)(pTHX_ SV* sv, const char* ptr);
PERL_CALLCONV void	(*dl_Perl_sv_setpvn)(pTHX_ SV* sv, const char* ptr, STRLEN len);
PERL_CALLCONV AV*	(*dl_Perl_newAV)(pTHX);
PERL_CALLCONV SV*	(*dl_Perl_newRV_noinc)(pTHX_ SV *sv);
PERL_CALLCONV HV*	(*dl_Perl_newHV)(pTHX);
PERL_CALLCONV SV*	(*dl_Perl_newSVpv)(pTHX_ const char* s, STRLEN len);
PERL_CALLCONV SV**	(*dl_Perl_hv_store)(pTHX_ HV* tb, _Const char* key, U32 klen, SV* val, U32 hash);
PERL_CALLCONV void	(*dl_Perl_hv_clear)(pTHX_ HV* tb);
PERL_CALLCONV SV**	(*dl_Perl_hv_fetch)(pTHX_ HV* tb, _Const char* key, U32 klen, I32 lval);
PERL_CALLCONV void	(*dl_Perl_push_scope)(pTHX);
PERL_CALLCONV void	(*dl_Perl_pop_scope)(pTHX);
PERL_CALLCONV void	(*dl_Perl_free_tmps)(pTHX);
PERL_CALLCONV I32	(*dl_Perl_call_pv)(pTHX_ const char* sub_name, I32 flags);
PERL_CALLCONV void	(*dl_Perl_markstack_grow)(pTHX);
PERL_CALLCONV void	(*dl_Perl_save_int)(pTHX_ int* intp);
PERL_CALLCONV IV	(*dl_Perl_sv_2iv)(pTHX_ SV* sv);
PERL_CALLCONV void	(*dl_Perl_av_undef)(pTHX_ AV* ar);
PERL_CALLCONV void	(*dl_Perl_sv_setpvf_nocontext)(SV* sv, const char* pat, ...)
#ifdef CHECK_FORMAT
 __attribute__((format(printf,2,3)))
#endif
;
PERL_CALLCONV int* (*dl_Perl_Iperl_destruct_level_ptr)(pTHXo);
PERL_CALLCONV GV** (*dl_Perl_Ierrgv_ptr)(pTHXo);
PERL_CALLCONV I32* (*dl_Perl_Tscopestack_ix_ptr)(pTHXo);
PERL_CALLCONV I32** (*dl_Perl_Tmarkstack_ptr)(pTHXo);
PERL_CALLCONV I32** (*dl_Perl_Tmarkstack_max_ptr)(pTHXo);
PERL_CALLCONV I32* (*dl_Perl_Ttmps_ix_ptr)(pTHXo);
PERL_CALLCONV I32* (*dl_Perl_Ttmps_floor_ptr)(pTHXo);
PERL_CALLCONV SV*  (*dl_Perl_Isv_undef_ptr)(pTHXo);
PERL_CALLCONV XPV** (*dl_Perl_TXpv_ptr)(pTHXo);
PERL_CALLCONV SV*** (*dl_Perl_Tstack_base_ptr)(pTHXo);
PERL_CALLCONV I32** (*dl_Perl_Tmarkstack_ptr_ptr)(pTHXo);
PERL_CALLCONV SV*** (*dl_Perl_Tstack_sp_ptr)(pTHXo);
PERL_CALLCONV void (*dl_boot_DynaLoader)(pTHXo_ CV* cv);
*/
/* below is the prototype with typedef:
typedef char* (PERL_CALLCONV *tf_Perl_sv_2pv)(pTHX_ SV* sv, STRLEN* lp);
tf_Perl_sv_2pv         dl_Perl_sv_2pv;
*/

/* the currently used way to define prototypes via macro */
#define VK_MAKE_DFL(t,n,a) t (PERL_CALLCONV *n) a
VK_MAKE_DFL(char*, dl_Perl_sv_2pv_flags, (pTHX_ SV* sv, STRLEN* lp, I32 flags));
VK_MAKE_DFL(char*, dl_Perl_sv_2pv, (pTHX_ SV* sv, STRLEN* lp));
PERL_CALLCONV char* hl_Perl_sv_2pv(pTHX_ SV* sv, STRLEN* lp) {
  if (dl_Perl_sv_2pv) return dl_Perl_sv_2pv(aTHX_ sv, lp);
  else return dl_Perl_sv_2pv_flags(aTHX_ sv, lp, SV_GMAGIC);
}
PERL_CALLCONV char* hl_Perl_sv_2pv_flags(pTHX_ SV* sv, STRLEN* lp, I32 flags) {
  if (dl_Perl_sv_2pv_flags) return dl_Perl_sv_2pv_flags(aTHX_ sv, lp, flags);
  else return dl_Perl_sv_2pv(aTHX_ sv, lp);
}
VK_MAKE_DFL(UV, dl_Perl_sv_2uv, (pTHX_ SV* sv));

VK_MAKE_DFL(SV*, dl_Perl_sv_2mortal, (pTHX_ SV* sv));
VK_MAKE_DFL(SV*, dl_Perl_newSViv, (pTHX_ IV i));

VK_MAKE_DFL(SV**, dl_Perl_av_store, (pTHX_ AV* ar, I32 key, SV* val));
VK_MAKE_DFL(SV**, dl_Perl_av_fetch, (pTHX_ AV* ar, I32 key, I32 lval));
VK_MAKE_DFL(I32, dl_Perl_av_len, (pTHX_ AV* ar));
VK_MAKE_DFL(void, dl_Perl_av_push, (pTHX_ AV* ar, SV* val));
VK_MAKE_DFL(void, dl_Perl_av_clear, (pTHX_ AV* ar));

VK_MAKE_DFL(CV*, dl_Perl_newXS, (pTHX_ char* name, XSUBADDR_t f, char* filename));
VK_MAKE_DFL(bool, dl_Perl_sv_2bool, (pTHX_ SV* sv));

VK_MAKE_DFL(PerlInterpreter*, dl_perl_alloc, (void));
VK_MAKE_DFL(void, dl_perl_construct, (PerlInterpreter* interp));
#if PERL_REVISION>5 || (PERL_REVISION==5 && PERL_VERSION>=8)
VK_MAKE_DFL(int, dl_perl_destruct, (PerlInterpreter* interp));
#else
VK_MAKE_DFL(void, dl_perl_destruct, (PerlInterpreter* interp));
#endif
VK_MAKE_DFL(void, dl_perl_free, (PerlInterpreter* interp));
VK_MAKE_DFL(int, dl_perl_run, (PerlInterpreter* interp));
VK_MAKE_DFL(int, dl_perl_parse, (PerlInterpreter* interp, XSINIT_t xsinit, int argc, char** argv, char** env));

VK_MAKE_DFL(void, dl_Perl_sv_setiv, (pTHX_ SV* sv, IV num));
VK_MAKE_DFL(void, dl_Perl_sv_setsv_flags, (pTHX_ SV* dsv, SV* ssv, I32 flags));
VK_MAKE_DFL(void, dl_Perl_sv_setsv, (pTHX_ SV* dsv, SV* ssv));
void hl_Perl_sv_setsv(pTHX_ SV* sv, SV* ssv) {
  if (dl_Perl_sv_setsv) dl_Perl_sv_setsv(aTHX_ sv, ssv);
  else dl_Perl_sv_setsv_flags(aTHX_ sv, ssv, SV_GMAGIC);
}
void hl_Perl_sv_setsv_flags(pTHX_ SV* sv, SV* ssv, I32 flags) {
  if (dl_Perl_sv_setsv) dl_Perl_sv_setsv_flags(aTHX_ sv, ssv, flags);
  else dl_Perl_sv_setsv(aTHX_ sv, ssv);
}
VK_MAKE_DFL(void, dl_Perl_sv_setpv, (pTHX_ SV* sv, const char* ptr));
VK_MAKE_DFL(void, dl_Perl_sv_setpvn, (pTHX_ SV* sv, const char* ptr, STRLEN len));

VK_MAKE_DFL(AV*, dl_Perl_newAV, (pTHX));
VK_MAKE_DFL(SV*, dl_Perl_newRV_noinc, (pTHX_ SV *sv));
VK_MAKE_DFL(HV*, dl_Perl_newHV, (pTHX));
VK_MAKE_DFL(SV*, dl_Perl_newSVpv, (pTHX_ _Const char* s, STRLEN len));

VK_MAKE_DFL(void, dl_Perl_hv_clear, (pTHX_ HV* tb));
#if PERL_REVISION<5 || (PERL_REVISION==5 && PERL_VERSION<8)
VK_MAKE_DFL(SV**, dl_Perl_hv_store, (pTHX_ HV* tb, _Const char* key, U32 klen, SV* val, U32 hash));
VK_MAKE_DFL(SV**, dl_Perl_hv_fetch, (pTHX_ HV* tb, _Const char* key, U32 klen, I32 lval));
#else
VK_MAKE_DFL(SV**, dl_Perl_hv_store, (pTHX_ HV* tb, _Const char* key, I32 klen, SV* val, U32 hash));
VK_MAKE_DFL(SV**, dl_Perl_hv_fetch, (pTHX_ HV* tb, _Const char* key, I32 klen, I32 lval));
#endif

VK_MAKE_DFL(void, dl_Perl_push_scope, (pTHX));
VK_MAKE_DFL(void, dl_Perl_pop_scope, (pTHX));
VK_MAKE_DFL(void, dl_Perl_free_tmps, (pTHX));
VK_MAKE_DFL(void, dl_Perl_markstack_grow, (pTHX));
VK_MAKE_DFL(void, dl_Perl_save_int, (pTHX_ int* intp));

VK_MAKE_DFL(IV, dl_Perl_sv_2iv, (pTHX_ SV* sv));
VK_MAKE_DFL(void, dl_Perl_av_undef, (pTHX_ AV* ar));

#ifdef OS2
VK_MAKE_DFL(void, dl_Perl_sv_setpvf, (SV* sv, const char* pat, ...));
VK_MAKE_DFL(CV*, dl_perl_get_cv, (pTHX_ _Const char* name, I32 create));
VK_MAKE_DFL(SV*, dl_perl_get_sv, (pTHX_ _Const char* name, I32 create));
VK_MAKE_DFL(AV*, dl_perl_get_av, (pTHX_ _Const char* name, I32 create));
VK_MAKE_DFL(HV*, dl_perl_get_hv, (pTHX_ _Const char* name, I32 create));
VK_MAKE_DFL(I32, dl_perl_call_pv, (pTHX_ _Const char* sub_name, I32 flags));
GV*  *dl_PL_errgv;
SV** *dl_PL_stack_sp;
I32* *dl_PL_markstack_ptr;
SV   *dl_PL_sv_undef;
SV** *dl_PL_stack_base;
I32* *dl_PL_markstack_max;
I32  *dl_PL_tmps_ix;
I32  *dl_PL_tmps_floor;
OS2_Perl_data_t *dl_OS2_Perl_data;
#else
VK_MAKE_DFL(CV*, dl_Perl_get_cv, (pTHX_ _Const char* name, I32 create));
VK_MAKE_DFL(SV*, dl_Perl_get_sv, (pTHX_ _Const char* name, I32 create));
VK_MAKE_DFL(AV*, dl_Perl_get_av, (pTHX_ _Const char* name, I32 create));
VK_MAKE_DFL(HV*, dl_Perl_get_hv, (pTHX_ _Const char* name, I32 create));
VK_MAKE_DFL(I32, dl_Perl_call_pv, (pTHX_ _Const char* sub_name, I32 flags));
VK_MAKE_DFL(GV**, dl_Perl_Ierrgv_ptr, (pTHXo));
VK_MAKE_DFL(SV***, dl_Perl_Tstack_sp_ptr, (pTHXo));
VK_MAKE_DFL(I32**, dl_Perl_Tmarkstack_ptr_ptr, (pTHXo));
VK_MAKE_DFL(SV*, dl_Perl_Isv_undef_ptr, (pTHXo));
VK_MAKE_DFL(SV***, dl_Perl_Tstack_base_ptr, (pTHXo));
VK_MAKE_DFL(I32**, dl_Perl_Tmarkstack_max_ptr, (pTHXo));
VK_MAKE_DFL(I32*, dl_Perl_Ttmps_ix_ptr, (pTHXo));
VK_MAKE_DFL(I32*, dl_Perl_Ttmps_floor_ptr, (pTHXo));
VK_MAKE_DFL(I32**, dl_Perl_Tmarkstack_ptr, (pTHXo));
VK_MAKE_DFL(XPV**, dl_Perl_TXpv_ptr, (pTHXo));
VK_MAKE_DFL(void, dl_boot_DynaLoader, (pTHXo_ CV* cv));

VK_MAKE_DFL(void*, dl_Perl_get_context, (void));
VK_MAKE_DFL(void, dl_Perl_set_context, (void *thx));
VK_MAKE_DFL(PerlInterpreter*, dl_perl_clone, (PerlInterpreter* interp, UV flags));
VK_MAKE_DFL(int*, dl_Perl_Iperl_destruct_level_ptr, (pTHXo));
VK_MAKE_DFL(I32*, dl_Perl_Tscopestack_ix_ptr, (pTHXo));

#ifdef CHECK_FORMAT
VK_MAKE_DFL(void, dl_Perl_sv_setpvf_nocontext, (SV* sv, const char* pat, ...) __attribute__((format(printf,2,3))) );
#else
VK_MAKE_DFL(void, dl_Perl_sv_setpvf_nocontext, (SV* sv, const char* pat, ...));
#endif

#endif

/* the list of functions to import from DLL */
#define VK_MAKE_DLFUNC(n) { (void **)&dl_##n, #n }

struct perl_dlfunc { void **f; char *name; } perl_dlfuncs[] = {
/*#if PERL_REVISION>5 || (PERL_REVISION==5 && PERL_VERSION>=8)
  VK_MAKE_DLFUNC(Perl_sv_2pv_flags),
#else
  VK_MAKE_DLFUNC(Perl_sv_2pv),
#endif*/
  VK_MAKE_DLFUNC(Perl_sv_2uv),
  VK_MAKE_DLFUNC(Perl_sv_2mortal),
  VK_MAKE_DLFUNC(Perl_newSViv),
  VK_MAKE_DLFUNC(Perl_av_store),
  VK_MAKE_DLFUNC(Perl_av_fetch),
  VK_MAKE_DLFUNC(Perl_av_len),
  VK_MAKE_DLFUNC(Perl_av_push),
  VK_MAKE_DLFUNC(Perl_av_clear),
  VK_MAKE_DLFUNC(Perl_newXS),
  VK_MAKE_DLFUNC(Perl_sv_2bool),
  VK_MAKE_DLFUNC(perl_alloc),
  VK_MAKE_DLFUNC(perl_construct),
  VK_MAKE_DLFUNC(perl_destruct),
  VK_MAKE_DLFUNC(perl_free),
  VK_MAKE_DLFUNC(perl_run),
  VK_MAKE_DLFUNC(perl_parse),
  VK_MAKE_DLFUNC(Perl_sv_setiv),
/*#if PERL_REVISION>5 || (PERL_REVISION==5 && PERL_VERSION>=8)
  VK_MAKE_DLFUNC(Perl_sv_setsv_flags),
#else
  VK_MAKE_DLFUNC(Perl_sv_setsv),
#endif*/
  VK_MAKE_DLFUNC(Perl_sv_setpv),
  VK_MAKE_DLFUNC(Perl_sv_setpvn),
  VK_MAKE_DLFUNC(Perl_newAV),
  VK_MAKE_DLFUNC(Perl_newRV_noinc),
  VK_MAKE_DLFUNC(Perl_newHV),
  VK_MAKE_DLFUNC(Perl_newSVpv),
  VK_MAKE_DLFUNC(Perl_hv_store),
  VK_MAKE_DLFUNC(Perl_hv_clear),
  VK_MAKE_DLFUNC(Perl_hv_fetch),
  VK_MAKE_DLFUNC(Perl_push_scope),
  VK_MAKE_DLFUNC(Perl_pop_scope),
  VK_MAKE_DLFUNC(Perl_free_tmps),
  VK_MAKE_DLFUNC(Perl_markstack_grow),
  VK_MAKE_DLFUNC(Perl_save_int),
  VK_MAKE_DLFUNC(Perl_sv_2iv),
  VK_MAKE_DLFUNC(Perl_av_undef),
#ifdef OS2
  VK_MAKE_DLFUNC(perl_get_cv),
  VK_MAKE_DLFUNC(perl_get_sv),
  VK_MAKE_DLFUNC(perl_get_av),
  VK_MAKE_DLFUNC(perl_get_hv),
  VK_MAKE_DLFUNC(perl_call_pv),
  VK_MAKE_DLFUNC(PL_errgv),
  VK_MAKE_DLFUNC(PL_stack_sp),
  VK_MAKE_DLFUNC(PL_markstack_ptr),
  VK_MAKE_DLFUNC(PL_sv_undef),
  VK_MAKE_DLFUNC(PL_stack_base),
  VK_MAKE_DLFUNC(PL_markstack_max),
  VK_MAKE_DLFUNC(PL_tmps_ix),
  VK_MAKE_DLFUNC(PL_tmps_floor),
  VK_MAKE_DLFUNC(OS2_Perl_data),
  VK_MAKE_DLFUNC(Perl_sv_setpvf),
#else
  VK_MAKE_DLFUNC(Perl_get_cv),
  VK_MAKE_DLFUNC(Perl_get_sv),
  VK_MAKE_DLFUNC(Perl_get_av),
  VK_MAKE_DLFUNC(Perl_get_hv),
  VK_MAKE_DLFUNC(Perl_call_pv),
  VK_MAKE_DLFUNC(Perl_Ierrgv_ptr),
  VK_MAKE_DLFUNC(Perl_Tstack_sp_ptr),
  VK_MAKE_DLFUNC(Perl_Tmarkstack_ptr_ptr),
  VK_MAKE_DLFUNC(Perl_Isv_undef_ptr),
  VK_MAKE_DLFUNC(Perl_Tstack_base_ptr),
  VK_MAKE_DLFUNC(Perl_Tmarkstack_max_ptr),
  VK_MAKE_DLFUNC(Perl_Ttmps_ix_ptr),
  VK_MAKE_DLFUNC(Perl_Ttmps_floor_ptr),
  VK_MAKE_DLFUNC(Perl_sv_setpvf_nocontext),
  VK_MAKE_DLFUNC(Perl_get_context),
  VK_MAKE_DLFUNC(Perl_set_context),
  VK_MAKE_DLFUNC(perl_clone),
  VK_MAKE_DLFUNC(Perl_Iperl_destruct_level_ptr),
  VK_MAKE_DLFUNC(Perl_Tscopestack_ix_ptr),
  VK_MAKE_DLFUNC(Perl_Tmarkstack_ptr),
  VK_MAKE_DLFUNC(Perl_TXpv_ptr),
  VK_MAKE_DLFUNC(boot_DynaLoader),
#endif
  { NULL, NULL }
};
#endif                                                      /* PERLDL */

#if defined(HAVE_FORK) && !defined(Perl_get_context)
#  define Perl_get_context() perl
#endif
/* =========================== vars ================================== */

static PerlInterpreter *perl = NULL;        /* root object for all threads */
static char *sv_config = "__config";  /* SV that keeps pointer to config */
static char *sv_state  = "__state";   /* SV that keeps pointer to state */

#define VK_FIND_CONFIG(cfg)                                        \
            { SV *sv = perl_get_sv(sv_config, FALSE);              \
              cfg = sv ? (BINKD_CONFIG*)SvIV(sv) : current_config; \
            }

/* bits for subroutines, must correspond to perl_subnames */
typedef enum { 
  PERL_ON_START, PERL_ON_EXIT, PERL_ON_CALL, PERL_ON_ERROR,
  PERL_ON_HANDSHAKE, PERL_AFTER_HANDSHAKE, PERL_AFTER_SESSION, 
  PERL_BEFORE_RECV, PERL_AFTER_RECV, PERL_BEFORE_SEND, PERL_AFTER_SENT,
  PERL_ON_LOG, PERL_ON_SEND, PERL_ON_RECV
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
  "on_log",
  "on_send",
  "on_recv"
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
    sv_setiv(_sv, _v); SvREADONLY_on(_sv);          \
  }
#define VK_ADD_strz(_sv, _name, _v)                 \
  if ( (_sv = perl_get_sv(_name, TRUE)) != NULL ) { \
    sv_setpv(_sv, _v ? _v : ""); SvREADONLY_on(_sv);\
  }
#define VK_ADD_str(_sv, _name, _v)                            \
  if ( (_sv = perl_get_sv(_name, TRUE)) != NULL ) {           \
    if (_v) sv_setpv(_sv, _v); else sv_setsv(_sv, &sv_undef); \
    SvREADONLY_on(_sv);                                       \
  }
#define B4(l) (l>>24 & 0xff), (l>>16 & 0xff), (l>>8 & 0xff), (l & 0xff)
#define VK_ADD_ip(_sv, _name, _v)                             \
  if ( (_sv = perl_get_sv(_name, TRUE)) != NULL ) {           \
    if (_v) sv_setpvf(_sv, "%u.%u.%u.%u", B4(ntohl(_v)));     \
    else sv_setsv(_sv, &sv_undef);                            \
    SvREADONLY_on(_sv);                                       \
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
int perl_errpid;
FILE *perl_olderr;
#ifdef HAVE_FORK
int perl_skipexitfunc = 0;
#endif

#ifdef HAVE_THREADS
/* thread that reads from pipe */
static void err_thread(void *arg) {
  FILE *R;
  char buf[256];

  R = fdopen(*((int*)arg), "r");
  if (R == NULL) return;
  buf[0] = 0;
  while ( fgets(buf, sizeof(buf), R) ) {
    int n = strlen(buf);
    if (n > 0 && buf[n-1] == '\n') {
      if (n > 1 && buf[n-2] == '\r') buf[--n] = 0;
      buf[--n] = 0;
    }
    if (n > 0) Log(LL_ERR, "Perl error: %s", buf);
  }
  fclose(R);
  fflush(perl_olderr);
  dup2(fileno(perl_olderr), fileno(stderr));
  R = perl_olderr;
  perl_olderr = NULL;
  fclose(R);
  perl_errpid = 0;
  PostSem(&eothread);
  _endthread();
}
#endif

/* set up perl errors handler, redirect stderr to pipe */
static void handle_perlerr2(void) {
  static int errpipe[2];
#ifdef HAVE_FORK
  int try = 0;

  perl_errpid = 0;
  fflush(stderr);
  pipe(errpipe);
  while (try < 10) {
    perl_errpid = fork();
    /* parent */
    if (perl_errpid > 0) {
      close(errpipe[0]);
      perl_olderr = fdopen(dup(fileno(stderr)), "a");
      dup2(errpipe[1], fileno(stderr));
      close(errpipe[1]);
      break;
    }
    /* child */
    else if (perl_errpid == 0) {
      FILE *R;
      char buf[256];

      perl_skipexitfunc = 1;
      close(errpipe[1]);
      R = fdopen(errpipe[0], "r");
      buf[0] = 0;
      while ( fgets(buf, sizeof(buf), R) ) {
        int n = strlen(buf);
        if (n > 0 && buf[n-1] == '\n') {
          if (n > 1 && buf[n-2] == '\r') buf[--n] = 0;
          buf[--n] = 0;
        }
        if (n > 0) Log(LL_ERR, "Perl error: %s", buf);
      }
      fclose(R);
      fflush(stderr);
      exit(0);
    }
    /* error */
    else if (errno != EINTR) {
      perl_errpid = 0;
      close(errpipe[0]);
      close(errpipe[1]);
      Log(LL_ERR, "handle_perlerr(): can't fork (%s)", strerror(errno));
      return;
    }
    perl_errpid = 0;
    try++;
  }
#elif defined(HAVE_THREADS)
   fflush(stderr);
   pipe(errpipe);
   perl_olderr = fdopen(dup(fileno(stderr)), "a");
   dup2(errpipe[1], fileno(stderr));
   close(errpipe[1]);
   perl_errpid = BEGINTHREAD(&err_thread, STACKSIZE, &(errpipe[0]));
   if (perl_errpid <= 0) {
     perl_errpid = 0;
     Log(LL_ERR, "handle_perlerr(): can't begin thread (%s)", strerror(errno));
     close(errpipe[0]);
     dup2(fileno(perl_olderr), fileno(stderr));
     fclose(perl_olderr);
     perl_olderr = NULL;
   }
#else
 /* Don't know how to hanlde Perl errors in this case */
#endif
}
#ifdef HAVE_THREADS
#define handle_perlerr()
#define restore_perlerr()
#else
#define handle_perlerr() handle_perlerr2()

/* restore perl errors handler, read pipe to var and restore stderr */
static void restore_perlerr(void) {
  if (perl_errpid) {
    fflush(perl_olderr);
    dup2(fileno(perl_olderr), fileno(stderr));
    fclose(perl_olderr);
#ifdef HAVE_WAITPID
    waitpid(perl_errpid, &perl_errpid, 0);
    perl_errpid = 0;
#else
    /* will be reaped by SIGCHLD handler */
#endif
  }
}
#endif

/* handle multi-line perl eval error message */
static void sub_err(int sub) {
STRLEN len;
char *s, *p;
  p = SvPV(ERRSV, len);
  if (len) { s = xalloc(len+1); strnzcpy(s, p, len+1); }
    else s = "(empty error message)";
  if ( strchr(s, '\n') == NULL )
    Log(LL_ERR, "Perl %s error: %s", perl_subnames[sub], s);
    else {
      p = s;
      Log(LL_ERR, "Perl %s error below:", perl_subnames[sub]);
      while ( *p && (*p != '\n' || *(p+1)) ) {
        char *r = strchr(p, '\n');
        if (r) { *r = 0; Log(LL_ERR, "  %s", p); p = r+1; }
        else { Log(LL_ERR, "  %s", p); break; }
      }
    }
  free(s);
}

/* =========================== xs ========================== */
/* interface to Log() */
#ifdef _MSC_VER
  EXTERN_C void xs_init (pTHXo);
# ifndef PERLDL
  EXTERN_C void boot_DynaLoader (pTHXo_ CV* cv);
# endif
  EXTERN_C void perl_Log(pTHXo_ CV* cv);
  EXTERN_C void perl_aeq(pTHXo_ CV* cv);
  EXTERN_C void perl_arm(pTHXo_ CV* cv);
#else
# ifndef PERLDL
  XS(boot_DynaLoader);
# else
  /* I don't know how to use DynaLoader with PERLDL under OS/2 :-( */
# endif
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
  BINKD_CONFIG *cfg;
  STRLEN len;
  int i;
  char *a, *b;
  FTN_ADDR A, B;
  int mask_mode = 0;

  if (items == 1) { 
    Log(LL_ERR, "aeq() requires 2 or more parameters, %d exist", items);
    XSRETURN_UNDEF;
  }
  VK_FIND_CONFIG(cfg);
  a = (char *)SvPV(ST(0), len); 
  if (len == 0) XSRETURN_UNDEF;
    else if (strchr(a, '*')) mask_mode = 1;
    else if (!parse_ftnaddress(a, &A, cfg->pDomains.first)) XSRETURN_UNDEF;
    else exp_ftnaddress(&A, cfg->pAddr, cfg->nAddr, cfg->pDomains.first);
  for (i = 1; i < items; i++) {
    b = (char *)SvPV(ST(i), len);
    if (len == 0 || !parse_ftnaddress(b, &B, cfg->pDomains.first)) continue;
    exp_ftnaddress(&B, cfg->pAddr, cfg->nAddr, cfg->pDomains.first);
    if (mask_mode) {
      char bb[FTN_ADDR_SZ];
      xftnaddress_to_str(bb, &B, 1);
      if (ftnamask_cmps(a, bb) == 0) XSRETURN_IV(1);
    } else {
      if (ftnaddress_cmp(&A, &B) == 0) XSRETURN_IV(1);
    }
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
  BINKD_CONFIG *cfg;
  STRLEN len;
  int i, j, k, K, N, m;
  char *a, *b, **C;
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
  VK_FIND_CONFIG(cfg);
  arr = (AV*)SvRV(ST(0));
  B = xalloc( (items-1)*sizeof(FTN_ADDR) );
  C = xalloc( (items-1)*sizeof(char*) );
  memset(C, 0, (items-1)*sizeof(char*));
  /* i - args, k - valid addresses */
  for (i = 1, k = K = 0; i < items; i++) {
    b = (char *)SvPV(ST(i), len);
    if (len == 0) continue;
    if (strchr(b, '*')) { C[K++] = b; continue; }
    if (!parse_ftnaddress(b, B+k, cfg->pDomains.first)) continue;
    exp_ftnaddress(B+k, cfg->pAddr, cfg->nAddr, cfg->pDomains.first);
    k++;
  }
  /* j - array elements, i - valid addresses */
  N = av_len(arr) + 1;
  for (j = m = 0; j < N; j++) {
    int found = 0;
    svp = av_fetch(arr, j, FALSE);
    if (j != m) av_store(arr, m, (*svp));
    if (!svp) { m++; continue; }
    a = (char *)SvPV((*svp), len);
    if (len == 0 || !parse_ftnaddress(a, &A, cfg->pDomains.first)) { m++; continue; }
    exp_ftnaddress(&A, cfg->pAddr, cfg->nAddr, cfg->pDomains.first);
    /* compare */
    for (i = 0; i < k; i++) {
      if (ftnaddress_cmp(&A, B+i) == 0) { found = 1; break; }
    }
    for (i = 0; i < K; i++) {
      char aa[FTN_ADDR_SZ];
      xftnaddress_to_str(aa, &A, 1);
      if (ftnamask_cmps(C[i], aa) == 0) { found = 1; break; }
    }
    if (!found) m++;
  }
  /* reduce array size */
  if (m != N) AvFILLp(arr) = m-1;
  free(B); free(C);
  XSRETURN_IV(N - m);
}
/* puts a message to msg-queue */
extern void msg_send2 (STATE *state, t_msg m, char *s1, char *s2);
#ifdef _MSC_VER
  EXTERN_C void perl_msg_send(pTHXo_ CV* cv)
#else
  static XS(perl_msg_send)
#endif
{
  dXSARGS;
  SV *sv;
  STATE *state;
  t_msg m;
  char *str;
  STRLEN n_a;

  if (items != 2) {
    Log(LL_ERR, "wrong params number to msg_send (needs 2, exist %d)", items);
    XSRETURN_EMPTY;
  }
  sv = perl_get_sv(sv_state, FALSE);
  if (!sv) { Log(LL_ERR, "can't find $%s pointer", sv_state); XSRETURN_EMPTY; }
  state = (STATE*)SvIV(sv);
  if (!state) { Log(LL_ERR, "$%s pointer is NULL", sv_state); XSRETURN_EMPTY; }
  m = (t_msg)SvIV(ST(0));
  str = (char *)SvPV(ST(1), n_a); if (n_a == 0) str = "";
  msg_send2(state, m, str, NULL);
  XSRETURN_EMPTY;
}

#if defined(OS2) && !defined(PERLDL)
void boot_DynaLoader(CV *cv);
void boot_DB_File(CV *cv);
void boot_Fcntl(CV *cv);
void boot_POSIX(CV *cv);
void boot_SDBM_File(CV *cv);
void boot_IO(CV *cv);
void boot_OS2__Process(CV *cv);
void boot_OS2__ExtAttr(CV *cv);
void boot_OS2__REXX(CV *cv);
#endif

/* xs_init */
#ifdef WIN32
EXTERN_C void xs_init (pTHXo)
#else
static void xs_init(void)
#endif
{
  static char *file = __FILE__;
#if defined(OS2)
#ifndef PERLDL
  newXS("DB_File::bootstrap", boot_DB_File, file);
  newXS("Fcntl::bootstrap", boot_Fcntl, file);
  newXS("POSIX::bootstrap", boot_POSIX, file);
  newXS("SDBM_File::bootstrap", boot_SDBM_File, file);
  newXS("IO::bootstrap", boot_IO, file);
  newXS("OS2::Process::bootstrap", boot_OS2__Process, file);
  newXS("OS2::ExtAttr::bootstrap", boot_OS2__ExtAttr, file);
  newXS("OS2::REXX::bootstrap", boot_OS2__REXX, file);
#endif
#else
  dXSUB_SYS;
#endif
#if !defined(OS2) || !defined(PERLDL)
  newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
#endif
  newXS("Log", perl_Log, file);
  newXS("aeq", perl_aeq, file);
  newXS("arm", perl_arm, file);
  newXS("msg_send", perl_msg_send, file);
}

/* =========================== sys ========================== */

static char *perlargs[] = {"", NULL, NULL};
/* init root perl, parse hooks file, return success */
int perl_init(char *perlfile, BINKD_CONFIG *cfg) {
  int rc, i;
  SV *sv;

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
#ifdef PERLDL
  /* load DLL */
  if (cfg->perl_dll[0]) {
    struct perl_dlfunc *dlfunc;
#ifdef OS2
    char buf[256];
    HMODULE hl;
    if (DosLoadModule(buf, sizeof(buf), cfg->perl_dll, &hl))
#else /* MSC */
    HINSTANCE hl = LoadLibrary(cfg->perl_dll);
    if (!hl)
#endif
    {
      Log(LL_ERR, "perl_init(): can't load library %s", cfg->perl_dll);
      return 0;
    }
    Log(LL_DBG2, "perl_init(): load library: %p", hl);

    for (dlfunc = perl_dlfuncs; dlfunc->name; dlfunc++) {
#ifdef OS2
      if (!DosQueryProcAddr(hl, 0, dlfunc->name, (PFN*)dlfunc->f))
#else
      if ((*(dlfunc->f) = GetProcAddress(hl, dlfunc->name)))
#endif
        Log(LL_DBG2, "perl_init(): load method %s: %p", dlfunc->name, *(dlfunc->f));
      else {
        Log(LL_ERR, "perl_init(): can't load method %s", dlfunc->name);
        return 0;
      }
    }
#ifdef OS2
    {
      if (!DosQueryProcAddr(hl, 0, "Perl_sv_2pv", (PFN*)dl_Perl_sv_2pv) &&
          !DosQueryProcAddr(hl, 0, "Perl_sv_2pv_flags", (PFN*)dl_Perl_sv_2pv_flags)) {
        return 0;
        Log(LL_ERR, "perl_init(): can't load method Perl_sv_2pv or Perl_sv_2pv_flags");
      }
      if (!DosQueryProcAddr(hl, 0, "Perl_sv_setsv", (PFN*)dl_Perl_sv_setsv) &&
          !DosQueryProcAddr(hl, 0, "Perl_sv_setsv_flags", (PFN*)dl_Perl_sv_setsv_flags)) {
        return 0;
        Log(LL_ERR, "perl_init(): can't load method Perl_sv_setsv or Perl_sv_setsv_flags");
      }
    }
#else
    {
      (void*)dl_Perl_sv_2pv = GetProcAddress(hl, "Perl_sv_2pv");
      (void*)dl_Perl_sv_2pv_flags = GetProcAddress(hl, "Perl_sv_2pv_flags");
      if (!dl_Perl_sv_2pv && !dl_Perl_sv_2pv_flags) {
        Log(LL_ERR, "perl_init(): can't load method Perl_sv_2pv or Perl_sv_2pv_flags");
        return 0;
      }
      (void*)dl_Perl_sv_setsv = GetProcAddress(hl, "Perl_sv_setsv");
      (void*)dl_Perl_sv_setsv_flags = GetProcAddress(hl, "Perl_sv_setsv_flags");
      if (!dl_Perl_sv_setsv && !dl_Perl_sv_setsv_flags) {
        Log(LL_ERR, "perl_init(): can't load method Perl_sv_setv or Perl_sv_setsv_flags");
      }
    }
#endif
  } 
  else {
    Log(LL_ERR, "You should define `perl-dll' in config to use Perl hooks");
    return 0;
  }
#endif                                                      /* PERLDL */
  /* init perl */
  perl = perl_alloc();
  perl_construct(perl);
  handle_perlerr2();
  rc = perl_parse(perl, xs_init, i, perlargs, NULL);
  restore_perlerr();
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
  perl_setup(cfg);
  /* run main program body */
  Log(LL_DBG, "perl_init(): running body");
  handle_perlerr();
  perl_run(perl);
  restore_perlerr();
  /* scan for present hooks */
  for (i = 0; i < sizeof(perl_subnames)/sizeof(perl_subnames[0]); i++) {
    if (perl_get_cv(perl_subnames[i], FALSE)) perl_ok |= (1 << i);
  }
  /* run on_start() */
  perl_on_start();
  /* init perl queue management */
  if ( (sv = perl_get_sv("want_queue", FALSE)) ) { perl_wants_queue = SvTRUE(sv); }
  if ( (sv = perl_get_sv("manage_queue", FALSE)) ) { perl_manages_queue = SvTRUE(sv); }
  if (perl_manages_queue && !perl_wants_queue) {
    Log(LL_ERR, "Perl queue management requires $want_queue to be set");
    perl_manages_queue = 0;
  }
  Log(LL_DBG, "perl_init(): end");
  return 1;
}

static int add_node_to_hv(FTN_NODE *node, void *hv)
{
  HV *hv2;
  SV *sv;
  char  buf[FTN_ADDR_SZ];

  hv2 = newHV();
  VK_ADD_HASH_str(hv2, sv, "hosts", node->hosts);
  VK_ADD_HASH_str(hv2, sv, "pwd", node->pwd);
  VK_ADD_HASH_str(hv2, sv, "ibox", node->ibox);
  VK_ADD_HASH_str(hv2, sv, "obox", node->obox);
  buf[0] = node->obox_flvr; buf[1] = 0;
  VK_ADD_HASH_str(hv2, sv, "obox_flvr", buf);
  VK_ADD_HASH_int(hv2, sv, "NR", node->NR_flag);
  VK_ADD_HASH_int(hv2, sv, "ND", node->ND_flag);
  VK_ADD_HASH_int(hv2, sv, "MD", node->MD_flag);
  VK_ADD_HASH_int(hv2, sv, "HC", node->HC_flag);
  VK_ADD_HASH_int(hv2, sv, "IP", node->restrictIP);
  sv = newRV_noinc( (SV*)hv2 );
  ftnaddress_to_str(buf, &(node->fa));
  VK_ADD_HASH_sv((HV *)hv, sv, buf);
  return 0;
}

/* set config vars to root perl */
void perl_setup(BINKD_CONFIG *cfg) {
  SV 	*sv;
  HV 	*hv, *hv2;
  AV 	*av;
  char  buf[FTN_ADDR_SZ];
  int   i;

  if (!perl) return;
  Log(LL_DBG, "perl_setup(): perl context %p", Perl_get_context());

  hv = perl_get_hv("config", TRUE);
  hv_clear(hv);
  VK_ADD_HASH_str(hv, sv, "log", cfg->logpath);
  VK_ADD_HASH_intz(hv, sv, "loglevel", cfg->loglevel);
  VK_ADD_HASH_intz(hv, sv, "conlog", cfg->conlog);
  VK_ADD_HASH_intz(hv, sv, "tzoff", cfg->tzoff);
  VK_ADD_HASH_str(hv, sv, "sysname", cfg->sysname);
  VK_ADD_HASH_str(hv, sv, "sysop", cfg->sysop);
  VK_ADD_HASH_str(hv, sv, "location", cfg->location);
  VK_ADD_HASH_str(hv, sv, "nodeinfo", cfg->nodeinfo);
  VK_ADD_HASH_str(hv, sv, "bindaddr", cfg->bindaddr);
  VK_ADD_HASH_intz(hv, sv, "iport", cfg->iport);
  VK_ADD_HASH_intz(hv, sv, "oport", cfg->oport);
  VK_ADD_HASH_intz(hv, sv, "maxservers", cfg->max_servers);
  VK_ADD_HASH_intz(hv, sv, "maxclients", cfg->max_clients);
  VK_ADD_HASH_intz(hv, sv, "oblksize", cfg->oblksize);
  VK_ADD_HASH_str(hv, sv, "inbound", cfg->inbound);
  VK_ADD_HASH_str(hv, sv, "inbound_nonsecure", cfg->inbound_nonsecure);
  VK_ADD_HASH_intz(hv, sv, "inboundcase", cfg->inboundcase);
  VK_ADD_HASH_str(hv, sv, "temp_inbound", cfg->temp_inbound);
  VK_ADD_HASH_intz(hv, sv, "minfree", cfg->minfree);
  VK_ADD_HASH_intz(hv, sv, "minfree_nonsecure", cfg->minfree_nonsecure);
  VK_ADD_HASH_intz(hv, sv, "hold", cfg->hold);
  VK_ADD_HASH_intz(hv, sv, "hold_skipped", cfg->hold_skipped);
  VK_ADD_HASH_intz(hv, sv, "backresolv", cfg->backresolv);
  VK_ADD_HASH_intz(hv, sv, "send_if_pwd", cfg->send_if_pwd);
  VK_ADD_HASH_str(hv, sv, "filebox", cfg->tfilebox);
  VK_ADD_HASH_str(hv, sv, "brakebox", cfg->bfilebox);
  VK_ADD_HASH_str(hv, sv, "root_domain", cfg->root_domain);
  VK_ADD_HASH_int(hv, sv, "check_pkthdr", cfg->pkthdr_type);
  VK_ADD_HASH_str(hv, sv, "pkthdr_badext", cfg->pkthdr_bad);
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
  VK_ADD_HASH_intz(hv, sv, "zaccept", cfg->zaccept);
  VK_ADD_HASH_intz(hv, sv, "zminsize", cfg->zminsize);
#endif
  Log(LL_DBG2, "perl_setup(): %%config done");
  /* domain */
  hv = perl_get_hv("domain", TRUE);
  hv_clear(hv);
  {
    FTN_DOMAIN *cur = cfg->pDomains.first;
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
  for (i = 0; i < cfg->nAddr; i++) {
    ftnaddress_to_str(buf, &(cfg->pAddr[i]));
    sv = newSVpv(buf, 0);
    SvREADONLY_on(sv);
    av_push(av, sv);
  }
  Log(LL_DBG2, "perl_setup(): @addr done");
  /* ftrans */
  av = perl_get_av("ftrans", TRUE);
  av_clear(av);
  {
    RF_RULE *cur = cfg->rf_rules.first;
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
    struct maskchain *cur = cfg->overwrite.first;
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
    struct skipchain *cur = cfg->skipmask.first;
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
    SHARED_CHAIN *cur = cfg->shares.first;
    FTN_ADDR_CHAIN *fa;
    while (cur) {
      av = newAV();
      fa = cur->sfa.first;
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
  foreach_node(add_node_to_hv, hv, cfg);
  Log(LL_DBG2, "perl_setup(): %%node done");
}

/* deallocate root perl, call on_exit() if master==1 */
void perl_done(int master) {
  Log(LL_DBG, "perl_done(): perl=%p", perl);
  if (perl) {
    /* run on_exit() */
    if (master) perl_on_exit();
#ifdef HAVE_THREADS
    /* exit err_thread */
    if (perl_olderr)
    {
      fflush(perl_olderr);
      fflush(stderr);
      dup2(fileno(perl_olderr), fileno(stderr)); /* close handled stderr */
      WaitSem (&eothread, 1);
    }
#endif
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
void *perl_init_clone(BINKD_CONFIG *cfg) {
  UV cflags = 0;
  PerlInterpreter *p;

  if (perl) {
    Log(LL_DBG2, "perl_init_clone(), parent perl=%p, context=%p", perl, Perl_get_context());
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
      perl_setup(cfg);
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
#if !defined(WIN32) || defined(CLONEf_CLONE_HOST)
  perl_free((PerlInterpreter *)p);
#endif
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
  SvREADONLY_on( (SV*)av );
}

/* set session vars */
static void setup_session(STATE *state, int lvl) {
  BINKD_CONFIG *cfg;
  SV 	*sv;
  HV 	*hv;
  struct sockaddr_in sin;
  socklen_t sin_len = sizeof(sin);

  if (!perl) return;
  Log(LL_DBG2, "perl_setup_session(), perl context %p", Perl_get_context());
  if (!Perl_get_context()) return;

  VK_FIND_CONFIG(cfg);
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
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
    VK_ADD_HASH_intz(hv, sv, "GZ", state->z_cansend);
#endif
    setup_addrs("he", state->nfa, state->fa);
    if (state->nAddr && state->pAddr)
      setup_addrs("me", state->nAddr, state->pAddr);
      else setup_addrs("me", cfg->nAddr, cfg->pAddr);
#if defined(WITH_ZLIB) || defined(WITH_BZLIB2)
    VK_ADD_intz(sv, "z_send", state->z_send);
    VK_ADD_intz(sv, "z_recv", state->z_recv);
#endif
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
  BINKD_CONFIG *cfg;
  FTNQ *q = NULL, *qp = NULL, *q0 = NULL;
  AV *av;
  HV *hv;
  SV **svp;
  STRLEN len;
  int i, n;
  char *s;

  Log(LL_DBG2, "perl_refresh_queue()");
  VK_FIND_CONFIG(cfg);
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
    s = SvPV(*svp, len);
    if (len == 0) continue;
    qp = q;
    q = xalloc( sizeof(FTNQ) ); FQ_ZERO(q);
    if (!q0) q0 = q;
      else { qp->next = q; q->prev = qp; }
    strnzcpy(q->path, s, min(len+1, MAXPATHLEN));
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
      if (!parse_ftnaddress(s, &(q->fa), cfg->pDomains.first)) q->fa = state->fa[0];
      else exp_ftnaddress(&(q->fa), cfg->pAddr, cfg->nAddr, cfg->pDomains.first);
    } else q->fa = state->fa[0];
  }
  if (queue != SCAN_LISTED) q_free(queue, cfg);
  Log(LL_DBG2, "perl_refresh_queue() end");
  return q0;
}

/* =========================== hooks ========================== */

/* start, after init */
void perl_on_start(void) {
  if (perl_ok & (1 << PERL_ON_START)) {
     Log(LL_DBG, "perl_on_start(), perl=%p", Perl_get_context());
     handle_perlerr();
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
     restore_perlerr();
     if (SvTRUE(ERRSV)) sub_err(PERL_ON_START);
/*     {
       Log(LL_ERR, "Perl on_start() error: %s", SvPV(ERRSV, n_a));
     }*/
     Log(LL_DBG, "perl_on_start() end");
  }
}

/* exit, just before destruction */
void perl_on_exit(void) {
  if (perl_ok & (1 << PERL_ON_EXIT)) {
#ifdef HAVE_THREADS
     PERL_SET_CONTEXT(perl);
#endif
     Log(LL_DBG, "perl_on_exit(), perl=%p", Perl_get_context());
     handle_perlerr();
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
     restore_perlerr();
     if (SvTRUE(ERRSV)) sub_err(PERL_ON_EXIT);
     Log(LL_DBG, "perl_on_exit() end");
  }
}

/* before outgoing call */
int perl_on_call(FTN_NODE *node) {
  char   buf[FTN_ADDR_SZ];
  int    rc;
  SV     *svret, *sv;

  if (perl_ok & (1 << PERL_ON_CALL)) {
    Log(LL_DBG, "perl_on_call(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return 1;
    { dSP;
      ftnaddress_to_str(buf, &(node->fa));
      VK_ADD_str(sv, "addr", buf);
      handle_perlerr();
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
      restore_perlerr();
      if (SvTRUE(ERRSV)) sub_err(PERL_ON_CALL);
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

  if (perl_ok & (1 << PERL_ON_ERROR)) {
    Log(LL_DBG, "perl_on_error(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return 1;
    { dSP;
      ftnaddress_to_str(buf, addr);
      VK_ADD_str (sv, "addr", buf);
      VK_ADD_str (sv, "error", error);
      VK_ADD_intz(sv, "where", where);
      handle_perlerr();
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
      restore_perlerr();
      if (SvTRUE(ERRSV)) sub_err(PERL_ON_ERROR);
    }
    Log(LL_DBG, "perl_on_error() end");
    return rc;
  }
  return 1;
}

/* before xmitting ADR */
char *perl_on_handshake(STATE *state, BINKD_CONFIG *cfg) {
  char buf[FTN_ADDR_SZ];
  char *prc;
  int i;
  STRLEN len;
  AV *he, *me;
  SV *svret, **svp;
  /* this pointer is used later */
  if (perl && perl_ok) 
    if (Perl_get_context()) {
      VK_ADD_intz(svret, sv_state, (long)state); 
      VK_ADD_intz(svret, sv_config, (long)cfg);
    }
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
      handle_perlerr();
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
      restore_perlerr();
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_ON_HANDSHAKE);
        if (prc) free(prc);
        prc = NULL;
      }
      /* make array of our aka */
      else if (!prc && ((me = perl_get_av("me", FALSE)) != NULL)) {
        FTN_ADDR addr;
        int n = 0, N = av_len(me) + 1;
        if (N > 0) state->pAddr = xalloc(N*sizeof(FTN_ADDR));
        for (i = 0; i < N; i++) {
          svp = av_fetch(me, i, FALSE);
          if (svp == NULL) continue;
          if (!parse_ftnaddress(SvPV(*svp, len), &addr, cfg->pDomains.first)) continue;
          exp_ftnaddress(&addr, cfg->pAddr, cfg->nAddr, cfg->pDomains.first);
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
      handle_perlerr();
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
      restore_perlerr();
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_AFTER_HANDSHAKE);
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

  if (perl && perl_ok) 
    if (Perl_get_context()) { VK_ADD_intz(sv, "__state", 0); }
  if (perl_ok & (1 << PERL_AFTER_SESSION)) {
    Log(LL_DBG, "perl_after_session(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return;
    { dSP;
      if (state->perl_set_lvl < 3) setup_session(state, 3);
      VK_ADD_intz(sv, "rc", (STRICMP(status, "OK") == 0));
      handle_perlerr();
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_AFTER_SESSION], G_EVAL|G_VOID);
      SPAGAIN;
      PUTBACK;
      FREETMPS;
      LEAVE;
      restore_perlerr();
      if (SvTRUE(ERRSV)) sub_err(PERL_AFTER_SESSION);
    }
    Log(LL_DBG, "perl_after_session() end");
  }
}

/* before receiving file */
int perl_before_recv(STATE *state, off_t offs) {
  int    rc;
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
      handle_perlerr();
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
      restore_perlerr();
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_BEFORE_RECV);
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
int perl_after_recv(STATE *state, TFILE *file, char *tmp_name, char *real_name) {
  int    rc;
  STRLEN len;
  SV     *svret, *sv;

  if (perl_ok & (1 << PERL_AFTER_RECV)) {
    Log(LL_DBG, "perl_after_recv(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return 0;
    { dSP;
      if (state->perl_set_lvl < 2) setup_session(state, 2);
      if (perl_wants_queue) setup_queue(state, state->q);
      VK_ADD_str (sv, "name", file->netname);
      VK_ADD_intz(sv, "size", file->size);
      VK_ADD_intz(sv, "time", file->time);
      VK_ADD_str (sv, "tmpfile", tmp_name);
      VK_ADD_str (sv, "file", real_name); if (sv) { SvREADONLY_off(sv); }
      handle_perlerr();
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
      restore_perlerr();
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_AFTER_RECV);
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
      handle_perlerr();
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
      restore_perlerr();
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_BEFORE_SEND);
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
      handle_perlerr();
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
      restore_perlerr();
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_AFTER_SENT);
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
int perl_on_log(char *s, int bufsize, int *lev) {
  int    rc = 1;
  STRLEN len;
  SV     *svret, *sv, *svchk;

  if (!perl || !perl_ok) return 1;
  if (!Perl_get_context()) return 1;
  if (perl_ok & (1 << PERL_ON_LOG)) {
    /* check: not to call while on_log() is running */
    svchk = perl_get_sv("__in_log", FALSE);
    if (svchk && SvIV(svchk) == 1) return 1;
    VK_ADD_intz(svchk, "__in_log", 1);
    if (!svchk) return 1; /* can't guarantee there won't be recursion */
    /* end of check */
    Log(LL_DBG2, "perl_on_log(), perl=%p", Perl_get_context());
    { dSP;
      VK_ADD_intz(sv, "lvl", *lev); if (sv) { SvREADONLY_off(sv); }
      VK_ADD_str (sv, "_", s); if (sv) { SvREADONLY_off(sv); }
      handle_perlerr();
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_ON_LOG], G_EVAL|G_SCALAR);
      SPAGAIN;
      svret=POPs;
      if (SvOK(svret)) rc = SvIV(svret); else rc = 0;
      PUTBACK;
      FREETMPS;
      LEAVE;
      restore_perlerr();
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_ON_LOG);
        rc = 1;
      }
      else if (rc && sv) {
        char *p = SvPV(sv, len);
        if (!p || len == 0 || *p == 0) rc = 0; 
        else {
          rc = 1;
          strnzcpy(s, p, min((int)len+1, bufsize));
          sv = perl_get_sv("lvl", FALSE); if (sv) *lev = SvIV(sv);
        }
      }
      else rc = 1;
    }
    Log(LL_DBG2, "perl_on_log() end");
    sv_setiv(svchk, 0); /* check: now we can restore */
    return rc;
  }
  return 1;
}

int perl_on_send(STATE *state, t_msg *m, char **s1, char **s2) {
  int    rc = 1;
  SV     *svret, *sv;

  if (perl_ok & (1 << PERL_ON_SEND)) {
    Log(LL_DBG2, "perl_on_send(), perl=%p", Perl_get_context());
    { dSP;
      VK_ADD_intz(sv, "type", *m);
      VK_ADD_str (sv, "s1", *s1);
      VK_ADD_str (sv, "s2", *s2);
      handle_perlerr();
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_ON_SEND], G_EVAL|G_SCALAR);
      SPAGAIN;
      svret=POPs;
      /*if (SvOK(svret)) rc = SvIV(svret); else rc = 0;*/
      PUTBACK;
      FREETMPS;
      LEAVE;
      restore_perlerr();
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_ON_SEND);
      }
    }
    Log(LL_DBG2, "perl_on_send() end");
    return rc;
  }
  return 1;
}

int perl_on_recv(STATE *state, char *s, int size) { 
  int    rc = 1;
  SV     *svret, *sv;

  if (perl_ok & (1 << PERL_ON_RECV)) {
    Log(LL_DBG2, "perl_on_recv(), perl=%p", Perl_get_context());
    { dSP;
      if ( (sv = perl_get_sv("s", TRUE)) ) {
        sv_setpvn(sv, s, size); SvREADONLY_on(sv);
      }
      handle_perlerr();
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_ON_RECV], G_EVAL|G_SCALAR);
      SPAGAIN;
      svret=POPs;
      /*if (SvOK(svret)) rc = SvIV(svret); else rc = 0;*/
      PUTBACK;
      FREETMPS;
      LEAVE;
      restore_perlerr();
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_ON_RECV);
      }
    }
    Log(LL_DBG2, "perl_on_recv() end");
    return rc;
  }
  return 1;
}
