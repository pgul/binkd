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

/*--------------------------------------------------------------------*/
/*                          RCS Information                           */
/*--------------------------------------------------------------------*/

/*
 *    $Id$
 *
 *    Revision history:
 *    $Log$
 *    Revision 1.1.1.1  2001/01/10 11:35:00  gul
 *    BinkD sources are under CVS again
 *
 *    Revision 1.2  1996/11/05 04:06:06  mff
 *    Added support for multiple semaphores
 *
 *
 * Revision 0.01  1996/12/04  14:52:58  ufm
 *      First revision
 *
 */

/*--------------------------------------------------------------------*/
/*                        System include files                        */
/*--------------------------------------------------------------------*/

#ifdef __WATCOMC__
  #define __IBMC__ 0
  #define __IBMCPP__ 0
#endif

#define INCL_DOS
#include <os2.h>

/*--------------------------------------------------------------------*/
/*                        Local include files                         */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                         Global definitions                         */
/*--------------------------------------------------------------------*/

#define hmtx (*(HMTX*)vpSem)

/*--------------------------------------------------------------------*/
/*                          Global variables                          */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                           Local variables                          */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                   Global functions prototypes                      */
/*--------------------------------------------------------------------*/

extern void Log (int lev, char *s,...);

/*--------------------------------------------------------------------*/
/*    int InitSem(void)                                               */
/*                                                                    */
/*    Initialise Semaphores.                                          */
/*--------------------------------------------------------------------*/

int _InitSem(void *vpSem) {

  if (DosCreateMutexSem (0, &hmtx, 0, FALSE)) {
     Log (0, "DosCreateMutexSem: error");
     return(-1);
   }
   return(0);
}

/*--------------------------------------------------------------------*/
/*    int CleanSem(void)                                              */
/*                                                                    */
/*    Clean Semaphores.                                               */
/*--------------------------------------------------------------------*/

int _CleanSem(void *vpSem) {
  DosCloseMutexSem (hmtx);
  return (0);
}

/*--------------------------------------------------------------------*/
/*    int LockSem(void)                                               */
/*                                                                    */
/*    Wait & lock semaphore                                           */
/*--------------------------------------------------------------------*/

int _LockSem(void *vpSem) {
  DosRequestMutexSem (hmtx, SEM_INDEFINITE_WAIT);
  return (0);
}

/*--------------------------------------------------------------------*/
/*    int ReleaseSem(void)                                            */
/*                                                                    */
/*    Release Semaphore.                                              */
/*--------------------------------------------------------------------*/

int _ReleaseSem(void *vpSem) {
  DosReleaseMutexSem (hmtx);
  return (0);
}

