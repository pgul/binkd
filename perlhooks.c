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
#include "common.h"
#include "perlhooks.h"
#include "rfc2553.h"
/* ---------------- perl stuff --------------- */
/* dynamic load */
#ifdef PERLDL
# define Perl_sv_2pv			(hl_Perl_sv_2pv)
# define Perl_sv_2pv_flags		(hl_Perl_sv_2pv_flags)
# define Perl_sv_2uv			(hl_Perl_sv_2uv)
# define Perl_sv_2uv_flags		(hl_Perl_sv_2uv_flags)
# define Perl_sv_2iv			(hl_Perl_sv_2iv)
# define Perl_sv_2iv_flags		(hl_Perl_sv_2iv_flags)
# define Perl_sv_setsv			(hl_Perl_sv_setsv)
# define Perl_sv_setsv_flags		(hl_Perl_sv_setsv_flags)
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
# define Perl_sv_setpv			(*dl_Perl_sv_setpv)
# define Perl_sv_setpvn			(*dl_Perl_sv_setpvn)
# define Perl_av_undef			(*dl_Perl_av_undef)
# define Perl_hv_fetch			(*dl_Perl_hv_fetch)
# define Perl_eval_pv			(*dl_Perl_eval_pv)
# define Perl_newRV			(*dl_Perl_newRV)
# define Perl_sv_free			(*dl_Perl_sv_free)
# define Perl_sv_free2			(*dl_Perl_sv_free2)
# define Perl_hv_common_key_len		(*dl_Perl_hv_common_key_len)
# define Perl_sys_init3			(*dl_Perl_sys_init3)
#ifdef OS2
# define PL_errgv			(*dl_PL_errgv)
# define PL_stack_sp			(*dl_PL_stack_sp)
# define PL_markstack_ptr		(*dl_PL_markstack_ptr)
# define PL_sv_undef			(*dl_PL_sv_undef)
# define PL_stack_base			(*dl_PL_stack_base)
# define PL_markstack_max		(*dl_PL_markstack_max)
# define PL_tmps_ix			(*dl_PL_tmps_ix)
# define PL_tmps_floor			(*dl_PL_tmps_floor)
# define PL_diehook			(*dl_PL_diehook)
# define PL_warnhook			(*dl_PL_warnhook)
# define Perl_sv_setpvf			(*dl_Perl_sv_setpvf)
# define perl_get_sv			(*dl_perl_get_sv)
# define perl_get_av			(*dl_perl_get_av)
# define perl_get_hv			(*dl_perl_get_hv)
# define perl_get_cv			(*dl_perl_get_cv)
# define perl_call_pv			(*dl_perl_call_pv)
#else
# define Perl_Ierrgv_ptr		(*dl_Perl_Ierrgv_ptr)
# define Perl_Isv_undef_ptr		(*dl_Perl_Isv_undef_ptr)
# define Perl_Tstack_sp_ptr		(*dl_Perl_Tstack_sp_ptr)
# define Perl_Tmarkstack_ptr_ptr	(*dl_Perl_Tmarkstack_ptr_ptr)
# define Perl_Tstack_base_ptr		(*dl_Perl_Tstack_base_ptr)
# define Perl_Tmarkstack_max_ptr	(*dl_Perl_Tmarkstack_max_ptr)
# define Perl_Ttmps_ix_ptr		(*dl_Perl_Ttmps_ix_ptr)
# define Perl_Ttmps_floor_ptr		(*dl_Perl_Ttmps_floor_ptr)
# define Perl_Tscopestack_ix_ptr	(*dl_Perl_Tscopestack_ix_ptr)
# define Perl_Tmarkstack_ptr    	(*dl_Perl_Tmarkstack_ptr)
# define Perl_TXpv_ptr			(*dl_Perl_TXpv_ptr)
# define Perl_Istack_base_ptr		(*dl_Perl_Istack_base_ptr)
# define Perl_Istack_sp_ptr		(*dl_Perl_Istack_sp_ptr)
# define Perl_Imarkstack_max_ptr	(*dl_Perl_Imarkstack_max_ptr)
# define Perl_Imarkstack_ptr_ptr	(*dl_Perl_Imarkstack_ptr_ptr)
# define Perl_Itmps_floor_ptr		(*dl_Perl_Itmps_floor_ptr)
# define Perl_Itmps_ix_ptr		(*dl_Perl_Itmps_ix_ptr)
# define Perl_Iscopestack_ix_ptr	(*dl_Perl_Iscopestack_ix_ptr)
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
# define Perl_Idiehook_ptr		(*dl_Perl_Idiehook_ptr)
# define Perl_Iwarnhook_ptr		(*dl_Perl_Iwarnhook_ptr)
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

#ifndef eval_pv
#  define eval_pv perl_eval_pv
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
PERL_CALLCONV void	(*dl_Perl_av_push)(pTHX_ AV* ar, SV* val);
PERL_CALLCONV void	(*dl_Perl_av_clear)(pTHX_ AV* ar);
#if PERL_REVISION<5 || (PERL_REVISION==5 && PERL_VERSION<10)
PERL_CALLCONV I32	(*dl_Perl_av_len)(pTHX_ AV* ar);
PERL_CALLCONV CV*	(*dl_Perl_newXS)(pTHX_ char* name, XSUBADDR_t f, char* filename);
#else
PERL_CALLCONV I32	(*dl_Perl_av_len)(pTHX_ const AV* ar);
PERL_CALLCONV CV*	(*dl_Perl_newXS)(pTHX_ const char* name, XSUBADDR_t f, const char* filename);
#endif
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
PERL_CALLCONV SV*       (*dl_Perl_eval_pv)(pTHX_ const char* p, I32 croak_on_error);
PERL_CALLCONV SV*       (*dl_Perl_newRV)(pTHX_ SV* pref);
PERL_CALLCONV void      (*dl_Perl_sv_free)(pTHX_ SV* sv);
PERL_CALLCONV void      (*dl_Perl_sv_free2)(pTHX_ SV* sv);
PERL_CALLCONV void*     (*dl_Perl_hv_common_key_len)(pTHX_ HV* hv, const char *key, I32 klen_i32, const int action, SV *val, const U32 hash);
PERL_CALLCONV void      (*dl_Perl_sys_init3)(int *argc, char ***argv, char ***env);
#ifdef CHECK_FORMAT
 __attribute__((format(printf,2,3)))
