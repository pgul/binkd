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

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "sys.h"
#include "run.h"
#include "tools.h"
#include "sem.h"

#ifdef UNIX
#define SHELL "/bin/sh"
#define SHELL_META "\"\'\\$`[]*?(){};&|<>~"
#define SHELLOPT "-c"
#elif defined(WIN32)
#define SHELL (getenv("COMSPEC") ? getenv("COMSPEC") : "cmd.exe")
#define SHELL_META "\"\'\\%<>|&^@"
#define SHELLOPT "/c"
#elif defined(OS2)
#define SHELL "cmd.exe"
#define SHELL_META "\"\'\\%<>|" /* not sure */
#define SHELLOPT "/c"
#elif defined(DOS)
#define SHELL (getenv("COMSPEC") ? getenv("COMSPEC") : "command.com")
#define SHELL_META "\"\'\\%<>|&^@"
#define SHELLOPT "/c"
#else
#error "Unknown platform"
#endif

int run (char *cmd)
{
  int rc=-1;
#if defined(EMX)
  sigset_t s;
    
  sigemptyset(&s);
  sigaddset(&s, SIGCHLD);
  sigprocmask(SIG_BLOCK, &s, NULL);
  Log (3, "executing `%s'", cmd);
  Log (3, "rc=%i", (rc=system (cmd)));
  sigprocmask(SIG_UNBLOCK, &s, NULL);
#elif defined(WIN32)
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  DWORD dw;
  char *cs, *sp=getenv("COMSPEC");

  Log (3, "executing `%s'", cmd);
  memset(&si, 0, sizeof(si));
  si.cb=sizeof(si);
  if (!sp)
  {
    if (Is9x())
      sp="command";
    else
      sp="cmd";
  }
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
#else
  Log (3, "executing `%s'", cmd);
  Log (3, "rc=%i", (rc=system (cmd)));
#endif
  return rc;
}

