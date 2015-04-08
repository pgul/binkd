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
