#include <dos/dos.h>
#include <proto/dos.h>

int o_rename(char *from, char *to)
{
  if (Rename((STRPTR)from, (STRPTR)to))	/* cross-volume move won't work */
    return -1;
  else
    return 0;
}