#ifdef __MINGW32__
static int set_cloexec(int fd)
{
  HANDLE h, parent;
  int newfd;

  // return fd;
  parent = GetCurrentProcess();
  if (!DuplicateHandle(parent, (HANDLE)_get_osfhandle(fd), parent, &h, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
    Log(1, "Error DuplicateHandle");
    return fd;
  }
  newfd = _open_osfhandle((int)h, O_NOINHERIT);
  if (newfd < 0) {
    Log(1, "Error open_odfhandle");
    CloseHandle(h);
    return fd;
  }
  close(fd);
  // Log(1, "NoInherit set for %i, new handle %i", fd, newfd);
  return newfd;
}
#endif

int run3 (const char *cmd, int *in, int *out, int *err)
{
  int pid;
  int pin[2], pout[2], perr[2];
  const char *shell;

  if (in && pipe(pin) == -1)
  {
    Log (1, "Cannot create input pipe (stdin): %s", strerror(errno));
    return -1;
  }
  if (out && pipe(pout) == -1)
  {
    Log (1, "Cannot create output pipe (stdout): %s", strerror(errno));
    if (in)  close(pin[1]),  close(pin[0]);
    return -1;
  }
  if (err && pipe(perr) == -1)
  {
    Log (1, "Cannot create error pipe (stderr): %s", strerror(errno));
    if (in)  close(pin[1]),  close(pin[0]);
    if (out) close(pout[1]), close(pout[0]);
    return -1;
  }

#ifdef HAVE_FORK
  pid = fork();
  if (pid == -1)
  {
    Log (1, "Cannot fork: %s", strerror(errno));
    if (in)  close(pin[1]),  close(pin[0]);
    if (out) close(pout[1]), close(pout[0]);
    if (err) close(perr[1]), close(perr[0]);
    return -1;
  }
  if (pid == 0)
  { /* child */
    if (in)
    {
      dup2(pin[0], fileno(stdin));
      close(pin[0]);
      close(pin[1]);
    }
    if (out)
    {
      dup2(pout[1], fileno(stdout));
      close(pout[0]);
      close(pout[1]);
    }
    if (err)
    {
      dup2(perr[1], fileno(stderr));
      close(perr[0]);
      close(perr[1]);
    }
    if (strpbrk(cmd, SHELL_META))
    {
      shell = SHELL;
      execl(shell, shell, SHELLOPT, cmd, (char *)NULL);
    }
    else
    {
      /* execute command directly */
      /* in case of shell builtin like "read line" you should specify 
       * shell exclicitly, such as "/bin/sh -c read line" */
      char **args, *word;
      int i;

      args = xalloc(sizeof(args[0]));
      for (i=1; (word = getword(cmd, i)) != NULL; i++)
      {
        args = xrealloc(args, (i+1) * sizeof(*args));
        args[i-1] = word;
      }
      args[i-1] = NULL;
      execvp(args[0], args);
      xfree(args);
    }
    Log (1, "Execution '%s' failed: %s", cmd, strerror(errno));
    return -1;
  }
  if (in)
  {
    *in = pin[1];
    close(pin[0]);
  }
  if (out)
  {
    *out = pout[0];
    close(pout[1]);
  }
  if (err)
  {
    *err = perr[0];
    close(perr[1]);
  }
#else

  /* redirect stdin/stdout/stderr takes effect for all threads */
  /* use lsem to avoid console output during this */
  {
    int save_errno = 0, savein = -1, saveout = -1, saveerr = -1;

    LockSem(&lsem);
    fflush(stdout);
    fflush(stderr);

    if (in)
    {
      savein = dup(fileno(stdin));
      dup2(pin[0], fileno(stdin));
      *in = pin[1];
      close(pin[0]);
#if defined(OS2)
      DosSetFHState(*in, OPEN_FLAGS_NOINHERIT);
#elif defined(EMX)
      fcntl(*in, F_SETFD, FD_CLOEXEC);
#elif defined __MINGW32__
      *in = set_cloexec(*in);
#endif
    }
    if (out)
    {
      saveout = dup(fileno(stdout));
      dup2(pout[1], fileno(stdout));
      *out = pout[0];
      close(pout[1]);
#if defined(OS2)
      DosSetFHState(*out, OPEN_FLAGS_NOINHERIT);
#elif defined(EMX)
      fcntl(*out, F_SETFD, FD_CLOEXEC);
#elif defined __MINGW32__
      *out = set_cloexec(*out);
#endif
    }
    if (err)
    {
      saveerr = dup(fileno(stderr));
      dup2(perr[1], fileno(stderr));
      *err = perr[0];
      close(perr[1]);
#if defined(OS2)
      DosSetFHState(*err, OPEN_FLAGS_NOINHERIT);
#elif defined(EMX)
      fcntl(*err, F_SETFD, FD_CLOEXEC);
#elif defined __MINGW32__
      *err = set_cloexec(*err);
#endif
    }
    if (strpbrk(cmd, SHELL_META) == NULL)
    {
      /* execute command directly */
      char **args, *word;
      int i;

      args = xalloc(sizeof(args[0]));
      for (i=1; (word = getword(cmd, i)) != NULL; i++)
      {
        args = xrealloc(args, (i+1) * sizeof(*args));
        args[i-1] = word;
      }
      args[i-1] = NULL;
      pid = spawnvp(P_NOWAIT, args[0], args);
      xfree(args);
    }
    else
    {
      shell = SHELL;
      pid = spawnl(P_NOWAIT, shell, shell, SHELLOPT, cmd, NULL);
    }

    if (pid == -1)
      save_errno = errno;
    if (savein != -1)
    {
      dup2(savein, fileno(stdin));
      close(savein);
    }
    if (saveout != -1)
    {
      dup2(saveout, fileno(stdout));
      close(saveout);
    }
    if (saveerr != -1)
    {
      dup2(saveerr, fileno(stderr));
      close(saveerr);
    }
    ReleaseSem(&lsem);
    if (pid == -1)
    {
      Log (1, "Cannot execute '%s': %s", cmd, strerror(save_errno));
      return -1;
    }
  }
#endif
  Log (2, "External command '%s' started, pid %i", cmd, pid);
  return pid;
}

