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
 * Revision 2.4.2.5  2003/09/12 09:44:57  hbrew
 * Fix old noncritical bug in binkd9x (STD_OUTPUT_HANDLE --> STD_INPUT_HANDLE)
 *
 * Revision 2.4.2.4  2003/08/28 06:29:51  hbrew
 * Update binkd9x for compability with binkd 0.9.6 commandline; put binkd9x.txt from current
 *
 * Revision 2.4.2.3  2003/07/07 01:01:13  hbrew
 * Remove unused parameter 'type' from win9x_service_control()
 *
 * Revision 2.4.2.2  2003/06/17 15:48:01  stas
 * Prevent service operations on incompatible OS (NT and 9x)
 *
 * Revision 2.4.2.1  2003/06/14 00:47:36  hbrew
 * Add NULL to new argv array.
 * Fix binkd9x -t(--all) and -u(--all) crashes
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

#ifdef BINKDW9X

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <process.h>
#include <io.h>
#include "../Config.h"
#include "../tools.h"
#include "win9x.h"

#if !defined(ENDSESSION_LOGOFF)
#define ENDSESSION_LOGOFF    0x80000000
#endif

PHANDLER_ROUTINE phandler;
HWND mainHWND = NULL;
WNDCLASS wc;

int s_console = 0;
FILE stdout_old, stderr_old;

enum serviceflags{
   w32_noservice=0,
   w32_installservice=1,
   w32_uninstallservice=-1,
   w32_startservice=2,
   w32_stopservice=-2,
   w32_restartservice=3,
   w32_queryservice=4,
   w32_run_as_service=-4
 };

enum serviceflags w9x_service = w32_noservice;

int w9x_service_reg = 0;
int s_quiet = 0;
char *srvname = NULL;
DWORD SigType = -1;

char *srvparm = NULL;

const char *Win9xWindowClassName = "binkdWin9xHandler";
const char *Win9xRegServ = "Software\\Microsoft\\Windows\\CurrentVersion\\RunServices";
const char *Win9xServPrefix = "binkd9x-service";
const char *Win9xStartService = "--service";

#define WM_BINKD9XCOMMAND  WM_USER+50

extern int checkcfg_flag;
extern int quiet_flag;

int binkd_main(int argc, char **argv, char **envp);
int win9x_service_cmdline(int argc, char **argv, char **envp);
void win9x_service_un_install(int argc, char **argv, char **envp);
void win9x_service_control(void);
int win9x_check_name_all(void);

int W32_CheckOS(unsigned long PlatformId); /* see TCPErr.c */

/* win9x service support :) :( */
typedef DWORD (WINAPI* RSPType)(DWORD, DWORD);
RSPType RegisterServiceProcess;
#if 0
DWORD WINAPI RegisterServiceProcess(DWORD dwProcessId,  /* process identifier */
                                    DWORD dwServiceType); /* type of service */
#endif
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

  memset(&si, 0, sizeof(si));
  si.cb=sizeof(si);

  return CreateProcess(NULL, cmdline, NULL, NULL, 0, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &si, &pi);
}

void win9xAtExit(void)
{
  if (srvname)
  {
        free(srvname);
        srvname = NULL;
  }

  FreeTempConsole();
  if (checkcfg_flag==2)
        win9xExec(GetCommandLine());
}

