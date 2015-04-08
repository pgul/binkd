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

#include <stdlib.h>
#include <string.h>
#include "sys.h"
#include "tools.h"

void *xalloc (size_t size)
{
  void *p = malloc (size);

  if (!p)
    Log (0, "Not enough memory (failed to allocate %lu byte(s))",
	 (unsigned long) size);
  memset(p, 0xEE, size);
  return p;
}

void *xrealloc (void *ptr, size_t size)
{
  void *p = realloc (ptr, size);

  if (!p)
    Log (0, "Not enough memory (failed to realloc %p to %lu byte(s))",
	 ptr, (unsigned long) size);
  return p;
}

void *xstrdup (const char *str)
{
  void *p = xalloc (strlen (str) + 1);

  strcpy (p, str);
  return p;
}

void *xstrcat (char **str, const char *s2)
{
  int n = strlen (*str);

  *str = (char *)xrealloc (*str, n + strlen (s2) + 1);
  strcpy (*str + n, s2);
  return *str;
}

void xfree(void *p)
{
  if (p)
    free(p);
}
