/* ######################################################################

   RFC 2553 Emulation - Provides emulation for RFC 2553 getaddrinfo,
                        freeaddrinfo and getnameinfo
   
   These functions are necessary to write portable protocol independent
   networking. They transparently support IPv4, IPv6 and probably many 
   other protocols too. This implementation is needed when the host does 
   not support these standards. It implements a simple wrapper that 
   basically supports only IPv4. 

   Perfect emulation is not provided, but it is passable..
   
   Originally written by Jason Gunthorpe <jgg@debian.org> and placed into
   the Public Domain, do with it what you will.
  
   ##################################################################### */

/*
 * $Id$
 *
 * $Log$
 * Revision 2.2.2.1  2014/08/20 06:12:37  gul
 * Fixed 100% cpu load if called with poll flag,
 * backport many fixes related to compilation on win32 and os/2.
 *
 * Revision 2.2  2012/01/07 13:52:23  green
 * Removed C++ comments (bad style, I know)
 *
 * Revision 2.1  2012/01/03 17:25:32  green
 * Implemented IPv6 support
 * - replace (almost) all getXbyY function calls with getaddrinfo/getnameinfo (RFC2553) calls
 * - Add compatibility layer for target systems not supporting RFC2553 calls in rfc2553.[ch]
 * - Add support for multiple listen sockets -- one for IPv4 and one for IPv6 (use V6ONLY)
 * - For WIN32 platform add configuration parameter IPV6 (mutually exclusive with BINKD9X)
 * - On WIN32 platform use Winsock2 API if IPV6 support is requested
 * - config: node IP address literal + port supported: [<ipv6 address>]:<port>
 *
 */

#ifndef RFC2553EMU_H
#define RFC2553EMU_H

#include "iphdr.h"

/* Autosense getaddrinfo */
#if defined(AI_PASSIVE) && defined(EAI_NONAME)
#define HAVE_GETADDRINFO
#endif

/* Autosense getnameinfo */
#if defined(NI_NUMERICHOST)
#define HAVE_GETNAMEINFO
#endif

/* getaddrinfo support? */
#ifndef HAVE_GETADDRINFO
  /* Renamed to avoid type clashing.. (for debugging) */
  struct addrinfo_emu
  {   
     int     ai_flags;     /* AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST */
     int     ai_family;    /* PF_xxx */
     int     ai_socktype;  /* SOCK_xxx */
     int     ai_protocol;  /* 0 or IPPROTO_xxx for IPv4 and IPv6 */
     size_t  ai_addrlen;   /* length of ai_addr */
     char   *ai_canonname; /* canonical name for nodename */
     struct sockaddr  *ai_addr; /* binary address */
     struct addrinfo_emu  *ai_next; /* next structure in linked list */
  };
  #define addrinfo addrinfo_emu

  int getaddrinfo(const char *nodename, const char *servname,
                  const struct addrinfo *hints,
                  struct addrinfo **res);
  void freeaddrinfo(struct addrinfo *ai);
  char * gai_strerror(int ecode);

  #ifndef AI_PASSIVE
  #define AI_PASSIVE (1<<1)
  #endif
  
  #ifndef EAI_NONAME
  #define EAI_NONAME     -1
  #define EAI_AGAIN      -2
  #define EAI_FAIL       -3
  #define EAI_NODATA     -4
  #define EAI_FAMILY     -5
  #define EAI_SOCKTYPE   -6
  #define EAI_SERVICE    -7
  #define EAI_ADDRFAMILY -8
  #define EAI_MEMORY     -9
  #define EAI_SYSTEM     -10
  #define EAI_UNKNOWN    -11
  #endif

  /* If we don't have getaddrinfo then we probably don't have 
     sockaddr_storage either (same RFC) so we definately will not be
     doing any IPv6 stuff. Do not use the members of this structure to
     retain portability, cast to a sockaddr. */
  #define sockaddr_storage sockaddr_in
#endif

/* getnameinfo support (glibc2.0 has getaddrinfo only) */
#ifndef HAVE_GETNAMEINFO

  int getnameinfo(const struct sockaddr *sa, socklen_t salen,
		  char *host, size_t hostlen,
		  char *serv, size_t servlen,
		  int flags);

  #ifndef NI_MAXHOST
  #define NI_MAXHOST 1025
  #define NI_MAXSERV 32
  #endif

  #ifndef NI_NUMERICHOST
  #define NI_NUMERICHOST (1<<0)
  #define NI_NUMERICSERV (1<<1)
/*  #define NI_NOFQDN (1<<2) */
  #define NI_NAMEREQD (1<<3)
  #define NI_DATAGRAM (1<<4)
  #endif

  #define sockaddr_storage sockaddr_in
#endif

/* Glibc 2.0.7 misses this one */
#ifndef AI_NUMERICHOST
#define AI_NUMERICHOST 0
#endif

/* Win32 doesn't support these */
#ifndef AI_NUMERICSERV
#define AI_NUMERICSERV 0
#endif

/* Win32 doesn't support these */
#ifndef NI_NUMERICSERV
#define NI_NUMERICSERV 0
#endif


#endif
