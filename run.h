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

/*
 * $Id$
 *
 * $Log$
 * Revision 2.3  2012/09/22 19:19:37  gul
 * Compilation under mingw
 *
 * Revision 2.2  2012/09/20 12:16:54  gul
 * Added "call via external pipe" (for example ssh) functionality.
 * Added "-a", "-f" options, removed obsoleted "-u" and "-i" (for win32).
 *
 * Revision 2.1  2001/10/27 08:07:18  gul
 * run and run_args returns exit code of calling process
 *
 * Revision 2.0  2001/01/10 12:12:39  gul
 * Binkd is under CVS again
 *
 * Revision 1.1  1997/03/28  06:16:56  mff
 * Initial revision
 *
 */
#ifndef _run_h
#define _run_h

int run (char *);
int run3 (const char *cmd, int *in, int *out, int *err);

#endif
