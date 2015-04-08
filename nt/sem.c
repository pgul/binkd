/*--------------------------------------------------------------------*/
/*       S e m . c                                                    */
/*                                                                    */
/*       Part of BinkD project                                        */
/*       Semaphore support (NT) for bsy.c module                      */
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
/*                        System include files                        */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                        Local include files                         */
/*--------------------------------------------------------------------*/

#include "../sys.h"
#include "../iphdr.h"
#include "../readcfg.h"
#include "../tools.h"

/*--------------------------------------------------------------------*/
/*                         Global definitions                         */
/*--------------------------------------------------------------------*/

#define BsySem (*(HANDLE*)vpSem)

#define EvtSem (*(HANDLE*)vpSem)

/*--------------------------------------------------------------------*/
/*    int InitSem(void)                                               */
/*                                                                    */
/*    Initialise Semaphores.                                          */
/*--------------------------------------------------------------------*/

int _InitSem(void *vpSem) {

   BsySem = CreateMutex(NULL,FALSE,NULL);
   if (BsySem == NULL) {
      Log(0,"Unable to create Mutex object");
      return (-1);
   }
   return(0);
}

/*--------------------------------------------------------------------*/
/*    int CleanSem(void *)                                            */
/*                                                                    */
/*    Clean Semaphores.                                               */
/*--------------------------------------------------------------------*/

int _CleanSem(void *vpSem) {
   if (BsySem) {
      CloseHandle(BsySem);
      BsySem = 0;
   }
   return(0);
}

/*--------------------------------------------------------------------*/
/*    int LockSem(void *)                                             */
/*                                                                    */
/*    Wait & lock semaphore                                           */
/*--------------------------------------------------------------------*/

int _LockSem(void *vpSem) {
   unsigned long errcode;

   if (BsySem == 0) return (-1);
   if (WaitForSingleObject(BsySem,INFINITE) == WAIT_FAILED) {
       errcode = GetLastError();
       _CleanSem(vpSem);
       Log(0, "Sem.c: WaitForSingleObject failed. Error code : %lx", errcode);
       return (-1);
   }
   return(0);
}

/*--------------------------------------------------------------------*/
/*    int ReleaseSem(void *)                                          */
/*                                                                    */
/*    Release Semaphore.                                              */
/*--------------------------------------------------------------------*/

int _ReleaseSem(void *vpSem) {
   
   if (BsySem == 0) return (-1);
   ReleaseMutex(BsySem);
   return(0);
}

/*--------------------------------------------------------------------*/
/*    int InitEventSem(void *)                                        */
/*                                                                    */
/*    Initialise Event Semaphores.                                    */
/*--------------------------------------------------------------------*/

int _InitEventSem(void *vpSem) {

   EvtSem = CreateEvent(NULL,FALSE,FALSE,NULL);
   if (EvtSem == NULL) {
      Log(0,"Unable to create Event object");
      return (-1);
   }
   return(0);
}

/*--------------------------------------------------------------------*/
/*    int PostSem(void *)                                             */
/*                                                                    */
/*    Post Event Semaphores.                                          */
/*--------------------------------------------------------------------*/

int _PostSem(void *vpSem) {
   if (EvtSem == 0) return (-1);
   SetEvent(EvtSem);
   return(0);
}

/*--------------------------------------------------------------------*/
/*    int WaitSem(void *, int)                                        */
/*                                                                    */
/*    Wait Event Semaphores.                                          */
/*--------------------------------------------------------------------*/

int _WaitSem(void *vpSem, int timeout) {
   if (EvtSem == 0) return (-1);
   if (WaitForSingleObject(EvtSem, timeout * 1000l) == WAIT_TIMEOUT)
      return -1;
   return(0);
}

/*--------------------------------------------------------------------*/
/*    int CleanEventSem(void *)                                       */
/*                                                                    */
/*    Clean Event Semaphores.                                         */
/*--------------------------------------------------------------------*/

int _CleanEventSem(void *vpSem) {
   if (EvtSem) {
      CloseHandle(EvtSem);
      EvtSem = 0;
   }
   return(0);
}

