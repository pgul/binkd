#ifdef __WATCOMC__
  #define __IBMC__ 0
  #define __IBMCPP__ 0
#endif

#include <dos.h>
#include <ctype.h>
#include <limits.h>

extern void Log (int lev, char *s,...);

unsigned long getfree (char *path)
{
  struct diskfree_t fsa;
  unsigned disknum = 0;
  int rc;

  if (isalpha (path[0]) && path[1] == ':')
    disknum = toupper (path[0]) - 'A' + 1;

  rc=_dos_getdiskfree(disknum,&fsa);

  if (rc)
  {
    Log (1, "_dos_gwtdiskfree error: return code = %u", rc);
    return ULONG_MAX;			    /* Assume enough disk space */
  }
  else
  {
    if (fsa.sectors_per_cluster * fsa.bytes_per_sector >= 1024)
      return (unsigned long)fsa.avail_clusters * (fsa.sectors_per_cluster * fsa.bytes_per_sector / 1024);
    else
      return (unsigned long)fsa.avail_clusters / (1024 / (fsa.sectors_per_cluster * fsa.bytes_per_sector));
  }
}
