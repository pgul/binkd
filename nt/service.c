/*
 *  service.c -- Windows NT services support for binkd
 *
 *  service.c is a part of binkd project
 *
 *  Copyright (C) 2000 Dima Afanasiev, da@4u.net (Fido 2:5020/463)
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
 * Revision 2.33  2003/10/09 17:14:01  stas
 * Load tray icon from file "binkd.ico"
 *
 * Revision 2.32  2003/10/09 09:41:07  stas
 * Change service stop sequence
 *
 * Revision 2.31  2003/10/07 18:03:18  stas
 * Fix error with MS VC. Thanks to Serguei Trouchelle <stro@isd.dp.ua>
 *
 * Revision 2.30  2003/10/07 14:41:04  stas
 * Fix NT service shutdown
 *
 * Revision 2.29  2003/10/06 18:59:58  stas
 * Prevent double calls of ReportStatusToSCMgr(SERVICE_STOPPED,...) and double restart service
 *
 * Revision 2.28  2003/10/06 17:42:27  stas
 * (Prevent compiler warning.) Remove type convertion at SetConsoleCtrlHandler() call
 *
 * Revision 2.27  2003/10/06 17:16:47  stas
 * (Cosmetics) Rename tcperr() to w32err() for win32/win9x versions
 *
 * Revision 2.26  2003/10/06 17:03:38  stas
 * Fix logic of checkservice()
 *
 * Revision 2.24  2003/10/06 16:54:51  stas
 * (Prevent warnings.) Prepare to implement full service control
 *
 * Revision 2.23  2003/10/06 16:47:28  stas
 * Use enumeration in parameter and return values of service_main()
 *
 * Revision 2.22  2003/10/05 10:01:15  stas
 * Remove unused code
 *
 * Revision 2.21  2003/10/05 09:37:43  stas
 * Optimize binkd/nt start: use hack to determine if we're running as a service without waiting for the service control manager to fail
 *
 * Revision 2.20  2003/10/05 07:37:47  stas
 * Fix NT service exit (don't hang service on receive CTRL_SERVICESTOP_EVENT)
 *
 * Revision 2.19  2003/10/05 04:59:11  stas
 * Fix service handler function definition; get service name from OS
 *
 * Revision 2.18  2003/08/26 22:18:49  gul
 * Fix compilation under w32-mingw and os2-emx
 *
 * Revision 2.17  2003/08/21 15:40:34  gul
 * Change building commandline for service under win32
 * (patch by Alexander Reznikov)
 *
 * Revision 2.16  2003/07/19 06:59:35  hbrew
 * Complex patch:
 * * nt/w32tools.c: Fix warnings
 * * nt/w32tools.c: Fix typo in #ifdef
 * * nt/win9x.c: Fix type in #include
 * * Config.h, sys.h, branch.c, nt/service.c,
 *     nt/win9x.c, : _beginthread()-->BEGINTHREAD()
 * * binkd.c, common.h, mkfls/nt95-msvc/Makefile.dep,
 *     nt/service.c, nt/w32tools.c,nt/win9x.c: cosmitic code cleanup
 *
 * Revision 2.15  2003/07/18 14:56:34  stas
 * Use description of win2000/XP services
 *
 * Revision 2.14  2003/07/18 13:44:32  stas
 * Difference NT service internal name and display name
 *
 * Revision 2.13  2003/07/18 12:35:59  stas
 * Remove old code; add some checks; use new option '--service' for win9x
 *
 * Revision 2.12  2003/07/18 10:30:34  stas
 * New functions: IsNT(), Is9x(); small code cleanup
 *
 * Revision 2.11  2003/07/18 04:15:03  hbrew
 * Fix 'tell_start_ntservice(): {120} ...' error on Win9x
 *
 * Revision 2.10  2003/07/17 02:53:04  hbrew
 * Fix MSVC warnings & errors
 *
 * Revision 2.9  2003/07/16 15:50:44  stas
 * Fix: restore "Minimise to tray"
 *
 * Revision 2.8  2003/07/16 15:42:53  stas
 * Fix: restore -T option
 *
 * Revision 2.7  2003/07/16 15:08:49  stas
 * Fix NT services to use getopt(). Improve logging for service
 *
 * Revision 2.6  2003/06/11 09:00:44  stas
 * Don't try to install/uninstall/control service on incompatible OS. Thanks to Alexander Reznikov
 *
 * Revision 2.5  2003/06/09 18:00:19  stas
 * New command line parser (continue)
 *
 * Revision 2.4  2003/05/15 06:51:58  gul
 * Do not get 'i' and 'u' options from FTN-domain in -P option
 * (patch from Stanislav Degteff).
 *
 * Revision 2.3  2003/02/28 20:39:08  gul
 * Code cleanup:
 * change "()" to "(void)" in function declarations;
 * change C++-style comments to C-style
 *
 * Revision 2.2  2001/09/24 10:31:39  gul
 * Build under mingw32
 *
 * Revision 2.1  2001/08/24 13:23:28  da
 * binkd/binkd.c
 * binkd/readcfg.c
 * binkd/readcfg.h
 * binkd/server.c
 * binkd/nt/service.c
 *
 * Revision 2.0  2001/01/10 12:12:40  gul
 * Binkd is under CVS again
 *
 *
 */

