/*--------------------------------------------------------------------*/
/*       w32tools.c                                                   */
/*                                                                    */
/*       Part of BinkD project                                        */
/*       Win32 specific functions                                     */
/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*  Copyright (c) 2003 by Stas Degteff                                */
/*                g@grumbler.org 2:5080/102@fidonet                   */
/*                                                                    */
/*  This program is  free software;  you can  redistribute it and/or  */
/*  modify it  under  the terms of the GNU General Public License as  */
/*  published  by the  Free Software Foundation; either version 2 of  */
/*  the License, or (at your option) any later version. See COPYING.  */
/*--------------------------------------------------------------------*/

/*
 * $Id$
 *
 * Revision history:
 * $Log$
 * Revision 2.13  2004/01/03 18:39:28  stas
 * Improve service identification
 *
 * Revision 2.12  2004/01/03 12:17:44  stas
 * Implement full icon support (winNT/2k/XP)
 *
 * Revision 2.11  2004/01/02 21:20:17  stas
 * GetMainWindow(): function retrieves the window handle used by the main window of application
 *
 * Revision 2.10  2003/10/18 18:50:48  stas
 * Move to new 'tray.c' file several functions when is related with 'minimize to tray' feature
 *
 * Revision 2.9  2003/10/18 17:02:29  stas
 * Don't set '-S name' option to NT service parameters list in registry
 *
 * Revision 2.8  2003/10/06 17:16:47  stas
 * (Cosmetics) Rename tcperr() to w32err() for win32/win9x versions
 *
 * Revision 2.7  2003/08/26 22:18:49  gul
 * Fix compilation under w32-mingw and os2-emx
 *
 * Revision 2.6  2003/08/21 15:40:34  gul
 * Change building commandline for service under win32
 * (patch by Alexander Reznikov)
 *
 * Revision 2.5  2003/08/04 12:23:40  gul
 * Add CVS tags
 *
 */

#include <stdlib.h>
#include <windows.h>
#include "../readcfg.h"
#include "../common.h"
#include "../tools.h"
#include "service.h"
#ifdef HAVE_GETOPT
#include <unistd.h>
#else
#include "../getopt.h"
#endif

extern const char *optstring; /* binkd.c */
extern enum serviceflags service_flag; /* binkd.c */


/* Windows version test
 * Parameter: Platform ID (VER_PLATFORM_WIN32_NT, VER_PLATFORM_WIN32_WINDOWS
 *            or other, see GetVersionEx() if MSDN)
 * Return 0 if match OS, not zero (usually -1) if do not match OS,
 * return 1 if can't retrieve OS version info.
 */
int W32_CheckOS(unsigned long PlatformId)
{ OSVERSIONINFO os_ver;
  static int first=1;
  static unsigned long os_id;

  if(first)
  {
    os_ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if( !GetVersionEx(&os_ver) )
      return 1;
    os_id=os_ver.dwPlatformId;
    first = 0;
  }

  return os_id != PlatformId;
}

/* Test on Windows 95/98/Me
   Return 0 if not match
*/
int Is9x(){
return W32_CheckOS(VER_PLATFORM_WIN32_WINDOWS)==0;
}

/* Test on Windows NT/2000/XP/2003
   Return 0 if not match
*/
int IsNT(){
return W32_CheckOS(VER_PLATFORM_WIN32_NT)==0;
}

/* Return service name.
   Parameter is "service display name"
   Based on get_service_name() from Apache sources (c) The Apache Software Foundation.
*/
char *get_service_name(char *display_name)
{
  /* Get the service's true name from the SCM on NT/2000/XP/2003, since it
   * can be changed by the user from the service control panel.  We can't
   * trust the service name to match a space-collapsed display name.
   */
  char *srv_name=NULL, *cp;

  if (IsNT())
  {
    char service_name[MAX_PATH];
    DWORD namelen = sizeof(service_name);
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (scm) {
        BOOL ok = GetServiceKeyName(scm, display_name, service_name,
                                    &namelen);
        CloseServiceHandle(scm);
        if (ok)
            return strdup(service_name);
    }

    srv_name = strdup(display_name);
    for(cp=srv_name; *cp; cp++)
    {
       if(*cp==' ') *cp='_';
    }
  }
  else
  { /* Windows 9x services: 'service name' is a registry key name (any char allowed) */
    srv_name = strdup(display_name);
  }
  return srv_name;
}

/* build_service_arguments helper */
int build_service_copystr(char **dst, char *src, int copynul)
{
  int len = 0;

  for (; *src; src++)  { *(*dst)++ = *src; len++; }
  if (copynul)         { *(*dst)++ = '\0'; len++; }
  return len;
}

/* Build service arguments list
 * Parameters:  asp       - new arguments list
 *              argv      - old arguments list in *argv[] format
 *              use_argv0 - use argv[0] or GetModuleFileName()
 * Function add two '\0' to indicate end of array.
 * Return asp size.
 */
