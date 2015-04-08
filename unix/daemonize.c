#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#include "../sys.h"
#include "../tools.h"
#include "daemonize.h"

/*									*/
/* Daemonize binkd if we know how to do it				*/
/*				 Alex Semenyaka, Andrew Kolchoogin 	*/

int already_daemonized = 0;

int binkd_daemonize(int nochdir)
{

if (already_daemonized)
	return 0;

#ifdef HAVE_DAEMON
if (daemon(nochdir, 0) == -1)
	{ Log(2, "Daemon() failed, %s", strerror(errno)); return -1; }

#elif defined(HAVE_SETSID)
if (fork() != 0) exit(0);
if (setsid() == -1)
#ifdef ultrix
	/* Sendmail wisdom has been used */
	if ((setpgrp() < 0) || (setsid() < 0))
#endif
	{ Log(2, "Setsid() failed, %s", strerror(errno)); }

freopen("/dev/null","r",stdin);
freopen("/dev/null","w",stdout);
freopen("/dev/null","w",stderr);

if (!nochdir)
	chdir("/");

#elif defined(HAVE_TIOCNOTTY)

if (fork() != 0) exit(0);
if (setpgrp() < 0)
	{ Log(2, "Setpgrp failed, %s", strerror(errno)); }

{ int fd;
  if ((fd = open("/dev/tty", O_RDWR)) >= 0)
	{ ioctl(fd, TIOCNOTTY, (char*)0);
	  close(fd);
	}
  else
	{ Log(2, "Cannot open /dev/tty, %s", strerror(errno)); }
}

if (!nochdir)
	chdir("/");

freopen("/dev/null","r",stdin);
freopen("/dev/null","w",stdout);
freopen("/dev/null","w",stderr);

#endif /* HAVE_TIOCNOTTY, HAVE_SETSID, HAVE_DAEMON */

/* BinkD is either daemonized here or OS does not support known methods to */
/* do it.								   */

already_daemonized = 1;

return 0;
}

