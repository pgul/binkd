/* ######################################################################

   RFC 2553 Emulation - Provides emulation for RFC 2553 getaddrinfo,
                        freeaddrinfo and getnameinfo

   Originally written by Jason Gunthorpe <jgg@debian.org> and placed into
   the Public Domain, do with it what you will.

   ##################################################################### */

#include "rfc2553.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sem.h"

#ifndef HAVE_GETADDRINFO
/* getaddrinfo - Resolve a hostname
 * ---------------------------------------------------------------------
 */
int getaddrinfo(const char *nodename, const char *servname,
		const struct addrinfo *hints,
		struct addrinfo **res)
{
   struct addrinfo **Result = res;
   struct hostent *Addr;
   unsigned int Port;
   int Proto;
   const char *End;
   char **CurAddr;
   int ret = 0;
   
   /* Sanitize return parameters */
   if (res == NULL)
      return EAI_UNKNOWN;
   *res = NULL;
   
   /* Try to convert the service as a number */
   Port = servname ? htons(strtol(servname,(char **)&End,0)) : 0;
   Proto = SOCK_STREAM;

   if (hints != NULL && hints->ai_socktype != 0)
      Proto = hints->ai_socktype;
   
   lockresolvsem();

   /* Not a number, must be a name. */
   if (servname != NULL && End != servname + strlen(servname))
   {
      struct servent *Srv = NULL;
      
      /* Do a lookup in the service database */
      if (hints == 0 || hints->ai_socktype == SOCK_STREAM)
	 Srv = getservbyname(servname,"tcp");
      if (hints != 0 && hints->ai_socktype == SOCK_DGRAM)
	 Srv = getservbyname(servname,"udp");
      if (Srv == 0) 
      {
	 ret = EAI_NONAME;  
	 goto cleanup;
      }
      
      /* Get the right protocol */
      Port = Srv->s_port;
      if (strcmp(Srv->s_proto,"tcp") == 0)
	 Proto = SOCK_STREAM;
      else
      {
	 if (strcmp(Srv->s_proto,"udp") == 0)
	    Proto = SOCK_DGRAM;
         else
         {
	    ret = EAI_NONAME;
            goto cleanup;
         }
             
      }      
      
      if (hints != 0 && hints->ai_socktype != Proto && 
	  hints->ai_socktype != 0)
      {
	 ret = EAI_SERVICE;
         goto cleanup;
      }
   }
      
   /* Hostname lookup, only if this is not a listening socket */
   if (hints != 0 && (hints->ai_flags & AI_PASSIVE) != AI_PASSIVE)
   {
      Addr = gethostbyname(nodename);
      if (Addr == 0)
      {
	 if (h_errno == TRY_AGAIN)
         {
	    ret = EAI_AGAIN;
	    goto cleanup;
	 }
	 if (h_errno == NO_RECOVERY)
	 {
	    ret = EAI_FAIL;
	    goto cleanup;
	 }
	 ret = EAI_NONAME;
         goto cleanup;
      }
   
      /* No A records */
      if (Addr->h_addr_list[0] == 0)
      {
	 ret = EAI_NONAME;
	 goto cleanup;
      }
      
      CurAddr = Addr->h_addr_list;
   }
   else
      CurAddr = (char **)&End;    /* Fake! */
   
   /* Start constructing the linked list */
   for (; *CurAddr != NULL; CurAddr++)
   {
      /* New result structure */
      *Result = (struct addrinfo *)calloc(sizeof(**Result),1);
      if (*Result == NULL)
      {
	 ret = EAI_MEMORY;
	 goto cleanup;
      }
      if (*res == NULL)
	 *res = *Result;
      
      (*Result)->ai_family = AF_INET;
      (*Result)->ai_socktype = Proto;

      /* If we have the IPPROTO defines we can set the protocol field */
      #ifdef IPPROTO_TCP
      if (Proto == SOCK_STREAM)
	 (*Result)->ai_protocol = IPPROTO_TCP;
      if (Proto == SOCK_DGRAM)
	 (*Result)->ai_protocol = IPPROTO_UDP;
      #endif

      /* Allocate space for the address */
      (*Result)->ai_addrlen = sizeof(struct sockaddr_in);
      (*Result)->ai_addr = (struct sockaddr *)calloc(sizeof(struct sockaddr_in),1);
      if ((*Result)->ai_addr == 0)
      {
	 ret = EAI_MEMORY;
	 goto cleanup;
      }
      
      /* Set the address */
      ((struct sockaddr_in *)(*Result)->ai_addr)->sin_family = AF_INET;
      ((struct sockaddr_in *)(*Result)->ai_addr)->sin_port = Port;
      
      if (hints != 0 && (hints->ai_flags & AI_PASSIVE) != AI_PASSIVE)
	 ((struct sockaddr_in *)(*Result)->ai_addr)->sin_addr = *(struct in_addr *)(*CurAddr);
      else
      {
         /* Already zerod by calloc. */
	 break;
      }
      
      Result = &(*Result)->ai_next;
   }
   
cleanup:
   releaseresolvsem();
   if (ret != 0 && *res != NULL)
   {
      freeaddrinfo(*res);
      *res = NULL;
   }

