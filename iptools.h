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
 * Finds ASCIIZ address
 */
const char *get_hostname (struct sockaddr_in * addr, char *host, int len);

#ifdef HAVE_THREADS
struct hostent *copy_hostent(struct hostent *dest, struct hostent *src);

void free_hostent(struct hostent *hp);
#else
#define free_hostent(hp)
#define copy_hostent(dest, src) (src)
#endif

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
int find_port (char *s);

/*
 *  * Find the host IP address list by a domain name or IP address string.
 *   * Returns NULL on error.
 *    */
struct hostent *find_host(char *host, struct hostent *he, struct in_addr *defaddr);
