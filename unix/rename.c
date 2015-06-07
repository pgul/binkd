#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

extern void Log (int lev, char *s,...);

int o_rename (const char *from, const char *to)
{
  int h, saved_errno;

  if (link (from, to) == 0)
  {
    unlink(from);
    return 0;
  }
  if (errno != EPERM && errno != EACCES
#ifdef EOPNOTSUPP
      && errno != EOPNOTSUPP
#endif
#ifdef LINK_OPNOTSUPP_ERRNO
      && errno != LINK_OPNOTSUPP_ERRNO
#endif
    )
    return -1;
  /* If filesystem does not support hardlinks, perform non-destructive rename another way */
  /* It's not completely clean and atomic, b/c in rare cases tosser can get zero-size file
   * between open() and rename(), and even data can be lost if tosser process this zero-size file,
   * then some tool create data file with the same name, and then that file will be overrided
   * by rename().
   * But it's only theoretical cases, and we have no way for true atomic non-destructive rename
   * in this case.
   */
  if ((h = open (to, O_CREAT | O_EXCL, 0666)) == -1)
    return -1;
  close (h);
  if (rename (from, to) == -1)
  {
    saved_errno = errno;
    unlink (to);
    errno = saved_errno;
    return -1;
  }
  return 0;
}