void win9x_extend_service_name(void)
{
  if (w9x_service != w32_noservice || srvname)
  {
    if (!srvname)
      srvname = strdup(Win9xServPrefix);
    else if (!win9x_check_name_all())
    {
      char *tmp;
      int len_sn = strlen(srvname);
      int len_pr = strlen(Win9xServPrefix);

      if ((strncmp(srvname, Win9xServPrefix, len_pr)==0) &&
          ((len_sn == len_pr)||(len_sn>(len_pr+1) && srvname[len_pr] == '-')))
        return;

      tmp = (char *)malloc(len_sn+len_pr+2);
      memcpy(tmp, Win9xServPrefix, len_pr);
      tmp[len_pr] = '-';
      memcpy(tmp+len_pr+1, srvname, len_sn);
      tmp[len_pr+len_sn+1] = 0;
      free(srvname);
      srvname = tmp;
    }
  }
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  int r;
  char *sp, *tmp = NULL;
  HINSTANCE hl;

  atexit(win9xAtExit);

  r = win9x_service_cmdline(__argc, __argv, environ);

  if (w9x_service != w32_installservice && srvparm)
  {
    free(srvparm); srvparm = NULL;
  }

  if (!r)
    return 1;

  if (w9x_service == w32_noservice)
    return binkd_main(__argc, __argv, environ);
  else
    win9x_extend_service_name();

  if( W32_CheckOS(VER_PLATFORM_WIN32_WINDOWS) )
  {
    if (!s_quiet)  AllocTempConsole();
    Log((s_quiet?0:-1), "Can't operate witn Windows 9x services: incompatible OS type%s", s_quiet?"":"\n");
    return 1;
  }

  if ((w9x_service == w32_installservice)||(w9x_service == w32_uninstallservice))
  {
    win9x_service_un_install(__argc, __argv, environ);
    if (srvparm)
    {
      free(srvparm); srvparm = NULL;
    }
    return 0;
  }

  if (w9x_service == w32_startservice   ||
      w9x_service == w32_stopservice    ||
      w9x_service == w32_restartservice ||
      w9x_service == w32_queryservice)
  {
    win9x_service_control();
    return 0;
  }

/* Running as Win9x service (r == 1) */
  for (sp = __argv[0]+strlen(__argv[0])-1;sp>__argv[0];sp--)
    if ((sp[0] == '\\')||(sp[0] == '/'))
    {
      sp--;
      if (sp>__argv[0])
      {
        tmp = (char *)malloc(sp-__argv[0]+2);
        memcpy(tmp, __argv[0], sp-__argv[0]+1);
        tmp[sp-__argv[0]+1] = 0;
      }
      break;
    }

  if (tmp)
  {
    SetCurrentDirectory(tmp);
    free(tmp);
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
                                if (!(w9x_service && w9x_service_reg))
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
        wc.lpszClassName = (w9x_service==0)?Win9xWindowClassName:srvname;

        RegisterClass(&wc);

        mainHWND= CreateWindow(wc.lpszClassName, wc.lpszClassName, 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

        while(GetMessage(&msg, NULL, 0, 0))
        {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }
        mainHWND = NULL;
        win9xAtExit();
       	if (SigType!=-1)
       		phandler(SigType);
}

void CreateWin9xThread(PHANDLER_ROUTINE phandler)
{
        _beginthread(Win9xWindowThread, 0, phandler);
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

int win9x_service_cmdline(int argc, char **argv, char **envp)
{
  int i, j, skip, r, len, opt;
  char *p, *s, *t;

  if (argc == 1)
    return 1;

  len = 1;

  for(i=1; i<argc; i++)
    len += strlen(argv[i])+1;

  srvparm = t = (char *)malloc(len);

  for(i=1; i<argc; i++)
  {
    j = i;
    p = argv[i];
    opt = 0;

    if (p[0] == '-')
    {
      if (!strcmp(p, Win9xStartService))
      {
        w9x_service = w32_run_as_service;
        continue;
      }

      skip = 0;
      for(++p;*p && !skip;p++)
      {
        r = 0;
        s = NULL;
        switch(*p)
        {
        case 'q':
          s_quiet = 1;
          break;
        case 'i':
          w9x_service = w32_installservice;
          r = 1;
          break;
        case 'u':
          w9x_service = w32_uninstallservice;
          r = 1;
          break;
        case 'S':
          if (*(p+1))
            s = p+1;
          else
          {
            if (++i < argc)
              s = argv[i];
            else
            {
              Log(0, "Parameter required after '-S' option (service name)\n");
              return 0;
            }
          }
          if (srvname)
            free(srvname);
          srvname = strdup(s);
          skip = 1;
          break;
        case 't':
          if (*(p+1))
            s = p+1;
          else
          {
            if (++i < argc)
              s = argv[i];
            else
            {
              Log(0, "Parameter required after '-t' option (service command)\n");
              return 0;
            }
          }
          if (!strcmp(s, "start")) { w9x_service = w32_startservice; }
          else
            if (!strcmp(s, "stop")) { w9x_service = w32_stopservice; }
            else
              if (!strcmp(s, "restart")) { w9x_service = w32_restartservice; }
              else
                if (!strcmp(s, "status")) { w9x_service =  w32_queryservice; }
                else
                {
                  Log(0, "Unknown command '-t %s'\n", s);
                  return 0;
                }
          skip = 1;
          r = 1;
          break;
        }
        if (!r)
        {
          if (!opt)
          {
            opt = 1;
            *t++ = '-';
          }
          *t++ = *p;
          if (s)
          {
            if (i != j)      { *t++ = '\0'; }
            for (; *s; s++)  { *t++ = *s; }
          }
        }
      }
      if (opt)
        *t++ = '\0';
    }
    else
    {
      for (; *p; p++)  { *t++ = *p; }
      *t++ = '\0';
    }
  }
  *t = '\0';
  return 1;
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
  if (!srvname) return 0;
  return stricmp(srvname, "all")? 0: 1;
}

HWND win9x_service_find(char *name)
{
  return FindWindow(name, NULL);
}

void win9x_service_control_exec(char *tmp, enum serviceflags cmd)
{
  HWND hwnd;

  hwnd = win9x_service_find(tmp);
  switch (cmd)
  {
  case w32_queryservice:
    if (!s_quiet)  Log(-1, "\'%s\': %s\n", tmp, hwnd?"started":"stopped");
    break;
  case w32_startservice:
    if (!hwnd)
    {
        if (win9x_service_start(tmp))
        {
                if (!s_quiet)   Log(-1, "\'%s\': started\n", tmp);
        }
        else
        {
                if (!s_quiet)  Log(-1, "\'%s\': starting failed!\n", tmp);
        }
    }
    else
    {
        if (!s_quiet)  Log(-1, "\'%s\': already started\n", tmp);
    }
    break;
  case w32_stopservice:
    if (hwnd) SendMessage(hwnd, WM_BINKD9XCOMMAND, 254, 0);
    if (!s_quiet)  Log(-1, "\'%s\': %s\n", tmp, hwnd?"stopped":"already stopped");
    break;
  case w32_restartservice:
    if (hwnd)
    {
        SendMessage(hwnd, WM_BINKD9XCOMMAND, 255, 0);
        if (win9x_service_start(tmp))
        {
                if (!s_quiet)  Log(-1, "\'%s\': restarted\n", tmp);
        }
        else
        {
                if (!s_quiet)  Log(-1, "\'%s\': restarting failed!\n", tmp);
        }
    }
    else
        if (!s_quiet)  Log(-1, "\'%s\': already stopped\n", tmp);
    break;
  default:  /* Avoid gcc warnings about non-handled enumeration values */
    break;
  }
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

void win9x_service_control(void)
{
  int i;
  char *msg;
  binkd_win9x_srvlst *srvlst;

  if (!s_quiet)  AllocTempConsole();

  switch (w9x_service)
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
                win9x_service_control_exec(srvlst->names[i], w9x_service);

        win9x_free_services_list(srvlst);
  }
  else
        win9x_service_control_exec(srvname, w9x_service);
}

void win9x_service_do_uninstall(char *srvname)
{
  HKEY hk;

  if (RegOpenKey(HKEY_LOCAL_MACHINE, Win9xRegServ, &hk)==ERROR_SUCCESS)
  {
    RegDeleteValue(hk, srvname);
    RegCloseKey(hk);
  }

  if (!s_quiet)  Log(-1, "\'%s\' uninstalled...\n", srvname);
  win9x_service_control_exec(srvname, w32_stopservice);
}

void win9x_service_un_install(int argc, char **argv, char **envp)
{
  int i, j, k, all, q, q1, tmplen1, tmplen2, len = 0;
  char *sp, *tmp, *path, *p;
  HKEY hk=0;
  binkd_win9x_srvlst *srvlst;

  if (!s_quiet)  AllocTempConsole();

  all = win9x_check_name_all();

  if (all&&(w9x_service != w32_uninstallservice))
  {
    Log((s_quiet?0:-1), "Invalid service name!%s", s_quiet?"":"\n");
    return;
  }

  if (!all)
  {
    j = win9x_checkservice(srvname);
    if (w9x_service == w32_uninstallservice)
      j = !j;

    if (j)
    {
      if (!s_quiet)  Log(-1, "Service already %sinstalled...\n", w9x_service==w32_installservice?"":"UN");
      return;
    }
  }

  if (w9x_service == w32_uninstallservice)
  {
    if (all)
    {
      srvlst = win9x_get_services_list(1);

      if (!srvlst->count)
        Log(-1, "No installed services.\n");
      else
        for(i=0; i<srvlst->count; i++)
          win9x_service_do_uninstall(srvlst->names[i]);

      win9x_free_services_list(srvlst);
    }
    else
      win9x_service_do_uninstall(srvname);
    return;
  }

/* service_flag == w32_installservice */

  k = 1;
  if (RegOpenKey(HKEY_LOCAL_MACHINE, Win9xRegServ, &hk)!=ERROR_SUCCESS)
    if (RegCreateKey(HKEY_LOCAL_MACHINE, Win9xRegServ, &hk)!=ERROR_SUCCESS)
      k = 0;
  if (k)
  {
    j = GetCurrentDirectory(0, NULL);
    p = (char *)malloc(j);
    GetCurrentDirectory(j, p);

    sp = argv[0]+strlen(argv[0])-1;
    for (;sp>argv[0];sp--)
      if ((sp[0] == '\\')||(sp[0] == '/'))
      {
        sp++;
        break;
      }

    q = 0;
    if (strchr(p, ' '))        { q = 1; }
    else
      if (strchr(sp, ' '))     { q = 1; }

    tmplen1 = strlen(sp);
    tmplen2 = strlen(Win9xStartService);

    len = j+tmplen1+tmplen2+2; /* {path(without '\0' (j-1))}+'\\'+{filename{argv[0]}}+' '+{--service}+...+'\0' */

    if (srvparm && *srvparm)
    {
      for (tmp = srvparm; *tmp; tmp++)
      {
        len++;
        for(q1 = 0; *tmp; tmp++)
        {
          len++;
          if (!q1 && *tmp == ' ')  { q1 = 1; len += 2; }
        }
      }
    }

    path = tmp = (char *)malloc(len);
    if (q) { *tmp++ = '"'; }
    memcpy(tmp, p, j-1); tmp += j-1;
    free(p);
    *tmp++ = '\\';
    memcpy(tmp, sp, tmplen1); tmp += tmplen1;
    if (q) { *tmp++ = '"'; }
    *tmp++ = ' ';
    memcpy(tmp, Win9xStartService, tmplen2); tmp += tmplen2;

    if (srvparm && *srvparm)
    {
      for(sp = srvparm; *sp; sp++)
      {
        *tmp++ = ' ';
        if (strchr(sp, ' '))  { *tmp++ = '"'; q = 1; } else { q = 0; }
        for(;*sp;sp++)        { *tmp++ = *sp; }
        if (q)                { *tmp++ = '"'; }
      }
    }

    *tmp = '\0';

    k = RegSetValueEx(hk, srvname, 0, REG_SZ, path, strlen(path)) == ERROR_SUCCESS;

    free(path);
    RegCloseKey(hk);
  }

  if (!k)
        Log((s_quiet?0:-1), "Unable to store data in registry...%s", s_quiet?"":"\n");
  else
  {
        if (win9x_service_start(srvname))
        {
                if (!s_quiet)  Log(-1, "\'%s\' installed and started...\n", srvname);
        }
        else
        {
                if (!s_quiet)
                {
                        Log(-1, "\'%s\' installed...\n", srvname);
                        Log(-1, "Unable to start service!\n");
                }
        }
  }

  return;
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