#endif
;
PERL_CALLCONV int* (*dl_Perl_Iperl_destruct_level_ptr)(pTHXo);
PERL_CALLCONV GV** (*dl_Perl_Ierrgv_ptr)(pTHXo);
PERL_CALLCONV SV*  (*dl_Perl_Isv_undef_ptr)(pTHXo);
PERL_CALLCONV SV**  (*dl_Perl_Idiehook_ptr)(pTHXo);
PERL_CALLCONV SV**  (*dl_Perl_Iwarnhook_ptr)(pTHXo);
#if PERL_REVISION<5 || (PERL_REVISION==5 && PERL_VERSION<10)
PERL_CALLCONV I32* (*dl_Perl_Tscopestack_ix_ptr)(pTHXo);
PERL_CALLCONV I32** (*dl_Perl_Tmarkstack_ptr)(pTHXo);
PERL_CALLCONV I32** (*dl_Perl_Tmarkstack_max_ptr)(pTHXo);
PERL_CALLCONV I32* (*dl_Perl_Ttmps_ix_ptr)(pTHXo);
PERL_CALLCONV I32* (*dl_Perl_Ttmps_floor_ptr)(pTHXo);
PERL_CALLCONV XPV** (*dl_Perl_TXpv_ptr)(pTHXo);
PERL_CALLCONV SV*** (*dl_Perl_Tstack_base_ptr)(pTHXo);
PERL_CALLCONV I32** (*dl_Perl_Tmarkstack_ptr_ptr)(pTHXo);
PERL_CALLCONV SV*** (*dl_Perl_Tstack_sp_ptr)(pTHXo);
#else
PERL_CALLCONV SV**  (*dl_Perl_Istack_base_ptr)(pTHXo);
PERL_CALLCONV SV**  (*dl_Perl_Istack_sp_ptr)(pTHXo);
PERL_CALLCONV I32*  (*dl_Perl_Imarkstack_max_ptr)(pTHXo);
PERL_CALLCONV I32*  (*dl_Perl_Imarkstack_ptr_ptr)(pTHXo);
PERL_CALLCONV I32   (*dl_Perl_Itmps_floor_ptr)(pTHXo);
PERL_CALLCONV I32   (*dl_Perl_Itmps_ix_ptr)(pTHXo);
PERL_CALLCONV I32   (*dl_Perl_Iscopestack_ix_ptr)(pTHXo);
#endif

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
VK_MAKE_DFL(UV, dl_Perl_sv_2uv_flags, (pTHX_ SV* sv, I32 flags));
PERL_CALLCONV UV hl_Perl_sv_2uv(pTHX_ SV* sv) {
  if (dl_Perl_sv_2uv) return dl_Perl_sv_2uv(aTHX_ sv);
  else return dl_Perl_sv_2uv_flags(aTHX_ sv, SV_GMAGIC);
}
PERL_CALLCONV UV hl_Perl_sv_2uv_flags(pTHX_ SV* sv, I32 flags) {
  if (dl_Perl_sv_2uv_flags) return dl_Perl_sv_2uv_flags(aTHX_ sv, flags);
  else return dl_Perl_sv_2uv(aTHX_ sv);
}
VK_MAKE_DFL(IV, dl_Perl_sv_2iv, (pTHX_ SV* sv));
VK_MAKE_DFL(IV, dl_Perl_sv_2iv_flags, (pTHX_ SV* sv, I32 flags));
PERL_CALLCONV IV hl_Perl_sv_2iv(pTHX_ SV* sv) {
  if (dl_Perl_sv_2iv) return dl_Perl_sv_2iv(aTHX_ sv);
  else return dl_Perl_sv_2iv_flags(aTHX_ sv, SV_GMAGIC);
}
PERL_CALLCONV IV hl_Perl_sv_2iv_flags(pTHX_ SV* sv, I32 flags) {
  if (dl_Perl_sv_2iv_flags) return dl_Perl_sv_2iv_flags(aTHX_ sv, flags);
  else return dl_Perl_sv_2iv(aTHX_ sv);
}

VK_MAKE_DFL(SV*, dl_Perl_sv_2mortal, (pTHX_ SV* sv));
VK_MAKE_DFL(SV*, dl_Perl_newSViv, (pTHX_ IV i));

VK_MAKE_DFL(SV**, dl_Perl_av_store, (pTHX_ AV* ar, I32 key, SV* val));
VK_MAKE_DFL(SV**, dl_Perl_av_fetch, (pTHX_ AV* ar, I32 key, I32 lval));
VK_MAKE_DFL(void, dl_Perl_av_push, (pTHX_ AV* ar, SV* val));
VK_MAKE_DFL(void, dl_Perl_av_clear, (pTHX_ AV* ar));

#if PERL_REVISION<5 || (PERL_REVISION==5 && PERL_VERSION<10)
VK_MAKE_DFL(CV*, dl_Perl_newXS, (pTHX_ char* name, XSUBADDR_t f, char* filename));
VK_MAKE_DFL(I32, dl_Perl_av_len, (pTHX_ AV* ar));
#else
VK_MAKE_DFL(CV*, dl_Perl_newXS, (pTHX_ const char* name, XSUBADDR_t f, const char* filename));
VK_MAKE_DFL(I32, dl_Perl_av_len, (pTHX_ const AV* ar));
#endif
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

VK_MAKE_DFL(void, dl_Perl_av_undef, (pTHX_ AV* ar));
VK_MAKE_DFL(SV*, dl_Perl_eval_pv, (pTHX_ const char* p, I32 croak_on_error));
VK_MAKE_DFL(SV*, dl_Perl_newRV, (pTHX_ SV* pref));
VK_MAKE_DFL(void, dl_Perl_sv_free, (pTHX_ SV* sv));
VK_MAKE_DFL(void, dl_Perl_sv_free2, (pTHX_ SV* sv));
VK_MAKE_DFL(void*, dl_Perl_hv_common_key_len, (pTHX_ HV* hv, const char *key, I32 klen_i32, const int action, SV *val, const U32 hash));
VK_MAKE_DFL(void, dl_Perl_sys_init3, (int *argc, char ***argv, char ***env));

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
SV*  *dl_PL_diehook;
SV*  *dl_PL_warnhook;
OS2_Perl_data_t *dl_OS2_Perl_data;
#else
VK_MAKE_DFL(CV*, dl_Perl_get_cv, (pTHX_ _Const char* name, I32 create));
VK_MAKE_DFL(SV*, dl_Perl_get_sv, (pTHX_ _Const char* name, I32 create));
VK_MAKE_DFL(AV*, dl_Perl_get_av, (pTHX_ _Const char* name, I32 create));
VK_MAKE_DFL(HV*, dl_Perl_get_hv, (pTHX_ _Const char* name, I32 create));
VK_MAKE_DFL(I32, dl_Perl_call_pv, (pTHX_ _Const char* sub_name, I32 flags));
VK_MAKE_DFL(GV**, dl_Perl_Ierrgv_ptr, (pTHXo));
VK_MAKE_DFL(SV*, dl_Perl_Isv_undef_ptr, (pTHXo));
VK_MAKE_DFL(SV**, dl_Perl_Idiehook_ptr, (pTHXo));
VK_MAKE_DFL(SV**, dl_Perl_Iwarnhook_ptr, (pTHXo));
#if PERL_REVISION<5 || (PERL_REVISION==5 && PERL_VERSION<10)
VK_MAKE_DFL(SV***, dl_Perl_Tstack_sp_ptr, (pTHXo));
VK_MAKE_DFL(I32**, dl_Perl_Tmarkstack_ptr_ptr, (pTHXo));
VK_MAKE_DFL(SV***, dl_Perl_Tstack_base_ptr, (pTHXo));
VK_MAKE_DFL(I32**, dl_Perl_Tmarkstack_max_ptr, (pTHXo));
VK_MAKE_DFL(I32*, dl_Perl_Ttmps_ix_ptr, (pTHXo));
VK_MAKE_DFL(I32*, dl_Perl_Ttmps_floor_ptr, (pTHXo));
VK_MAKE_DFL(I32**, dl_Perl_Tmarkstack_ptr, (pTHXo));
VK_MAKE_DFL(XPV**, dl_Perl_TXpv_ptr, (pTHXo));
VK_MAKE_DFL(int*, dl_Perl_Iperl_destruct_level_ptr, (pTHXo));
#else
VK_MAKE_DFL(SV***, dl_Perl_Istack_base_ptr, (pTHXo));
VK_MAKE_DFL(SV***, dl_Perl_Istack_sp_ptr, (pTHXo));
VK_MAKE_DFL(I32**, dl_Perl_Imarkstack_max_ptr, (pTHXo));
VK_MAKE_DFL(I32**, dl_Perl_Imarkstack_ptr_ptr, (pTHXo));
VK_MAKE_DFL(I32*, dl_Perl_Itmps_floor_ptr, (pTHXo));
VK_MAKE_DFL(I32*, dl_Perl_Itmps_ix_ptr, (pTHXo));
VK_MAKE_DFL(I32*, dl_Perl_Iscopestack_ix_ptr, (pTHXo));
VK_MAKE_DFL(signed char*, dl_Perl_Iperl_destruct_level_ptr, (pTHXo));
#endif

