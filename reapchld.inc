/*
 * Common part of SIGCHLD handling for diff.parts of binkd
 * Should be included in chld()
 */
/* 
 * $Id$
 *
 * $Log$
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
      Log (0, "client manager (pid=%u) exited, retcode %u", pid, status);
      exit(4);
    }
#ifdef CHILDCOUNT
#ifdef WITH_PERL
    if (pid != perl_errpid)
#endif
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
