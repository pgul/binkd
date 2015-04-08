/*--------------------------------------------------------------------*/
/*       S e m . c                                                    */
/*                                                                    */
/*       Part of BinkD project                                        */
/*       Semaphore support (OS/2) for bsy.c module                    */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*       Copyright (c) 1996 by Fydodor Ustinov                        */
/*                             FIDONet 2:5020/79                      */
/*                                                                    */
/*  This program is  free software;  you can  redistribute it and/or  */
/*  modify it  under  the terms of the GNU General Public License as  */ 
/*  published  by the  Free Software Foundation; either version 2 of  */
/*  the License, or (at your option) any later version. See COPYING.  */
/*--------------------------------------------------------------------*/

#include "../sys.h"
#include "../readcfg.h"
#include "../tools.h"

#define INCL_DOS
#include <os2.h>

#define hmtx (*(HMTX*)vpSem)

#define hevt (*(HEV*)vpSem)

/*--------------------------------------------------------------------*/
/*    int InitSem(void *)                                             */
/*                                                                    */
/*    Initialise Mutex Semaphore.                                     */
/*--------------------------------------------------------------------*/

int _InitSem(void *vpSem) {
  ULONG rc;

  if ((rc = DosCreateMutexSem (0, &hmtx, 0, FALSE)) != 0) {
     Log (0, "DosCreateMutexSem: error 0x%lx", rc);
     return(-1);
   }
   return(0);
}

/*--------------------------------------------------------------------*/
/*    int InitEventSem(void *)                                        */
/*                                                                    */
/*    Initialise Event Semaphore.                                     */
/*--------------------------------------------------------------------*/

int _InitEventSem(void *vpSem) {
  ULONG rc;

  if ((rc = DosCreateEventSem (NULL, &hevt, 0, FALSE)) != 0) {
     Log (0, "DosCreateEventSem: error 0x%lx", rc);
     return(-1);
   }
   return(0);
}

/*--------------------------------------------------------------------*/
/*    int CleanSem(void *)                                            */
/*                                                                    */
/*    Clean Mutex Semaphore.                                          */
/*--------------------------------------------------------------------*/

int _CleanSem(void *vpSem) {
  if (hmtx)
  { DosCloseMutexSem (hmtx);
    hmtx = 0;
  }
  return (0);
}

/*--------------------------------------------------------------------*/
/*    int LockSem(void *)                                             */
/*                                                                    */
/*    Wait & lock semaphore                                           */
/*--------------------------------------------------------------------*/

int _LockSem(void *vpSem) {
  ULONG rc;

  if (hmtx == 0) return (-1);
  if ((rc = DosRequestMutexSem (hmtx, SEM_INDEFINITE_WAIT)) != 0) {
    _CleanSem (vpSem);
    Log (0, "DosRequestMutexSem retcode 0x%lx", rc);
  }
  return (0);
}

/*--------------------------------------------------------------------*/
/*    int ReleaseSem(void *)                                          */
/*                                                                    */
/*    Release Semaphore.                                              */
/*--------------------------------------------------------------------*/

int _ReleaseSem(void *vpSem) {
  if (hmtx == 0) return (-1);
  DosReleaseMutexSem (hmtx);
  return (0);
}

/*--------------------------------------------------------------------*/
/*    int PostSem(void *)                                             */
/*                                                                    */
/*    Post Event Semaphore.                                           */
/*--------------------------------------------------------------------*/

int _PostSem(void *vpSem) {
  if (hmtx == 0) return (-1);
  DosPostEventSem (hevt);
  return (0);
}

/*--------------------------------------------------------------------*/
/*    int WaitSem(void *, int)                                        */
/*                                                                    */
/*    Wait Event Semaphore.                                           */
/*--------------------------------------------------------------------*/

int _WaitSem(void *vpSem, int timeout) {
  ULONG semcount;

  if (hmtx == 0) return (-1);
  if (DosWaitEventSem (hevt, timeout * 1000ul))
    return -1;
  DosResetEventSem (hevt, &semcount);
  return 0;
}

/*--------------------------------------------------------------------*/
/*    int CleanEventSem(void *)                                       */
/*                                                                    */
/*    Clean Semaphores.                                               */
/*--------------------------------------------------------------------*/

int _CleanEventSem(void *vpSem) {
  if (hevt)
  { DosCloseEventSem (hevt);
    hevt = 0;
  }
  return 0;
}