#include <stdio.h>
#include <process.h>
#include <windows.h>
#include <io.h>
#include <direct.h>
#include <string.h>
#include <malloc.h>
#include "../readcfg.h"
#include "../tools.h"
#include "../common.h"
#include "../iphdr.h"
#include "../sys.h"
#include "service.h"
#include "w32tools.h"
#include "brw32sig.h"

#define BINKD_ICON_FILE "binkd.ico" /* place this file into binkd directory and binkd loads it for tray icon */

/* ChangeServiceConfig2() prototype:
 */
typedef BOOL (WINAPI *CSD_T)(SC_HANDLE, DWORD, LPCVOID);

extern enum serviceflags service_flag;
extern char *configpath;

static const char libname[]="ADVAPI32";
char *srvname = "binkd-service";
static const char *description = "BinkD: Fidonet TCP/IP mailer uses binkp protocol";
static const char dependencies[] = "Tcpip\0Afd\0"; /* Afd is the winsock handler */
static char reg_path_prefix[]="SYSTEM\\CurrentControlSet\\Services\\";
static char reg_path_suffix[]="\\Parameters";
static SERVICE_STATUS_HANDLE sshan;
static SERVICE_STATUS sstat;
static int res_checkservice=0;
static DWORD dwErr=0;
static char **serv_argv=NULL;
static char **serv_envp=NULL;
static enum service_main_retcodes service_main(enum service_main_types type);
extern int checkcfg_flag;
int _isService=-1;
int init_exit_service_thread = 0;


BOOL ReportStatusToSCMgr(DWORD dwCurrentState,
                         DWORD dwWin32ExitCode,
                         DWORD dwWaitHint)
{
  static DWORD dwCheckPoint = 1;
  BOOL fResult = TRUE;

  Log(12,"ReportStatusToSCMgr(%lu, %lu, %lu)",dwCurrentState,dwWin32ExitCode,dwWaitHint);

  if (dwCurrentState == SERVICE_START_PENDING)
    sstat.dwControlsAccepted = 0;
  else
    sstat.dwControlsAccepted = SERVICE_ACCEPT_STOP;
  sstat.dwCurrentState = dwCurrentState;
  sstat.dwWin32ExitCode = dwWin32ExitCode;
  sstat.dwWaitHint = dwWaitHint;

  if ( ( dwCurrentState == SERVICE_RUNNING ) ||
       ( dwCurrentState == SERVICE_STOPPED ) )
    sstat.dwCheckPoint = 0;
  else
    sstat.dwCheckPoint = dwCheckPoint++;

  fResult = SetServiceStatus( sshan, &sstat);
  return fResult;
}

static void WINAPI ServiceCtrl(DWORD dwCtrlCode)
{
  switch(dwCtrlCode)
  {
  case SERVICE_CONTROL_STOP:
    ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0);
    Log(1, "Interrupted by service stop");
/*    SigHandler(CTRL_SERVICESTOP_EVENT); */ /* Only report to log "Interrupted by service stop" */
/*    exit(0); */ /* Produce SCM error "109" */
    sstat.dwCurrentState = SERVICE_STOPPED;
  case SERVICE_CONTROL_INTERROGATE:
  default:
    break;
  }
  ReportStatusToSCMgr(sstat.dwCurrentState, NO_ERROR, 0);
}

