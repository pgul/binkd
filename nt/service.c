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

#include <stdio.h>
#include <windows.h>
#include <direct.h>
#include <string.h>
#include "../tools.h"
#include "service.h"

#ifdef __WATCOMC__
#define MAXPATHLEN NAME_MAX
#else
#define MAXPATHLEN _MAX_PATH
#endif

static char libname[]="ADVAPI32";
static char srvname[]="binkd-service";
static char reg_path[]="Software\\";
static SERVICE_STATUS_HANDLE sshan;
static SERVICE_STATUS sstat;
static int res_checkservice=0;
static DWORD dwErr=0;
static char **serv_argv=NULL;
static char **serv_envp=NULL;
int service_main(int type);
extern int checkcfg_flag;
int isService=0;

static BOOL ReportStatusToSCMgr(DWORD dwCurrentState, 
                         DWORD dwWin32ExitCode, 
                         DWORD dwWaitHint) 
{ 
  static DWORD dwCheckPoint = 1; 
  BOOL fResult = TRUE; 
 
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

BOOL SigHandler(DWORD SigType);

static void WINAPI ServiceCtrl(DWORD dwCtrlCode)
{
  switch(dwCtrlCode)
  {
  case SERVICE_CONTROL_STOP:
    ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0);
    SigHandler(CTRL_SHUTDOWN_EVENT);
    return;
  case SERVICE_CONTROL_INTERROGATE:
  default:
    break;
  }
  ReportStatusToSCMgr(sstat.dwCurrentState, NO_ERROR, 0); 
}

void atServiceExit(void)
{
  char *sp;
  if(res_checkservice>0)
    ReportStatusToSCMgr(SERVICE_STOPPED, dwErr, 3000);
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
  if((checkcfg_flag==2)&&(res_checkservice>0))
    service_main(4);
}

int main(int argc, char **argv, char **envp);
static void ServiceStart(LPTSTR args)
{
  HKEY hk;
  LONG rc;
  DWORD dw, sw=MAXPATHLEN+1;
  int i, argc;
  char *sp=(char*)malloc(sw);
  char *env=NULL;
  int stopping=0;

  strcpy(sp, reg_path);
  strcat(sp, srvname);
  isService=1;

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
    serv_argv=(char**)malloc(sizeof(char*)*argc);
    for(i=argc=0;sp[i];i+=strlen(sp+i)+1) serv_argv[argc++]=sp+i;
    main(argc, serv_argv, serv_envp);
    break;
  }
  if(hk) RegCloseKey(hk);
  atServiceExit();
}

static void WINAPI ServiceMain(LPTSTR args) 
{
  sshan=RegisterServiceCtrlHandler(srvname, ServiceCtrl);
  while(sshan)
  {
    sstat.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    sstat.dwServiceSpecificExitCode = 0;
    if (!ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 3000)) break;
    ServiceStart(args);
    break;
  }
  if(sshan) 
    atServiceExit();
  exit(0);
}

static int service_main(int type)
{
  SC_HANDLE sman=NULL, shan=NULL;
  HINSTANCE hl;
  int i, rc=0;

  if((!type)||(type==6)) 
  {
    hl=LoadLibrary(libname);
    if(!hl) return 1;
    if(!GetProcAddress(hl, "OpenSCManagerA"))
    {
      FreeLibrary(hl);
      return 2;
    }
    FreeLibrary(hl);
  }
  
  sman=OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if(!sman)
  {
    if(res_checkservice)
      Log(-1, "OpenSCManager failed\n");
    return 3;
  }
  if(!type)  /* return 0 if we can install service */
  {
    CloseServiceHandle(sman);
    return 0;
  }
  shan=OpenService(sman, srvname, SERVICE_ALL_ACCESS);

  switch(type)
  {
  case 6:
    if(!shan) rc=1; else
    if(!QueryServiceStatus(shan,&sstat)) rc=2; else
    if(sstat.dwCurrentState != SERVICE_START_PENDING) rc=3;
    break;
  case 1: /* return 0 if service installed, or 1 if not */
    if(!shan) rc=1;
    break;
  case 5:
  case 2: /* install, if we don have one */
    if(!shan) 
    {
      char path[MAXPATHLEN+1];
      if(GetModuleFileName(NULL, path, MAXPATHLEN)<1)
      {
        Log(-1, "Error in GetModuleFileName()=%d\n", GetLastError());
        CloseServiceHandle(sman);
        return 1;
      }
      shan=CreateService(sman, srvname, srvname, SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, 
        SERVICE_ERROR_NORMAL, path, NULL, NULL, NULL, NULL, NULL);
      if(!shan)
        Log(-1, "Error in CreateService()=%d\n", GetLastError());
    }
    if(!shan) rc=1;
    if((rc)||(type==2)) break;
  case 4: /* start service */
    if(!shan)
    {
      Log(-1, "Service \"%s\" not installed...\n", srvname);
      rc=1;
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
      if(sstat.dwCurrentState != SERVICE_RUNNING) 
      {
        Log(-1, "Service not started...\n");
        rc=1;
      }
    }
    else
    {
      Log(-1, "Error in StartService()=%d\n", GetLastError());
      rc=1;
    }
    break;
  case 3: /* uninstall. */
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
      if(sstat.dwCurrentState!=SERVICE_STOPPED)
        Log(-1, "Unable to stop service!\n");
    }
    if(!DeleteService(shan))
    {
      Log(-1, "Error in DeleteService()=%d\n", GetLastError());
      rc=1;
    }
    break;
  }

  if(shan) CloseServiceHandle(shan);
  CloseServiceHandle(sman);
  return rc;
}

