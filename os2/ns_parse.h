/*
 * ns_parse.h -- DNS resolver parsing functions for OS/2
 *
 * Copyright 2012 Andre Grueneberg for BinkD
 *
 * inspired by ns_parse.c and nameser.h, Copyright 1996-1999 by 
 *	Internet Software Consortium.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/*
 * $Id$
 *
 * $Log$
 * Revision 1.4  2014/08/19 06:41:36  gul
 * OS/2 compilation
 *
 * Revision 1.3  2014/08/18 16:57:55  gul
 * Fixed OS/2 compilation
 *
 * Revision 1.2  2012/01/23 21:01:59  green
 * Add FSP1035 support for EMX (using own import library)
 *
 * Revision 1.1  2012/01/22 01:27:20  green
 * Implement FSP1035 support for OS/2/Watcom
 *
 */

#ifndef __NS_PARSE_H__
#define __NS_PARSE_H__

#ifdef HAVE_ARPA_NAMESER_H
#  include <arpa/nameser.h>
#endif
#ifdef HAVE_RESOLV_H
#  include <resolv.h>
#endif

/*
 * These can be expanded with synonyms, just keep ns_parse.c:ns_parserecord()
 * in synch with it.
 */
typedef enum __ns_sect {
            ns_s_qd = 0,            /*%< Query: Question. */
            ns_s_zn = 0,            /*%< Update: Zone. */
            ns_s_an = 1,            /*%< Query: Answer. */
            ns_s_pr = 1,            /*%< Update: Prerequisites. */
            ns_s_ns = 2,            /*%< Query: Name servers. */
            ns_s_ud = 2,            /*%< Update: Update. */
            ns_s_ar = 3,            /*%< Query|Update: Additional records. */
            ns_s_max = 4
} ns_sect;

/*%
 * This is a message handle.  It is caller allocated and has no dynamic data.
 * This structure is intended to be opaque to all but ns_parse.c, thus the
 * leading _'s on the member names.  Use the accessor functions, not the _'s.
 */
typedef struct __ns_msg {
            const u_char    *_msg, *_eom;
            u_int16_t       _id, _flags, _counts[ns_s_max];
            const u_char    *_sections[ns_s_max];
            ns_sect         _sect;
            int             _rrnum;
            const u_char    *_msg_ptr;
} ns_msg;

/* Accessor macros - this is part of the public interface. */
#define ns_msg_id(handle) ((handle)._id + 0)
#define ns_msg_base(handle) ((handle)._msg + 0)
#define ns_msg_end(handle) ((handle)._eom + 0)
#define ns_msg_size(handle) ((handle)._eom - (handle)._msg)
#define ns_msg_count(handle, section) ((handle)._counts[section] + 0)

/*%
 * This is a parsed record.  It is caller allocated and has no dynamic data.
 */
typedef struct __ns_rr {
            char            name[MAXDNAME];
            u_int16_t       type;
            u_int16_t       rr_class;
            u_int32_t       ttl;
            u_int16_t       rdlength;
            const u_char *  rdata;
} ns_rr;

#define ns_rr_name(rr)	(((rr).name[0] != '\0') ? (rr).name : ".")
#define ns_rr_type(rr)	((rr).type + 0)
#define ns_rr_class(rr)	((rr).rr_class + 0)
#define ns_rr_ttl(rr)	((rr).ttl + 0)
#define ns_rr_rdlen(rr)	((rr).rdlength + 0)
#define ns_rr_rdata(rr)	((rr).rdata + 0)

/* constants usually found in newer nameser.h */
#ifdef T_SRV
#define ns_t_srv T_SRV
#else
#define ns_t_srv 33
#endif
#define ns_c_in C_IN
#define NS_MAXDNAME MAXDNAME
#define NS_INT32SZ	4	/*%< #/bytes of data in a u_int32_t */
#define NS_INT16SZ	2	/*%< #/bytes of data in a u_int16_t */

/*%
 * Inline versions of get/put short/long.  Pointer is advanced.
 */
#define NS_GET16(s, cp) do { \
	register const u_char *t_cp = (const u_char *)(cp); \
	(s) = ((u_int16_t)t_cp[0] << 8) \
	    | ((u_int16_t)t_cp[1]) \
	    ; \
	(cp) += NS_INT16SZ; \
} while (0)

#define NS_GET32(l, cp) do { \
	register const u_char *t_cp = (const u_char *)(cp); \
	(l) = ((u_int32_t)t_cp[0] << 24) \
	    | ((u_int32_t)t_cp[1] << 16) \
	    | ((u_int32_t)t_cp[2] << 8) \
	    | ((u_int32_t)t_cp[3]) \
	    ; \
	(cp) += NS_INT32SZ; \
} while (0)

int ns_initparse(const u_char *, int, ns_msg *);
int ns_parserr(ns_msg *, ns_sect, int, ns_rr *);

#endif /* __NS_PARSE_H__ */
