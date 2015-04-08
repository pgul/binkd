/*
 *  run.h -- Run external programs
 *
 *  run.h is a part of binkd project
 *
 *  Copyright (C) 1996-1997  Dima Maloff, 5047/13
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

#ifndef _run_h
#define _run_h

int run (char *);
int run3 (const char *cmd, int *in, int *out, int *err);

#endif
