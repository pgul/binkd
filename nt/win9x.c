/*
 *  win9x.c -- Windows 95/98/ME support for binkd
 *
 *  win9x.c is a part of binkd project
 *
 *  Copyright (C) 2002 Alexander Reznikov, homebrewer@yandex.ru (Fido 2:4600/220)
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
 * Revision 2.27  2004/02/07 14:06:06  hbrew
 * Macros: RTLDLL-->RTLSTATIC, BINKDW9X-->BINKD9X
 *
 * Revision 2.26  2004/01/04 16:55:00  stas
 * Move declarations of the 'binkd_main' into one place (nt/w32tools.h)
 *
 * Revision 2.25  2003/10/06 17:53:15  stas
 * (Prevent compiler warning.) Remove type convertion at CreateWin9xThread() call
 *
 * Revision 2.24  2003/09/11 13:04:14  hbrew
 * Undo 'move binkd9x deinit to exitfunc()' patch
 *
 * Revision 2.23  2003/09/11 12:23:25  hbrew
 * Fix 'suggest parentheses around assignment used as truth value'.
 *
 * Revision 2.22  2003/09/07 04:49:42  hbrew
 * Remove binkd9x restart-on-config-change code; move binkd9x deinit to exitfunc()
 *
 * Revision 2.21  2003/09/07 04:39:16  hbrew
 * Memory leak (binkd9x service startup)
 *
 * Revision 2.20  2003/09/07 04:37:02  hbrew
 * Close process and thread handles after CreateProcess()
 *
 * Revision 2.19  2003/09/07 04:35:15  hbrew
 * Fix old noncritical bug in binkd9x (STD_OUTPUT_HANDLE --> STD_INPUT_HANDLE)
 *
 * Revision 2.18  2003/08/30 16:38:55  gul
 * Fix compilation warnings
 *
 * Revision 2.17  2003/08/21 15:40:35  gul
 * Change building commandline for service under win32
 * (patch by Alexander Reznikov)
 *
 * Revision 2.16  2003/08/20 07:33:38  hbrew
 * Addon for 'Avoid double exitfunc() call' patch
 *
 * Revision 2.15  2003/07/19 06:59:35  hbrew
 * Complex patch:
 * * nt/w32tools.c: Fix warnings
 * * nt/w32tools.c: Fix typo in #ifdef
 * * nt/win9x.c: Fix type in #include
 * * Config.h, sys.h, branch.c, nt/service.c,
 *     nt/win9x.c, : _beginthread()-->BEGINTHREAD()
 * * binkd.c, common.h, mkfls/nt95-msvc/Makefile.dep,
 *     nt/service.c, nt/w32tools.c,nt/win9x.c: cosmitic code cleanup
 *
 * Revision 2.14  2003/07/18 10:30:34  stas
 * New functions: IsNT(), Is9x(); small code cleanup
 *
 * Revision 2.13  2003/07/18 07:48:57  hbrew
 * binkd9x: Store current dir in registry
 *
 * Revision 2.12  2003/07/17 03:08:21  hbrew
 * Fix uninstall of binkd9x service
 *
 * Revision 2.11  2003/07/17 02:41:48  hbrew
 * Compability with nt/service.c & nt/win9x.c.
 * Usage "--service" options as win9x "run-as-service" flag.
 *
 * Revision 2.10  2003/07/07 18:38:25  hbrew
 * Fix gcc(mingw) warnings:
 *
 * getopt.c:   suggest explicit braces to avoid ambiguous `else'
 * nt/win9x.c: Avoid gcc warnings about non-handled enumeration values
 *
 * Revision 2.9  2003/07/07 10:13:54  gul
 * Use getopt() for commandline parse
 *
 * Revision 2.8  2003/06/14 00:44:53  hbrew
 * Fix binkd9x -t(--all) and -u(--all) crashes
 *
 * Revision 2.7  2003/06/13 03:07:16  hbrew
 * Fix win9x-service crash (add NULL to new argv array)
 *
 * Revision 2.6  2003/06/12 00:50:09  hbrew
 * Fix MSVC compilation and logging.
 *
 * Revision 2.5  2003/06/11 09:00:44  stas
 * Don't try to install/uninstall/control service on incompatible OS. Thanks to Alexander Reznikov
 *
 * Revision 2.4  2003/05/10 00:30:37  hbrew
 * binkd9x: -u option now support '--all' service name (uninstall all services).
 * Unneeded spaces cleanup.
 *
 * Revision 2.3  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.2  2002/11/13 07:58:19  gul
 * Add CVS macros
 *
 *
 */

