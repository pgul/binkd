/* $Id$ */
/*
 * Common part of SIGCHLD handling for diff.parts of binkd
 * Should be included in chld()
 */
{
  int old_errno = errno;
  int status;
  int pid;
  extern int pidcmgr;

#ifdef HAVE_WAITPID
  while ((pid = waitpid (-1, &status, WNOHANG)) > 0)	/* NO ";" !!! */
#else
  pid = (int) wait (&status);
#endif
  if (pid > 0)
  {
    if (pidcmgr && pid == pidcmgr) {
      Log (0, "client manager (pid=%u) exited, retcode %u", pid, status);
      exit(4);
    }
#ifdef CHILDCOUNT
    CHILDCOUNT--;
#endif
    Log (4, "rc(%i)=%i", pid, status);
  }

#ifdef SYS5SIGNALS
  signal (SIGCHLD, chld);
#endif
#ifdef EMXSIGNALS
  signal (SIGCHLD, SIG_ACK);
#endif
  errno = old_errno;
}