/* Start service-specific cleanup procedure.
 * Must be set in last call of atexit().
 * (Report to SCM about service stop pending)
 */
void atServiceExitBegins(void)
{
  Log(10,"atServiceExitBegins()");
  if(IsNT() && isService())
    ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0);
}

/* Service-specific cleanup procedure. Not an thread-safe!
 * Must be set in first call of atexit().
 */
void atServiceExit(void)
{
  char *sp;
  Log(10,"atServiceExit()");

  if(!IsNT() || !isService())
    return;

  if(serv_argv)
  {
    sp=serv_argv[0];
    free(serv_argv);
    free(sp);
    serv_argv=NULL;
  }
  if(serv_envp)
  {
    sp=serv_envp[0];
    free(serv_envp);
    free(sp);
    serv_envp=NULL;
  }
  ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0);
}

int binkd_main(int argc, char **argv, char **envp);
static void ServiceStart()
{
  HKEY hk;
  LONG rc;
  DWORD dw, sw=MAXPATHLEN+1;
  int i, argc;
  char *sp=(char*)malloc(sw);
  char *env=NULL;

  strcpy(sp, reg_path_prefix);
  strcat(sp, srvname);
  strcat(sp, reg_path_suffix);

  atexit(atServiceExit);
  for(;;)
  {
    if(RegOpenKey(HKEY_LOCAL_MACHINE, sp, &hk)!=ERROR_SUCCESS)
    {
      dwErr=GetLastError();
      break;
    }
    if(RegQueryValueEx(hk, "path", NULL, &dw, sp, &sw)!=ERROR_SUCCESS)
    {
      dwErr=GetLastError();
      break;
    }
    SetCurrentDirectory(sp);
    sw=MAXPATHLEN+1;
    switch(RegQueryValueEx(hk, "args", NULL, &dw, sp, &sw))
    {
    case ERROR_SUCCESS: break;
    case ERROR_MORE_DATA:
      free(sp);
      sp=(char*)malloc(sw);
      if(RegQueryValueEx(hk, "args", NULL, &dw, sp, &sw)==ERROR_SUCCESS)
        break;
    default:
      dwErr=GetLastError();
    }
    if(dwErr!=NO_ERROR)
      break;
    sw=0;
    rc=RegQueryValueEx(hk, "env", NULL, &dw, env, &sw);
    if(((rc==ERROR_MORE_DATA)||(rc==ERROR_SUCCESS))&&(sw))
    {
      env=(char*)malloc(sw);
      if(RegQueryValueEx(hk, "env", NULL, &dw, env, &sw)!=ERROR_SUCCESS)
      {
        free(env);
        env=NULL;
      }
    }
    RegCloseKey(hk);
    hk=0;

    if (!ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0)) break;

    if(env)
    {
      for(i=argc=0;env[i];i+=strlen(env+i)+1) argc++;
      serv_envp=(char**)malloc(sizeof(char*)*(argc+1));
      for(i=argc=0;env[i];i+=strlen(env+i)+1) serv_envp[argc++]=env+i;
      serv_envp[argc++]=NULL;
    }
    for(i=argc=0;sp[i];i+=strlen(sp+i)+1) argc++;
    serv_argv=(char**)malloc(sizeof(char*)*(argc+1));
    for(i=argc=0;sp[i];i+=strlen(sp+i)+1) serv_argv[argc++]=sp+i;
    serv_argv[argc]=NULL;

    binkd_main(argc, serv_argv, serv_envp);
    break;
  }
  if(hk) RegCloseKey(hk);
}

static void WINAPI ServiceMain(DWORD argc,LPSTR* args)
{

  if(argc && args && args[0]) srvname = strdup(args[0]); /* save service name */

  sshan=RegisterServiceCtrlHandler(srvname, ServiceCtrl);
  if(sshan)
  {
    sstat.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    sstat.dwServiceSpecificExitCode = 0;
    if (ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 3000))
    {
      ServiceStart();
    }
  }
/*  if(sshan)
    atServiceExit();*/
  exit(0);
}

static DWORD srvtype = SERVICE_WIN32_OWN_PROCESS;
static enum service_main_retcodes service_main(enum service_main_types type)
{
  SC_HANDLE sman=NULL, shan=NULL;
  int i;
  enum service_main_retcodes rc=service_main_ret_ok;

