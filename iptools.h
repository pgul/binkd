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

void copy_hostent(struct hostent *dest, struct hostent *src);

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
