/* MD5.H - header file for MD5C.C
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */

/* ------------------------------------------------------------------ */
/* GLOBAL.H - RSAREF types and constants
 */

/* RFC 1321              MD5 Message-Digest Algorithm            April 1992 */
/* PROTOTYPES should be set to one if and only if the compiler supports
  function argument prototyping.
  The following makes PROTOTYPES default to 0 if it has not already
  been defined with C compiler flags.
 */
#ifndef PROTOTYPES
#define PROTOTYPES 0
#endif

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

#ifdef UINT16_TYPE
typedef UINT16_TYPE UINT2;
typedef UINT32_TYPE UINT4;
#elif defined(SIZEOF_INT) && SIZEOF_INT!=0
#if SIZEOF_SHORT==2
typedef unsigned short int UINT2;
#else
#error Cannot find type for 16-bit integer!
#endif
#if SIZEOF_INT==4
typedef unsigned int UINT4;
#elif SIZEOF_LONG==4
typedef unsigned long int UINT4;
#else
#error Cannot find type for 32-bit integer!
#endif
#else // default, no configure and Makefile UINT16_TYPE defines
/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;
/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;
#endif

/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
  returns an empty list.
 */
#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif
/* end of GLOBAL.H ---------------------------------------------------------- */

/* MD5 context. */
typedef struct {
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

#define MD5_DIGEST_LEN 16

/* MD5 digest */
typedef unsigned char MDcaddr_t[MD5_DIGEST_LEN];

#define MD_CHALLENGE_LEN 16
#include "prothlp.h"
#include "readcfg.h"
#include "ftnnode.h"
#include "iphdr.h"
#include "protoco2.h"

char *MD_getChallenge(char *src, STATE *st);
char *MD_buildDigest(char *pw, unsigned char *challenge);
void MD_toString(char *rs, int len, unsigned char *digest);