#ifdef BINKD9X

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <process.h>
#include <io.h>
#include "../readcfg.h"
#include "../tools.h"
#include "../common.h"
#include "win9x.h"
#include "w32tools.h"

#if !defined(ENDSESSION_LOGOFF)
#define ENDSESSION_LOGOFF    0x80000000
#endif

PHANDLER_ROUTINE phandler;  /* BOOL CALLBACK *phandler(DWORD); */
HWND mainHWND = NULL;
WNDCLASS wc;

int s_console = 0;
FILE stdout_old, stderr_old;

int w9x_service_reg = 0;

DWORD SigType = -1;

const char *Win9xWindowClassName = "binkdWin9xHandler";
const char *Win9xRegServ = "Software\\Microsoft\\Windows\\CurrentVersion\\RunServices";
#define WIN9XREGPARM_PREFIX "Software"
#define WIN9XREGPARM_SUFFIX "binkd9x"
const char *Win9xRegParm = WIN9XREGPARM_PREFIX "\\" WIN9XREGPARM_SUFFIX;
const char *Win9xRegParm_Path = "Path";
const char *Win9xServPrefix = "binkd9x-service";
const char *Win9xStartService = "--service";

#define WM_BINKD9XCOMMAND  WM_USER+50

extern enum serviceflags service_flag;
extern char *service_name;

int win9x_service_un_install(int argc, char **argv);
int win9x_service_control(void);

/* win9x service support :) :( */
typedef DWORD (WINAPI* RSPType)(DWORD, DWORD);
RSPType RegisterServiceProcess;

#if !defined(RSP_SIMPLE_SERVICE)
#define RSP_SIMPLE_SERVICE 0x00000001
/*  Registers the process as a simple service process. */
#endif
#if !defined(RSP_UNREGISTER_SERVICE)
#define RSP_UNREGISTER_SERVICE 0x00000000
/*  Unregisters the process as a service process. */
#endif

typedef struct _binkd_win9x_srvlst binkd_win9x_srvlst;
struct _binkd_win9x_srvlst
{
  int count;
  char **names;
};

int win9xExec(char *cmdline)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  BOOL rc;

  memset(&si, 0, sizeof(si));
  si.cb=sizeof(si);

  if ((rc = CreateProcess(NULL, cmdline, NULL, NULL, 0, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &si, &pi)))
  {
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
  }

  return rc;
}

void win9xAtExit(void)
{
  FreeTempConsole();
}

void win9x_extend_service_name(void)
{
  if (service_flag != w32_noservice || service_name)
  {
    if (!service_name)
      service_name = strdup(Win9xServPrefix);
    else if (!win9x_check_name_all())
    {
      char *tmp;
      int len_sn = strlen(service_name);
      int len_pr = strlen(Win9xServPrefix);

      if ((strncmp(service_name, Win9xServPrefix, len_pr)==0) &&
          ((len_sn == len_pr)||(len_sn>(len_pr+1) && service_name[len_pr] == '-')))
        return;

      tmp = (char *)malloc(len_sn+len_pr+2);
      memcpy(tmp, Win9xServPrefix, len_pr);
      tmp[len_pr] = '-';
      memcpy(tmp+len_pr+1, service_name, len_sn);
      tmp[len_pr+len_sn+1] = 0;
      free(service_name);
      service_name = tmp;
    }
  }
}

