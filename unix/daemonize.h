/*
 * $Id$
 *
 * Revision history:
 * $Log$
 * Revision 2.1  2003/03/01 17:33:25  gul
 * Clean daemonize code
 *
 * Revision 2.1  2001/01/15 22:04:52  gul
 * Added -D switch (run as daemon)
 *
 */
#if defined(HAVE_DAEMON) || defined(HAVE_SETSID) || defined(HAVE_TIOCNOTTY)

#define BINKD_DAEMONIZE

int binkd_daemonize(int);

#endif
