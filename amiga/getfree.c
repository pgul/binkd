#include <dos/dos.h>
#include <proto/dos.h>
#include <limits.h>

extern void Log (int lev, char *s,...);

unsigned long getfree (char *path)
{
  BPTR lock;
  struct InfoData id;

  if ((lock = Lock (path, ACCESS_READ)))
  {
    if (Info (lock, &id))
    {
      if (id.id_DiskState != ID_VALIDATED) return 0;
      UnLock(lock);
      return (id.id_NumBlocks - id.id_NumBlocksUsed) * id.id_BytesPerBlock;
    }
    UnLock(lock);
  }

  Log (1, "cannot get info for \"%s\", assume enough space", path);
  return ULONG_MAX;
}
