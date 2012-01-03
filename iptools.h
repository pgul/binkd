/*
 *  iptools.h -- Some useful TCP/IP utils
 *
 *  iptools.h is a part of binkd project
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
 * Revision 2.9  2012/01/03 17:52:32  green
 * Implement FSP-1035 (SRV record usage)
 * - add SRV enabled getaddrinfo() wrapper (srv_gai.[ch])
 * - Unix (libresolv, autodetected) and Win32 support implemented
 * - Port information is stored as string now, i.e. may be service name
 *
 * Revision 2.8  2012/01/03 17:25:32  green
 * Implemented IPv6 support
 * - replace (almost) all getXbyY function calls with getaddrinfo/getnameinfo (RFC2553) calls
 * - Add compatibility layer for target systems not supporting RFC2553 calls in rfc2553.[ch]
 * - Add support for multiple listen sockets -- one for IPv4 and one for IPv6 (use V6ONLY)
 * - For WIN32 platform add configuration parameter IPV6 (mutually exclusive with BINKD9X)
 * - On WIN32 platform use Winsock2 API if IPV6 support is requested
 * - config: node IP address literal + port supported: [<ipv6 address>]:<port>
 *
 * Revision 2.7  2003/10/29 21:08:39  gul
 * Change include-files structure, relax dependences
 *
 * Revision 2.6  2003/08/26 16:06:26  stream
 * Reload configuration on-the fly.
 *
 * Warning! Lot of code can be broken (Perl for sure).
 * Compilation checked only under OS/2-Watcom and NT-MSVC (without Perl)
 *
 * Revision 2.5  2003/03/26 10:44:40  gul
 * Code cleanup
 *
 * Revision 2.4  2003/03/25 21:09:04  gul
 * Memory leak
 *
 * Revision 2.3  2003/03/25 20:37:46  gul
 * free_hostent() function
 *
 * Revision 2.2  2003/02/28 08:53:38  gul
 * Fixed proxy usage
 *
 * Revision 2.1  2003/02/22 11:45:41  gul
 * Do not resolve hosts if proxy or socks5 using
 *
 * Revision 2.0  2001/01/10 12:12:38  gul
 * Binkd is under CVS again
 *
 * Revision 1.2  1997/10/23  04:00:57  mff
 * +find_port()
 *
 * Revision 1.1  1997/03/28  06:52:14  mff
 * Initial revision
 *
 */

/*
 * Sets non-blocking mode for a given socket
 */
void setsockopts (SOCKET s);

/*
 * Find the port number (in the host byte order) by a port number string or
 * a service name. Find_port ("") will return binkp's port from
 * /etc/services or even (if there is no binkp entry) 24554.
 * Returns 0 on error.
 */
char * find_port (char *s);

/*
 * address family agnostic comparison functions
 */
extern int sockaddr_cmp_addr(const struct sockaddr *, const struct sockaddr *);
extern int sockaddr_cmp_port(const struct sockaddr *, const struct sockaddr *);
