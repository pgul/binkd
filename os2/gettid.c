/*
 * $Id$
 *
 * Revision history:
 * $Log$
 * Revision 2.1  2003/02/28 20:39:09  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
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

int gettid(void)
{ 
  PTIB ptib;
  PPIB ppib;
 
  DosGetInfoBlocks(&ptib, &ppib);
 
  return (int) (ptib->tib_ptib2->tib2_ultid);
}

