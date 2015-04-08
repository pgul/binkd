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
/*                        System include files                        */
/*--------------------------------------------------------------------*/

#include <stdlib.h>

/*--------------------------------------------------------------------*/
/*                        Local include files                         */
/*--------------------------------------------------------------------*/

#include "../sys.h"
#include "../iphdr.h"
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
