/*--------------------------------------------------------------------*/
/*       T c p E r r . c                                              */
/*                                                                    */
/*       Part of BinkD project                                        */
/*       WinSock error's                                              */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*       Copyright (c) 1996 by Fydodor Ustinov                        */
/*                             FIDONet 2:5020/79                      */
/*                                                                    */
/*  This program is  free software;  you can  redistribute it and/or  */
/*  modify it  under  the terms of the GNU General Public License as  */
/*  published  by the  Free Software Foundation; either version 2 of  */
/*  the License, or (at your option) any later version. See COPYING.  */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                          RCS Information                           */
/*--------------------------------------------------------------------*/

/*
 * $Id$
 *
 * Revision history:
 * $Log$
 * Revision 2.7  2003/06/04 10:36:59  stas
 * Thread-safety tcperr() implementation on Win32
 *
 * Revision 2.6  2003/05/25 12:52:35  stas
 * Replace CR and LF with spaces in system error messages
 *
 * Revision 2.5  2003/05/23 17:54:08  stas
 * Display default text at unknown win32 error
 *
 * Revision 2.4  2003/05/23 09:11:13  stas
 * Improve diagnostic: print Win32API error message on unknown error
 *
 * Revision 2.3  2003/05/23 07:55:23  stas
 * Update error messages, add comments with messages published in MSDN
 *
 * Revision 2.2  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.1  2003/02/13 19:44:44  gul
 * Change \r\n -> \n
 *
 * Revision 2.0  2001/01/10 12:12:40  gul
 * Binkd is under CVS again
 *
 * Revision 0.02  1996/12/15  18:58:09  ufm
 *      Fixed bug with wrong IP Errors numeration.
 *      For sample, Error "Connection refused" (61) says as
 *      "Too many levels of symbolic links"  (62)
 *
 * Revision 0.01  1996/12/03  12:15:15  ufm
 *      First revision
 *
 */

 static const char rcsid[] =
      "$Id$";

/*--------------------------------------------------------------------*/
/*                        System include files                        */
/*--------------------------------------------------------------------*/

#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <stdlib.h>
#include "../sem.h"

/*--------------------------------------------------------------------*/
/*                        Local include files                         */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                         Global definitions                         */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                          Global variables                          */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                           Local variables                          */
/*--------------------------------------------------------------------*/

static MUTEXSEM err_sem;

typedef struct s_StrErrList{
  unsigned code;
  char *text;
  struct s_StrErrList *next;
} *pStrErrList;

static pStrErrList errlist;

/*--------------------------------------------------------------------*/
/*                    Local functions prototypes                      */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*    int tcperr(void)                                                */
/*                                                                    */
/*    return string to winsock error.                                 */
/*--------------------------------------------------------------------*/

/* Return error string for win32 API error
 * return pointer to malloc'ed string or NULL
 */
char *W32APIstrerror(int errnum)
{ char *st, *cp=NULL;
  int len;

  FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errnum,
        0, /* Default language */
        (LPTSTR) &cp,
        0,
        NULL
    );
  if( !cp || !(*cp) ){
    st = malloc(40);
    if(st) sprintf(st, "{%u} Unknown Win32API error", errnum);
  }else{
    len = strlen(cp)+15;
    st=malloc(len);
    if(st){
      len = sprintf(st, "{%u} ",errnum);
      AnsiToOem(cp,st+len);
      LocalFree(cp);  /* Win32 API free() implementation */
    }else st=cp;

    for(cp=st+len; *cp; cp++)
      switch(*cp){           /* Replace '\r' and '\n' with space */
        case '\r':
        case '\n':
          *cp=' ';
      }
  }

  return st;
}

/* Store new error message taken from FormatMessage() to errlist
 * Return message
 */
const char *newerrortolist(int err){
  pStrErrList newe;

  if( !(newe = malloc(sizeof(*newe))) ){
/*    Log( 0, "Out of memory (malloc())" );*/
    return NULL;
  }
  newe->code = err;
  newe->text = W32APIstrerror(err);
  newe->next = errlist;
  errlist = newe;

  if( newe->text )
    return newe->text;

  return "Unknown error"; /* Don't return NULL! */
}

