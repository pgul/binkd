/*
 *  Amiga semaphores
 */
#include <exec/exec.h>
#include <proto/exec.h>
#include <inline/strsup.h>

extern void Log (int lev, char *s,...);


int _InitSem(void *vpSem) {
   memset(vpSem, 0, sizeof (struct SignalSemaphore));
   InitSemaphore ((struct SignalSemaphore*)vpSem);
   return(0);
}

int _CleanSem(void *vpSem) {
  return (0);
}

int _LockSem(void *vpSem) {
  ObtainSemaphore ((struct SignalSemaphore *)vpSem);
  return (0);
}

int _ReleaseSem(void *vpSem) {
  ReleaseSemaphore ((struct SignalSemaphore *)vpSem);
  return (0);
}
