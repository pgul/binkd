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

#include <stdlib.h>
#include <windows.h>
#include "../Config.h"
#include "../common.h"
#include "../tools.h"
#include "service.h"

extern char *configpath;


/* Windows version test
 * Parameter: Platform ID (VER_PLATFORM_WIN32_NT, VER_PLATFORM_WIN32_WINDOWS
 *            or other, see GetVersionEx() if MSDN)
 * Return 0 if match OS, not zero (usually -1) if do not match OS,
 * return 1 if can't retrieve OS version info.
 */
int W32_CheckOS(unsigned long PlatformId)
{ OSVERSIONINFO os_ver;
  static int first=1;
  static int os_id;

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


/* Build service arguments list
 * Parameters:  asp - new arguments list
 *              sep - arguments separator char:
 * Set sep to '\0' to build argv, function add two '\0' to indicate end of array.
 * Return asp size.
 */
int build_service_arguments(char **asp, char sep)
{ int len,size;
  char pathname[MAXPATHLEN];

  if(GetModuleFileName(NULL, pathname, MAXPATHLEN)<1)
  {
    Log(0, "Error in GetModuleFileName()=%s\n", tcperr(GetLastError()) );
  }

  size = strlen(pathname) + strlen(service_name) + strlen(configpath) +2 +11 +3*11; /* max: 11 options */
  *asp=(char*)malloc(size);
  memset(*asp,sep,size-1);
  (*asp)[size-1]='\0';
  len = sprintf(*asp, "%s%c-S%c\"%s\"", pathname, sep, sep, service_name);
  (*asp)[len++]=sep;
#ifdef BINKD9X
    memcpy(*asp+len,"-Z",2);
    len+=3;
#else
  if(tray_flag){
    memcpy(*asp+len,"-T",2);
    len+=3;
  }
#endif
  if(server_flag){
    memcpy(*asp+len,"-s",2);
    len+=3;
  }
  if(client_flag){
    memcpy(*asp+len,"-c",2);
    len+=3;
  }
  if(poll_flag){
    memcpy(*asp+len,"-p",2);
    len+=3;
  }
  if(quiet_flag){
    memcpy(*asp+len,"-q",2);
    len+=3;
  }
  if(verbose_flag){
    memcpy(*asp+len,"-v",2);
    len+=3;
  }
  if(checkcfg_flag){
    memcpy(*asp+len,"-C",2);
    len+=3;
  }
  if(no_MD5){
    memcpy(*asp+len,"-m",2);
    len+=3;
  }
  if(no_crypt){
    memcpy(*asp+len,"-r",2);
    len+=3;
  }
  if(configpath){
    strcpy(*asp+len,configpath);
    len+=strlen(configpath)+1;
  }
  return ++len;
}