char *win9x_make_Win9xRegParm(char *srvname)
{
  char *tmp;
  int tmplen1, tmplen2;

  tmplen1 = strlen(Win9xRegParm);
  tmplen2 = strlen(srvname);
  tmp = (char *)malloc(tmplen1+tmplen2+2);  /* [Win9xRegParm] + '\\' + [srvname] + '\0' */
  memcpy(tmp, Win9xRegParm, tmplen1);
  tmp[tmplen1] = '\\';
  memcpy(tmp+tmplen1+1, srvname, tmplen2);
  tmp[tmplen1+1+tmplen2] = '\0';
  return tmp;
}

/* return -1 for normal run or any other value as exitcode */
int win9x_process(int argc, char **argv)
{
  win9x_extend_service_name();

  if (service_flag != w32_noservice && !Is9x())
  {
    if (!quiet_flag)  AllocTempConsole();
    Log((quiet_flag?0:-1), "Can't operate witn Windows 9x services: incompatible OS type%s", quiet_flag?"":"\n");
    return 1;
  }

  switch(service_flag)
  {
  case w32_queryservice:                       /* service control */
  case w32_startservice:
  case w32_stopservice:
  case w32_restartservice:
    return !win9x_service_control();

  case w32_installservice:                     /* (un)install binkd9x service */
  case w32_uninstallservice:
    return !win9x_service_un_install(argc, argv);

  case w32_noservice:                          /* Run  binkd9x */
    quiet_flag = 1;
    return -1;

  case w32_run_as_service:                     /* Run  binkd9x as service */
    {
      char *tmp, *path = NULL;
      int pathlen;
      HINSTANCE hl;
      HKEY hk;
      DWORD reg_type, size;

      tmp = win9x_make_Win9xRegParm(service_name);

      if (RegOpenKey(HKEY_LOCAL_MACHINE, tmp, &hk)==ERROR_SUCCESS)
      { /* extract current directory from registry */
        size = 0;
        if ((RegQueryValueEx(hk, Win9xRegParm_Path, NULL, &reg_type, NULL, &size)==ERROR_SUCCESS)&&(reg_type == REG_SZ))
        {
          path = (char *)malloc(size);
          if ((RegQueryValueEx(hk, Win9xRegParm_Path, NULL, &reg_type, path, &size) != ERROR_SUCCESS)||(reg_type != REG_SZ))
          {
            free(path); path = NULL;
          }
        }
        RegCloseKey(hk);
      }
      free(tmp);

      if (!path)
      { /* extract current directory from argv[0] */
        tmp = extract_filename(argv[0]);
        pathlen = tmp-argv[0];
        if (pathlen>0)
        {
          path = (char *)malloc(pathlen+1);
          memcpy(path, argv[0], pathlen);
          path[pathlen] = 0;
        }
      }

      if (path)
      {
        SetCurrentDirectory(path);
        free(path);
      }

      hl = LoadLibrary("KERNEL32.DLL");
      if (hl != NULL)
      {
        RegisterServiceProcess = (RSPType)GetProcAddress(hl, "RegisterServiceProcess");
        if (RegisterServiceProcess!=NULL)
          if (RegisterServiceProcess(0, RSP_SIMPLE_SERVICE))
            w9x_service_reg = 1;
        FreeLibrary(hl);
      }
      quiet_flag = 1;
    }
    return -1;

  default:
    Log((quiet_flag?0:-1), "Unknown service control code (%i)\n", service_flag);
    return 1;
  }

  return -1;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  atexit(win9xAtExit);

  return binkd_main(__argc, __argv, environ);
}

LRESULT CALLBACK MainWin9xWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
        switch (msg)
        {
        case WM_DESTROY:
                PostQuitMessage(0);
                break;

        case WM_CLOSE:
        	SigType = CTRL_CLOSE_EVENT;
                DestroyWindow(hWnd);
                break;

        case WM_ENDSESSION:
                if (wParam)
                {
                        if (lParam & ENDSESSION_LOGOFF)
                        {
                                if (!(service_flag == w32_run_as_service && w9x_service_reg))
                                {
                                        SigType = CTRL_LOGOFF_EVENT;
                                        DestroyWindow(hWnd);
                                }
                        }
                        else
                        {
                                SigType = CTRL_SHUTDOWN_EVENT;
                                DestroyWindow(hWnd);
                        }
                }
                break;

        case WM_BINKD9XCOMMAND:
                SigType = wParam;
                DestroyWindow(hWnd);
                break;

        default:
                return DefWindowProc(hWnd, msg, wParam, lParam);
        }

        return 0;
}

