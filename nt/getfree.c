/*--------------------------------------------------------------------*/
/*       G e t f r e e . c                                            */
/*                                                                    */
/*       Part of BinkD project                                        */
/*       Get free space at selected path (UNC supported)              */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*       Copyright (c) 1997 by Fydodor Ustinov                        */
/*                             FIDONet 2:5020/79                      */
/*                                                                    */
/*  This program is  free software;  you can  redistribute it and/or  */
/*  modify it  under  the terms of the GNU General Public License as  */ 
/*  published  by the  Free Software Foundation; either version 2 of  */
/*  the License, or (at your option) any later version. See COPYING.  */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                          RCS Information                           */
/*--------------------------------------------------------------------*/

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
 * Revision 0.01  1997/01/08  09:00:25 ufm
 *      First revision
 *
 */

 static const char rcsid[] =
      "$Id$";

/*--------------------------------------------------------------------*/
/*                        System include files                        */
/*--------------------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>
#include <limits.h>

/*--------------------------------------------------------------------*/
/*                        Local include files                         */
/*--------------------------------------------------------------------*/

#include "..\tools.h"
#include "..\config.h"

/*--------------------------------------------------------------------*/
/*                         Global definitions                         */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                          Global variables                          */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                           Local variables                          */
/*--------------------------------------------------------------------*/


unsigned long getfree (char *path) {
char RPN[MAXPATHLEN];	// root path 
char *pRPN;             // Pointer to Root path
DWORD SPC;				// sectors per cluster 
DWORD BPS;				// bytes per sector 
DWORD FC;				// number of free clusters  
DWORD TNC;				// total number of clusters  
BOOL rc;

  pRPN = RPN;
  if (isalpha(path[0]) && path[1] == ':' ) {
	  // Drive letter
	  RPN[0] = path[0];
	  RPN[1] = ':';
	  RPN[2] = '\\';
	  RPN[3] = '\0';
  } else if (path[0] == '\\' && path[1] == '\\') {
	  // UNC path
	  int i;
      RPN[0] = '\\';
	  RPN[1] = '\\';
	  i = 2;
	  // copy server name....
      do {
		  RPN[i] = path[i];
	  } while (path[i++] != '\\');
      // .... and share name
      do {
		  RPN[i] = path[i];
	  } while (path[i++] != '\\');
      
      RPN[i] = '\0';

  } else {
	  // Current Drive
	  pRPN = NULL;
  }
  rc = GetDiskFreeSpace(pRPN,&SPC,&BPS,&FC,&TNC);
  if (rc != TRUE) {
    Log (1, "GetDiskFreeSpace error: return code = %lu", GetLastError());
    return ULONG_MAX;		    /* Assume enough disk space */
  } else {
    if (BPS * SPC >= 1024)
      return (unsigned long) ((BPS * SPC / 1024l) * SPC * FC);
    else
      return (unsigned long) (FC / (1024 / (BPS * SPC)));
  }
}