  if(!IsNT())
    return service_main_ret_not;

  sman=OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if(!sman)
  { int err = GetLastError();
    if(res_checkservice)
      Log(1, "OpenSCManager failed: %s",w32err(err));
    else if(err==ERROR_ACCESS_DENIED)
    {
      Log(isService()?1:-1, "Access to NT service controls is denied.");
    }
    return 3;
  }
  if(!type)  /* return 0 if we can install service */
  {
    CloseServiceHandle(sman);
    return service_main_ret_ok;
  }
  shan=OpenService(sman, srvname, SERVICE_ALL_ACCESS);

  switch(type)
  {
  case service_main_testrunning:
    if(!shan) rc=service_test_notinstall; else
    if(!QueryServiceStatus(shan,&sstat)) rc=service_test_fail; else
    if(sstat.dwCurrentState != SERVICE_START_PENDING) rc=service_test_notrunning;
    break;
  case service_main_testinstalled: /* return 0 if service installed, or 1 if not */
    if(!shan) rc=service_test_notinstall;
    break;
  case service_main_installstart:
  case service_main_install: /* install, if we don have one */
    if(!shan)
    {
      char path[MAXPATHLEN+1];
      if(GetModuleFileName(NULL, path, MAXPATHLEN)<1)
      {
        Log(1, "Error in GetModuleFileName()=%s", w32err(GetLastError()) );
        CloseServiceHandle(sman);
        return service_main_ret_failinstall;
      }
      shan=CreateService( sman,                 /* SCManager database */
                          srvname,              /* name of service */
                          service_name,         /* name to display */
                          SERVICE_ALL_ACCESS,   /* desired access */
                          srvtype,              /* service type */
                          SERVICE_AUTO_START,   /* start type */
                          SERVICE_ERROR_NORMAL, /* error control type */
                          path,                 /* service's binary */
                          NULL,                 /* no load ordering group */
                          NULL,                 /* no tag identifier */
                          dependencies,         /* dependencies */
                          NULL,                 /* user account */
                          NULL );               /* account password */

      if(!shan)
      {
        Log(1, "Error in CreateService()=%s", w32err(GetLastError()) );
        rc=service_main_ret_failinstall;
      }
      else
      {
        CSD_T ChangeServiceDescription;
        HANDLE hwin2000scm;

        hwin2000scm = GetModuleHandle(libname);
        if (hwin2000scm) {
          ChangeServiceDescription = (CSD_T) GetProcAddress(hwin2000scm,
                                                    "ChangeServiceConfig2A");
          if (ChangeServiceDescription)
          {
            ChangeServiceDescription( shan,
                                      1 /* SERVICE_CONFIG_DESCRIPTION */,
                                      &description );
          }
        }
      }
    }
    if((rc)||(type==service_main_install)) break;
  case service_main_start: /* start service */
    if(!shan)
    {
      Log(-1, "Service \"%s\" not installed...", srvname);
      rc=service_test_notinstall;
      break;
    }
    if(StartService(shan, 0, NULL))
    {
      int j;
      for(i=j=0;(i<30)&&(QueryServiceStatus(shan,&sstat));i++)
      {
        if((sstat.dwCurrentState == SERVICE_START_PENDING)||
          ((i<3)&&(sstat.dwCurrentState != SERVICE_RUNNING))||((j++)<9))
        {
          printf(".");
          Sleep(300);
        }
        else break;
      }
      putchar('\n');
      if(sstat.dwCurrentState != SERVICE_RUNNING)
      {
        Log(-1, "Service not started...");
        rc=service_main_ret_failstart;
      }
    }
    else
    {
      Log(1, "Error in StartService()=%s", w32err(GetLastError()) );
      rc=service_main_ret_failstart;
    }
    break;
  case service_main_stop:      /* stop */
  case service_main_uninstall: /* stop & uninstall. */
    if(!shan) break;
    /* try to stop the service  */
    if(ControlService(shan, SERVICE_CONTROL_STOP, &sstat))
    {
      for(i=0;(i<30)&&(QueryServiceStatus(shan,&sstat));i++)
      {
        if((sstat.dwCurrentState==SERVICE_STOP_PENDING) ||
          ((i<3)&&(sstat.dwCurrentState!=SERVICE_STOPPED)))
        {
          printf(".");
          Sleep(300);
        }
        else break;
      }
      putchar('\n');
      if(sstat.dwCurrentState!=SERVICE_STOPPED)
      {
        Log(1, "Unable to stop service!");
        rc=service_main_ret_failstop;
      }
    }
    if(( type==service_main_uninstall) && !DeleteService(shan) )
    {
      Log(1, "Error in DeleteService()=%s", w32err(GetLastError()) );
      rc=service_main_ret_faildelete;
    }
    break;
    default:
    ;
  }

