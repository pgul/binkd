/*
 *  run.c -- Run external programs
 *
 *  run.c is a part of binkd project
 *
 *  Copyright (C) 1996-1997  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

/*
 * $Id$
 *
 * $Log$
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 * Revision 1.1  1997/03/28  06:16:56  mff
 * Initial revision
 *
 */

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#if defined WIN32
#include <windows.h>
#endif

#include "run.h"
#include "tools.h"

void run (char *cmd)
{
#if !defined(WIN32) && !defined(EMX)
  Log (3, "executing `%s'", cmd);
  Log (3, "rc=%i", system (cmd));
#elif defined(EMX)
  {
    sigset_t s;
    
    sigemptyset(&s);
    sigaddset(&s, SIGCHLD);
    sigprocmask(SIG_BLOCK, &s, NULL);
    Log (3, "executing `%s'", cmd);
    Log (3, "rc=%i", system (cmd));
    sigprocmask(SIG_UNBLOCK, &s, NULL);
  }
#else /* WIN32 */
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  DWORD dw;
  char *cs, *sp=getenv("COMSPEC");

  Log (3, "executing `%s'", cmd);
  memset(&si, 0, sizeof(si));
  si.cb=sizeof(si);
  if(!sp) sp="command";
  cs=(char*)malloc(strlen(sp)+strlen(cmd)+6);
  dw=CREATE_DEFAULT_ERROR_MODE;
  strcpy(cs, sp);
  strcat(cs, " /C ");
  sp=cmd;
  if (sp[0]=='@')
  {
    dw|=CREATE_NEW_CONSOLE|CREATE_NEW_PROCESS_GROUP;
    sp++;
    if (sp[0]=='@')
    {
      si.dwFlags=STARTF_USESHOWWINDOW;
      si.wShowWindow=SW_HIDE;
      sp++;
    }
    else 
      si.lpTitle=sp;
  }
  strcat(cs, sp);
  if (!CreateProcess(NULL, cs, NULL, NULL, 0, dw, NULL, NULL, &si, &pi))
    Log (1, "Error in CreateProcess()=%d", GetLastError());
  else if (sp==cmd)
    for(;;)
    {
      if (!GetExitCodeProcess(pi.hProcess, &dw))
      {
        Log (1, "Error in GetExitCodeProcess()=%d", GetLastError());
        break;
      }
      else if (dw!=STILL_ACTIVE)
      {
        Log (3, "rc=%i", dw);
        break;
      }
      Sleep(100);
    }
  free(cs);
#endif
}
