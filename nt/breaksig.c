/*--------------------------------------------------------------------*/
/*       B r e a k S i g . c                                          */
/*                                                                    */
/*       Part of BinkD project                                        */
/*       Handle Ctrl-C & Ctrl-Break signals                           */
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
 *    $Id$
 *
 *    Revision history:
 *    $Log$
 *    Revision 1.1.1.1  2001/01/10 11:34:59  gul
 *    BinkD sources are under CVS again
 *
 *
 * Revision 1.02  1997/05/25  19:15:45 ufm
 *      Add binary log support
 *
 * Revision 1.01  1996/12/11  22:40:05  ufm
 *      First revision
 *
 */

 static const char rcsid[] =
      "$Id$";

/*--------------------------------------------------------------------*/
/*                        System include files                        */
/*--------------------------------------------------------------------*/

#include <windows.h>

/*--------------------------------------------------------------------*/
/*                        Local include files                         */
/*--------------------------------------------------------------------*/

#include "..\sys.h"
#include "..\tools.h"
#include "..\iphdr.h"
#include "..\bsy.h"
#include "..\binlog.h"
#include "..\readcfg.h"

/*--------------------------------------------------------------------*/
/*                         Global definitions                         */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                          Global variables                          */
/*--------------------------------------------------------------------*/
extern int pidcmgr;             /* pid for clientmgr */
extern int pid_file_created;    /* we've created the pid_file */
/*--------------------------------------------------------------------*/
/*                           Local variables                          */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                    Local functions prototypes                      */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*    BOOL SigHandler(DWORD SigType)                                  */
/*                                                                    */
/*    Signal handler                                                  */
/*--------------------------------------------------------------------*/
extern int isService;

BOOL SigHandler(DWORD SigType) {
   switch (SigType) {
      case CTRL_C_EVENT:
      case CTRL_BREAK_EVENT:
         Log(1,"Interrupted by keyboard");
         break;
      case CTRL_CLOSE_EVENT:
         Log(1,"Interrupted by Close");
         break;
      case CTRL_LOGOFF_EVENT:
         if(isService) return (TRUE);
         Log(1,"Interrupted by LogOff");
         break;
      case CTRL_SHUTDOWN_EVENT:
         Log(1,"Interrupted by Shutdown");
         break;
      default:
         Log(1,"Interrupted by unknown signal");
         break;
   }
   exitfunc();
   return (FALSE);
}

/*--------------------------------------------------------------------*/
/*    int HandleSignals(void)                                         */
/*                                                                    */
/*    Set signal handler                                              */
/*--------------------------------------------------------------------*/

int set_break_handlers () {
   atexit (exitfunc);
   if (SetConsoleCtrlHandler((PHANDLER_ROUTINE) &SigHandler,TRUE) != TRUE) {
      return (0);
   }
   return (1);
}
