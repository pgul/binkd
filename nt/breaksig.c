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
 * $Id$
 *
 * Revision history:
 * $Log$
 * Revision 2.7  2003/08/19 18:08:08  gul
 * Avoid double exitfunc() call
 *
 * Revision 2.6  2003/08/05 05:36:14  hbrew
 * 'static const char rcsid[]' removed
 *
 * Revision 2.5  2003/07/16 15:08:49  stas
 * Fix NT services to use getopt(). Improve logging for service
 *
 * Revision 2.4  2003/03/10 18:19:51  gul
 * Use common.h
 *
 * Revision 2.3  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.2  2002/11/12 16:55:59  gul
 * Run as service under win9x
 *
 * Revision 2.1  2001/09/24 10:31:39  gul
 * Build under mingw32
 *
 * Revision 2.0  2001/01/10 12:12:40  gul
 * Binkd is under CVS again
 *
 * Revision 1.02  1997/05/25  19:15:45 ufm
 *      Add binary log support
 *
 * Revision 1.01  1996/12/11  22:40:05  ufm
 *      First revision
 *
 */

/*--------------------------------------------------------------------*/
/*                        System include files                        */
/*--------------------------------------------------------------------*/

#include <windows.h>
#include <stdlib.h>

/*--------------------------------------------------------------------*/
/*                        Local include files                         */
/*--------------------------------------------------------------------*/

#include "..\sys.h"
#include "..\common.h"
#include "..\tools.h"
#include "..\iphdr.h"
#include "..\bsy.h"
#include "..\binlog.h"
#include "..\readcfg.h"
#ifdef BINKDW9X
#include "win9x.h"
#endif

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
#ifndef BINKDW9X
extern int isService;
#endif

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
#ifndef BINKDW9X
         if(isService) return (TRUE);
#endif
         Log(1,"Interrupted by LogOff");
         break;
      case CTRL_SHUTDOWN_EVENT:
         Log(1,"Interrupted by Shutdown");
         break;
      case CTRL_SERVICESTOP_EVENT:
         Log(1, "Interrupted by service stop");
         break;
      case CTRL_SERVICERESTART_EVENT:
         Log(1, "Interrupted by service restart");
         break;
      default:
         Log(1,"Interrupted by unknown signal");
         break;
   }
   return (FALSE);
}

/*--------------------------------------------------------------------*/
/*    int HandleSignals(void)                                         */
/*                                                                    */
/*    Set signal handler                                              */
/*--------------------------------------------------------------------*/

int set_break_handlers (void) {
   atexit (exitfunc);
#if BINKDW9X
   CreateWin9xThread((PHANDLER_ROUTINE) &SigHandler);
#else
   if (SetConsoleCtrlHandler((PHANDLER_ROUTINE) &SigHandler,TRUE) != TRUE) {
      return (0);
   }
#endif
   return (1);
}