  if(shan) CloseServiceHandle(shan);
  CloseServiceHandle(sman);
  return rc;
}

static HWND mainWindow;
static NOTIFYICONDATA nd;
static int wstate = 0;

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,  LPARAM lParam)
{
    if ((lParam == WM_LBUTTONDBLCLK) || (lParam == WM_RBUTTONUP))
    {
    	ShowWindow(mainWindow, SW_RESTORE);
        SetForegroundWindow(mainWindow);
        Shell_NotifyIcon(NIM_DELETE, &nd);
        wstate = 2;
    }
    return 1;
}

static void closeProcess(void)
{
    Shell_NotifyIcon(NIM_DELETE, &nd);
    ShowWindow(mainWindow, SW_RESTORE);
}

static void processKeyCode(WORD kc, DWORD cs, HANDLE out)
{
    if ((cs & SHIFT_PRESSED) &&
        ((kc == VK_UP) || (kc == VK_DOWN) || (kc == VK_LEFT) || (kc == VK_RIGHT)))
    {
        CONSOLE_SCREEN_BUFFER_INFO cb;
        if(!GetConsoleScreenBufferInfo(out, &cb)) return;
        switch(kc)
        {
            case VK_UP: cb.dwSize.Y-=10; break;
            case VK_DOWN: cb.dwSize.Y+=10; break;
            case VK_LEFT: cb.dwSize.X-=10; break;
            case VK_RIGHT: cb.dwSize.X+=10; break;
        }
        SetConsoleScreenBufferSize(out, cb.dwSize);
    }
}

