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
static const char *sockerrors[] =
{ /*string*/                                /*index*/          /*Message taken from MSDN*/
  NULL,                                 /* 0 */
  NULL,                                 /* 1 */
  NULL,                                 /* 2 */
  NULL,                                 /* 3 */
  "The (blocking) call was canceled via WSACancelBlockingCall().", /* +4 */ /*A blocking operation was interrupted by a call to WSACancelBlockingCall*/
  NULL,                                 /* 5 */
  "No such device or address",		    /* SOCBASEERR+6 */
  NULL,                                 /* 7 */
  NULL,                                 /* 8 */
  "Bad file number",			    /* SOCBASEERR+9 */ /*The file handle supplied is not valid*/
  NULL,                                /* 10 */
  NULL,                                /* 11 */
  NULL,                                /* 12 */
  "Permission denied",			    /* SOCBASEERR+13 */ /*An attempt was made to access a socket in a way forbidden by its access permissions*/
  "Bad address",			    /* SOCBASEERR+14 */ /*The system detected an invalid pointer address in attempting to use a pointer argument in a call*/
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  "Invalid argument",			    /* SOCBASEERR+22 */ /*An invalid argument was supplied*/
  NULL,
  "Too many open sockets",		    /* SOCBASEERR+24 */ /*Too many open sockets*/
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  "Broken pipe",			    /* SOCBASEERR+32 */
  NULL,
  NULL,
  "Operation would block",		    /* SOCBASEERR+35 */ /*A non-blocking socket operation could not be completed immediately*/
  "Operation now in progress",		    /* SOCBASEERR+36 */ /*A blocking operation is currently executing*/
  "Operation already in progress",	    /* SOCBASEERR+37 */ /*An operation was attempted on a non-blocking socket that already had an operation in progress*/
  "Socket operation on non-socket",	    /* SOCBASEERR+38 */ /*An operation was attempted on something that is not a socket*/
  "Destination address required",	    /* SOCBASEERR+39 */ /*A required address was omitted from an operation on a socket*/
  "Message too long",			    /* SOCBASEERR+40 */ /*A message sent on a datagram socket was larger than the internal message buffer or some other network limit, or the buffer used to receive a datagram into was smaller than the datagram itself*/
  "Protocol wrong type for socket",	    /* SOCBASEERR+41 */ /*A protocol was specified in the socket function call that does not support the semantics of the socket type requested*/
  "Protocol not available",		    /* SOCBASEERR+42 */ /*An unknown, invalid, or unsupported option or level was specified in a getsockopt or setsockopt call*/
  "Protocol not supported",		    /* SOCBASEERR+43 */ /*The requested protocol has not been configured into the system, or no implementation for it exists*/
  "Socket type not supported",		    /* SOCBASEERR+44 */ /*The support for the specified socket type does not exist in this address family*/
  "Operation not supported on socket",	    /* SOCBASEERR+45 */ /*The attempted operation is not supported for the type of object referenced*/
  "Protocol family not supported",	    /* SOCBASEERR+46 */ /*The protocol family has not been configured into the system or no implementation for it exists*/
  "Address family not supported by protocol family",	/* SOCBASEERR+47 */ /*An address incompatible with the requested protocol was used*/
  "Address already in use",		    /* SOCBASEERR+48 */ /*Only one usage of each socket address (protocol/network address/port) is normally permitted*/
  "Can't assign requested address",	    /* SOCBASEERR+49 */ /*The requested address is not valid in its context*/
  "Network is down",			    /* SOCBASEERR+50 */ /*A socket operation encountered a dead network*/
  "Network is unreachable",		    /* SOCBASEERR+51 */ /*A socket operation was attempted to an unreachable network*/
  "Network dropped connection on reset",    /* SOCBASEERR+52 */ /*The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress*/
  "Software caused connection abort",	    /* SOCBASEERR+53 */ /*An established connection was aborted by the software in your host machine*/
  "Connection reset by peer",		    /* SOCBASEERR+54 */ /*An existing connection was forcibly closed by the remote host*/
  "No buffer space available",		    /* SOCBASEERR+55 */ /*An operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full*/
  "Socket is already connected",	    /* SOCBASEERR+56 */ /*A connect request was made on an already connected socket*/
  "Socket is not connected",		    /* SOCBASEERR+57 */ /*A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) no address was supplied*/
  "Can't send after socket shutdown",	    /* SOCBASEERR+58 */ /*A request to send or receive data was disallowed because the socket had already been shut down in that direction with a previous shutdown call*/
  "Too many references: can't splice",	    /* SOCBASEERR+59 */ /*Too many references to some kernel object*/
  "Connection timed out",		    /* SOCBASEERR+60 */ /*A connection attempt failed because the connected party did not properly respond after a period of time, or established connection failed because connected host has failed to respond*/
  "Connection refused",			    /* SOCBASEERR+61 */ /*No connection could be made because the target machine actively refused it*/
  "Too many levels of symbolic links",	    /* SOCBASEERR+62 */ /*Cannot translate name*/
  "File name too long",			    /* SOCBASEERR+63 */ /*Name component or name was too long*/
  "Host is down",			    /* SOCBASEERR+64 */ /*A socket operation failed because the destination host was down*/
  "No route to host",			    /* SOCBASEERR+65 */ /*A socket operation was attempted to an unreachable host*/
  "Directory not empty",		    /* SOCBASEERR+66 */ /*Cannot remove a directory that is not empty*/
  "Applications limit reached",                                 /*A Windows Sockets implementation may have a limit on the number of applications that may use it simultaneously*/
  NULL,                                                   /*Ran out of quota*/
  NULL,                                                   /*Ran out of disk quota*/
  NULL,                                                   /*File handle reference is no longer available*/
  NULL                                                    /*Item is not available locally*/

};

/*--------------------------------------------------------------------*/
/*                    Local functions prototypes                      */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*    int tcperr(void)                                                */
/*                                                                    */
/*    return string to winsock error.                                 */
/*--------------------------------------------------------------------*/

#define W32API_StrErrorSize 255
/* Return error string for win32 API error
 * return pointer to static char array
 */
char *W32APIstrerror(int errnum)
{ static char st[W32API_StrErrorSize];
  char stemp[W32API_StrErrorSize];
  char *cp;

  stemp[0]='\0';
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errnum,
        0, /* Default language */
        (LPTSTR) &stemp,
        sizeof(stemp),
        NULL
    );
    AnsiToOem(stemp,st);
  for(cp=st; *cp; cp++)
    switch(*cp){           /* Replace '\r' and '\n' with space */
      case '\r':
      case '\n':
        *cp=' ';
    }

  return st;
}

const char *tcperr (void) {
  static char Str[W32API_StrErrorSize+15];
  int err = h_errno - WSABASEERR;
  char *stemp;

   if ( (err<0) || (err > (sizeof (sockerrors) / sizeof (char *))) || !sockerrors[err] || !sockerrors[err][0] ) {
     stemp = W32APIstrerror(h_errno);
     if(stemp[0])
      sprintf(Str,"{%d} %s",h_errno,stemp);
     else
      sprintf(Str,"{%d} unknown Win32 API error",h_errno);
   } else {
      sprintf(Str,"{%d} %s",h_errno,sockerrors[err]);
   }
   return Str;
}
