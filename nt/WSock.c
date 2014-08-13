/*--------------------------------------------------------------------*/
/*       W S o c k . c                                                */
/*                                                                    */
/*       Part of BinkD project                                        */
/*       WinSock Initialisation/Deinitialisation module               */
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
 * Revision 2.5  2014/08/13 20:50:55  gul
 * Fixed IPv6 support with MSVC build
 *
 * Revision 2.4  2012/01/03 17:25:35  green
 * Implemented IPv6 support
 * - replace (almost) all getXbyY function calls with getaddrinfo/getnameinfo (RFC2553) calls
 * - Add compatibility layer for target systems not supporting RFC2553 calls in rfc2553.[ch]
 * - Add support for multiple listen sockets -- one for IPv4 and one for IPv6 (use V6ONLY)
 * - For WIN32 platform add configuration parameter IPV6 (mutually exclusive with BINKD9X)
 * - On WIN32 platform use Winsock2 API if IPV6 support is requested
 * - config: node IP address literal + port supported: [<ipv6 address>]:<port>
 *
 * Revision 2.3  2003/08/26 22:18:49  gul
 * Fix compilation under w32-mingw and os2-emx
 *
 * Revision 2.2  2003/08/05 05:36:14  hbrew
 * 'static const char rcsid[]' removed
 *
 * Revision 2.1  2003/02/13 19:44:45  gul
 * Change \r\n -> \n
 *
 * Revision 2.0  2001/01/10 12:12:40  gul
 * Binkd is under CVS again
 *
 * Revision 0.01  1996/12/03  10:57:05  ufm
 *      First revision
 *
 */

/*--------------------------------------------------------------------*/
/*                        System include files                        */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                        Local include files                         */
/*--------------------------------------------------------------------*/

#include "../sys.h"
#include "../iphdr.h"
#include "../readcfg.h"
#include "../tools.h"

/*--------------------------------------------------------------------*/
/*                         Global definitions                         */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                          Global variables                          */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                           Local variables                          */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                    Local functions prototypes                      */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*    int WinsockIni(void)                                            */
/*                                                                    */
/*    Initialise Winsock.                                             */
/*--------------------------------------------------------------------*/

int WinsockIni(void) {
    WORD wVersionRequested;
    WSADATA wsaData;
    int Err;

    wVersionRequested = MAKEWORD( 1, 1 );

    Err = WSAStartup(wVersionRequested, &wsaData);
    if (Err != 0) {
       Log (0, "Cannot initialise WinSock");
       return (-1);
    }
    /*----------------------------------------------------------------*/
    /* than 1.1 in addition to 1.1, it will still return 1.1 in       */
    /* wVersion since that is the version we requested                */
    /*----------------------------------------------------------------*/

    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
       Log (0, "WinSock %d.%d detected. Required version 1.1",LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
       WSACleanup( );
       return (-1);
    }
    return 0;
}

/*--------------------------------------------------------------------*/
/*    int WinsockClean(void)                                          */
/*                                                                    */
/*    Initialise Winsock.                                             */
/*--------------------------------------------------------------------*/

int WinsockClean(void) {
    WSACleanup();
    return 0;
}
