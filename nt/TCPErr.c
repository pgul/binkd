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
  "Error 0",                                 /* 0 */
  "Error 1",                                 /* 1 */
  "Error 2",                                 /* 2 */
  "Error 3",                                 /* 3 */
  "The (blocking) call was canceled via WSACancelBlockingCall().", /* +4 */ /*A blocking operation was interrupted by a call to WSACancelBlockingCall*/
  "Error 5",                                 /* 5 */
  "No such device or address",		    /* SOCBASEERR+6 */
  "Error 7",                                 /* 7 */
  "Error 8",                                 /* 8 */
  "Bad file number",			    /* SOCBASEERR+9 */ /*The file handle supplied is not valid*/
  "Error 10",                                /* 10 */
  "Error 11",                                /* 11 */
  "Error 12",                                /* 12 */
  "Permission denied",			    /* SOCBASEERR+13 */ /*An attempt was made to access a socket in a way forbidden by its access permissions*/
  "Bad address",			    /* SOCBASEERR+14 */ /*The system detected an invalid pointer address in attempting to use a pointer argument in a call*/
  "Error 15",
  "Error 16",
  "Error 17",
  "Error 18",
  "Error 19",
  "Error 20",
  "Error 21",
  "Invalid argument",			    /* SOCBASEERR+22 */ /*An invalid argument was supplied*/
  "Error 23",
  "Too many open sockets",		    /* SOCBASEERR+24 */ /*Too many open sockets*/
  "Error 25",
  "Error 26",
  "Error 27",
  "Error 28",
  "Error 29",
  "Error 30",
  "Error 31",
  "Broken pipe",			    /* SOCBASEERR+32 */
  "Error 33",
  "Error 34",
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
  "Error 67",                                                   /*A Windows Sockets implementation may have a limit on the number of applications that may use it simultaneously*/
  "Error 68",                                                   /*Ran out of quota*/
  "Error 69",                                                   /*Ran out of disk quota*/
  "Error 70",                                                   /*File handle reference is no longer available*/
  "Error 71"                                                    /*Item is not available locally*/

};

/*--------------------------------------------------------------------*/
/*                    Local functions prototypes                      */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*    int tcperr(void)                                                */
/*                                                                    */
/*    return string to winsock error.                                 */
/*--------------------------------------------------------------------*/

const char *tcperr (void) {
static char Str[512];
int err = h_errno - WSABASEERR;

   if (err > (sizeof (sockerrors) / sizeof (char *))) {   
      sprintf(Str,"TCP/IP error (%d)",err);
      return Str;
   } else {
      sprintf(Str,"{%d} %s",err,sockerrors[err]);
      return Str;
   }
}