int service(int argc, char **argv, char **envp)
{
  int i, j, k, len;
  char *sp=NULL;
  char *esp=NULL, *asp=NULL;
  HKEY hk=0;

  if(argc==1)
  {
    SERVICE_TABLE_ENTRY dt[]= { srvname, (LPSERVICE_MAIN_FUNCTION)ServiceMain, NULL, NULL};
    if(service_main(6)) return argc;
    if(!StartServiceCtrlDispatcher(dt)) return argc;
    exit(0);
  }
  if((j=checkservice())<1) 
    return argc;
  for(i=len=0;i<argc;i++) 
  {
    if(argv[i][0]=='-')
    {
      if(!sp) sp=strchr(argv[i], j==1?'i':'u');
      if(strchr(argv[i], j==2?'i':'u'))
      {
        Log(-1, "Service already %sinstalled...", j==2?"":"UN");
        exit(0);
      }
    }    
    len+=strlen(argv[i])+1;
  }

  if(sp)
  {
    switch(checkservice())
    {
    case 1: /* service is not installed */
      asp=(char*)malloc(len);
      for(i=len=0;i<argc;i++)
      {
        if((argv[i][0]=='-')&&((sp=strchr(argv[i], 'i'))!=NULL))
        {
          if(!argv[i][2]) continue;
          j=sp-argv[i];
          memcpy(asp+len, argv[i], j);
          len+=j;
          if((j=strlen(sp+1))>0) memcpy(asp+len, sp+1, j);
          len+=j;
        }
        else if((j=strlen(argv[i]))>0) 
        {
          memcpy(asp+len, argv[i], j);
          len+=j;
        }
        asp[len++]=0;
      }
      asp[len++]=0;
      if(envp)
      {
        for(i=j=0; envp[i];i++) j+=strlen(envp[i])+1;
        esp=(char*)malloc(++j);
        for(i=j=0; envp[i];i++) 
        {
          strcpy(esp+j, envp[i]);
          j+=strlen(envp[i])+1;
        }
        esp[j++]=0;
      }
      sp=(char*)malloc(MAXPATHLEN+1);
      strcpy(sp, reg_path);
      strcat(sp, srvname);
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
        Log(-1, "Unable to store data in registry...\n");
        res_checkservice=(-1);
      }
      free(sp);
      free(asp);
      if(esp) free(esp);
      if(!service_main(5))
      {
        Log(-1, "%s installed and started...\n", srvname);
        exit(0);
      }
      Log(1, "Unable to start service!");
    case 2: /* service is installed */
      if(service_main(3))  return argc;
      Log(-1, "%s uninstalled...\n", srvname);
      sp=(char *)malloc(MAXPATHLEN);
      strcpy(sp, reg_path);
      strcat(sp, srvname);
      RegDeleteKey(HKEY_LOCAL_MACHINE, sp);
      free(sp);
      exit(0);      
    default:
      break;
    }
  }
  return argc;
}

int checkservice()
{
  if(res_checkservice) return res_checkservice;
  if(service_main(0)) return res_checkservice=(-1);
  if(service_main(1)) return res_checkservice=1;
  return res_checkservice=2;
}