VK_MAKE_DFL(void, dl_boot_DynaLoader, (pTHXo_ CV* cv));

VK_MAKE_DFL(void*, dl_Perl_get_context, (void));
VK_MAKE_DFL(void, dl_Perl_set_context, (void *thx));
VK_MAKE_DFL(PerlInterpreter*, dl_perl_clone, (PerlInterpreter* interp, UV flags));
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
  VK_MAKE_DLFUNC(Perl_av_undef),
  VK_MAKE_DLFUNC(Perl_eval_pv),
  VK_MAKE_DLFUNC(Perl_newRV),
  VK_MAKE_DLFUNC(Perl_sv_free),
  VK_MAKE_DLFUNC(Perl_sys_init3),
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
  VK_MAKE_DLFUNC(PL_diehook),
  VK_MAKE_DLFUNC(PL_warnhook),
  VK_MAKE_DLFUNC(OS2_Perl_data),
  VK_MAKE_DLFUNC(Perl_sv_setpvf),
#else
  VK_MAKE_DLFUNC(Perl_hv_common_key_len),
  VK_MAKE_DLFUNC(Perl_get_cv),
  VK_MAKE_DLFUNC(Perl_get_sv),
  VK_MAKE_DLFUNC(Perl_get_av),
  VK_MAKE_DLFUNC(Perl_get_hv),
  VK_MAKE_DLFUNC(Perl_call_pv),
  VK_MAKE_DLFUNC(Perl_Ierrgv_ptr),
#if PERL_REVISION<5 || (PERL_REVISION==5 && PERL_VERSION<10)
  VK_MAKE_DLFUNC(Perl_Tstack_sp_ptr),
  VK_MAKE_DLFUNC(Perl_Tmarkstack_ptr_ptr),
  VK_MAKE_DLFUNC(Perl_Tmarkstack_ptr),
  VK_MAKE_DLFUNC(Perl_Tstack_base_ptr),
  VK_MAKE_DLFUNC(Perl_Tmarkstack_max_ptr),
  VK_MAKE_DLFUNC(Perl_Ttmps_ix_ptr),
  VK_MAKE_DLFUNC(Perl_Ttmps_floor_ptr),
  VK_MAKE_DLFUNC(Perl_Tscopestack_ix_ptr),
  VK_MAKE_DLFUNC(Perl_TXpv_ptr),
#else
  VK_MAKE_DLFUNC(Perl_Istack_base_ptr),
  VK_MAKE_DLFUNC(Perl_Istack_sp_ptr),
  VK_MAKE_DLFUNC(Perl_Imarkstack_max_ptr),
  VK_MAKE_DLFUNC(Perl_Imarkstack_ptr_ptr),
  VK_MAKE_DLFUNC(Perl_Itmps_floor_ptr),
  VK_MAKE_DLFUNC(Perl_Itmps_ix_ptr),
  VK_MAKE_DLFUNC(Perl_Iscopestack_ix_ptr),
  VK_MAKE_DLFUNC(Perl_sv_free2),
#endif
  VK_MAKE_DLFUNC(Perl_Isv_undef_ptr),
  VK_MAKE_DLFUNC(Perl_sv_setpvf_nocontext),
  VK_MAKE_DLFUNC(Perl_get_context),
  VK_MAKE_DLFUNC(Perl_set_context),
  VK_MAKE_DLFUNC(perl_clone),
  VK_MAKE_DLFUNC(Perl_Iperl_destruct_level_ptr),
  VK_MAKE_DLFUNC(Perl_Idiehook_ptr),
  VK_MAKE_DLFUNC(Perl_Iwarnhook_ptr),
  VK_MAKE_DLFUNC(boot_DynaLoader),
#endif
  { NULL, NULL }
};
#endif                                                      /* PERLDL */

#if defined(HAVE_FORK) && !defined(HAVE_THREADS) && !defined(Perl_get_context)
#  define Perl_get_context() cfg->perl
#endif
#if !defined(PERL_SET_CONTEXT)
#  if defined(HAVE_THREADS)
#    error Your perl is not thread-safe!
#  endif
#  define PERL_SET_CONTEXT(p)
#endif
/* =========================== vars ================================== */

#define sv_config_format "__config_%u" /* SV that keeps pointer to config */
#define sv_state_format  "__state_%u"  /* SV that keeps pointer to state */
#define def_state()  sprintf(sv_state,  sv_state_format,  (unsigned)PID())
#define def_config() sprintf(sv_config, sv_config_format, (unsigned)PID())
#if defined(HAVE_THREADS) && defined(PERL_MULTITHREAD)
MUTEXSEM perlsem;
#endif

#define VK_FIND_CONFIG(cfg)                                        \
            { char sv_config[20];                                  \
              SV *sv;                                              \
              STRLEN n_a;                                          \
              def_config();                                        \
              sv = perl_get_sv(sv_config, FALSE);                  \
              if (sv) {                                            \
                cfg = *((BINKD_CONFIG**)SvPV(sv, n_a));            \
                if (!cfg || n_a == 0) cfg = current_config;        \
              } else                                               \
                cfg = current_config;                              \
            }

#define SET_CONFIG(cfg)                                            \
  { SV *svret;                                                     \
    char sv_config[20];                                            \
    def_config();                                                  \
    if ( ( svret = perl_get_sv(sv_config, TRUE)) != NULL ) {       \
      sv_setpvn(svret, (char *)&cfg, sizeof(&cfg));                \
        SvREADONLY_on(svret);                                      \
    }                                                              \
  }

#define CLEAR_CONFIG                                               \
  { SV *svret;                                                     \
    char sv_config[20];                                            \
    def_config();                                                  \
    if ( ( svret = perl_get_sv(sv_config, FALSE)) != NULL ) {      \
      SvREFCNT_dec(svret);                                         \
    }                                                              \
  }

#define FIND_STATE(state)                                          \
    {   SV *sv;                                                    \
        char sv_state[20];                                         \
        def_state();                                               \
        sv = perl_get_sv(sv_state, FALSE);                         \
        if (!sv) {                                                 \
          Log(LL_ERR, "can't find $%s pointer", sv_state);         \
          state = NULL;                                            \
        } else {                                                   \
          state = *((STATE**)SvPV(sv, n_a));                       \
          if (!(state) || !n_a) {                                  \
            Log(LL_ERR, "$%s pointer is NULL", sv_state);          \
            state = NULL;                                          \
          }                                                        \
        }                                                          \
    }

#define SET_STATE(state)                                           \
  { SV *svret;                                                     \
    char sv_state[20];                                             \
    def_state();                                                   \
    if ( ( svret = perl_get_sv(sv_state, TRUE)) != NULL ) {        \
      sv_setpvn(svret, (char *)&state, sizeof(&state));            \
      SvREADONLY_on(svret);                                        \
    }                                                              \
  }

#define CLEAR_STATE                                                \
  { SV *svret;                                                     \
    char sv_state[20];                                             \
    def_state();                                                   \
    if ( ( svret = perl_get_sv(sv_state, FALSE)) != NULL ) {       \
      SvREFCNT_dec(svret);                                         \
    }                                                              \
  }

/* bits for subroutines, must correspond to perl_subnames */
typedef enum { 
  PERL_ON_START, PERL_ON_EXIT, PERL_ON_CALL, PERL_ON_ERROR,
  PERL_ON_HANDSHAKE, PERL_AFTER_HANDSHAKE, PERL_AFTER_SESSION, 
  PERL_BEFORE_RECV, PERL_AFTER_RECV, PERL_BEFORE_SEND, PERL_AFTER_SENT,
  PERL_ON_LOG, PERL_ON_SEND, PERL_ON_RECV, PERL_SETUP_RLIMIT,
  PERL_NEED_RELOAD, PERL_CONFIG_LOADED
} perl_subs;
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
  "on_recv",
  "setup_rlimit",
  "need_reload",
  "config_loaded"
};
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
#define VK_ADD_strs(_sv, _name, _v)                           \
  if ( (_sv = perl_get_sv(_name, TRUE)) != NULL ) {           \
    sv_setpv(_sv, (char *)_v);                                \
    SvREADONLY_on(_sv);                                       \
  }
