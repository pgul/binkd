/*
 *  xalloc.c -- wrappers for malloc(), realloc(), strdup()
 *
 *  xalloc.c is a part of binkd project
 *
 *  Copyright (C) 1998  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

/*
 * $Id$
 *
 * $Log$
 * Revision 1.1  2001/01/10 11:34:59  gul
 * Initial revision
 *
 * Revision 1.1  1998/05/08  03:37:28  mff
 * Initial revision
 *
 */
#include <stdlib.h>
#include <string.h>

#include "tools.h"

void *xalloc (size_t size)
{
  void *p = malloc (size);

  if (!p)
    Log (0, "Not enough memory (failed to allocate %lu byte(s))",
	 (unsigned long) size);
  return p;
}

void *xrealloc (void *ptr, size_t size)
{
  if ((ptr = realloc (ptr, size)) == NULL)
    Log (0, "Not enough memory (failed to realloc %p to %lu byte(s))",
	 ptr, (unsigned long) size);
  return ptr;
}

void *xstrdup (const char *str)
{
  void *p = strdup (str);

  if (!p)
    Log (0, "Not enough memory (failed to strdup %lu byte(s) at %p)",
	 (unsigned long) strlen (str), str);
  return p;
}