int build_service_arguments(char **asp, char *argv[], int use_argv0)
{
  char pathname[MAXPATHLEN], *p;
  DWORD pathlen;
  int i, curind, oldind, n_opt, n_nul;
  int argc = 0, len = 0, l = 1;

  if (!use_argv0)
  {
    if (!(pathlen = GetModuleFileName(NULL, pathname, MAXPATHLEN)))
      Log(0, "Error in GetModuleFileName()=%s\n", w32err(GetLastError()) );
    l += pathlen+1;
    argc++;
  }

  for (i = use_argv0? 0: 1; argv[i]; i++)
  {
    l += strlen(argv[i]) + 1;
    argc++;
  }

  *asp = p = (char *)malloc(l);

  if (use_argv0)  len += build_service_copystr(&p, argv[0], 1);
  else            len += build_service_copystr(&p, pathname, 1);

  curind = optind = 1; /* restart getopt */
  init_getopt();
  oldind = 0;
  n_opt = n_nul = 0;

  while((i = getopt(argc, argv, optstring)) != -1)
  {
    if (curind != oldind)
      n_opt = 1;

    if (i != 'i' && i != 'u' && i != 't' && (Is9x() || i != 'S'))
    {
      if (n_opt)
      {
        *p++ = '-'; len++;
        n_opt = 0;
        n_nul = 1;
      }

      *p++ = i; len++;

      if (optarg)
      {
        if (optind != (curind+1))  { *p++ = '\0'; len++; }
        len += build_service_copystr(&p, optarg, 0);
      }
    }

    oldind = curind;
    curind = optind;

    if (n_nul && curind != oldind)
    {
      *p++ = '\0'; len++;
      n_nul = 0;
    }
  }

  for (; optind<argc; optind++)
    len += build_service_copystr(&p, argv[optind], 1);

  *p = '\0';
  return ++len;
}

/**************************************************************************
 * Determine if we're running as a service. Return 0 if binkd running not *
 * as a service. Universal: any 32-bit version of Windows.                *
 *                                                                        *
 * Windows NT/2000/XP/2003: a hack to determine if we're running          *
 * as a service without waiting for the SCM to fail.                      *
 * (Idea taken from Apache sources)                                       *
 * Windows 9x/Me: service indicated via undocumented command line option  *
 */
int isService()
{ static int _isService=-1;

  if (_isService != -1)
    return _isService;

  if (!IsNT())
  {
    _isService = (service_flag == w32_run_as_service);
  }
  else if (!AllocConsole())
  {
    _isService = 0;
  }
  else
  {
    FreeConsole();
    _isService = 1;
    service_flag = w32_run_as_service;
  }

  return _isService;
}

/* The prototype of the GetConsoleWindow() function is not declared in
   wincon.h that was included with the Platform SDK for Windows 2000.
   GetConsoleWindow() is not exists in Windows NT and 9x  */
typedef HWND (WINAPI* GCW)(VOID);

/**************************************************************************
 * The GetMainWindow function retrieves the window handle used by the main
 * window.
 */
HWND GetMainWindow(void)
{ static HWND wh=NULL;
  GCW pGetConsoleWindow;

  if(wh) return wh;

  pGetConsoleWindow = (GCW) GetProcAddress( GetModuleHandle("kernel32.dll"),
                           "GetConsoleWindow");
  if(pGetConsoleWindow) /* Windows 2000 and above */
  {
    wh = pGetConsoleWindow();
  }
  else /* Windows NT and Windows 9x: searching for window is needed */
  {
    DWORD i;
    char buf[160], bn[21];

    i = GetConsoleTitle(buf, sizeof(buf)); /* don't detect code page, may be need? */
    buf[i] = 0;
    snprintf( bn, sizeof(bn), "%lx", (unsigned long)GetCurrentThreadId() );
    SetConsoleTitle(bn);
    wh = FindWindow(NULL, bn);
    SetConsoleTitle(buf);
  }

  if(!IsWindow(wh)) wh=NULL;  /* detached or NT service */

  return wh;
}

static HICON save_icon_big=NULL, save_icon_small=NULL;
static HWND mwnd;
MUTEXSEM iconsem;

/* Load the icon
 */
HICON LoadBinkdIcon(void)
{
  static HICON hi=NULL;

  LockSem(&iconsem);

  if(hi)
  {
    ReleaseSem(&iconsem);
    return hi;
  }

  if (!(mwnd=GetMainWindow()))
  {
    ReleaseSem(&iconsem);
    return NULL;
  }

  /* Save icon of window */
  save_icon_small = (HICON)SendMessage(mwnd, WM_GETICON, ICON_SMALL, 0);
  save_icon_big = (HICON)SendMessage(mwnd, WM_GETICON, ICON_BIG, 0);

  /* Load icon from file */
  hi = LoadImage( NULL, BINKD_ICON_FILE, IMAGE_ICON, 0, 0,
                  LR_SHARED | LR_LOADFROMFILE | LR_LOADTRANSPARENT );
  if(hi)
    Log(12,"Icon for systray is loaded from %s", BINKD_ICON_FILE);

  /* Load icon from resource */
  if (!hi)
  { HMODULE hModule;
    if( (hModule = GetModuleHandle(NULL)) )
      hi = LoadImage( hModule, MAKEINTRESOURCE(0), IMAGE_ICON,
                      0, 0, LR_SHARED | LR_LOADTRANSPARENT);
  }

  /* Load standard icon "?" */
  if (!hi)
  {
      hi = LoadIcon(NULL, IDI_INFORMATION);
  }

  /* Set icon of window */
  SendMessage(mwnd, WM_SETICON, ICON_SMALL, (LPARAM)hi);
  SendMessage(mwnd, WM_SETICON, ICON_BIG,   (LPARAM)hi);

  ReleaseSem(&iconsem);
  return hi;
}

/* Unload the icon
 */
void UnloadBinkdIcon(void)
{
  /* Restore icon of window */
  if(save_icon_small)
    SendMessage(mwnd, WM_SETICON, ICON_SMALL, (LPARAM)save_icon_small);
  if(save_icon_big)
    SendMessage(mwnd, WM_SETICON, ICON_BIG,   (LPARAM)save_icon_big);
}
