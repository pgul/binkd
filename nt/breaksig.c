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
 * Revision 2.23  2004/02/07 14:06:06  hbrew
 * Macros: RTLDLL-->RTLSTATIC, BINKDW9X-->BINKD9X
 *
 * Revision 2.22  2003/10/23 21:38:52  gul
 * Remove C++ style comments
 *
 * Revision 2.21  2003/10/17 04:20:29  hbrew
 * Fix binkd9x atexit()
 *
 * Revision 2.20  2003/10/13 08:48:10  stas
 * Implement true NT service stop sequence
 *
 * Revision 2.19  2003/10/11 17:31:27  stas
 * cosmetics (indent nt/breaksig.c)
 *
 * Revision 2.18  2003/10/09 09:41:07  stas
 * Change service stop sequence
 *
 * Revision 2.17  2003/10/08 11:10:49  stas
 * remove illegal call
 *
 * Revision 2.16  2003/10/08 05:48:57  stas
 * Fix w9x compilation
 *
 * Revision 2.15  2003/10/07 14:41:04  stas
 * Fix NT service shutdown
 *
 * Revision 2.14  2003/10/06 18:59:58  stas
 * Prevent double calls of ReportStatusToSCMgr(SERVICE_STOPPED,...) and double restart service
 *
 * Revision 2.13  2003/10/06 17:53:15  stas
 * (Prevent compiler warning.) Remove type convertion at CreateWin9xThread() call
 *
 * Revision 2.11  2003/10/05 07:37:47  stas
 * Fix NT service exit (don't hang service on receive CTRL_SERVICESTOP_EVENT)
 *
 * Revision 2.10  2003/08/26 16:06:27  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.9  2003/08/24 18:05:59  hbrew
 * Update for previous patch
 *
 * Revision 2.8  2003/08/24 17:28:31  hbrew
 * Fix work with sighandler on win32
 *
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

#include "../readcfg.h"
#include "../common.h"
#include "../tools.h"
#ifdef BINKD9X
#include "win9x.h"
#endif
#include "service.h"
#include "w32tools.h"

/*--------------------------------------------------------------------*/
/*                         Global definitions                         */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                          Global variables                          */
/*--------------------------------------------------------------------*/
extern int pidcmgr;		/* pid for clientmgr */
extern int pid_file_created;	/* we've created the pid_file */
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

static BOOL CALLBACK
SigHandler (DWORD SigType)
{
  Log (10, "SigHandler(%lu)", SigType);
  switch (SigType)
    {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
      Log (1, "Interrupted by keyboard");
      break;
    case CTRL_CLOSE_EVENT:
      Log (1, "Interrupted by Close");
      break;
    case CTRL_LOGOFF_EVENT:
#ifndef BINKD9X
      if (isService ())
	return (TRUE);
#endif
      Log (1, "Interrupted by LogOff");
      break;
    case CTRL_SHUTDOWN_EVENT:
      Log (1, "Interrupted by Shutdown");
      break;
    case CTRL_SERVICESTOP_EVENT:
      Log (1, "Interrupted by service stop");
      break;
    case CTRL_SERVICERESTART_EVENT:
      Log (1, "Interrupted by service restart");
      break;
    default:
      Log (1, "Interrupted by unknown signal %lu", SigType);
      break;
    }
  return (FALSE);
}


#if !defined(BINKD9X)
/*--------------------------------------------------------------------*/
/*  Signal handler for NT console                                     */
/*--------------------------------------------------------------------*/
static BOOL CALLBACK
SigHandlerNT (DWORD SigType)
{

  Log (10, "SigHandlerNT(%lu)", SigType);
  if (SigHandler (SigType) == FALSE)
    {
      exit (0);
    }
  return TRUE;
}
#endif

/*--------------------------------------------------------------------*/
/*    int set_break_handlers(void)                                    */
/*                                                                    */
/*    Set signal handler                                              */
/*--------------------------------------------------------------------*/

int
set_break_handlers (void)
{
  atexit (exitfunc);
#if BINKD9X
  CreateWin9xThread (&SigHandler);
#else
  if (IsNT () && isService ())
    atexit (&atServiceExitBegins);
  if (SetConsoleCtrlHandler (&SigHandlerNT, TRUE) != TRUE)
    {
      return (0);
    }
#endif
  return (1);
}
