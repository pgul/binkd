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
/*
 * $Id$
 *
 * $Log$
 * Revision 2.4  2005/09/23 13:32:46  gul
 * Bugfix in work via proxy with authorization
 *
 * Revision 2.3  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.2  2003/03/01 18:16:04  gul
 * Use HAVE_SYS_TIME_H macro
 *
 * Revision 2.1  2003/02/22 11:45:41  gul
 * Do not resolve hosts if proxy or socks5 using
 *
 * Revision 2.0  2001/01/10 12:12:38  gul
 * Binkd is under CVS again
 *
 *
 */

int h_connect(int socket, char *host, BINKD_CONFIG *config, char *proxy, char *socks);

