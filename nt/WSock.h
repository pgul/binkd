/*--------------------------------------------------------------------*/
/*       W S o c k . h                                                */
/*                                                                    */
/*       Part of BinkD project                                        */
/*                                                                    */
/*       WinSock Initialisation/Deinitialisation module               */
/*                                                                    */
/*       Definition file.                                             */
/*--------------------------------------------------------------------*/
/*
 * $Id$
 *
 * $Log$
 * Revision 2.1  2003/02/13 19:44:45  gul
 * Change \r\n -> \n
 *
 * Revision 2.0  2001/01/10 12:12:40  gul
 * Binkd is under CVS again
 *
 *
 */

#ifndef __WSOCK_H__
#define __WSOCK_H__

int WinsockIni(void);

int WinsockClean(void);

#endif
