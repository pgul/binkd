/*
 *  setpttl.c -- Dummy setproctitle() and initsetproctitle()
 *
 *  setpttl.c is a part of binkd project
 *
 *  Copyright (C) 1998  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#include "setpttl.h"

#ifdef __WATCOMC__
#pragma off(unreferenced)
#endif

void initsetproctitle (int argc, char **argv, char **envp)
{
}

void setproctitle (const char *fmt,...)
{
}