#define VK_ADD_strn(_sv, _name, _v, len)                      \
  if ( (_sv = perl_get_sv(_name, TRUE)) != NULL ) {           \
    sv_setpvn(_sv, (char *)_v, len);                          \
    SvREADONLY_on(_sv);                                       \
  }

#define VK_ADD_HASH_sv(_hv,_sv,_name)                         \
    if (_sv != NULL) {                                        \
      SvREADONLY_on(_sv);                                     \
      (void) hv_store(_hv, _name, strlen(_name), _sv, 0);     \
    }
#define VK_ADD_HASH_str(_hv,_sv,_name,_value)                            \
    if ( ((_value) != NULL) && (_sv = newSVpv(_value, 0)) != NULL ) {    \
      SvREADONLY_on(_sv);                                                \
      (void) hv_store(_hv, _name, strlen(_name), _sv, 0);                \
    }                                                                    \
    else (void) hv_store(_hv, _name, strlen(_name), &sv_undef, 0);
#define VK_ADD_HASH_intz(_hv,_sv,_name,_value)                           \
    if ( (_sv = newSViv(_value)) != NULL ) {                             \
      SvREADONLY_on(_sv);                                                \
      (void) hv_store(_hv, _name, strlen(_name), _sv, 0);                \
    }
#define VK_ADD_HASH_int(_hv,_sv,_name,_value)                            \
    if (_value) { VK_ADD_HASH_intz(_hv,_sv,_name,_value) }

/* =========================== err handling ========================== */

static void perl_warn_str (char* str) {
  while (str && *str) {
    char* cp = strchr (str, '\n');
    char  c  = 0;
    if (cp) { c = *cp; *cp = 0; }
    Log(LL_ERR, "Perl error: %s", str);
    if (cp) *cp = c;
    else break;
    str = cp + 1;
  }
}

static void perl_warn_sv (SV* sv) {
  STRLEN n_a;
  char * str = (char *) SvPV (sv, n_a);
  perl_warn_str (str);
}

#ifdef _MSC_VER
EXTERN_C void perl_warn(pTHXo_ CV* cv)
#else
static XS(perl_warn)
#endif
{
  dXSARGS;
  if (items == 1) perl_warn_sv (ST(0));
  XSRETURN_EMPTY;
}

/* handle multi-line perl eval error message */
static void sub_err(int sub) {
  STRLEN len;
  char *s, *p;
  p = SvPV(ERRSV, len);
  if (len) {
    s = xalloc(len+1);
    strnzcpy(s, p, len+1);
  }
  else
    s = xstrdup("(empty error message)");
  if ( strchr(s, '\n') == NULL )
    Log(LL_ERR, "Perl %s error: %s", perl_subnames[sub], s);
  else {
    p = s;
    Log(LL_ERR, "Perl %s error below:", perl_subnames[sub]);
    while ( *p && (*p != '\n' || *(p+1)) ) {
      char *r = strchr(p, '\n');
      if (r) {
        *r = 0;
        Log(LL_ERR, "  %s", p);
        p = r+1;
      }
      else {
        Log(LL_ERR, "  %s", p);
        break;
      }
    }
  }
  xfree(s);
}

/* =========================== xs ========================== */
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

/* interface to Log() */
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
  STATE *state;
  t_msg m;
  char *str;
  STRLEN n_a;

  if (items != 2) {
    Log(LL_ERR, "wrong params number to msg_send (needs 2, exist %d)", items);
    XSRETURN_EMPTY;
  }
  FIND_STATE(state);
  if (!state) {
    XSRETURN_EMPTY;
  }
  m = (t_msg)SvIV(ST(0));
  str = (char *)SvPV(ST(1), n_a); if (n_a == 0) str = "";
  msg_send2(state, m, str, NULL);
  XSRETURN_EMPTY;
}

#if defined(OS2) && !defined(PERLDL)
XS(boot_DB_File);
XS(boot_Fcntl);
XS(boot_POSIX);
XS(boot_SDBM_File);
XS(boot_IO);
XS(boot_OS2__Process);
XS(boot_OS2__ExtAttr);
XS(boot_OS2__REXX);
#endif

/* xs_init */
#ifdef WIN32
EXTERN_C void xs_init (pTHXo)
#else
#ifdef pTHX
static void xs_init(pTHX)
#else
static void xs_init(void)
#endif
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
  newXS("binkd_warn", perl_warn, file);
}

/* =========================== sys ========================== */

static int add_node_to_hv(FTN_NODE *node, void *hv)
{
  HV *hv2;
  SV *sv;
  char  buf[FTN_ADDR_SZ];

  hv2 = newHV();
  VK_ADD_HASH_str(hv2, sv, "hosts", node->hosts);
  VK_ADD_HASH_str(hv2, sv, "pwd", node->pwd);
  VK_ADD_HASH_str(hv2, sv, "pkt_pwd", node->pkt_pwd);
  VK_ADD_HASH_str(hv2, sv, "out_pwd", node->out_pwd);
  VK_ADD_HASH_str(hv2, sv, "ibox", node->ibox);
  VK_ADD_HASH_str(hv2, sv, "obox", node->obox);
  buf[0] = node->obox_flvr; buf[1] = 0;
  VK_ADD_HASH_str(hv2, sv, "obox_flvr", buf);
  VK_ADD_HASH_int(hv2, sv, "NR", node->NR_flag);
  VK_ADD_HASH_int(hv2, sv, "ND", node->ND_flag);
  VK_ADD_HASH_int(hv2, sv, "MD", node->MD_flag);
  VK_ADD_HASH_int(hv2, sv, "HC", node->HC_flag);
  VK_ADD_HASH_int(hv2, sv, "IP", node->restrictIP);
  VK_ADD_HASH_int(hv2, sv, "NP", node->NP_flag);
  sv = newRV_noinc( (SV*)hv2 );
  ftnaddress_to_str(buf, &(node->fa));
  VK_ADD_HASH_sv((HV *)hv, sv, buf);
  return 0;
}

