/*
 *  https.h -- header for https & socks4/5 proxy
 *
 *  https.h is an addition part of binkd project
 *
 *  Copyright (C) 1996-1998  Dima Maloff, 5047/13
 *  Copyright (C) 1998-2000  Dima Afanasiev, 5020/463
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

int h_connect(int *socket, struct sockaddr_in *sin);

#if !defined(WIN32)
#include <sys/time.h>
#endif
