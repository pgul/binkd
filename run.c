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
 * Revision 2.6.2.2  2014/08/20 06:12:37  gul
 * Fixed 100% cpu load if called with poll flag,
 * backport many fixes related to compilation on win32 and os/2.
 *
 * Revision 2.6.2.1  2014/08/09 15:17:44  gul
 * Large files support on Win32 (backport from develop branch)
 *
 * Revision 2.6  2003/10/29 21:08:39  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.5  2003/08/26 21:01:10  gul
 * Fix compilation under unix
 *
 * Revision 2.4  2003/04/07 18:22:16  gul
 * Wait for external process under win32 bugfix
 *
 * Revision 2.3  2003/04/06 08:01:32  gul
 * Close handles after CreateProcess()
 *
 * Revision 2.2  2003/04/06 07:54:41  gul
 * Change wait for child process function for win32
 *
 * Revision 2.1  2001/10/27 08:07:17  gul
 * run and run_args returns exit code of calling process
 *
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

#include "sys.h"
#include "run.h"
#include "tools.h"

int run (char *cmd)
{
  int rc=-1;
#if !defined(WIN32) && !defined(EMX)
  Log (3, "executing `%s'", cmd);
  Log (3, "rc=%i", (rc=system (cmd)));
#elif defined(EMX)
  sigset_t s;
    
  sigemptyset(&s);
  sigaddset(&s, SIGCHLD);
  sigprocmask(SIG_BLOCK, &s, NULL);
  Log (3, "executing `%s'", cmd);
  Log (3, "rc=%i", (rc=system (cmd)));
  sigprocmask(SIG_UNBLOCK, &s, NULL);
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
    Log (1, "Error in CreateProcess()=%ld", (long)GetLastError());
  else if (sp==cmd)
  {
    if (WaitForSingleObject(pi.hProcess, INFINITE) != WAIT_OBJECT_0)
      Log (1, "Error in WaitForSingleObject()=%ld", (long)GetLastError());
    else if (!GetExitCodeProcess(pi.hProcess, &dw))
      Log (1, "Error in GetExitCodeProcess()=%ld", (long)GetLastError());
    else
      Log (3, "rc=%i", rc = (int)dw);
  }
  free(cs);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
#endif
  return rc;
}