/* set config vars to root perl */
static void perl_setup(BINKD_CONFIG *cfg, PerlInterpreter *perl) {
  SV 	*sv;
  HV 	*hv, *hv2;
  AV 	*av;
  char  buf[FTN_ADDR_SZ];
  int   i;

  if (!perl) return;
  Log(LL_DBG, "perl_setup(): perl context %p", perl);

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
  VK_ADD_HASH_str(hv, sv, "iport", cfg->iport);
  VK_ADD_HASH_str(hv, sv, "oport", cfg->oport);
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
  VK_ADD_HASH_intz(hv, sv, "zminsize", cfg->zminsize);
#endif
  /* perl_vars */
  {
    struct perl_var *cur = cfg->perl_vars.first;
	while (cur) {
      VK_ADD_HASH_str(hv, sv, cur->name, cur->val);
	  cur = cur->next;
	}
  }
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
        if (cur->idomain) {
          VK_ADD_HASH_str(hv2, sv, "root_domain", cur->idomain);
        }
      } else {
        VK_ADD_HASH_str(hv2, sv, "path", cur->alias4->path);
        VK_ADD_HASH_str(hv2, sv, "dir", cur->alias4->dir);
        VK_ADD_HASH_int(hv2, sv, "defzone", cur->alias4->z[0]);
        if (cur->idomain) {
          VK_ADD_HASH_str(hv2, sv, "root_domain", cur->idomain);
        }
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
  /* listen */
  av = perl_get_av("listen", TRUE);
  av_clear(av);
  {
    struct listenchain *cur = cfg->listen.first;
    while (cur) {
      hv2 = newHV();
      VK_ADD_HASH_str(hv2, sv, "port", cur->port);
      VK_ADD_HASH_str(hv2, sv, "addr", cur->addr[0] ? cur->addr : "*");
      sv = newRV_noinc((SV*)hv2);
      SvREADONLY_on(sv);
      av_push(av, sv);
      cur = cur->next;
    }
  }
  Log(LL_DBG2, "perl_setup(): @listen done");
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

/* init root perl, parse hooks file, return success */
int perl_init(char *perlfile, BINKD_CONFIG *cfg) {
  int rc, i;
  SV *sv;
  char *cmd;
  char *perlargs[] = {"", NULL, NULL, NULL};
  char **perlargv = (char **)perlargs;
  char **perlenv = saved_envp;
  PerlInterpreter *perl;

  Log(LL_DBG, "perl_init(): %s", perlfile);
  /* try to find out the actual path to perl script and set dir to -I */
  i = 1;
  perlargs[i++] = "-e";
  perlargs[i++] = "0";
  /* check perm */
  if (access(perlfile, R_OK)) {
    Log(LL_ERR, "Cannot open %s: %s", perlfile, strerror(errno));
    return 0;
  }
#ifdef PERLDL
  /* load DLL */
  if (!cfg->perl_dll[0]) {
    Log(LL_ERR, "You should define `perl-dll' in config to use Perl hooks");
    return 0;
  } else if (*(perl_dlfuncs->f) == NULL) { /* not already loaded */
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
        Log(LL_ERR, "perl_init(): can't load method Perl_sv_2pv or Perl_sv_2pv_flags");
        return 0;
      }
      if (!DosQueryProcAddr(hl, 0, "Perl_sv_2uv", (PFN*)dl_Perl_sv_2uv) &&
          !DosQueryProcAddr(hl, 0, "Perl_sv_2uv_flags", (PFN*)dl_Perl_sv_2uv_flags)) {
        Log(LL_ERR, "perl_init(): can't load method Perl_sv_2uv or Perl_sv_2uv_flags");
        return 0;
      }
      if (!DosQueryProcAddr(hl, 0, "Perl_sv_2iv", (PFN*)dl_Perl_sv_2iv) &&
          !DosQueryProcAddr(hl, 0, "Perl_sv_2iv_flags", (PFN*)dl_Perl_sv_2iv_flags)) {
        Log(LL_ERR, "perl_init(): can't load method Perl_sv_2iv or Perl_sv_2iv_flags");
        return 0;
      }
      if (!DosQueryProcAddr(hl, 0, "Perl_sv_setsv", (PFN*)dl_Perl_sv_setsv) &&
          !DosQueryProcAddr(hl, 0, "Perl_sv_setsv_flags", (PFN*)dl_Perl_sv_setsv_flags)) {
        Log(LL_ERR, "perl_init(): can't load method Perl_sv_setsv or Perl_sv_setsv_flags");
        return 0;
      }
    }
#else
    {
      *(void**)&dl_Perl_sv_2pv = GetProcAddress(hl, "Perl_sv_2pv");
      *(void**)&dl_Perl_sv_2pv_flags = GetProcAddress(hl, "Perl_sv_2pv_flags");
      if (!dl_Perl_sv_2pv && !dl_Perl_sv_2pv_flags) {
        Log(LL_ERR, "perl_init(): can't load method Perl_sv_2pv or Perl_sv_2pv_flags");
        return 0;
      }
      *(void**)&dl_Perl_sv_2uv = GetProcAddress(hl, "Perl_sv_2uv");
      *(void**)&dl_Perl_sv_2uv_flags = GetProcAddress(hl, "Perl_sv_2uv_flags");
      if (!dl_Perl_sv_2uv && !dl_Perl_sv_2uv_flags) {
        Log(LL_ERR, "perl_init(): can't load method Perl_sv_2uv or Perl_sv_2uv_flags");
        return 0;
      }
      *(void**)&dl_Perl_sv_2iv = GetProcAddress(hl, "Perl_sv_2iv");
      *(void**)&dl_Perl_sv_2iv_flags = GetProcAddress(hl, "Perl_sv_2iv_flags");
      if (!dl_Perl_sv_2iv && !dl_Perl_sv_2iv_flags) {
        Log(LL_ERR, "perl_init(): can't load method Perl_sv_2iv or Perl_sv_2iv_flags");
        return 0;
      }
      *(void**)&dl_Perl_sv_setsv = GetProcAddress(hl, "Perl_sv_setsv");
      *(void**)&dl_Perl_sv_setsv_flags = GetProcAddress(hl, "Perl_sv_setsv_flags");
      if (!dl_Perl_sv_setsv && !dl_Perl_sv_setsv_flags) {
        Log(LL_ERR, "perl_init(): can't load method Perl_sv_setv or Perl_sv_setsv_flags");
      }
    }
#endif
  } 
#endif                                                      /* PERLDL */
  /* init perl */
#ifdef PERL_SYS_INIT3
  PERL_SYS_INIT3(&i, &perlargv, &perlenv);
#endif
  perl = perl_alloc();
  PERL_SET_CONTEXT(perl);
  perl_construct(perl);
  rc = perl_parse(perl, xs_init, i, perlargv, (char **)NULL);
  Log(LL_DBG, "perl_init(): parse rc=%d", rc);
  /* can't parse */
  if (rc) {
    perl_destruct(perl);
    perl_free(perl);
    Log(LL_ERR, "Can't parse %s, perl filtering disabled", perlfile);
    return 0;
  }
  /* setup consts */
  for (i = 0; i < sizeof(perl_consts)/sizeof(perl_consts[0]); i++) {
    VK_ADD_intz(sv, perl_consts[i].name, perl_consts[i].value);
  }
  /* setup vars */
  perl_setup(cfg, perl);
  /* Set warn and die hooks */
  if (PL_warnhook) SvREFCNT_dec (PL_warnhook);
  if (PL_diehook ) SvREFCNT_dec (PL_diehook );
  PL_warnhook = newRV_inc ((SV*) perl_get_cv ("binkd_warn", TRUE));
  PL_diehook  = newRV_inc ((SV*) perl_get_cv ("binkd_warn", TRUE));

  /* run main program body */
  Log(LL_DBG, "perl_init(): running body");
  cmd = xstrdup ("do '");
  xstrcat (&cmd, perlfile);
  xstrcat (&cmd, "'; $@ ? $@ : '';");
  sv = perl_eval_pv (cmd, TRUE);
  if (!SvPOK(sv)) {
    Log(LL_ERR, "Syntax error in internal perl expression: %s", cmd);
    rc = 1;
  } else if (SvTRUE (sv)) {
    perl_warn_sv (sv);
    rc = 1;
  }
  xfree(cmd);
  if (rc) {
    perl_destruct(perl);
    perl_free(perl);
    return 0;
  }
  /* scan for present hooks */
  for (i = 0; i < sizeof(perl_subnames)/sizeof(perl_subnames[0]); i++) {
    if (perl_get_cv(perl_subnames[i], FALSE)) cfg->perl_ok |= (1 << i);
  }
#if defined(HAVE_THREAD) && defined(PERL_MULTITHREAD)
  InitSem (&perlsem);
#endif
  cfg->perl = perl;
  Log(LL_DBG, "perl_init(): end");
  return 1;
}

/* exit, just before destruction */
static void perl_on_exit(BINKD_CONFIG *cfg) {
  if (cfg->perl_ok & (1 << PERL_ON_EXIT)) {
     Log(LL_DBG, "perl_on_exit(), perl");
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
     if (SvTRUE(ERRSV)) sub_err(PERL_ON_EXIT);
     Log(LL_DBG, "perl_on_exit() end");
  }
}