static void Win9xWindowThread(void *p)
{
        HINSTANCE hInstance = GetModuleHandle(NULL);
        MSG msg;
        phandler = (PHANDLER_ROUTINE)p;

        memset(&wc, 0, sizeof(WNDCLASS));
        wc.lpfnWndProc = (WNDPROC)MainWin9xWndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = (service_flag == w32_noservice)?Win9xWindowClassName:service_name;

        RegisterClass(&wc);

        mainHWND= CreateWindow(wc.lpszClassName, wc.lpszClassName, 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

        while(GetMessage(&msg, NULL, 0, 0))
        {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }
        mainHWND = NULL;
       	if (SigType!=-1)
       		phandler(SigType);
       	exit(0);
}

void CreateWin9xThread(PHANDLER_ROUTINE phandler)
{
        BEGINTHREAD(Win9xWindowThread, 0, phandler);
}

/*
void DestroyWin9xThread()
{
  if (mainHWND)
        SendMessage(mainHWND, WM_DESTROY, 0, 0);
  win9xAtExit();
}
*/

/* Win9x service */

int win9x_checkservice(char *srvname)
{
  HKEY hk=0;
  int r=0;

  if (RegOpenKey(HKEY_LOCAL_MACHINE, Win9xRegServ, &hk)!=ERROR_SUCCESS)
        return 0;

  if (RegQueryValueEx(hk, srvname, NULL, NULL, NULL, NULL)==ERROR_SUCCESS)
        r=1;

  RegCloseKey(hk);
  return r;
}

int win9x_service_start(char *name)
{
  HKEY hk;
  int k;
  char *cmdline;
  DWORD size, reg_type;

  if (RegOpenKey(HKEY_LOCAL_MACHINE, Win9xRegServ, &hk)!=ERROR_SUCCESS)
        return 0;

  size = 0;
  k = RegQueryValueEx(hk, name, NULL, &reg_type, NULL, &size) == ERROR_SUCCESS;

  k = k&&(reg_type == REG_SZ);

  if(k)
  {
        cmdline = (char *)malloc(size);
        k = RegQueryValueEx(hk, name, NULL, &reg_type, cmdline, &size) == ERROR_SUCCESS;

        if(k)  k = win9xExec(cmdline);
        free(cmdline);
  }

  RegCloseKey(hk);

  return k!=0;
}

int win9x_check_name_all(void)
{
  if (!service_name) return 0;
  return stricmp(service_name, "all")? 0: 1;
}

HWND win9x_service_find(char *name)
{
  return FindWindow(name, NULL);
}

/* return 1 if success, 0 if fail */
int win9x_service_control_exec(char *tmp, enum serviceflags cmd, int qflag)
{
  int rc;
  HWND hwnd;

  hwnd = win9x_service_find(tmp);

  rc = 1;

  switch (cmd)
  {
  case w32_queryservice:
    if (!qflag)  Log(-1, "\'%s\': %s\n", tmp, hwnd?"started":"stopped");
    break;
  case w32_startservice:
    if (!hwnd)
    {
      if (win9x_service_start(tmp))
      {
        if (!qflag)   Log(-1, "\'%s\': started\n", tmp);
      }
      else
      {
        if (!qflag)  Log(-1, "\'%s\': starting failed!\n", tmp);
        rc = 0;
      }
    }
    else
    {
      if (!qflag)  Log(-1, "\'%s\': already started\n", tmp);
    }
    break;
  case w32_stopservice:
    if (hwnd) SendMessage(hwnd, WM_BINKD9XCOMMAND, CTRL_SERVICESTOP_EVENT, 0);
    if (!qflag)  Log(-1, "\'%s\': %s\n", tmp, hwnd?"stopped":"already stopped");
    break;
  case w32_restartservice:
    if (hwnd)
    {
      SendMessage(hwnd, WM_BINKD9XCOMMAND, CTRL_SERVICERESTART_EVENT, 0);
      if (win9x_service_start(tmp))
      {
        if (!qflag)  Log(-1, "\'%s\': restarted\n", tmp);
      }
      else
      {
        if (!qflag)  Log(-1, "\'%s\': restarting failed!\n", tmp);
        rc = 0;
      }
    }
    else
      if (!qflag)  Log(-1, "\'%s\': already stopped\n", tmp);
    break;
  default:  /* Avoid gcc warnings about non-handled enumeration values */
    rc = 0;
  }
  return rc;
}

static int win9x_compare_strings(const void *a, const void *b)
{
  return strcmp(*(char **)a, *(char **)b);
}

binkd_win9x_srvlst *win9x_get_services_list(int sort)
{
  binkd_win9x_srvlst *srvlst;
  HKEY hk;
  char *tmp;
  int i, len, prefixlen;
  DWORD size, reg_type;

  srvlst = (binkd_win9x_srvlst *)malloc(sizeof(binkd_win9x_srvlst));
  srvlst->count = 0;
  srvlst->names = NULL;

  if (RegOpenKey(HKEY_LOCAL_MACHINE, Win9xRegServ, &hk)==ERROR_SUCCESS)
  {
    tmp = (char *)malloc(256);
    prefixlen = strlen(Win9xServPrefix);

    for (i=0, size = 256; RegEnumValue(hk, i, tmp, &size, NULL, &reg_type, NULL, NULL)==ERROR_SUCCESS; i++, size = 256)
    {
      if (reg_type != REG_SZ) continue;

      len = strlen(tmp);
      if ((len<prefixlen)||(strncmp(tmp, Win9xServPrefix, prefixlen)!=0)||((len>prefixlen)&&(tmp[prefixlen]!='-')))
        continue;

      srvlst->names = (char **)realloc(srvlst->names, (srvlst->count+1)*sizeof(char *));
      srvlst->names[srvlst->count] = strdup(tmp);
      srvlst->count++;
    }
    free(tmp);
    RegCloseKey(hk);

    if (sort && srvlst->count)
      qsort(srvlst->names, srvlst->count, sizeof(char **), win9x_compare_strings);
  }

  return srvlst;
}

void win9x_free_services_list(binkd_win9x_srvlst *srvlst)
{
  int i;

  if (srvlst == NULL)
    return;

  for(i=0; i<srvlst->count; i++)
    if (srvlst->names[i])
      free(srvlst->names[i]);

  if (srvlst->names)
    free(srvlst->names);
  free(srvlst);
}

/* return 0 if at least one service fail, return 1 otherwise */
int win9x_service_control(void)
{
  int i, rc = 1;
  char *msg;
  binkd_win9x_srvlst *srvlst;

  if (!quiet_flag)  AllocTempConsole();

  switch (service_flag)
  {
  case w32_queryservice:
    msg = "Status of binkd9x service(s):\n";
    break;
  case w32_startservice:
    msg = "Starting service(s):\n";
    break;
  case w32_stopservice:
    msg = "Stopping service(s):\n";
    break;
  case w32_restartservice:
    msg = "Restarting service(s):\n";
    break;
  default:
    msg = NULL;
    break;
  }

  if (!quiet_flag && msg)  Log(-1, msg);

  if (win9x_check_name_all())
  {
    srvlst = win9x_get_services_list(1);

    for(i=0; i<srvlst->count; i++)
      if (!win9x_service_control_exec(srvlst->names[i], service_flag, quiet_flag))
        rc = 0;

    win9x_free_services_list(srvlst);
  }
  else
    rc = win9x_service_control_exec(service_name, service_flag, quiet_flag);

  return rc;
}

/* return 1 if success, return 0 if fail */
int win9x_service_do_uninstall(char *srvname, int qflag)
{
  HKEY hk;
  DWORD hk_keys, hk_vals;
  int hk_del = 0, rc = 1;

  if (RegOpenKey(HKEY_LOCAL_MACHINE, Win9xRegServ, &hk)==ERROR_SUCCESS)
  {
    if (!RegDeleteValue(hk, srvname))  rc = 0;
    RegCloseKey(hk);
  } else rc = 0;

  if (RegOpenKey(HKEY_LOCAL_MACHINE, Win9xRegParm, &hk)==ERROR_SUCCESS)
  {
/* MSDN says:
   RegDeleteKey([...]) [...]
   Windows 95/98: The function also deletes all subkeys and values. [...]
   Windows NT/2000: The subkey to be deleted must not have subkeys. [...]
*/
    RegDeleteKey(hk, srvname);

    if ((RegQueryInfoKey(hk, NULL, NULL, NULL, &hk_keys, NULL, NULL, &hk_vals, NULL, NULL, NULL, NULL)==ERROR_SUCCESS) &&
        !hk_keys && !hk_vals)
      hk_del = 1;

    RegCloseKey(hk);

    if (hk_del && (RegOpenKey(HKEY_LOCAL_MACHINE, WIN9XREGPARM_PREFIX, &hk)==ERROR_SUCCESS))
    {
      RegDeleteKey(hk, WIN9XREGPARM_SUFFIX);
      RegCloseKey(hk);
    }
  }

  if (!qflag)  Log(-1, "\'%s\' uninstalled...\n", srvname);
  if (!win9x_service_control_exec(srvname, w32_stopservice, qflag))  rc = 0;
  return rc;
}

/* return 1 if success, return 0 if fail */
int win9x_service_un_install(int argc, char **argv)
{
  int i, j, all, rc = 1;
  HKEY hk=0;
  binkd_win9x_srvlst *srvlst;

  if (!quiet_flag)  AllocTempConsole();

  all = win9x_check_name_all();

  if (all&&(service_flag != w32_uninstallservice))
  {
    Log((quiet_flag?0:-1), "Invalid service name!%s", quiet_flag?"":"\n");
    return 0;
  }

  if (!all)
  {
    j = win9x_checkservice(service_name);
    if (service_flag == w32_uninstallservice)
      j = !j;

    if (j)
    {
      if (!quiet_flag)  Log(-1, "Service already %sinstalled...\n", service_flag==w32_installservice?"":"UN");
      return 1;
    }
  }

  if (service_flag == w32_uninstallservice)
  {
    if (all)
    {
      srvlst = win9x_get_services_list(1);

      if (!srvlst->count)
        Log(-1, "No installed services.\n");
      else
        for(i=0; i<srvlst->count; i++)
          if (!win9x_service_do_uninstall(srvlst->names[i], quiet_flag))  rc = 0;

      win9x_free_services_list(srvlst);
    }
    else
      rc = win9x_service_do_uninstall(service_name, quiet_flag);
    return rc;
  }

/* service_flag == w32_installservice */

  if (RegOpenKey(HKEY_LOCAL_MACHINE, Win9xRegServ, &hk)!=ERROR_SUCCESS)
    if (RegCreateKey(HKEY_LOCAL_MACHINE, Win9xRegServ, &hk)!=ERROR_SUCCESS)
      rc = 0;

  if (rc)
  {
    char *sp, *path, *asp, *p;
    int q = 0, q1, tmplen1, tmplen2, len = 1; /* '\0' */

    build_service_arguments(&asp, argv, 1);

    for(p=asp, tmplen1=0; *p; p++, tmplen1++)
      if (*p == ' ')  { q = 1; }

    p++; len += tmplen1+1;                    /* binkd9x path & filename + (' ') */
    if (q)  { len += 2; }

    tmplen2 = strlen(Win9xStartService);
    len += tmplen2;                           /* Win9xStartService */

    for (sp = p; *sp; sp++)
    {
      len++;
      for(q1 = 0; *sp; sp++)
      {
        len++;
        if (!q1 && *sp == ' ')  { q1 = 1; len += 2; }
      }
    }

    sp = path = (char *)malloc(len);

    if (q)  { *(sp++) = '"'; }
    memcpy(sp, asp, tmplen1); sp += tmplen1;
    if (q)  { *(sp++) = '"'; }
    *(sp++) = ' ';
    memcpy(sp, Win9xStartService, tmplen2); sp += tmplen2;

    for(; *p; p++)
    {
      *(sp++) = ' ';
      if (strchr(p, ' '))  { *(sp++) = '"'; q = 1; } else { q = 0; }
      for(;*p;p++)         { *(sp++) = *p; }
      if (q)               { *(sp++) = '"'; }
    }

    *sp = '\0';

    if (RegSetValueEx(hk, service_name, 0, REG_SZ, path, len-1) != ERROR_SUCCESS)
      rc = 0;

    free(path);
    free(asp);
    RegCloseKey(hk);

/* Store current directory */
    if (rc)
    {
      sp = win9x_make_Win9xRegParm(service_name);

      if (RegOpenKey(HKEY_LOCAL_MACHINE, sp, &hk)!=ERROR_SUCCESS)
        if (RegCreateKey(HKEY_LOCAL_MACHINE, sp, &hk)!=ERROR_SUCCESS)
          rc = 0;

      if (rc)
      {
        j = GetCurrentDirectory(0, NULL);
        path = (char *)malloc(j);
        GetCurrentDirectory(j, path);
        if (RegSetValueEx(hk, Win9xRegParm_Path, 0, REG_SZ, path, j-1) != ERROR_SUCCESS)
          rc = 0;
        free(path);

      }

      free(sp);
      RegCloseKey(hk);
      if (!rc)  win9x_service_do_uninstall(service_name, 1); /* Rollback */
    }
  }

  if (!rc)
    Log((quiet_flag?0:-1), "Unable to store data in registry...%s", quiet_flag?"":"\n");
  else
  {
    if (win9x_service_start(service_name))
    {
      if (!quiet_flag)  Log(-1, "\'%s\' installed and started...\n", service_name);
    }
    else
    {
      rc = 0;
      if (!quiet_flag)
      {
        Log(-1, "\'%s\' installed...\n", service_name);
        Log(-1, "Unable to start service!\n");
      }
    }
  }
  return rc;
}

/* TempConsole */
void AllocTempConsole(void)
{
        if (s_console)
                return;

        if (AllocConsole())
        {
                HANDLE ha = GetStdHandle(STD_OUTPUT_HANDLE);
                int hCrt = _open_osfhandle((long) ha, 0x4000);
                FILE *hf = _fdopen( hCrt, "w" );
                stdout_old = *stdout;
                *stdout = *hf;

                ha = GetStdHandle(STD_ERROR_HANDLE);
                hCrt = _open_osfhandle((long) ha, 0x4000);
                hf = _fdopen( hCrt, "w" );
                stderr_old = *stderr;
                *stderr = *hf;
                setvbuf( stdout, NULL, _IONBF, 0 );
                setvbuf( stderr, NULL, _IONBF, 0 );

                s_console = 1;
        }
}

void FreeTempConsole(void)
{
        HANDLE in;
        int hCrt;
        FILE stdin_old, *hf;

        if (!s_console)
                return;

        printf("Press any key...\n");

        in = GetStdHandle(STD_INPUT_HANDLE);
        FlushConsoleInputBuffer(in);
        hCrt = _open_osfhandle((long) in, 0x4000);
        hf = _fdopen( hCrt, "r" );
        stdin_old = *stdin;
        *stdin = *hf;
        setvbuf(stdin, NULL, _IONBF, 0 );

        getch();

        FlushConsoleInputBuffer(in);

        *stdin  = stdin_old;
        *stdout = stdout_old;
        *stderr = stderr_old;

        FreeConsole();

        s_console = 0;
}

#endif