/* Release memory from errlist, release semaphore
 */
void ReleaseErrorList(void){
  pStrErrList el;

  if( errlist && !LockSem(&err_sem) ){
    while(errlist){
      if(errlist->text) free(errlist->text);
      el = errlist;
      errlist=errlist->next;
      free(el);
    }
    ReleaseSem(&err_sem);
  }
  CleanSem(&err_sem);
}

/* Return error message from errlist
 */
const char *errorfromlist(int err){
  pStrErrList el;
  const char *errmess="";
  static char first = 1;

  for( el=errlist; el; el=el->next )
    if( el->code==err )
      return el->text;

  if(first){ InitSem(&err_sem); first=0; }

  if( !LockSem(&err_sem) ){
    errmess = newerrortolist(err);
    ReleaseSem(&err_sem);
  }

  return errmess;
}

/* Return error message
 * (Massages taken from MSDN)
 */
const char *tcperr (int errnum) {
  const char *Str;

  switch( errnum ){
  case 10004:
     Str = "{10004} A blocking operation was interrupted by a call to WSACancelBlockingCall";
     break;
/* Not in MSDN, not return by FormatMessage
  case 10006:
     Str = "{10006} No such device or address";
     break; */
  case 10009:
/*     Str = "{10009} Bad file number";*/
     Str = "{10009} The file handle supplied is not valid";
     break;
  case 10013:
/*     Str = "{10013} Permission denied";*/
     Str = "{10013} An attempt was made to access a socket in a way forbidden by its access permissions";
     break;
  case 10014:
/*     Str = "{10014} Bad address";*/
     Str = "{10014} The system detected an invalid pointer address in attempting to use a pointer argument in a call";
     break;
  case 10022:
/*     Str = "{10022} Invalid argument";*/
     Str = "{10022} An invalid argument was supplied";
     break;
  case 10024:
     Str = "{10024} Too many open sockets";
     break;
/* Not in MSDN, not return by FormatMessage
  case 10032:
     Str = "{10032} Broken pipe";
     break;*/
  case 10035:
/*     Str = "{10035} Operation would block";*/
     Str = "{10035} A non-blocking socket operation could not be completed immediately";
     break;
  case 10036:
/*     Str = "{10036} Operation now in progress";*/
     Str = "{10036} A blocking operation is currently executing";
     break;
  case 10037:
/*     Str = "{10037} Operation already in progress";*/
     Str = "{10037} An operation was attempted on a non-blocking socket that already had an operation in progress";
     break;
  case 10038:
/*     Str = "{10038} Socket operation on non-socket";*/
     Str = "{10038} An operation was attempted on something that is not a socket";
     break;
  case 10039:
/*     Str = "{10039} Destination address required";*/
     Str = "{10039} A required address was omitted from an operation on a socket";
     break;
  case 10040:
/*     Str = "{10040} Message too long";*/
     Str = "{10040} A message sent on a datagram socket was larger than the internal message buffer or some other network limit, or the buffer used to receive a datagram into was smaller than the datagram itself";
     break;
  case 10041:
/*     Str = "{10041} Protocol wrong type for socket";*/
     Str = "{10041} A protocol was specified in the socket function call that does not support the semantics of the socket type requested";
     break;
  case 10042:
/*     Str = "{10042} Protocol not available";*/
     Str = "{10042} An unknown, invalid, or unsupported option or level was specified in a getsockopt or setsockopt call";
     break;
  case 10043:
/*     Str = "{10043} Protocol not supported";*/
     Str = "{10043} The requested protocol has not been configured into the system, or no implementation for it exists";
     break;
  case 10044:
/*     Str = "{10044} Socket type not supported";*/
     Str = "{10044} The support for the specified socket type does not exist in this address family";
     break;
  case 10045:
/*     Str = "{10045} Operation not supported on socket";*/
     Str = "{10045} The attempted operation is not supported for the type of object referenced";
     break;
  case 10046:
/*     Str = "{10046} Protocol family not supported";*/
     Str = "{10046} The protocol family has not been configured into the system or no implementation for it exists";
     break;
  case 10047:
/*     Str = "{10047} Address family not supported by protocol family";*/
     Str = "{10047} An address incompatible with the requested protocol was used";
     break;
  case 10048:
/*     Str = "{10048} Address already in use";*/
     Str = "{10048} Only one usage of each socket address (protocol/network address/port) is normally permitted";
     break;
  case 10049:
/*     Str = "{10049} Can't assign requested address";*/
     Str = "{10049} The requested address is not valid in its context";
     break;
  case 10050:
/*     Str = "{10050} Network is down";*/
     Str = "{10050} A socket operation encountered a dead network";
     break;
  case 10051:
/*     Str = "{10052} Network is unreachable";*/
     Str = "{10052} A socket operation was attempted to an unreachable network";
     break;
  case 10052:
/*     Str = "{10052} Network dropped connection on reset";*/
     Str = "{10052} The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress";
     break;
  case 10053:
/*     Str = "{10053} Software caused connection abort";*/
     Str = "{10053} An established connection was aborted by the software in your host machine";
     break;
  case 10054:
/*     Str = "{10054} Connection reset by peer›Ê*/
     Str = "{10054} An existing connection was forcibly closed by the remote host";
     break;
  case 10055:
     Str = "{10055} No buffer space available";
/*     Str = "{10055} An operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full";*/
     break;
  case 10056:
/*     Str = "{10056} Socket is already connected";*/
     Str = "{10056} A connect request was made on an already connected socket";
     break;
  case 10057:
     Str = "{10057} Socket is not connected";
/*     Str = "{10057} A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) no address was supplied";*/
     break;
  case 10058:
     Str = "{10058} Can't send after socket shutdown";
/*     Str = "{10058} A request to send or receive data was disallowed because the socket had already been shut down in that direction with a previous shutdown call";*/
     break;
  case 10059:
/*     Str = "{10059} Too many references: can't splice";*/
     Str = "{10059} Too many references to some kernel object";
     break;
  case 10060:
     Str = "{10060} Connection timed out";
/*     Str = "{10060} A connection attempt failed because the connected party did not properly respond after a period of time, or established connection failed because connected host has failed to respond"; */
     break;
  case 10061:
     Str = "{10060} Connection refused";
/*     Str = "{10061} No connection could be made because the target machine actively refused it";*/
     break;
  case 10062:
/*     Str = "{10062} Too many levels of symbolic links";*/
     Str = "{10062} Cannot translate name";
     break;
  case 10063:
/*     Str = "{10063} File name too long";*/
     Str = "{10063} Name component or name was too long";
     break;
  case 10064:
/*     Str = "{10064} Host is down";*/
     Str = "{10064} A socket operation failed because the destination host was down";
     break;
  case 10065:
/*     Str = "{10065} No route to host";*/
     Str = "{10065} A socket operation was attempted to an unreachable host";
     break;
  case 10066:
/*     Str = "{10066} Directory not empty";*/
     Str = "{10066} Cannot remove a directory that is not empty";
     break;
  case 10067:
/*     Str = "{10067} Applications limit reached";*/
     Str = "{10067} A Windows Sockets implementation may have a limit on the number of applications that may use it simultaneously.";
     break;
/*  case 10068:
     Str = "{10068} Ran out of quota.";
     break;*/
  case 10070:
     Str = "{10070} File handle reference is no longer available.";
     break;
  case 10071:
     Str = "{10071} Item is not available locally.";
     break;
  case 10091:
     Str = "{10091} WSAStartup cannot function at this time because the underlying system it uses to provide network services is currently unavailable.";
     break;
  case 10092:
     Str = "{10092} The Windows Sockets version requested is not supported.";
     break;
  case 10093:
     Str = "{10093} Either the application has not called WSAStartup, or WSAStartup failed.";
     break;
  case 10101:
     Str = "{10101} Returned by WSARecv or WSARecvFrom to indicate the remote party has initiated a graceful shutdown sequence.";
     break;
  case 11001:
     Str = "{11001} No such host is known.";
     break;
  case 11002:
     Str = "{11002} This is usually a temporary error during hostname resolution and means that the local server did not receive a response from an authoritative server.";
     break;
  default:
     Str = errorfromlist(errnum);
  }
   return Str;
}