/* deallocate root perl, call on_exit() if master==1 */
void perl_done(BINKD_CONFIG *cfg, int master) {
  Log(LL_DBG, "perl_done(): perl=%p", cfg->perl);
  if (cfg->perl) {
    PERL_SET_CONTEXT((PerlInterpreter *)cfg->perl);
    /* run on_exit() */
    if (master) perl_on_exit(cfg);
    /* de-allocate */
    Log(LL_DBG, "perl_done(): destructing perl %p", cfg->perl);
#ifndef _MSC_VER
    perl_destruct((PerlInterpreter *)cfg->perl);
    perl_free((PerlInterpreter *)cfg->perl);
#endif
    cfg->perl = NULL;
    Log(LL_DBG, "perl_done(): end");
  }
  if (current_config && current_config->perl)
    PERL_SET_CONTEXT((PerlInterpreter *)current_config->perl);
}

#ifdef HAVE_THREADS
/* clone root perl */
void *perl_init_clone(BINKD_CONFIG *cfg) {
#ifndef PERL_MULTITHREAD
  UV cflags = 0;
#endif
  PerlInterpreter *p;

  if (cfg->perl) {
    Log(LL_DBG2, "perl_init_clone(), parent perl=%p, context=%p", cfg->perl, Perl_get_context());
    PERL_SET_CONTEXT((PerlInterpreter *)cfg->perl);
#ifndef PERL_MULTITHREAD
#if defined(WIN32) && defined(CLONEf_CLONE_HOST)
    cflags |= CLONEf_CLONE_HOST | CLONEf_KEEP_PTR_TABLE;
#endif
    p = perl_clone((PerlInterpreter *)cfg->perl, cflags);
    if (p) { 
      PERL_SET_CONTEXT(p);
      /* perl<5.6.1 hack, see http://svn.apache.org/viewvc/perl/modperl/trunk/src/modules/perl/modperl_interp.c?r1=67986&r2=67992 */
      if (PL_scopestack_ix == 0) { ENTER; } 
      perl_setup(cfg, p);
    }
#else
    p = (PerlInterpreter *)cfg->perl;
#endif
    /* this pointer is used later */
    if (p) {
      SET_CONFIG(cfg);
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
  CLEAR_CONFIG;
  PL_perl_destruct_level = 2;
  PERL_SET_CONTEXT((PerlInterpreter *)p); /* as in mod_perl */
#ifndef PERL_MULTITHREAD
  perl_destruct((PerlInterpreter *)p);
/* #ifndef WIN32 - mod_perl has it unless CLONEf_CLONE_HOST */
#if !defined(WIN32) || defined(CLONEf_CLONE_HOST)
  perl_free((PerlInterpreter *)p);
#endif
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
  SV 	*sv;
  HV 	*hv;
  BINKD_CONFIG *cfg = state->config;

  if (!Perl_get_context()) return;
  Log(LL_DBG2, "perl_setup_session(), perl context %p", Perl_get_context());

  /* lvl 1 */
  if (lvl >= 1 && state->perl_set_lvl < 1) {
    VK_ADD_intz(sv, "call", state->to != NULL);
    VK_ADD_intz(sv, "start", (int)state->start_time);
    VK_ADD_str (sv, "host", state->peer_name);
    VK_ADD_str (sv, "ip", state->ipaddr);
    VK_ADD_str (sv, "our_ip", state->our_ip);
    VK_ADD_intz(sv, "our_port", state->our_port);
    state->perl_set_lvl = 1;
  }
  /* lvl 2 */
  if (lvl >= 2 && state->perl_set_lvl < 2) {
    int secure;
    uintmax_t netsize, filessize;
    if (state->state_ext != P_NA)
      secure = state->state_ext;
    else
      secure = state->state;
    VK_ADD_intz(sv, "secure", secure);
    VK_ADD_str (sv, "sysname", state->sysname);
    VK_ADD_str (sv, "sysop", state->sysop);
    VK_ADD_str (sv, "location", state->location);
    if (perl_get_sv("traf_mail", FALSE) == NULL) {
      q_get_sizes (state->q, &netsize, &filessize);
      VK_ADD_intz(sv, "traf_mail", (IV)netsize);
      VK_ADD_intz(sv, "traf_file", (IV)filessize);
    }
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
#ifdef BW_LIM
    if (state->bw_send.rlim)
      VK_ADD_intz(sv, "bw_send", state->bw_send.rlim);
    if (state->bw_recv.rlim)
      VK_ADD_intz(sv, "bw_recv", state->bw_recv.rlim);
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
  FTNQ *q = NULL, *qp = NULL, *q0 = NULL;
  AV *av;
  HV *hv;
  SV **svp;
  STRLEN len;
  int i, n;
  char *s;
  BINKD_CONFIG *cfg;

  Log(LL_DBG2, "perl_refresh_queue()");
  cfg = state->config;
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
void perl_on_start(BINKD_CONFIG *cfg) {
  if (cfg->perl_ok & (1 << PERL_ON_START)) {
     Log(LL_DBG, "perl_on_start()");
     lockperlsem();
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
     if (SvTRUE(ERRSV)) sub_err(PERL_ON_START);
/*     {
       Log(LL_ERR, "Perl on_start() error: %s", SvPV(ERRSV, n_a));
     }*/
     releaseperlsem();
     Log(LL_DBG, "perl_on_start() end");
  }
}

/* before outgoing call */
int perl_on_call(FTN_NODE *node, BINKD_CONFIG *cfg, char **hosts
#ifdef HTTPS
		 , char **proxy, char **socks
#endif
		) {
  char   buf[FTN_ADDR_SZ];
  int    rc, rcok;
  uintmax_t netsize, filessize;
  FTNQ   *q;
  SV     *svret, *svhosts, *sv;
#ifdef HTTPS
  SV     *svproxy, *svsocks;
#endif

  if (cfg->perl_ok & (1 << PERL_ON_CALL)) {
    Log(LL_DBG, "perl_on_call(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return 1;
    lockperlsem();
    { dSP;
      q = q_scan_addrs (0, &(node->fa), 1, 1, cfg);
      q_get_sizes (q, &netsize, &filessize);
      VK_ADD_intz(sv, "traf_mail", (IV)netsize);
      VK_ADD_intz(sv, "traf_file", (IV)filessize);
      ftnaddress_to_str(buf, &(node->fa));
      VK_ADD_strs(sv, "addr", buf);
      if ((svhosts = perl_get_sv("hosts", TRUE))) sv_setpv(svhosts, *hosts);
#ifdef HTTPS
      if ((svproxy = perl_get_sv("proxy", TRUE))) sv_setpv(svproxy, *proxy);
      if ((svsocks = perl_get_sv("socks", TRUE))) sv_setpv(svsocks, *socks);
#endif
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_ON_CALL], G_EVAL|G_SCALAR);
      SPAGAIN;
      svret=POPs;
      rcok = SvOK(svret);
      if (rcok) rc = SvIV(svret);
      else rc = 1;
      PUTBACK;
      FREETMPS;
      LEAVE;
      if (SvTRUE(ERRSV)) sub_err(PERL_ON_CALL);
      else if (rc && rcok) {
          STRLEN len;
          char *s;
          sv = perl_get_sv("hosts", FALSE);
          if (sv && SvOK(sv)) {
            s = SvPV(sv, len);
            if (len == 0) s = "";
            if (strcmp(s, *hosts)) {
              xfree(*hosts);
              *hosts = xstrdup(s);
            }
          }
#ifdef HTTPS
          sv = perl_get_sv("proxy", FALSE);
          if (sv && SvOK(sv)) {
            s = SvPV(sv, len);
            if (len == 0) s = "";
            if (strcmp(s, *proxy)) {
              xfree(*proxy);
              *proxy = xstrdup(s);
            }
          }
          sv = perl_get_sv("socks", FALSE);
          if (sv && SvOK(sv)) {
            s = SvPV(sv, len);
            if (len == 0) s = "";
            if (strcmp(s, *socks)) {
              xfree(*socks);
              *socks = xstrdup(s);
            }
          }
#endif
      }
    }
    releaseperlsem();
    Log(LL_DBG, "perl_on_call() end");
    return rc;
  }
  return 1;
}

/* after unsuccessfull call */
int perl_on_error(BINKD_CONFIG *cfg, FTN_ADDR *addr, const char *error, const int where) {
  char   buf[FTN_ADDR_SZ];
  int    rc;
  SV     *svret, *sv;

  if (cfg->perl_ok & (1 << PERL_ON_ERROR)) {
    Log(LL_DBG, "perl_on_error(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return 1;
    lockperlsem();
    { dSP;
      ftnaddress_to_str(buf, addr);
      VK_ADD_strs(sv, "addr", buf);
      VK_ADD_str (sv, "error", error);
      VK_ADD_intz(sv, "where", where);
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
      if (SvTRUE(ERRSV)) sub_err(PERL_ON_ERROR);
    }
    releaseperlsem();
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
  uintmax_t netsize, filessize;
  FTNQ *q;
  STRLEN len;
  AV *he, *me;
  SV *svret, **svp, *sv, *passwd;
  BINKD_CONFIG *cfg = state->config;
  /* this pointer is used later */
  if (cfg->perl_ok && Perl_get_context()) {
    SET_STATE(state);
  }
  if (cfg->perl_ok & (1 << PERL_ON_HANDSHAKE)) {
    Log(LL_DBG, "perl_on_handshake(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return NULL;
    lockperlsem();
    { dSP;
      if ( (me = perl_get_av("me", FALSE)) != NULL ) av_undef(me);
      if ( (passwd = perl_get_sv("passwd", FALSE)) != NULL ) passwd = &sv_undef;
      he = perl_get_av("he", TRUE);
      av_clear(he);
      if (!state->to) {
        for (i = 0; i < state->nfa; i++) {
          ftnaddress_to_str(buf, &(state->fa[i]));
          av_push(he, newSVpv(buf, 0));
        }
        if (state->q)
          q = state->q;
        else if (OK_SEND_FILES (state, cfg))
          q = state->q = q_scan_addrs (0, state->fa, state->nfa, 0, cfg);
        else
          q = NULL;
      } else {
        ftnaddress_to_str(buf, &(state->to->fa));
        av_push(he, newSVpv(buf, 0));
        q = q_scan_addrs (0, &(state->to->fa), 1, 1, cfg);
      }
      if (perl_get_sv("traf_mail", FALSE) == NULL) {
        q_get_sizes (q, &netsize, &filessize);
        VK_ADD_intz(sv, "traf_mail", (IV)netsize);
        VK_ADD_intz(sv, "traf_file", (IV)filessize);
      }
      setup_session(state, 1);
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
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_ON_HANDSHAKE);
        if (prc) free(prc);
        prc = NULL;
      }
      /* make array of our aka */
      else if (!prc) {
        if ((me = perl_get_av("me", FALSE)) != NULL) {
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
        if ((passwd = perl_get_sv("passwd", FALSE)) != NULL && SvOK(passwd)) {
          strncpy(state->expected_pwd, SvPV(passwd, len), sizeof(state->expected_pwd));
          state->expected_pwd[sizeof(state->expected_pwd) - 1] = '\0';
          if (state->expected_pwd[0] == '\0') strcpy(state->expected_pwd, "-");
        }
      }
    }
    releaseperlsem();
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
  BINKD_CONFIG *cfg = state->config;

  if (cfg->perl_ok & (1 << PERL_AFTER_HANDSHAKE)) {
    Log(LL_DBG, "perl_after_handshake(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return NULL;
    lockperlsem();
    { dSP;
      if (state->perl_set_lvl < 2) setup_session(state, 2);
      setup_queue(state, state->q);
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
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_AFTER_HANDSHAKE);
        if (prc) free(prc);
        prc = NULL;
      }
      else
        state->q = refresh_queue(state, state->q);
    }
    releaseperlsem();
    Log(LL_DBG, "perl_after_handshake() end");
    return prc;
  }
  return NULL;
}

/* after session done */
void perl_after_session(STATE *state, int status) {
  SV *sv;
  BINKD_CONFIG *cfg = state->config;

  if (cfg->perl_ok && Perl_get_context()) {
    CLEAR_STATE;
  }

  if (cfg->perl_ok & (1 << PERL_AFTER_SESSION)) {
    Log(LL_DBG, "perl_after_session(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return;
    lockperlsem();
    { dSP;
      if (state->perl_set_lvl < 3) setup_session(state, 3);
      VK_ADD_intz(sv, "rc", status == 0);
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_AFTER_SESSION], G_EVAL|G_VOID);
      SPAGAIN;
      PUTBACK;
      FREETMPS;
      LEAVE;
      if (SvTRUE(ERRSV)) sub_err(PERL_AFTER_SESSION);
    }
    releaseperlsem();
    Log(LL_DBG, "perl_after_session() end");
  }
}

/* before receiving file */
int perl_before_recv(STATE *state, boff_t offs) {
  int    rc;
  SV     *svret, *sv;
  BINKD_CONFIG *cfg = state->config;

  if (cfg->perl_ok & (1 << PERL_BEFORE_RECV)) {
    Log(LL_DBG, "perl_before_recv(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return 0;
    lockperlsem();
    { dSP;
      if (state->perl_set_lvl < 2) setup_session(state, 2);
      setup_queue(state, state->q);
      VK_ADD_str (sv, "name", state->in.netname);
      VK_ADD_intz(sv, "size", state->in.size);
      VK_ADD_intz(sv, "time", state->in.time);
      VK_ADD_intz(sv, "offs", offs);
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
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_BEFORE_RECV);
        rc = 0;
      }
    }
    releaseperlsem();
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
  BINKD_CONFIG *cfg = state->config;

  if (cfg->perl_ok & (1 << PERL_AFTER_RECV)) {
    Log(LL_DBG, "perl_after_recv(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return 0;
    lockperlsem();
    { dSP;
      if (state->perl_set_lvl < 2) setup_session(state, 2);
      setup_queue(state, state->q);
      VK_ADD_str (sv, "name", file->netname);
      VK_ADD_intz(sv, "size", file->size);
      VK_ADD_intz(sv, "time", file->time);
      VK_ADD_str (sv, "tmpfile", tmp_name);
      VK_ADD_str (sv, "file", real_name); if (sv) { SvREADONLY_off(sv); }
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
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_AFTER_RECV);
        rc = 0;
      }
      /* update real_name */
      else {
        state->q = refresh_queue(state, state->q);
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
    releaseperlsem();
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
  BINKD_CONFIG *cfg = state->config;

  if (cfg->perl_ok & (1 << PERL_BEFORE_SEND)) {
    Log(LL_DBG, "perl_before_send(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return 0;
    lockperlsem();
    { dSP;
      if (state->perl_set_lvl < 2) setup_session(state, 2);
      setup_queue(state, state->q);
      VK_ADD_str (sv, "file", state->out.path);
      VK_ADD_str (sv, "name", state->out.netname); if (sv) { SvREADONLY_off(sv); }
      VK_ADD_intz(sv, "size", state->out.size);
      VK_ADD_intz(sv, "time", state->out.time);
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
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_BEFORE_SEND);
        rc = 0;
      }
      else {
        state->q = refresh_queue(state, state->q);
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
    releaseperlsem();
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
  BINKD_CONFIG *cfg = state->config;

  if (cfg->perl_ok & (1 << PERL_AFTER_SENT)) {
    Log(LL_DBG, "perl_after_sent(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return 0;
    lockperlsem();
    { dSP;
      if (state->perl_set_lvl < 2) setup_session(state, 2);
      setup_queue(state, state->q);
      VK_ADD_str (sv, "file", state->sent_fls[n].path);
      VK_ADD_str (sv, "name", state->sent_fls[n].netname);
      VK_ADD_intz(sv, "size", state->sent_fls[n].size);
      VK_ADD_intz(sv, "time", state->sent_fls[n].time);
      VK_ADD_intz(sv, "start", state->sent_fls[n].start);
      buf[0] = state->sent_fls[n].action; buf[1] = 0;
      VK_ADD_strs(sv, "action", buf); if (sv) { SvREADONLY_off(sv); }
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
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_AFTER_SENT);
        rc = 0;
      }
      else {
        if (rc) {
          sv = perl_get_sv("action", FALSE);
          if (sv && SvOK(sv)) {
            char *s = SvPV(sv, len);
            state->sent_fls[n].action = len ? *s : 0;
          }
        }
      }
    }
    releaseperlsem();
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
  BINKD_CONFIG *cfg;
  char inlog[20];

  cfg = lock_current_config(); /* Log() always working by current_config */
  if (!cfg) return 1;
  if (!cfg->perl_ok || !Perl_get_context()) {
    unlock_config_structure(cfg, 0);
    return 1;
  }
  if (cfg->perl_ok & (1 << PERL_ON_LOG)) {
    /* check: not to call while on_log() is running */
    sprintf(inlog, "__in_log_%u", (unsigned)PID());
    svchk = perl_get_sv(inlog, FALSE);
    if (svchk && SvIV(svchk) == 1) {
      unlock_config_structure(cfg, 0);
      return 1;
    }
    if (svchk)
      SvREADONLY_off(svchk);
    else
      if ((svchk = perl_get_sv(inlog, TRUE)) == NULL) {
        unlock_config_structure(cfg, 0);
        return 1; /* can't guarantee there won't be recursion */
      }
    sv_setiv(svchk, 1);
    SvREADONLY_on(svchk);
    /* end of check */
    Log(LL_DBG2, "perl_on_log(), perl=%p", Perl_get_context());
    { dSP;
      VK_ADD_intz(sv, "lvl", *lev); if (sv) { SvREADONLY_off(sv); }
      VK_ADD_str (sv, "_", s); if (sv) { SvREADONLY_off(sv); }
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
    SvREADONLY_off(svchk);
    sv_setiv(svchk, 0); /* check: now we can restore */
    /* SvREFCNT_dec(svchk); */
    unlock_config_structure(cfg, 0);
    return rc;
  }
  unlock_config_structure(cfg, 0);
  return 1;
}

int perl_on_send(STATE *state, t_msg *m, char **s1, char **s2) {
  int    rc = 1;
  SV     *sv;
  BINKD_CONFIG *cfg = state->config;

  if (cfg->perl_ok & (1 << PERL_ON_SEND)) {
    Log(LL_DBG2, "perl_on_send(), perl=%p", Perl_get_context());
    lockperlsem();
    { dSP;
      VK_ADD_intz(sv, "type", *m);
      VK_ADD_str (sv, "s1", *s1);
      VK_ADD_str (sv, "s2", *s2);
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_ON_SEND], G_EVAL|G_SCALAR);
      SPAGAIN;
      PUTBACK;
      FREETMPS;
      LEAVE;
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_ON_SEND);
      }
    }
    releaseperlsem();
    Log(LL_DBG2, "perl_on_send() end");
    return rc;
  }
  return 1;
}

int perl_on_recv(STATE *state, char *s, int size) { 
  int    rc = 1;
  SV     *sv;
  BINKD_CONFIG *cfg = state->config;

  if (cfg->perl_ok & (1 << PERL_ON_RECV)) {
    Log(LL_DBG2, "perl_on_recv(), perl=%p", Perl_get_context());
    lockperlsem();
    { dSP;
      if ( (sv = perl_get_sv("s", TRUE)) ) {
        sv_setpvn(sv, s, size); SvREADONLY_on(sv);
      }
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_ON_RECV], G_EVAL|G_SCALAR);
      SPAGAIN;
      PUTBACK;
      FREETMPS;
      LEAVE;
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_ON_RECV);
      }
    }
    releaseperlsem();
    Log(LL_DBG2, "perl_on_recv() end");
    return rc;
  }
  return 1;
}

#ifdef BW_LIM
int perl_setup_rlimit(STATE *state, BW *bw, char *fname)
{
  int    rc = 1;
  STRLEN len;
  SV     *sv;
  BINKD_CONFIG *cfg = state->config;

  if (cfg->perl_ok & (1 << PERL_SETUP_RLIMIT)) {
    Log(LL_DBG2, "perl_set_rlimit(), perl=%p", Perl_get_context());
    lockperlsem();
    { dSP;
      if (state->perl_set_lvl < 2) setup_session(state, 2);
      setup_queue(state, state->q);
      VK_ADD_str (sv, "file", fname);
      VK_ADD_intz (sv, "rlimit", bw->rlim); if (sv) { SvREADONLY_off(sv); }
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_SETUP_RLIMIT], G_EVAL|G_SCALAR);
      SPAGAIN;
      PUTBACK;
      FREETMPS;
      LEAVE;
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_SETUP_RLIMIT);
        rc = 1;
      }
      else if (rc && sv) {
        char *p = SvPV(sv, len);
        if (!p || len == 0 || *p == 0) rc = 0; 
        else {
          rc = 1;
          sv = perl_get_sv("rlimit", FALSE); if (sv) bw->rlim = SvIV(sv);
        }
      }
    }
    releaseperlsem();
    Log(LL_DBG2, "perl_setup_rlimit() end");
    return rc;
  }
  return 1;
}
#endif