extern int percents;
extern int conlog;
extern int printq;
extern int quiet_flag;
static void wndthread(void *par)
{
    WINDOWPLACEMENT wp;
    WNDCLASS rc;
    char *cn = "testclass";
    char buf[256];
    char bn[20];
    ATOM wa;
    HWND wnd;
    HICON hi;
    HANDLE in, out;
    int i;

    i = GetConsoleTitle(buf, sizeof(buf));
    if (i < 0) i = 0;
    buf[i] = 0;
    sprintf(bn, "%x", (unsigned int)GetCurrentThreadId());
    for (i = 0; i < 40; i++)
    {
        SetConsoleTitle(bn);
        if (((mainWindow = FindWindow(NULL, bn)) != NULL) || (isService()>0)) break;
        Sleep(100);
    }
    SetConsoleTitle(buf);
    if ((!IsWindow(mainWindow)) || (!mainWindow))
    {
        if (!AllocConsole())
        {
            Log(-1, "unable to find main window... (%s)", bn);
            return;
        }
        else
        {
            HANDLE ha = GetStdHandle(STD_OUTPUT_HANDLE);
            int hCrt = _open_osfhandle((long) ha, 0x4000);
            FILE *hf = _fdopen( hCrt, "w" );
            *stdout = *hf;

            ha = GetStdHandle(STD_ERROR_HANDLE);
            hCrt = _open_osfhandle((long) ha, 0x4000);
            hf = _fdopen( hCrt, "w" );
            *stderr = *hf;
            setvbuf( stdout, NULL, _IONBF, 0 );
            setvbuf( stderr, NULL, _IONBF, 0 );

            strcpy(buf, srvname);
            SetConsoleTitle(service_name);
            for (i = 0; i < 40; i++)
            {
                if ((mainWindow = FindWindow(NULL, service_name)) != NULL) break;
                Sleep(100);
            }
            if (!mainWindow) return;
            /*_isService=0; Why this need? (commented out by stas)*/
        }
    }

    hi = (HICON)SendMessage(mainWindow, WM_GETICON, ICON_SMALL, 0);
    if (!hi)
    {
        hi = (HICON)SendMessage(mainWindow, WM_GETICON, ICON_BIG, 0);
    }
    if (!hi)
    {
        hi = LoadImage( NULL, BINKD_ICON_FILE, IMAGE_ICON, 0, 0,
                        LR_LOADFROMFILE /*| LR_LOADTRANSPARENT*/ );
    }
    if (!hi)
    {
        hi = LoadIcon(NULL, IDI_INFORMATION);
    }

    memset(&rc, 0, sizeof(rc));
    rc.lpszClassName = cn;
    rc.lpfnWndProc = WindowProc;
    wa = RegisterClass(&rc);
    if (!wa)
    {
        Log(-1, "unable to register class...");
        return;
    }
    wnd = CreateWindow(cn, "", 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
    if (!wnd)
    {
        Log(-1, "Unable to create message window...");
        return;
    }
    memset(&nd, 0, sizeof(nd));
    nd.cbSize = sizeof(nd);
    nd.hWnd = wnd;
    nd.uID = 1111;
    nd.uFlags = NIF_TIP|NIF_MESSAGE|NIF_ICON;
    nd.uCallbackMessage = 1111;
    nd.hIcon = hi;
    atexit(closeProcess);
    strncpy(nd.szTip, buf, 63);
    i = 1000;
    in = GetStdHandle(STD_INPUT_HANDLE);
    out = GetStdHandle(STD_OUTPUT_HANDLE);
    for (;;)
    {
        MSG msg;
        INPUT_RECORD cb;
        DWORD dw;
        if (in)
        {
            while(PeekConsoleInput(in, &cb, 1, &dw))
            {
                if (!dw)
                {
                    break;
                }
                ReadConsoleInput(in, &cb, 1, &dw);
                if ((cb.EventType == KEY_EVENT) && (cb.Event.KeyEvent.bKeyDown))
                {
                    processKeyCode(cb.Event.KeyEvent.wVirtualKeyCode,
                        cb.Event.KeyEvent.dwControlKeyState, out);
                }
            }
	    }

        while(PeekMessage( &msg, wnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        if ((i++) < 10)
        {
            Sleep(50);
            continue;
        }
        i = 0;
        if (wstate != 1)
        {
            memset(&wp, 0, sizeof(wp));
            wp.length = sizeof(wp);
            if ((GetWindowPlacement(mainWindow, &wp)) &&
                (wp.showCmd == SW_SHOWMINIMIZED))
            {
    	        wstate = 1;
                Shell_NotifyIcon(NIM_ADD, &nd);
                ShowWindow(mainWindow, SW_HIDE);
            }
    	}
    }
}


int service(int argc, char **argv, char **envp)
{
  int i, j, k, len;
  char *sp=NULL;
  char *esp=NULL, *asp=NULL;
  HKEY hk=0;

  if(service_flag==w32_noservice) return 0;
  else{
    if( !IsNT() )
    {
      Log(0,"Can't operate witn Windows NT services: incompatible OS type");
      return 1;
    }
  }


  if(service_name) srvname = get_service_name(service_name);  /* Use service name from command line if specified */
  else service_name = srvname;

  if (tray_flag)
  {
     srvtype |= SERVICE_INTERACTIVE_PROCESS;
#ifdef HAVE_THREADS
     BEGINTHREAD(wndthread, 0, NULL);
#endif

  }

  j=checkservice();

  if(service_flag && j==CHKSRV_CANT_INSTALL){
      Log(0, "Can't operate witn Windows NT services...");
  }

  switch(service_flag){

  case w32_installservice:
    if (j==CHKSRV_INSTALLED){
      Log(-1, "Service %s already installed...", srvname);
      exit(0);
    }else{

      sp=(char*)malloc(MAXPATHLEN+1);
      strcpy(sp, reg_path_prefix);
      strcat(sp, srvname);
      if(!service_main(service_main_install))
      {
        Log(-1, "Service '%s' installed...", srvname);
      }
      else
      {
        Log(1, "Unable to install service %s!",srvname);
        free(sp);
        return -1;
      }
      if( RegOpenKey(HKEY_LOCAL_MACHINE, sp, &hk)==ERROR_SUCCESS )
      {
        RegCloseKey(hk);
        hk = NULL;
      }
      else
      {
        Log(1, "Unable to open registry key %s!", sp);
        free(sp);
        return -1;
      }

      /* build arguments list for service */
      len = build_service_arguments(&asp, argv, 0);

      /* build enviroment */
      if(envp)
      {
        for(i=j=0; envp[i];i++) j+=strlen(envp[i])+1;
        j += strlen("BINKD_RERUN=NTSERVICE");
        esp=(char*)malloc(++j);
        for(i=j=0; envp[i];i++)
        {
          strcpy(esp+j, envp[i]);
          j+=strlen(envp[i])+1;
        }
          strcpy(esp+j, "BINKD_RERUN=NTSERVICE");
        esp[j++]=0;
      }

      strcat(sp, reg_path_suffix);
      k=1;
      if((RegOpenKey(HKEY_LOCAL_MACHINE, sp, &hk)==ERROR_SUCCESS) ||
         (RegCreateKey(HKEY_LOCAL_MACHINE, sp, &hk)==ERROR_SUCCESS))
      for(;;)
      {
        if(RegSetValueEx(hk, "args", 0, REG_BINARY, asp, len)!=ERROR_SUCCESS) break;
        if((esp)&&
           (RegSetValueEx(hk, "env", 0, REG_BINARY, esp, j)!=ERROR_SUCCESS)) break;
        if(GetCurrentDirectory(MAXPATHLEN, sp)<1) break;
        if(RegSetValueEx(hk, "path", 0, REG_SZ, sp, strlen(sp))!=ERROR_SUCCESS) break;
        k=0;
        break;
      }
      RegCloseKey(hk);
      if(k)
      {
        Log(1, "Unable to store data in registry...");
        res_checkservice=(-1);
      }
      free(sp);
      free(asp);
      if(esp) free(esp);
      if(!service_main(service_main_start))
      {
        Log(-1, "Service '%s' started.", srvname);
        exit(0);
      }
      Log(1, "Unable to start service!");
      return -1;
    }
    break;

  case w32_uninstallservice:
    if (j==CHKSRV_NOT_INSTALLED){
      Log(-1, "Service '%s' already uninstalled...", srvname);
      exit(0);
    }else{
      if(service_main(service_main_uninstall))  return argc;
      Log(-1, "Service '%s' uninstalled.", srvname);
      exit(0);
    }
    break;

  default:
    break;
  }
  return 0;
}

/* Return:
 * -1 : can't install service
 *  1 : Service not installed
 *  2 : Service installed
 */
int checkservice(void)
{
  if(res_checkservice) return res_checkservice;
  if(!IsNT()) return res_checkservice=(CHKSRV_CANT_INSTALL);
  if(service_main(service_main_services)) return res_checkservice=(CHKSRV_CANT_INSTALL);
  if(service_main(service_main_testinstalled)) return res_checkservice=CHKSRV_NOT_INSTALLED;
  return res_checkservice=CHKSRV_INSTALLED;
}

/* A hack to determine if we're running as a service without waiting for
 * the SCM to fail.
 * Idea taken from Apache sources
 */
int isService()
{
  if (_isService != -1)
    return _isService;

  if (!IsNT())
  {
    _isService = 0;
  }
  else if (!AllocConsole())
  {
    _isService = 0;
  }
  else
  {
    FreeConsole();
    _isService = 1;
  }

  return _isService;
}

/* Try connect to NT service controller
 * Return 1 if program running standalone or system error
 */
int tell_start_ntservice(void)
{
  SERVICE_TABLE_ENTRY dt[]= { {"", ServiceMain}, {NULL, NULL}};
  int res=0;

  if( !isService() )
    return 1;

  if(!StartServiceCtrlDispatcher(dt)){
    switch( GetLastError() ){           /* Can't start service */
    case ERROR_FAILED_SERVICE_CONTROLLER_CONNECT:  /*1063*/
       res=1;   /* Program running not an as service */
       break;
    case ERROR_SERVICE_ALREADY_RUNNING:
       Log(-1,"Error %u: Double call of StartServiceCtrlDispatcher()",ERROR_SERVICE_ALREADY_RUNNING);
       break;
    case ERROR_INVALID_DATA:
       Log(-1,"Error %u: The specified dispatch table contains entries that are not in the proper format.", ERROR_INVALID_DATA);
       break;
    default:
       Log(-1, "tell_start_ntservice(): %s", w32err(GetLastError()) );
    }
  }
  return res;
}

void do_tray_flag(void)
{
  BEGINTHREAD(wndthread, 0, NULL);
}
