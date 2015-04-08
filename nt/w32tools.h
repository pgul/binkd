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

#ifndef __W32TOOLS_H
#define __W32TOOLS_H

#define AlertWin(text) MessageBox(NULL, text, MYNAME, MB_OK|MB_ICONSTOP|0x00200000L|MB_SYSTEMMODAL|MB_SETFOREGROUND)

/* Declaration of main binkd function (see binkd.c) */
int binkd_main(int argc, char **argv);

#define IsNTService (IsNT()&&isService())
#define Is9xService (Is9x()&&isService())

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


/**************************************************************************
 * Determine if we're running as a service. Return 0 if binkd running not *
 * as a service. Universal: any 32-bit version of Windows.                *
 *                                                                        *
 * Windows NT/2000/XP/2003: a hack to determine if we're running          *
 * as a service without waiting for the SCM to fail.                      *
 * (Idea taken from Apache sources)                                       *
 * Windows 9x/Me: service indicated via undocumented command line option  *
 */
int isService();

/**************************************************************************
 * The GetMainWindow function retrieves the window handle used by the main
 * window.
 */
HWND GetMainWindow(void);

/* Load the icon
 */
HICON LoadBinkdIcon(void);

/* Unload the icon
 */
void UnloadBinkdIcon(void);

/* Startup actions for win32 and win9x versions of binkd
 */
void w32Init(void);

#endif
