/*
   This was taken from ifmail, and modified a bit for binkd -- mff, 1997

   Copyright (c) 1993-1995 Eugene G. Crosser

   ifcico is a FidoNet(r) compatible mailer for U*IX platforms.

   You may do virtually what you wish with this software, as long as the
   explicit reference to its original author is retained:

   Eugene G. Crosser <crosser@pccross.msk.su>, 2:5020/230@FidoNet

   THIS SOFTWARE IS PROVIDED AS IS AND COME WITH NO WARRANTY OF ANY KIND,
   EITHER EXPRESSED OR IMPLIED.  IN NO EVENT WILL THE COPYRIGHT HOLDER BE
   LIABLE FOR ANY DAMAGES RESULTING FROM THE USE OF THIS SOFTWARE.
 */
#include <sys/types.h>
#ifdef HAVE_SYS_VFS_H
#include <sys/vfs.h>
#endif
#ifdef HAVE_SYS_STATFS_H
#include <sys/statfs.h>
#endif
#ifdef HAVE_SYS_STATVFS_H
#include <sys/statvfs.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_SYS_MOUNT_H
#include <sys/mount.h>
#endif

#include <limits.h>

extern void Log (int lev, char *s,...);

#if defined(HAVE_STATFS) || defined(HAVE_STATVFS)
unsigned long getfree (char *path)
{
#ifdef HAVE_STATVFS
  struct statvfs sfs;

  if (statvfs (path, &sfs) != 0)
#else
  struct statfs sfs;

#ifdef SCO_STYLE_STATFS
  if (statfs (path, &sfs, sizeof (sfs), 0) != 0)
#else
  if (statfs (path, &sfs) != 0)
#endif
#endif
  {
    Log (1, "cannot statfs \"%s\", assume enough space", path);
    return ULONG_MAX;
  }
  else
    /* return (sfs.f_bsize * sfs.f_bfree); */
    return (sfs.f_bsize * sfs.f_bavail);
}

#else
unsigned long getfree (char *path)
{
  return ULONG_MAX;
}

#endif /* defined(HAVE_STATFS) | defined(HAVE_STATVFS) */
