/*
 * $Id$
 *
 * Revision history:
 * $Log$
 * Revision 2.1  2001/04/23 07:58:57  gul
 * getfree() on large drives fixed
 *
 * Revision 2.0  2001/01/10 12:12:40  gul
 * Binkd is under CVS again
 *
 *
 */
#ifdef __WATCOMC__
  #define __IBMC__ 0
  #define __IBMCPP__ 0
#endif

#define INCL_DOS
#include <os2.h>
#include <ctype.h>
#include <limits.h>

extern void Log (int lev, char *s,...);

unsigned long getfree (char *path)
{
  FSALLOCATE fsa;
  ULONG disknum = 0;
  APIRET rc;

  if (isalpha (path[0]) && path[1] == ':')
    disknum = toupper (path[0]) - 'A' + 1;

  rc = DosQueryFSInfo (disknum,		    /* Drive number            */
		       FSIL_ALLOC,	    /* Level 1 allocation info */
		       (PVOID) & fsa,	    /* Buffer                  */
		       sizeof (fsa));	    /* Size of buffer          */

  if (rc)
  {
    Log (1, "DosQueryFSInfo error: return code = %u", rc);
    return ULONG_MAX;			    /* Assume enough disk space */
  }
  else
  {
    if (fsa.cSectorUnit * fsa.cbSector >= 1024)
      return fsa.cUnitAvail * (fsa.cSectorUnit * fsa.cbSector / 1024);
    else
      return fsa.cUnitAvail * (1024 / (fsa.cSectorUnit * fsa.cbSector));
  }
}