/* need reload config? */
int perl_need_reload(BINKD_CONFIG *cfg, struct conflist_type *conflist, int need_reload)
{
  struct conflist_type *pc;
  int    rc;
  AV     *av;
  SV     *sv, *svret;

  if (cfg->perl_ok & (1 << PERL_NEED_RELOAD)) {
    Log(LL_DBG, "perl_need_reload(), perl=%p", Perl_get_context());
    if (!Perl_get_context()) return 0;
    lockperlsem();
    { dSP;
      av = perl_get_av("conflist", TRUE);
      av_clear(av);
      for (pc = conflist; pc; pc = pc->next) {
        sv = newSVpv(pc->path, 0);
        SvREADONLY_on(sv);
        av_push(av, sv);
      }
      Log(LL_DBG2, "perl_need_reload(): @conflist done");
      VK_ADD_intz (sv, "_", need_reload); if (sv) { SvREADONLY_off(sv); }
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_NEED_RELOAD], G_EVAL|G_SCALAR);
      SPAGAIN;
      svret=POPs;
      if (SvOK(svret)) rc = SvIV(svret); else rc = 0;
      PUTBACK;
      FREETMPS;
      LEAVE;
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_NEED_RELOAD);
        rc = 0;
      }
    }
    releaseperlsem();
    Log(LL_DBG, "perl_need_reload() end, returns %i", rc);
    return rc;
  }
  return 0;
}

/* config loaded */
void perl_config_loaded(BINKD_CONFIG *cfg)
{
  if (cfg->perl && (cfg->perl_ok & (1 << PERL_CONFIG_LOADED))) {
    Log(LL_DBG, "perl_config_loaded()");
    lockperlsem();
    { dSP;
      ENTER;
      SAVETMPS;
      PUSHMARK(SP);
      PUTBACK;
      perl_call_pv(perl_subnames[PERL_CONFIG_LOADED], G_EVAL|G_SCALAR);
      SPAGAIN;
      PUTBACK;
      FREETMPS;
      LEAVE;
      if (SvTRUE(ERRSV)) {
        sub_err(PERL_CONFIG_LOADED);
      }
    }
    releaseperlsem();
    Log(LL_DBG, "perl_config_loaded() end");
    return;
  }
  return;
}

