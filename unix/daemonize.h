#if defined(HAVE_DAEMON) || defined(HAVE_SETSID) || defined(HAVE_TIOCNOTTY)

#define BINKD_DAEMONIZE

int binkd_daemonize(int);

#endif