   return ret;
}

/* freeaddrinfo - Free the result of getaddrinfo
 * ---------------------------------------------------------------------
 */
void freeaddrinfo(struct addrinfo *ai)
{
   struct addrinfo *Tmp;
   while (ai != 0)
   {
      free(ai->ai_addr);
      Tmp = ai;
      ai = ai->ai_next;
      free(Tmp);
   }
}

/* gai_strerror - error number to string
 * ---------------------------------------------------------------------
 */
static char *ai_errlist[] = {
    "Success",
    "hostname nor servname provided, or not known", /* EAI_NONAME     */
    "Temporary failure in name resolution", /* EAI_AGAIN     */
    "Non-recoverable failure in name resolution",   /* EAI_FAIL	 */
    "No address associated with hostname",  /* EAI_NODATA     */
    "ai_family not supported",	    /* EAI_FAMILY     */
    "ai_socktype not supported",    /* EAI_SOCKTYPE   */
    "service name not supported for ai_socktype",   /* EAI_SERVICE    */
    "Address family for hostname not supported",    /* EAI_ADDRFAMILY */
    "Memory allocation failure",    /* EAI_MEMORY     */
    "System error returned in errno",	/* EAI_SYSTEM     */
    "Unknown error",		/* EAI_UNKNOWN    */
};

char *gai_strerror(int ecode)
{
    if (ecode > EAI_NONAME || ecode < EAI_UNKNOWN)
	ecode = EAI_UNKNOWN;
    return ai_errlist[-ecode];
}

#endif /* HAVE_GETADDRINFO */

#ifndef HAVE_GETNAMEINFO
/* getnameinfo - Convert a sockaddr to a string
 * ---------------------------------------------------------------------
 */
int getnameinfo(const struct sockaddr *sa, socklen_t salen,
		char *host, size_t hostlen,
		char *serv, size_t servlen,
		int flags)
{
   struct sockaddr_in *sin = (struct sockaddr_in *)sa;
   
   /* This routine only supports internet addresses */
   if (sa->sa_family != AF_INET)
      return EAI_ADDRFAMILY;
   
   if (host != 0)
   {
      /* Try to resolve the hostname */
      if ((flags & NI_NUMERICHOST) != NI_NUMERICHOST)
      {
	 struct hostent *Ent;

	 lockresolvsem();
	 Ent = gethostbyaddr((char *)&sin->sin_addr,sizeof(sin->sin_addr),
					     AF_INET);
	 if (Ent != 0)
	    strncpy(host,Ent->h_name,hostlen);
	 else
	 {
	    if ((flags & NI_NAMEREQD) == NI_NAMEREQD)
	    {
	       if (h_errno == TRY_AGAIN)
	       {
		  releaseresolvsem();
		  return EAI_AGAIN;
	       }
	       if (h_errno == NO_RECOVERY)
	       {
		  releaseresolvsem();
		  return EAI_FAIL;
	       }
	       releaseresolvsem();
	       return EAI_NONAME;
	    }
	    flags |= NI_NUMERICHOST;
	 }
	 releaseresolvsem();
      }
      
      /* Resolve as a plain numberic */
      if ((flags & NI_NUMERICHOST) == NI_NUMERICHOST)
      {
	 lockhostsem();
	 strncpy(host,inet_ntoa(sin->sin_addr),hostlen);
	 releasehostsem();
      }
   }
   
   if (serv != 0)
   {
      /* Try to get service name */
      if ((flags & NI_NUMERICSERV) != NI_NUMERICSERV)
      {
	 struct servent *Ent;
	 lockresolvsem();
	 if ((flags & NI_DATAGRAM) == NI_DATAGRAM)
	    Ent = getservbyport(ntohs(sin->sin_port), "udp");
	 else
	    Ent = getservbyport(ntohs(sin->sin_port), "tcp");
	 
	 if (Ent != 0)
	    strncpy(serv,Ent->s_name, servlen);
	 else
	 {
	    if ((flags & NI_NAMEREQD) == NI_NAMEREQD)
	    {
	       releaseresolvsem();
	       return EAI_NONAME;
	    }

	    flags |= NI_NUMERICSERV;
	 }
	 releaseresolvsem();
      }
      
      /* Resolve as a plain numeric */
      if ((flags & NI_NUMERICSERV) == NI_NUMERICSERV)
	 snprintf(serv, servlen, "%u", ntohs(sin->sin_port));
   }
   
   return 0;
}

#endif /* HAVE_GETNAMEINFO */
