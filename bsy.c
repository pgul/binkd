/*
 * You must be VERY CAREFUL with this module. Note, this
 * code is working in VERY diff. ways in forking vs. threading versions!!
 */

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "sys.h"
#include "readcfg.h"
#include "bsy.h"
#include "ftnaddr.h"
#include "ftndom.h"
#include "sem.h"
#include "tools.h"
#include "iphdr.h"
#include "assert.h"
#include "readdir.h" /* for rmdir() */

#if defined(HAVE_THREADS) || defined(AMIGA)
static MUTEXSEM sem;	/* =0 initializer fails for amiga. removed. useless anyway? */
#endif

typedef struct _BSY_ADDR BSY_ADDR;
struct _BSY_ADDR
{
  BSY_ADDR *next;
  FTN_ADDR fa;
  bsy_t bt;
#ifndef UNIX
  int h;
#endif
};

BSY_ADDR *bsy_list = 0;

void bsy_init (void)
{
  InitSem (&sem);
}

void bsy_deinit (void)
{
  CleanSem (&sem);
}

static BSY_ADDR *bsy_get_free_cell (void)
{
  BSY_ADDR *lst;

  for (lst = bsy_list; lst; lst = lst->next)
    if (FA_ISNULL (&lst->fa))
      break;
  if (!lst)
  {
    lst = xalloc (sizeof (BSY_ADDR));
    FA_ZERO (&lst->fa);
    lst->next = bsy_list;
    bsy_list = lst;
  }
  return lst;
}

int bsy_add (FTN_ADDR *fa0, bsy_t bt, BINKD_CONFIG *config)
{
  char buf[MAXPATHLEN + 1];
  int ok = 0;

  ftnaddress_to_filename (buf, fa0, config);

  LockSem (&sem);
  if (*buf)
  {
    strnzcat (buf, bt == F_CSY ? ".csy" : ".bsy", sizeof (buf));
    if (mkpath (buf) == -1)
      Log (1, "mkpath('%s'): %s", buf, strerror (errno));

    if (create_sem_file (buf, 5))
    {
      BSY_ADDR *new_bsy = bsy_get_free_cell ();

      memcpy (&new_bsy->fa, fa0, sizeof (FTN_ADDR));

      new_bsy->bt = bt;

#ifndef UNIX
      new_bsy->h = open(buf, O_RDONLY|O_NOINHERIT);
      if (new_bsy->h == -1)
        Log (2, "Can't open %s: %s!", buf, strerror(errno));
#if defined(OS2)
      else
        DosSetFHState(new_bsy->h, OPEN_FLAGS_NOINHERIT);
#elif defined(EMX)
      else
        fcntl(new_bsy->h,  F_SETFD, FD_CLOEXEC);
#endif
#endif

      ok = 1;
    }
  }
  ReleaseSem (&sem);
  return ok;
}

/*
 * Test a busy-flag. 1 -- free, 0 -- busy
 */
int bsy_test (FTN_ADDR *fa0, bsy_t bt, BINKD_CONFIG *config)
{
  char buf[MAXPATHLEN + 1];

  ftnaddress_to_filename (buf, fa0, config);
  if (*buf)
  {
    strnzcat (buf, bt == F_CSY ? ".csy" : ".bsy", sizeof (buf));

    if (mkpath (buf) == -1)
      Log (1, "mkpath('%s'): %s", buf, strerror (errno));

    if (access (buf, F_OK) == -1)
      return 1;
  }
  return 0;
}

void bsy_remove (FTN_ADDR *fa0, bsy_t bt, BINKD_CONFIG *config)
{
  char buf[MAXPATHLEN + 1], *p;
  BSY_ADDR *bsy;

  ftnaddress_to_filename (buf, fa0, config);
  if (*buf)
  {
    strnzcat (buf, bt == F_CSY ? ".csy" : ".bsy", sizeof (buf));

    LockSem (&sem);
    for (bsy = bsy_list; bsy; bsy = bsy->next)
    {
      if (!ftnaddress_cmp (&bsy->fa, fa0) && bsy->bt == bt)
      {
#ifndef UNIX
	if (bsy->h != -1)
	  if (close(bsy->h))
            Log (2, "Can't close %s (handle %d): %s!", buf, bsy->h, strerror(errno));
#endif
	delete (buf);
	/* remove empty point directory */
	if (config->deletedirs)
	{
	  FTN_DOMAIN *d;
	  if (fa0->p != 0 && (p = last_slash(buf)) != NULL)
	  {
	    *p = '\0';
	    rmdir(buf);
	  }
	  /* remove empty zone directory */
	  d = get_domain_info (fa0->domain, config->pDomains.first);
	  if (d && (fa0->z != d->z[0]) && (p = last_slash(buf)) != NULL)
	  {
	    *p = '\0';
	    rmdir(buf);
	  }
	}
	FA_ZERO (&bsy->fa);
	break;
      }
    }
    ReleaseSem (&sem);
  }
}

/*
 * For exitlist...
 */
void bsy_remove_all (BINKD_CONFIG *config)
{
  char buf[MAXPATHLEN + 1], *p;
  BSY_ADDR *bsy;

  for (bsy = bsy_list; bsy; bsy = bsy->next)
  {
    if (FA_ISNULL (&bsy->fa)) continue; /* free cell */
    ftnaddress_to_filename (buf, &bsy->fa, config);
    if (*buf)
    {
      strnzcat (buf, bsy->bt == F_CSY ? ".csy" : ".bsy", sizeof (buf));
#ifndef UNIX
      if (bsy->h != -1)
        if (close(bsy->h))
          Log (2, "Can't close %s (handle %d): %s!", buf, bsy->h, strerror(errno));
#endif
      delete (buf);
      /* remove empty point directory */
      if (config->deletedirs && bsy->fa.p != 0 && (p = last_slash(buf)) != NULL)
      {
	*p = '\0';
	rmdir(buf);
      }

      FA_ZERO (&bsy->fa);
    }
  }
  Log (6, "bsy_remove_all: done");
  bsy_deinit ();
}

/*
 * Touchs all our .bsy's if needed
 */
void bsy_touch (BINKD_CONFIG *config)
{
  static time_t last_touch = 0;

  LockSem (&sem);
  if (time (0) - last_touch > BSY_TOUCH_DELAY)
  {
    BSY_ADDR *bsy;
    char buf[MAXPATHLEN + 1];

    for (bsy = bsy_list; bsy; bsy = bsy->next)
    {
      ftnaddress_to_filename (buf, &bsy->fa, config);
      if (*buf)
      {
	strnzcat (buf, bsy->bt == F_CSY ? ".csy" : ".bsy", sizeof (buf));
	if (touch (buf, time (0)) == -1)
          Log (1, "touch %s: %s", buf, strerror (errno));
	else
	  Log (6, "touched %s", buf);
      }
    }
    last_touch = time (0);
  }
  ReleaseSem (&sem);
}
