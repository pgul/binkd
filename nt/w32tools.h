/*
 *  w32tools.h -- Windows NT specific functions declarations
 *
 *  w32tools.h is a part of binkd project
 *
 *  Copyright (c) 2003 by Stas Degteff g@grumbler.org 2:5080/102@fidonet
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. See COPYING.
 */

/*
 * $Id$
 *
 * Revision history:
 * $Log$
 * Revision 2.4  2003/08/21 15:40:35  gul
 * Change building commandline for service under win32
 * (patch by Alexander Reznikov)
 *
 * Revision 2.3  2003/08/04 12:23:40  gul
 * Add CVS tags
 *
 */

/* Test on Windows 95/98/Me
   Return 0 if not match
   (This is call to W32_CheckOS())
*/
int Is9x();

/* Test on Windows NT/2000/XP/2003
   Return 0 if not match
   (This is call to W32_CheckOS())
*/
int IsNT();

/* Windows version test
 * Parameter: Platform ID (VER_PLATFORM_WIN32_NT, VER_PLATFORM_WIN32_WINDOWS
 *            or other, see GetVersionEx() if MSDN)
 * Return 0 if match OS, not zero (usually -1) if do not match OS,
 * return 1 if can't retrieve OS version info.
 */
int W32_CheckOS(unsigned long PlatformId);

/* Return service name (retrieve from installed service or replace spaces with underscores)
   Parameter is "service display name"
   Based on get_service_name() from Apache sources (c) The Apache Software Foundation.
*/
char *get_service_name(char *display_name);

/* Build service arguments list
 * Parameters:  asp       - new arguments list
 *              argv      - old arguments list in *argv[] format
 *              use_argv0 - use argv[0] or GetModuleFileName()
 * Function add two '\0' to indicate end of array.
 * Return asp size.
 */
int build_service_arguments(char **asp, char *argv[], int use_argv0);
