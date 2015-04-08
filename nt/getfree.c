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
/*                        System include files                        */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <limits.h>
#include <ctype.h>

/*--------------------------------------------------------------------*/
/*                        Local include files                         */
/*--------------------------------------------------------------------*/

#include "../sys.h"
#include "../iphdr.h"
#include "../readcfg.h"
#include "../tools.h"
#include "w32tools.h"

/*--------------------------------------------------------------------*/
/*                         Global definitions                         */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                          Global variables                          */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                           Local variables                          */
/*--------------------------------------------------------------------*/

typedef  BOOL (WINAPI* PGFDE)( LPCTSTR, PULARGE_INTEGER,
                              PULARGE_INTEGER, PULARGE_INTEGER );


unsigned long getfree (char *path) {
 { /*  w95OSR2, wNT, ... */
  PGFDE pGetDiskFreeSpaceEx;
  ULARGE_INTEGER i64FreeBytesToCaller, i64TotalBytes, i64FreeBytes;

  pGetDiskFreeSpaceEx = (PGFDE)GetProcAddress( GetModuleHandle("kernel32.dll"),
                           "GetDiskFreeSpaceExA");

  if (pGetDiskFreeSpaceEx)
  {
    if( pGetDiskFreeSpaceEx (path,
                (PULARGE_INTEGER)&i64FreeBytesToCaller,
                (PULARGE_INTEGER)&i64TotalBytes,
                (PULARGE_INTEGER)&i64FreeBytes)
      ) {

      if( i64FreeBytesToCaller.u.HighPart > 1023 ) {
        Log( 6, "GetDiskFreeSpaceEx() returns too big value (%lu*2^32) and getfree() is return ULONG_MAX (%lu)",i64FreeBytesToCaller.u.HighPart,ULONG_MAX);
        return ULONG_MAX;
      }
      return (unsigned long)(i64FreeBytesToCaller.QuadPart>>10 & ULONG_MAX);
    }
  }
 }

 { /* w95 4.0.950 */
  char RPN[MAXPATHLEN];	/* root path  */
  char *pRPN;             /* Pointer to Root path */
  DWORD SPC;				/* sectors per cluster */
  DWORD BPS;				/* bytes per sector    */
  DWORD FC;				/* number of free clusters  */
  DWORD TNC;				/* total number of clusters */
  BOOL rc;

  pRPN = RPN;
  if (isalpha(path[0]) && path[1] == ':' ) {
	  /* Drive letter */
	  RPN[0] = path[0];
	  RPN[1] = ':';
	  RPN[2] = '\\';
	  RPN[3] = '\0';
  } else if (path[0] == '\\' && path[1] == '\\') {
	  /* UNC path */
	  int i;
      RPN[0] = '\\';
	  RPN[1] = '\\';
	  i = 2;
	  /* copy server name.... */
      do {
		  RPN[i] = path[i];
	  } while (path[i++] != '\\');
      /* .... and share name */
      do {
		  RPN[i] = path[i];
	  } while (path[i++] != '\\');
      
      RPN[i] = '\0';

  } else {
	  /* Current Drive */
	  pRPN = NULL;
  }
  rc = GetDiskFreeSpace(pRPN,&SPC,&BPS,&FC,&TNC);
  if (rc != TRUE) {
    Log (1, "GetDiskFreeSpace error: return code = %lu", GetLastError());
    return ULONG_MAX;		    /* Assume enough disk space */
  } else {
    if (BPS * SPC >= 1024)
      return (unsigned long) ((BPS * SPC / 1024l) * FC);
    else
      return (unsigned long) (FC / (1024 / (BPS * SPC)));
  }
 }
}
