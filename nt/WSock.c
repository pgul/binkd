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

 static const char rcsid[] =
      "$Id$";

/*--------------------------------------------------------------------*/
/*                        System include files                        */
/*--------------------------------------------------------------------*/

#include <windows.h>
#include <winsock.h>


/*--------------------------------------------------------------------*/
/*                        Local include files                         */
/*--------------------------------------------------------------------*/

#include "..\tools.h"

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
