/*
 * Common part of SIGCHLD handling for diff.parts of binkd
 * Should be included in chld()
 */
/* 
 * $Id$
 *
 * $Log$
 * Revision 2.4  2003/12/26 21:31:45  gul
 * Report exit code or signal number of exited processes
 *
 * Revision 2.3  2003/09/21 17:34:27  gul
 * Change perl stderr handling for thread vertions,
 * some small changes.
 *
 * Revision 2.2  2003/08/18 09:41:01  gul
 * Little cleanup in handle perl errors
 *
 * Revision 2.1  2003/03/10 08:38:07  gul
 * Make n_servers/n_clients changes thread-safe
 *
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 *
 */
{
  int old_errno = errno;
  int status;
  int pid;
  extern int pidcmgr;

#ifdef HAVE_WAITPID
  while ((pid = waitpid (-1, &status, WNOHANG)) > 0)
#else
  if ((pid = (int) wait (&status)) > 0)
#endif
  {
    if (pidcmgr && pid == pidcmgr) {
      if (WIFSIGNALED(status))
        Log (0, "client manager (pid=%u) exited by signal %u", pid, WTERMSIG(status));
      else
        Log (0, "client manager (pid=%u) exited, retcode %u", pid, WEXITSTATUS(status));
      exit(4);
    }
#ifdef CHILDCOUNT
#ifdef WITH_PERL
    if (pid == perl_errpid)
      perl_errpid = 0;
    else
#endif
      CHILDCOUNT--;
#endif
    if (WIFSIGNALED(status))
      Log (2, "process %u exited by signal %u", pid, WTERMSIG(status));
    else
      Log (4, "rc(%u)=%u", pid, WEXITSTATUS(status));
  }

#ifdef SYS5SIGNALS
  signal (SIGCHLD, chld);
#endif
#ifdef EMXSIGNALS
  signal (SIGCHLD, SIG_ACK);
#endif
  errno = old_errno;
}
