/*
 *  assert.h -- ...
 *
 *  assert.h is a part of binkd project
 *
 *  Copyright (C) 1997  Dima Maloff, 5047/13
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
 * Revision 1.1.1.1  2001/01/10 11:34:57  gul
 * BinkD sources are under CVS again
 *
 * Revision 1.1  1997/03/28  06:56:22  mff
 * Initial revision
 *
 */
#ifndef _assert_h
#define _assert_h

#include "tools.h"

#if defined (NDEBUG)
  #define assert(exp) ((void)0)
#else
#if defined (EMX)
  #define assert(exp) if ((exp)==0) \
           { int hcore=open("c:\\binkd.core",O_BINARY|O_RDWR|O_CREAT,0600); \
             if (hcore!=-1) _core(hcore), close(hcore); \
             Log (0, "%s: %i: %s: assertion failed", __FILE__, __LINE__, #exp); \
             abort(); }
#else
  #define assert(exp) ((exp) ? (void)0 : \
          Log (0, "%s: %i: %s: assertion failed", __FILE__, __LINE__, #exp))
#endif
#endif

#endif

